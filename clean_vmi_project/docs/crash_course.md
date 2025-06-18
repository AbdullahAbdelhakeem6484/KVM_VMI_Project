# KVM, Hypervisors, and VMI Crash Course

## Table of Contents
1. [KVM Fundamentals](#kvm-fundamentals)
2. [Hypervisor Types and Architecture](#hypervisor-types)
3. [Virtual Machine Introspection](#vmi)
4. [Interview Preparation](#interview-prep)
5. [Further Reading](#further-reading)

## KVM Fundamentals

### What is KVM?
- Kernel-based Virtual Machine (KVM) is a Linux kernel module that transforms Linux into a type-1 (bare-metal) hypervisor
- Each VM is a regular Linux process, scheduled by the standard Linux scheduler
- Uses hardware virtualization extensions (Intel VT-x, AMD-V)

### Key Components
1. **KVM Kernel Module**
   - Provides core virtualization capabilities
   - Manages CPU and memory virtualization
   - Handles VM exits and entries

2. **QEMU**
   - User-space emulator
   - Handles device emulation
   - Manages VM lifecycle

3. **Libvirt**
   - Management API
   - VM configuration and control
   - Network and storage management

## Hypervisor Types

### Type 1 (Bare-Metal)
- Runs directly on hardware
- Examples: KVM, Xen, VMware ESXi
- Advantages:
  - Better performance
  - Lower overhead
  - Direct hardware access

### Type 2 (Hosted)
- Runs on top of an OS
- Examples: VirtualBox, VMware Workstation
- Advantages:
  - Easier setup
  - More flexible
  - Better hardware compatibility

## Virtual Machine Introspection (VMI)

### What is VMI?
- Technique to inspect VM state from hypervisor layer
- No agent required inside guest
- Used for:
  - Security monitoring
  - Forensics
  - Malware analysis
  - System debugging

### LibVMI
- C library for VMI
- Supports multiple hypervisors:
  - Xen (native)
  - KVM (with KVM-VMI patches)
  - QEMU
- Features:
  - Memory reading
  - Process enumeration
  - Module enumeration
  - Thread enumeration

### KVM-VMI
- Kernel patch for KVM
- Exposes VM memory to LibVMI
- Not in mainline Linux
- Required for true VMI on KVM

## Interview Preparation

### Key Concepts to Master

1. **Virtualization Basics**
   - CPU virtualization
   - Memory virtualization
   - I/O virtualization
   - VM exits and entries

2. **KVM Architecture**
   - Kernel module role
   - QEMU integration
   - Memory management
   - Device emulation

3. **VMI Techniques**
   - Memory parsing
   - OS structure analysis
   - Process tracking
   - Event monitoring

### Common Interview Questions

1. **How does KVM provide isolation?**
   - Hardware virtualization
   - Separate address spaces
   - VMCS/VMCB
   - EPT/NPT

2. **How does VMI work?**
   - Hypervisor exposes guest memory
   - Tools parse OS structures
   - Memory pattern matching
   - Event monitoring

3. **What are the challenges of VMI on KVM?**
   - Standard KVM limitations
   - KVM-VMI patch requirement
   - Performance overhead
   - Detection possibilities

4. **Alternatives to KVM for VMI?**
   - Xen (native support)
   - QEMU (monitor interface)
   - VMware (VM introspection APIs)

## Further Reading

### Documentation
- [KVM Documentation](https://www.linux-kvm.org/page/Documentation)
- [LibVMI Documentation](https://libvmi.com/docs/)
- [KVM-VMI Project](https://github.com/libvmi/kvm-vmi)
- [Xen VMI](https://wiki.xenproject.org/wiki/VMI)

### Books
- "Professional Xen Virtualization" by William von Hagen
- "KVM Virtualization Cookbook" by Konstantin Ivanov
- "Virtual Machine Introspection" by Tal Garfinkel

### Online Resources
- [KVM Forum](https://kvmforum2020.sched.com/)
- [Xen Project](https://xenproject.org/)
- [LibVMI Blog](https://libvmi.com/blog/) 