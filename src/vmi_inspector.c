int main(int argc, char *argv[]) {
    vmi_instance_t vmi;
    status_t status;
    uint64_t init_flags = VMI_INIT_DOMAINNAME;

    /* Initialize LibVMI */
    status = vmi_init(&vmi, VMI_KVM, "win10-vmi", init_flags, NULL, NULL);
    if (status == VMI_FAILURE) {
        printf("Failed to initialize LibVMI\n");
        return 1;
    }

    printf("LibVMI initialization succeeded\n");

    return 0;
} 