# KVM-VMI Project Makefile
# Virtual Machine Introspection using LibVMI and KVM

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE -g
LIBS = -lvmi

# Directories
SRC_DIR = src
BUILD_DIR = build
CONFIG_DIR = config
SCRIPTS_DIR = scripts
DOCS_DIR = docs

# LibVMI paths
INCLUDE_DIRS = -I/usr/local/include -I/usr/include/libvmi
LIB_DIRS = -L/usr/local/lib -L/usr/lib

# Source files and targets
SOURCES = $(wildcard $(SRC_DIR)/*.c)
TARGETS = $(BUILD_DIR)/vmi_complete_inspector $(BUILD_DIR)/vmi_windows_inspector $(BUILD_DIR)/vmi_inspector $(BUILD_DIR)/vmi_working_inspector $(BUILD_DIR)/vmi_real_inspector

# Default target
.PHONY: all clean install test demo help setup run-working run-real solution

all: setup $(TARGETS)

# Create build directory
setup:
	@mkdir -p $(BUILD_DIR)
	@echo "Build directory created: $(BUILD_DIR)"

# Build targets
$(BUILD_DIR)/vmi_complete_inspector: $(SRC_DIR)/vmi_complete_inspector.c
	@echo "Building complete VMI inspector..."
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -o $@ $< $(LIB_DIRS) $(LIBS)
	@echo "✓ Complete VMI inspector built successfully"

$(BUILD_DIR)/vmi_windows_inspector: $(SRC_DIR)/vmi_windows_inspector.c
	@echo "Building Windows VMI inspector..."
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -o $@ $< $(LIB_DIRS) $(LIBS)
	@echo "✓ Windows VMI inspector built successfully"

$(BUILD_DIR)/vmi_inspector: $(SRC_DIR)/vmi_inspector.c
	@echo "Building basic VMI inspector..."
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -o $@ $< $(LIB_DIRS) $(LIBS)
	@echo "✓ Basic VMI inspector built successfully"

$(BUILD_DIR)/vmi_working_inspector: $(SRC_DIR)/vmi_working_inspector.c
	@echo "Building working VMI inspector with fallbacks..."
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -o $@ $< $(LIB_DIRS) $(LIBS)
	@echo "✓ Working VMI inspector built successfully"

$(BUILD_DIR)/vmi_real_inspector: $(SRC_DIR)/vmi_real_inspector.c
	@echo "Building real VMI inspector with enhanced capabilities..."
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -o $@ $< $(LIB_DIRS) $(LIBS)
	@echo "✓ Real VMI inspector built successfully"

# Install configuration
install: all
	@echo "Installing VMI configuration..."
	sudo cp $(CONFIG_DIR)/libvmi.conf /etc/libvmi.conf
	@echo "✓ Configuration installed to /etc/libvmi.conf"

# Test the VMI inspector
test: $(BUILD_DIR)/vmi_complete_inspector
	@echo "Testing VMI inspector..."
	@echo "Note: Requires Windows 10 VM 'win10-vmi' to be running"
	@virsh list | grep -q "win10-vmi.*running" || (echo "❌ VM win10-vmi is not running" && exit 1)
	@echo "✓ VM win10-vmi is running"
	sudo $(BUILD_DIR)/vmi_complete_inspector win10-vmi

# Run the working VMI inspector (with fallbacks)
run-working: $(BUILD_DIR)/vmi_working_inspector
	@echo "Running working VMI inspector with fallbacks..."
	@virsh list | grep -q "win10-vmi.*running" || (echo "❌ VM win10-vmi is not running" && exit 1)
	@echo "✓ VM win10-vmi is running"
	sudo $(BUILD_DIR)/vmi_working_inspector

# Run the real VMI inspector (enhanced solution)
run-real: $(BUILD_DIR)/vmi_real_inspector
	@echo "Running real VMI inspector with enhanced capabilities..."
	@virsh list | grep -q "win10-vmi.*running" || (echo "❌ VM win10-vmi is not running" && exit 1)
	@echo "✓ VM win10-vmi is running"
	sudo $(BUILD_DIR)/vmi_real_inspector

# Complete solution demonstration
solution: all
	@echo "=== KVM-VMI Complete Solution Demo ==="
	@echo "1. Checking VM status..."
	@virsh list | grep win10-vmi || (echo "❌ Start win10-vmi VM first" && exit 1)
	@echo "2. Running enhanced VMI inspector..."
	sudo $(BUILD_DIR)/vmi_real_inspector
	@echo "3. Solution documentation available in $(DOCS_DIR)/VMI_SOLUTION_COMPLETE.md"
	@echo "✓ Complete solution demonstrated successfully"

# Run demonstration
demo: all
	@echo "Running project demonstration..."
	chmod +x $(SCRIPTS_DIR)/demo_vmi_project.sh
	cd .. && ./clean_vmi_project/$(SCRIPTS_DIR)/demo_vmi_project.sh

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR)
	@echo "✓ Clean completed"

# Check dependencies
check-deps:
	@echo "Checking dependencies..."
	@which gcc >/dev/null || (echo "❌ GCC not found" && exit 1)
	@pkg-config --exists libvmi 2>/dev/null || (echo "❌ LibVMI not found" && exit 1)
	@which virsh >/dev/null || (echo "❌ libvirt not found" && exit 1)
	@echo "✓ All dependencies found"

# Install dependencies (Ubuntu/Debian)
install-deps:
	@echo "Installing dependencies..."
	sudo apt-get update
	sudo apt-get install -y build-essential libvmi-dev libvirt-clients qemu-kvm
	@echo "✓ Dependencies installed"

# Help message
help:
	@echo "KVM-VMI Project Build System"
	@echo "============================"
	@echo ""
	@echo "Available targets:"
	@echo "  all           - Build all VMI inspector programs"
	@echo "  setup         - Create build directories"
	@echo "  install       - Install VMI configuration files"
	@echo "  test          - Test the complete VMI inspector"
	@echo "  run-working   - Run working VMI inspector with fallbacks"
	@echo "  run-real      - Run enhanced VMI inspector (RECOMMENDED)"
	@echo "  solution      - Run complete solution demonstration"
	@echo "  demo          - Run project demonstration"
	@echo "  clean         - Remove build artifacts"
	@echo "  check-deps    - Check if all dependencies are installed"
	@echo "  install-deps  - Install required dependencies"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Usage Examples:"
	@echo "  make all         - Build everything"
	@echo "  make solution    - Run the complete solution (RECOMMENDED)"
	@echo "  make run-real    - Run enhanced VMI inspector"
	@echo "  make demo        - Run full demonstration"
	@echo ""
	@echo "For the complete solution addressing LibVMI Error 2:"
	@echo "  make solution" 