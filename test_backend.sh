#!/bin/bash
# Backend Testing Script for YAC Compiler
# Tests end-to-end compilation: C source -> IR -> Assembly -> Executable

set -e  # Exit on error

COMPILER="./build/tools/yac"
TEST_DIR="test/fixtures"
TMP_DIR="/tmp/yac_tests"

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "=== YAC Backend Test Suite ==="
echo ""

# Create temp directory
mkdir -p "$TMP_DIR"

# Test cases: filename, expected_exit_code, description
declare -a tests=(
    "simple.c:42:Basic return value"
    "two_vars.c:30:Multiple variables and arithmetic"
    "simple_loop.c:3:Simple while loop with counter"
    "loop_test.c:45:Loop with accumulation (sum 0-9)"
)

passed=0
failed=0

for test in "${tests[@]}"; do
    IFS=':' read -r file expected desc <<< "$test"
    echo -n "Testing $file: $desc... "

    # Compile to assembly
    if ! $COMPILER -O1 -S "$TEST_DIR/$file" > /dev/null 2>&1; then
        echo -e "${RED}FAILED${NC} (compilation error)"
        ((failed++))
        continue
    fi

    # Assemble and link
    asm_file="$TEST_DIR/${file}.s"
    exe_file="$TMP_DIR/${file%.c}_test"
    if ! clang -arch x86_64 "$asm_file" -o "$exe_file" 2>/dev/null; then
        echo -e "${RED}FAILED${NC} (assembly error)"
        ((failed++))
        continue
    fi

    # Execute and check result
    "$exe_file"
    result=$?

    if [ "$result" -eq "$expected" ]; then
        echo -e "${GREEN}PASSED${NC} (exit code: $result)"
        ((passed++))
    else
        echo -e "${RED}FAILED${NC} (expected $expected, got $result)"
        ((failed++))
    fi
done

echo ""
echo "=== Test Summary ==="
echo "Passed: $passed"
echo "Failed: $failed"
echo "Total:  $((passed + failed))"

# Cleanup
rm -rf "$TMP_DIR"

if [ $failed -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed!${NC}"
    exit 1
fi
