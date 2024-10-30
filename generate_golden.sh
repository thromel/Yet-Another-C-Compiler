#!/bin/bash
# Generate Golden IR Test Files
# Extracts IR output from compiler for regression testing

set -e

COMPILER="./build/tools/yac"
FIXTURES_DIR="test/fixtures"
GOLDEN_DIR="test/golden"

echo "=== Generating Golden IR Files ==="
echo ""

# Test files to generate golden outputs for
test_files=(
    "simple.c"
    "two_vars.c"
    "simple_loop.c"
    "loop_test.c"
)

# Optimization levels
opt_levels=("O0" "O1" "O2" "O3")

for file in "${test_files[@]}"; do
    basename="${file%.c}"
    echo "Generating golden files for $file..."

    for opt in "${opt_levels[@]}"; do
        opt_flag="-${opt}"
        golden_file="$GOLDEN_DIR/$opt/${basename}.ir"

        # Run compiler and extract just the IR section
        $COMPILER $opt_flag "$FIXTURES_DIR/$file" 2>&1 | \
            sed -n '/^=== IR Module ===/,/^---/p' | \
            sed '$ d' > "$golden_file"

        echo "  ✓ $opt/${basename}.ir ($(wc -l < "$golden_file" | tr -d ' ') lines)"
    done
done

echo ""
echo "Golden files generated successfully!"
echo "Location: $GOLDEN_DIR/"
