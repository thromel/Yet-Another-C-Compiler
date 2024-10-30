#!/bin/bash

# Test script for YAC optimizer
# Tests various optimization levels on all test fixtures

set -e  # Exit on error

YAC="./build/tools/yac"
FIXTURES="test/fixtures"

echo "========================================"
echo "YAC Optimization Pipeline Test Suite"
echo "========================================"
echo

# Color codes for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

test_count=0
pass_count=0
fail_count=0

# Test a single file with verification
test_file() {
    local file=$1
    local opt_level=$2
    local test_name="$(basename $file) -O$opt_level"

    test_count=$((test_count + 1))

    echo -n "Testing $test_name... "

    if $YAC -O$opt_level --verify "$file" > /dev/null 2>&1; then
        echo -e "${GREEN}✓ PASS${NC}"
        pass_count=$((pass_count + 1))
    else
        echo -e "${RED}✗ FAIL${NC}"
        fail_count=$((fail_count + 1))
        # Show error details
        $YAC -O$opt_level --verify "$file" 2>&1 | tail -10
    fi
}

# Test all fixtures at all optimization levels
for file in $FIXTURES/*.c; do
    if [ -f "$file" ]; then
        for opt_level in 0 1 2 3; do
            test_file "$file" $opt_level
        done
        echo
    fi
done

echo "========================================"
echo "Test Results"
echo "========================================"
echo "Total tests: $test_count"
echo -e "Passed: ${GREEN}$pass_count${NC}"
echo -e "Failed: ${RED}$fail_count${NC}"
echo

if [ $fail_count -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed.${NC}"
    exit 1
fi
