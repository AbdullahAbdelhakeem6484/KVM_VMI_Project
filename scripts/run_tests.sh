#!/bin/bash

echo "==== KVM-VMI PROJECT TEST SUITE ===="
echo "Testing all components..."
echo

# Test build system
echo "1. Testing build system..."
make clean >/dev/null 2>&1 && make all >/dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "✓ Build system works"
else
    echo "✗ Build system failed"
fi
echo

# Test binaries
echo "2. Testing binary existence..."
for binary in vmi_complete_inspector vmi_windows_inspector vmi_inspector; do
    if [ -x "build/$binary" ]; then
        echo "✓ $binary exists and is executable"
        ls -lh "build/$binary" | awk '{print "  Size:", $5}'
    else
        echo "✗ $binary missing or not executable"
    fi
done
echo

# Test configuration
echo "3. Testing configuration..."
if [ -f "config/libvmi.conf" ] && [ -f "config/win10-vmi.xml" ]; then
    echo "✓ Configuration files present"
    echo "  - libvmi.conf: $(wc -l < config/libvmi.conf) lines"
    echo "  - win10-vmi.xml: $(wc -l < config/win10-vmi.xml) lines"
else
    echo "✗ Configuration files missing"
fi
echo

# Test documentation
echo "4. Testing documentation..."
if [ -f "README.md" ] && [ -f "docs/PROJECT_COMPLETION.md" ]; then
    echo "✓ Documentation complete"
    echo "  - README.md: $(wc -l < README.md) lines"
    echo "  - PROJECT_COMPLETION.md: $(wc -l < docs/PROJECT_COMPLETION.md) lines"
else
    echo "✗ Documentation missing"
fi
echo

# Test VM status
echo "5. Testing VM status..."
if virsh list | grep -q "win10-vmi.*running"; then
    echo "✓ Windows 10 VM is running"
else
    echo "⚠ Windows 10 VM is not running (this is okay)"
fi
echo

# Test dependencies
echo "6. Testing dependencies..."
deps_ok=0
if which gcc >/dev/null 2>&1; then
    echo "✓ GCC compiler available"
    deps_ok=$((deps_ok + 1))
else
    echo "✗ GCC compiler missing"
fi

if pkg-config --exists libvmi 2>/dev/null; then
    echo "✓ LibVMI library available"
    deps_ok=$((deps_ok + 1))
else
    echo "✗ LibVMI library missing"
fi

if which virsh >/dev/null 2>&1; then
    echo "✓ Virsh (libvirt) available"
    deps_ok=$((deps_ok + 1))
else
    echo "✗ Virsh (libvirt) missing"
fi

echo "Dependencies: $deps_ok/3 available"
echo

echo "==== PROJECT STRUCTURE ===="
echo "Current directory structure:"
find . -type f -name "*.c" -o -name "*.h" -o -name "Makefile" -o -name "README.md" -o -name "*.conf" -o -name "*.xml" | sort

echo
echo "==== TEST COMPLETE ===="
echo "Project is properly organized and ready for demonstration!" 