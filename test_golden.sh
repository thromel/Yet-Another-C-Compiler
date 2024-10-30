#!/bin/bash
# Golden IR Test Suite
# Compares current IR output against golden reference files

set -e

COMPILER="./build/tools/yac"
FIXTURES_DIR="test/fixtures"
GOLDEN_DIR="test/golden"
TMP_DIR="/tmp/yac_golden_tests"

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo "=== YAC Golden IR Test Suite ==="
echo ""

mkdir -p "$TMP_DIR"

# Test files
test_files=(
    "simple.c"
    "two_vars.c"
    "simple_loop.c"
    "loop_test.c"
)

# Optimization levels
opt_levels=("O0" "O1" "O2" "O3")

passed=0
failed=0
total=0

for file in "${test_files[@]}"; do
    basename="${file%.c}"

    for opt in "${opt_levels[@]}"; do
        ((total++))
        opt_flag="-${opt}"
        golden_file="$GOLDEN_DIR/$opt/${basename}.ir"
        tmp_file="$TMP_DIR/${basename}_${opt}.ir"

        # Extract current IR
        $COMPILER $opt_flag "$FIXTURES_DIR/$file" 2>&1 | \
            sed -n '/^=== IR Module ===/,/^---/p' | \
            sed '$ d' > "$tmp_file"

        # Compare with golden
        if diff -q "$golden_file" "$tmp_file" > /dev/null 2>&1; then
            echo -e "  ${GREEN}✓${NC} $opt/$basename"
            ((passed++))
        else
            echo -e "  ${RED}✗${NC} $opt/$basename"
            ((failed++))

            # Show diff for debugging
            if [ "$VERBOSE" = "1" ]; then
                echo -e "${YELLOW}Diff:${NC}"
                diff -u "$golden_file" "$tmp_file" || true
                echo ""
            fi
        fi
    done
done

# Cleanup
rm -rf "$TMP_DIR"

echo ""
echo "=== Test Summary ==="
echo "Passed: $passed/$total"
echo "Failed: $failed/$total"

if [ $failed -eq 0 ]; then
    echo -e "${GREEN}All golden tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some golden tests failed!${NC}"
    echo "Run with VERBOSE=1 to see diffs"
    exit 1
fi
