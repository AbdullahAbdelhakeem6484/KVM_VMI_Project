#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <libvmi/libvmi.h>

#define MAX_NAME_LENGTH 256

// Global VMI instance
vmi_instance_t vmi;

// Function to print process information
void print_process_info(addr_t process_addr) {
    char *procname = NULL;
    vmi_pid_t pid = 0;
    addr_t dtb = 0;
    
    // Try to read process name (EPROCESS.ImageFileName)
    procname = vmi_read_str_va(vmi, process_addr + 0x5a8, 0);
    if (!procname) {
        procname = vmi_read_str_va(vmi, process_addr + 0x450, 0); // Try alternative offset
    }
    
    // Read process ID (EPROCESS.UniqueProcessId)
    if (VMI_SUCCESS != vmi_read_32_va(vmi, process_addr + 0x2e0, 0, (uint32_t*)&pid)) {
        vmi_read_32_va(vmi, process_addr + 0x180, 0, (uint32_t*)&pid); // Try alternative offset
    }
    
    // Read DirectoryTableBase (CR3)
    vmi_read_addr_va(vmi, process_addr + 0x28, 0, &dtb);
    
    printf("%-25s PID: %-8d DTB: 0x%016lx\n", 
           procname ? procname : "Unknown", pid, dtb);
    
    if (procname) free(procname);
}

// Function to list running processes
int list_processes() {
    addr_t list_head = 0, current_process = 0, next_process = 0;
    int count = 0;
    
    printf("\n=== RUNNING PROCESSES ===\n");
    printf("%-25s %-13s %s\n", "Process Name", "PID", "DTB");
    printf("================================================================\n");
    
    // Try multiple methods to get process list
    if (VMI_FAILURE == vmi_translate_ksym2v(vmi, "PsActiveProcessHead", &list_head)) {
        if (VMI_FAILURE == vmi_translate_ksym2v(vmi, "PsInitialSystemProcess", &list_head)) {
            printf("Failed to find process list head\n");
            return -1;
        }
        // If we got PsInitialSystemProcess, read the first process directly
        current_process = list_head;
    } else {
        // If we got PsActiveProcessHead, read the first process from the list
        if (VMI_FAILURE == vmi_read_addr_va(vmi, list_head, 0, &current_process)) {
            printf("Failed to read first process from list\n");
            return -1;
        }
        current_process -= 0x2e8; // Adjust for ActiveProcessLinks offset
    }
    
    addr_t start_process = current_process;
    
    do {
        print_process_info(current_process);
        count++;
        
        // Read next process (EPROCESS.ActiveProcessLinks.Flink)
        if (VMI_FAILURE == vmi_read_addr_va(vmi, current_process + 0x2e8, 0, &next_process)) {
            break;
        }
        
        current_process = next_process - 0x2e8; // Adjust for list entry offset
        
        // Prevent infinite loops
        if (count > 1000 || current_process == start_process) {
            break;
        }
        
    } while (next_process != 0 && current_process != start_process);
    
    printf("\nTotal processes found: %d\n", count);
    return count;
}

// Function to list loaded modules for a specific process
int list_modules_for_process(addr_t process_addr, const char* process_name) {
    addr_t peb = 0, ldr = 0, module_list = 0, current_module = 0;
    int count = 0;
    
    printf("\n=== LOADED MODULES FOR %s ===\n", process_name);
    
    // Read PEB address (EPROCESS.Peb)
    if (VMI_FAILURE == vmi_read_addr_va(vmi, process_addr + 0x3f8, 0, &peb)) {
        printf("Failed to read PEB address\n");
        return -1;
    }
    
    if (peb == 0) {
        printf("PEB is NULL (likely system process)\n");
        return 0;
    }
    
    // Read Ldr address (PEB.Ldr)
    if (VMI_FAILURE == vmi_read_addr_va(vmi, peb + 0x18, 0, &ldr)) {
        printf("Failed to read PEB.Ldr\n");
        return -1;
    }
    
    if (ldr == 0) {
        printf("PEB.Ldr is NULL\n");
        return 0;
    }
    
    // Read InLoadOrderModuleList (PEB_LDR_DATA.InLoadOrderModuleList)
    if (VMI_FAILURE == vmi_read_addr_va(vmi, ldr + 0x10, 0, &module_list)) {
        printf("Failed to read module list\n");
        return -1;
    }
    
    current_module = module_list;
    addr_t start_module = module_list;
    
    do {
        unicode_string_t *module_name = NULL;
        addr_t base_address = 0;
        uint32_t size = 0;
        
        // Read module name (LDR_DATA_TABLE_ENTRY.BaseDllName)
        module_name = vmi_read_unicode_str_va(vmi, current_module + 0x60, 0);
        if (module_name && module_name->contents) {
            // Read base address (LDR_DATA_TABLE_ENTRY.DllBase)
            vmi_read_addr_va(vmi, current_module + 0x30, 0, &base_address);
            // Read size (LDR_DATA_TABLE_ENTRY.SizeOfImage)
            vmi_read_32_va(vmi, current_module + 0x40, 0, &size);
            
            printf("  %-40s Base: 0x%016lx Size: 0x%08x\n", 
                   module_name->contents, base_address, size);
            count++;
        }
        
        if (module_name) vmi_free_unicode_str(module_name);
        
        // Read next module (Flink)
        if (VMI_FAILURE == vmi_read_addr_va(vmi, current_module, 0, &current_module)) {
            break;
        }
        
        // Prevent infinite loops
        if (count > 500 || current_module == start_module) {
            break;
        }
        
    } while (current_module != 0 && current_module != start_module);
    
    printf("Total modules found: %d\n", count);
    return count;
}

// Function to list threads for a specific process
int list_threads_for_process(addr_t process_addr, const char* process_name) {
    addr_t thread_list = 0, current_thread = 0;
    int count = 0;
    
    printf("\n=== ACTIVE THREADS FOR %s ===\n", process_name);
    
    // ThreadListHead is at offset 0x5e0 in EPROCESS
    thread_list = process_addr + 0x5e0;
    
    if (VMI_FAILURE == vmi_read_addr_va(vmi, thread_list, 0, &current_thread)) {
        printf("Failed to read thread list\n");
        return -1;
    }
    
    addr_t start_thread = current_thread;
    current_thread -= 0x5e0; // Adjust for ThreadListEntry offset
    
    do {
        uint32_t thread_id = 0;
        uint32_t process_id = 0;
        
        // Read ClientId.UniqueThread (ETHREAD.Cid.UniqueThread)
        if (VMI_SUCCESS == vmi_read_32_va(vmi, current_thread + 0x648, 0, &thread_id)) {
            // Read ClientId.UniqueProcess (ETHREAD.Cid.UniqueProcess)  
            vmi_read_32_va(vmi, current_thread + 0x644, 0, &process_id);
            printf("  Thread ID: %-8d Process ID: %-8d\n", thread_id, process_id);
            count++;
        }
        
        // Read next thread (ThreadListEntry.Flink)
        if (VMI_FAILURE == vmi_read_addr_va(vmi, current_thread + 0x5e0, 0, &current_thread)) {
            break;
        }
        current_thread -= 0x5e0; // Adjust for ThreadListEntry offset
        
        // Prevent infinite loops
        if (count > 1000 || current_thread == (start_thread - 0x5e0)) {
            break;
        }
        
    } while (current_thread != 0 && current_thread != (start_thread - 0x5e0));
    
    printf("Total threads found: %d\n", count);
    return count;
}

// Main function
int main(int argc, char **argv) {
    vmi_init_error_t error;
    char *vm_name = "win10-vmi";
    
    if (argc == 2) {
        vm_name = argv[1];
    }
    
    printf("=== Windows 10 VMI Inspector ===\n");
    printf("Target VM: %s\n", vm_name);
    
    // Initialize LibVMI with basic flags
    if (VMI_FAILURE == vmi_init(&vmi, VMI_KVM, vm_name, VMI_INIT_DOMAINNAME, NULL, &error)) {
        printf("Failed to initialize LibVMI (Error: %d)\n", error);
        printf("Make sure:\n");
        printf("1. VM '%s' is running\n", vm_name);
        printf("2. libvmi.conf is properly configured\n");
        printf("3. You have proper permissions\n");
        return 1;
    }
    
    printf("LibVMI initialization successful!\n");
    
    // Get OS information
    os_t os = vmi_get_ostype(vmi);
    printf("Detected OS: %s\n", os == VMI_OS_WINDOWS ? "Windows" : "Unknown");
    
    // Pause the VM for consistent introspection
    if (VMI_SUCCESS == vmi_pause_vm(vmi)) {
        printf("VM paused for introspection\n");
        
        // List all processes
        int process_count = list_processes();
        
        if (process_count > 0) {
            // Find a user process for module/thread enumeration
            addr_t current_process = 0;
            if (VMI_SUCCESS == vmi_translate_ksym2v(vmi, "PsInitialSystemProcess", &current_process)) {
                char *procname = vmi_read_str_va(vmi, current_process + 0x5a8, 0);
                if (procname) {
                    list_modules_for_process(current_process, procname);
                    list_threads_for_process(current_process, procname);
                    free(procname);
                }
            }
        }
        
        // Resume the VM
        vmi_resume_vm(vmi);
        printf("\nVM resumed\n");
    } else {
        printf("Warning: Could not pause VM, results may be inconsistent\n");
    }
    
    // Cleanup
    vmi_destroy(vmi);
    printf("\nVMI inspection completed successfully!\n");
    
    return 0;
} 