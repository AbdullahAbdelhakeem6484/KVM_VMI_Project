echo -e "${BLUE}=== COMPILED VMI PROGRAMS ===${NC}"
ls -la build/vmi_*inspector* 2>/dev/null | while read -r line; do
    echo -e "${GREEN}✓${NC} $line"
done

# Attempt to run the VMI inspector with timeout
echo -e "${YELLOW}Running VMI Inspector...${NC}"
if [ -f "clean_vmi_project/build/vmi_complete_inspector" ]; then
    timeout 10s sudo ./clean_vmi_project/build/vmi_complete_inspector win10-vmi 2>&1 | head -20
elif [ -f "build/vmi_complete_inspector" ]; then
    timeout 10s sudo ./build/vmi_complete_inspector win10-vmi 2>&1 | head -20
else
    echo "VMI inspector not found in expected locations"
fi 