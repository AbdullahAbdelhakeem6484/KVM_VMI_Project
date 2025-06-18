#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <inttypes.h>
#include <libvmi/libvmi.h>
#include <libvmi/peparse.h>

#define MAX_NAME_LENGTH 100

// Function to list running processes
void list_processes(vmi_instance_t vmi) {
    addr_t list_head, current_process;
    addr_t next_process = 0;
    vmi_pid_t pid = 0;
    char *procname = NULL;
    status_t status;

    printf("\n=== Running Processes ===\n");

    // Get the head of the process list
    if(VMI_FAILURE == vmi_translate_ksym2v(vmi, "PsActiveProcessHead", &list_head)) {
        printf("Failed to find PsActiveProcessHead\n");
        return;
    }

    current_process = list_head;

    do {
        // Read process name
        procname = vmi_read_str_va(vmi, current_process + 0x5a8, 0);  // EPROCESS.ImageFileName offset
        if (procname) {
            // Read process ID
            status = vmi_read_32_va(vmi, current_process + 0x2e0, 0, (uint32_t*)&pid);  // UniqueProcessId offset
            if (status == VMI_SUCCESS) {
                printf("Process: %-20s (PID: %d)\n", procname, pid);
            }
            free(procname);
        }

        // Read next process pointer
        status = vmi_read_addr_va(vmi, current_process + 0x2e8, 0, &next_process);  // ActiveProcessLinks offset
        if (status == VMI_FAILURE) break;

        current_process = next_process - 0x2e8;  // Adjust for list entry offset

    } while (next_process != list_head);
}

// Function to list loaded modules for a process
void list_modules(vmi_instance_t vmi, addr_t process_base) {
    addr_t peb, ldr, module_list, current_module;
    addr_t next_module = 0;
    unicode_string_t *us = NULL;
    status_t status;

    printf("\n=== Loaded Modules ===\n");

    // Read PEB address
    status = vmi_read_addr_va(vmi, process_base + 0x3f8, 0, &peb);  // PEB offset
    if (status == VMI_FAILURE) return;

    // Read Ldr address
    status = vmi_read_addr_va(vmi, peb + 0x18, process_base, &ldr);
    if (status == VMI_FAILURE) return;

    // Get module list head
    status = vmi_read_addr_va(vmi, ldr + 0x20, process_base, &module_list);
    if (status == VMI_FAILURE) return;

    current_module = module_list;

    do {
        // Read module name
        us = vmi_read_unicode_str_va(vmi, current_module + 0x60, process_base);
        if (us) {
            printf("Module: %s\n", us->contents);
            vmi_free_unicode_str(us);
        }

        // Read next module
        status = vmi_read_addr_va(vmi, current_module, process_base, &next_module);
        if (status == VMI_FAILURE) break;

        current_module = next_module;

    } while (next_module != module_list);
}

// Function to list threads for a process
void list_threads(vmi_instance_t vmi, addr_t process_base) {
    addr_t thread_list, current_thread;
    addr_t next_thread = 0;
    uint32_t thread_id = 0;
    status_t status;

    printf("\n=== Active Threads ===\n");

    // Get thread list head
    thread_list = process_base + 0x30;  // ThreadListHead offset
    current_thread = thread_list;

    do {
        // Read thread ID
        status = vmi_read_32_va(vmi, current_thread + 0x3b0, 0, &thread_id);
        if (status == VMI_SUCCESS) {
            printf("Thread ID: %d\n", thread_id);
        }

        // Read next thread
        status = vmi_read_addr_va(vmi, current_thread, 0, &next_thread);
        if (status == VMI_FAILURE) break;

        current_thread = next_thread;

    } while (next_thread != thread_list);
}

int main(int argc, char *argv[]) {
    vmi_instance_t vmi;
    status_t status;
    uint64_t domid = 0;
    uint64_t init_flags = VMI_INIT_DOMAINNAME;

    /* Initialize LibVMI */
    status = vmi_init(&vmi, VMI_KVM, "win10-vmi", init_flags, NULL, NULL);
    if (status == VMI_FAILURE) {
        printf("Failed to initialize LibVMI\n");
        return 1;
    }

    printf("LibVMI initialization succeeded\n");

    /* Get the offset of the process list head */
    addr_t list_head = 0;
    addr_t current_process = 0;
    addr_t next_process = 0;

    /* Get the head of the process list */
    status = vmi_translate_ksym2v(vmi, "PsInitialSystemProcess", &list_head);
    if (status == VMI_FAILURE) {
        printf("Failed to find PsInitialSystemProcess\n");
        goto exit;
    }

    /* Read the first process */
    current_process = list_head;

    printf("\nProcess Listing:\n");
    printf("%-20s %-6s\n", "Process", "PID");
    printf("-------------------- ------\n");

    while (1) {
        /* Get the process name */
        char *procname = vmi_read_str_va(vmi, current_process + 0x5a8, 0);
        if (!procname) {
            goto next;
        }

        /* Get the process ID */
        vmi_pid_t pid = 0;
        status = vmi_read_32_va(vmi, current_process + 0x2e0, 0, (uint32_t*)&pid);
        if (status == VMI_FAILURE) {
            free(procname);
            goto next;
        }

        /* Print the process information */
        printf("%-20s %-6d\n", procname, pid);
        free(procname);

next:
        /* Follow the next pointer */
        status = vmi_read_addr_va(vmi, current_process + 0x2e8, 0, &next_process);
        if (status == VMI_FAILURE || next_process == list_head) {
            break;
        }
        current_process = next_process;
    }

exit:
    vmi_destroy(vmi);
    return 0;
} 