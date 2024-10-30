# Golden IR Test Files

This directory contains "golden" reference IR outputs for regression testing. Golden tests ensure that optimizations and IR generation remain consistent across code changes.

## Structure

```
test/golden/
├── O0/          # No optimization (-O0)
├── O1/          # Basic optimization (-O1)
├── O2/          # Advanced optimization (-O2)
├── O3/          # Aggressive optimization (-O3)
└── README.md
```

Each optimization level directory contains `.ir` files corresponding to test fixtures in `test/fixtures/`.

## Test Files

- **simple.ir** - Basic return value (42)
- **two_vars.ir** - Multiple variables and arithmetic
- **simple_loop.ir** - Simple while loop with counter
- **loop_test.ir** - Loop with accumulation (sum 0-9)

## Usage

### Running Golden Tests

```bash
./test_golden.sh
```

This compares current compiler output against golden files. All tests should pass for a clean build.

### Viewing Diff Details

```bash
VERBOSE=1 ./test_golden.sh
```

Shows detailed diffs when tests fail.

### Regenerating Golden Files

**⚠️ Only regenerate when IR format changes are intentional!**

```bash
./generate_golden.sh
```

This overwrites all golden files with current compiler output.

## When to Regenerate

Regenerate golden files when:
- ✅ You intentionally change IR format (e.g., new instruction syntax)
- ✅ You improve an optimization pass (expected behavior change)
- ✅ You fix a bug that changes correct IR output

**DO NOT regenerate to make failing tests pass!** Investigate failures first.

## Test Coverage

Current coverage: **16 tests** (4 files × 4 optimization levels)

### Optimization Level Differences

**-O0 (No optimization)**
- Raw IR from IRBuilder
- Stack-based (allocas, loads, stores)
- No SSA form
- Most instructions

**-O1 (Basic)**
- Mem2Reg: Promotes allocas to SSA registers
- SimplifyCFG: Cleans up control flow
- ConstProp, CopyProp, DCE
- ~30-40% fewer instructions

**-O2 (Advanced)**
- All -O1 passes
- SCCP: Sparse conditional constant propagation
- GVN: Global value numbering (CSE)
- LICM: Loop invariant code motion
- Multiple rounds
- ~40-50% fewer instructions

**-O3 (Aggressive)**
- All -O2 passes
- Additional optimization rounds
- Maximum code quality
- Similar to -O2 for current test suite

## Integration with CI

To integrate with continuous integration:

```bash
# In CI script
./test_golden.sh || exit 1
./test_backend.sh || exit 1
```

## Example Output

```
=== YAC Golden IR Test Suite ===

  ✓ O0/simple
  ✓ O1/simple
  ✓ O2/simple
  ✓ O3/simple
  ...

=== Test Summary ===
Passed: 16/16
Failed: 0/16
All golden tests passed!
```

## Maintenance

- Add new test fixtures to both `generate_golden.sh` and `test_golden.sh`
- Keep golden files in version control (git)
- Document any intentional IR format changes in commit messages
- Review diffs carefully before regenerating

## Technical Details

**IR Extraction:** Scripts use `sed` to extract the IR section between markers:
```
=== IR Module ===
...
---
```

**Comparison:** Uses `diff -q` for fast comparison, `diff -u` for detailed output.

**Cleanup:** Temporary files stored in `/tmp/yac_golden_tests/`, cleaned automatically.
