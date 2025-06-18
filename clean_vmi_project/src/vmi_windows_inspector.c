#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <libvmi/libvmi.h>
#include <libvmi/peparse.h>

#define MAX_NAME_LENGTH 100

static vmi_instance_t vmi;

// Function to list running processes
void list_processes(vmi_instance_t vmi) {
    addr_t list_head, current_process;
    addr_t next_process = 0;
    char *procname = NULL;
    vmi_pid_t pid = 0;

    printf("\n=== Running Processes ===\n");

    if(VMI_FAILURE == vmi_read_addr_ksym(vmi, "PsActiveProcessHead", &list_head)) {
        printf("Failed to find PsActiveProcessHead\n");
        return;
    }

    current_process = list_head;
    
    do {
        procname = vmi_read_str_va(vmi, current_process + 0x5a8, 0);  // EPROCESS.ImageFileName offset
        if (procname) {
            vmi_read_32_va(vmi, current_process + 0x2e0, 0, (uint32_t*)&pid);  // UniqueProcessId offset
            printf("Process: %-20s (PID: %d)\n", procname, pid);
            free(procname);
        }

        if(VMI_FAILURE == vmi_read_addr_va(vmi, current_process + 0x2f0, 0, &next_process)) {
            break;
        }
        
        current_process = next_process - 0x2e8;  // Adjust for ActiveProcessLinks offset
        
    } while(next_process && current_process != list_head);
}

// Function to list loaded modules
void list_modules(vmi_instance_t vmi) {
    addr_t current_process, peb, ldr, module_list;
    addr_t next_module;
    unicode_string_t *us = NULL;

    printf("\n=== Loaded Modules ===\n");

    // Get System process
    if(VMI_FAILURE == vmi_translate_ksym2v(vmi, "PsInitialSystemProcess", &current_process)) {
        printf("Failed to find PsInitialSystemProcess\n");
        return;
    }

    // Get PEB
    vmi_read_addr_va(vmi, current_process + 0x3f8, 0, &peb);  // PEB offset
    
    // Get Ldr
    vmi_read_addr_va(vmi, peb + 0x18, 0, &ldr);
    
    // Get InLoadOrderModuleList
    vmi_read_addr_va(vmi, ldr + 0x10, 0, &module_list);
    
    next_module = module_list;
    
    do {
        us = vmi_read_unicode_str_va(vmi, next_module + 0x60, 0);  // BaseDllName offset
        if (us) {
            unicode_string_t out = { 0 };
            if (VMI_SUCCESS == vmi_convert_str_encoding(us, &out, "UTF-8")) {
                printf("Module: %s\n", out.contents);
                free(out.contents);
            }
            vmi_free_unicode_str(us);
        }

        vmi_read_addr_va(vmi, next_module, 0, &next_module);
        
    } while(next_module && next_module != module_list);
}

// Function to list active threads
void list_threads(vmi_instance_t vmi) {
    addr_t current_process;
    addr_t thread_list, current_thread;
    vmi_pid_t pid = 0;

    printf("\n=== Active Threads ===\n");

    if(VMI_FAILURE == vmi_translate_ksym2v(vmi, "PsInitialSystemProcess", &current_process)) {
        printf("Failed to find PsInitialSystemProcess\n");
        return;
    }

    thread_list = current_process + 0x5e0;  // ThreadListHead offset
    current_thread = thread_list;

    do {
        vmi_read_32_va(vmi, current_thread - 0x3b8 + 0x2e8, 0, (uint32_t*)&pid);  // Get thread's process ID
        printf("Thread in process PID: %d\n", pid);

        vmi_read_addr_va(vmi, current_thread + 0x8, 0, &current_thread);
        current_thread -= 0x5e0;  // Adjust for ThreadListEntry offset
        
    } while(current_thread && current_thread != thread_list);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <VM name>\n", argv[0]);
        return 1;
    }

    // Initialize LibVMI
    vmi_init_error_t error;
    if (VMI_FAILURE == vmi_init(&vmi, VMI_KVM, argv[1], VMI_INIT_DOMAINNAME, NULL, &error)) {
        printf("Failed to initialize LibVMI\n");
        return 1;
    }

    // Pause the VM for consistent introspection
    if (VMI_FAILURE == vmi_pause_vm(vmi)) {
        printf("Failed to pause VM\n");
        goto cleanup;
    }

    // Perform introspection
    list_processes(vmi);
    list_modules(vmi);
    list_threads(vmi);

    // Resume the VM
    vmi_resume_vm(vmi);

cleanup:
    vmi_destroy(vmi);
    return 0;
} 