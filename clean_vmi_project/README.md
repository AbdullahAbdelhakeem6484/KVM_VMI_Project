# KVM-VMI Project: Virtual Machine Introspection

A complete implementation of Virtual Machine Introspection (VMI) using KVM hypervisor and LibVMI for Windows 10 guest analysis.

## 🏗️ Project Structure

```
clean_vmi_project/
├── src/                          # Source code
│   ├── vmi_complete_inspector.c  # Main VMI program (recommended)
│   ├── vmi_windows_inspector.c   # Windows-specific implementation
│   └── vmi_inspector.c           # Basic VMI implementation
├── config/                       # Configuration files
│   ├── libvmi.conf              # LibVMI Windows 10 configuration
│   └── win10-vmi.xml            # VM configuration
├── build/                        # Compiled binaries (created by make)
├── scripts/                      # Utility scripts
│   └── demo_vmi_project.sh      # Project demonstration
├── docs/                         # Documentation
│   ├── PROJECT_COMPLETION.md    # Detailed project report
│   └── *.pdf                    # Reference materials
├── Makefile                      # Build system
└── README.md                     # This file
```

## ✅ Features Implemented

### 1. Process Enumeration
- Lists all running processes with PIDs and process names
- Uses Windows EPROCESS structure traversal
- Accesses process list via PsActiveProcessHead/PsInitialSystemProcess

### 2. Module Enumeration  
- Lists loaded modules for processes
- Extracts module names, base addresses, and sizes
- Traverses PEB (Process Environment Block) and LDR structures

### 3. Thread Enumeration
- Lists active threads for processes
- Shows thread IDs and associated process IDs
- Traverses ETHREAD structures via ThreadListHead

## 🚀 Quick Start

### Prerequisites
- Ubuntu/Debian Linux system
- KVM hypervisor installed
- Windows 10 VM named 'win10-vmi'
- LibVMI library installed

### Build and Test

```bash
# 1. Check dependencies
make check-deps

# 2. Build all programs
make all

# 3. Install configuration
make install

# 4. Test with Windows 10 VM (must be running)
make test

# 5. Run full demonstration
make demo
```

## 🛠️ Detailed Usage

### Building Individual Components

```bash
# Build specific program
make build/vmi_complete_inspector

# Clean and rebuild
make clean && make all

# Get help
make help
```

### Running VMI Inspector

```bash
# Run the complete inspector (recommended)
sudo ./build/vmi_complete_inspector win10-vmi

# Run Windows-specific version
sudo ./build/vmi_windows_inspector win10-vmi

# Run basic version
sudo ./build/vmi_inspector win10-vmi
```

## 📋 System Requirements

### Hardware
- x86_64 processor with VT-x/AMD-V support
- Minimum 8GB RAM (4GB for host, 4GB for VM)
- 50GB+ free disk space

### Software
- Linux kernel with KVM support
- libvirt and QEMU
- GCC compiler
- LibVMI library
- Windows 10 ISO for VM installation

## 🔧 Configuration

### LibVMI Configuration (`config/libvmi.conf`)
Contains Windows 10 specific offsets for EPROCESS and other structures:

```conf
win10-vmi {
    ostype = "Windows";
    win_pdbase = 0x28;
    win_pid = 0x2e0;
    win_pname = 0x5a8;
    win_tasks = 0x2e8;
    # ... additional offsets
}
```

### VM Configuration (`config/win10-vmi.xml`)
KVM/QEMU configuration for Windows 10 VM with proper UEFI setup.

## 📊 Technical Implementation

### Process Enumeration Algorithm
1. Resolve kernel symbol (PsActiveProcessHead/PsInitialSystemProcess)
2. Read initial process address
3. Traverse ActiveProcessLinks linked list
4. Extract process name from ImageFileName offset (0x5a8)
5. Extract PID from UniqueProcessId offset (0x2e0)

### Module Enumeration Algorithm
1. Read PEB address from EPROCESS.Peb (offset 0x3f8)
2. Access PEB_LDR_DATA via PEB.Ldr (offset 0x18)
3. Traverse InLoadOrderModuleList linked list
4. Extract module information from LDR_DATA_TABLE_ENTRY

### Thread Enumeration Algorithm
1. Access ThreadListHead from EPROCESS (offset 0x5e0)
2. Traverse ETHREAD structures in linked list
3. Extract thread IDs from ClientId structure
4. Handle proper offset adjustments for list traversal

## 🛡️ Code Quality Features

- **Error Handling**: Comprehensive error checking and recovery
- **Memory Safety**: Proper cleanup to prevent memory leaks  
- **Compatibility**: Multiple offset fallbacks for Windows versions
- **Protection**: Infinite loop detection and bounds checking
- **VM Control**: Safe VM pause/resume for consistent results

## 🔍 Troubleshooting

### Common Issues

1. **LibVMI initialization fails**
   - Ensure VM is running: `virsh list`
   - Check configuration: `/etc/libvmi.conf`
   - Verify permissions: run with `sudo`

2. **Compilation errors**
   - Install dependencies: `make install-deps`
   - Check LibVMI installation: `pkg-config --exists libvmi`

3. **No processes found**
   - Verify Windows 10 VM is fully booted
   - Check Windows version compatibility
   - Try alternative offset fallbacks

### Debug Commands

```bash
# Check VM status
virsh list --all

# Verify LibVMI installation
ldconfig -p | grep vmi

# Check system logs
journalctl -f
```

## 📈 Testing Results

The VMI inspector successfully demonstrates:
- ✅ Process enumeration with PID extraction
- ✅ Module listing with base addresses
- ✅ Thread enumeration with thread IDs
- ✅ Proper VM control (pause/resume)
- ✅ Error handling and recovery

## 🔄 Development

### Adding New Features
1. Create new C file in `src/`
2. Add build target to `Makefile`
3. Update documentation
4. Test thoroughly

### Code Style
- Follow C99 standard
- Use descriptive variable names
- Include comprehensive error checking
- Document complex algorithms

## 📚 References

- [LibVMI Documentation](https://libvmi.com/docs/)
- [KVM-VMI Project](https://github.com/KVM-VMI/kvm-vmi)
- [Windows Internals](https://docs.microsoft.com/en-us/sysinternals/)
- [VMI Research Papers](./docs/)

## 🏆 Achievement

Successfully implemented complete Virtual Machine Introspection system demonstrating deep understanding of:
- Windows kernel structures (EPROCESS, ETHREAD, PEB)
- LibVMI API usage and memory introspection
- KVM hypervisor integration
- System-level programming and debugging

## 📝 License

This project is for educational and research purposes. Please ensure compliance with applicable laws and regulations when using VMI techniques.

---

**Project Status: ✅ COMPLETE**  
All required VMI features implemented and tested successfully. 