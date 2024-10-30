# YAC - Yet Another C Compiler

A modern, educational C compiler implementing a complete multi-pass compilation pipeline from C source code to intermediate representation. Built with clean architecture, comprehensive type checking, and industry-standard practices.

[![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Build System](https://img.shields.io/badge/Build-CMake-green.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

## Features

🎯 **Complete Compiler Frontend**
- Hand-written lexer with full C subset support
- Recursive descent parser with operator precedence
- Comprehensive semantic analysis and type checking
- Clean intermediate representation (three-address code)
- Professional diagnostic system with source locations

⚡ **Advanced Optimization Pipeline** *(NEW)*
- **SSA Construction**: Mem2Reg pass with proper phi node insertion
- **Copy Propagation**: Eliminates redundant move operations
- **Constant Propagation**: Compile-time constant evaluation with folding
- **Dead Code Elimination**: Removes unused instructions
- **SimplifyCFG**: Control flow graph optimization
- **IR Verification**: Comprehensive correctness checking
- **Multiple Optimization Levels**: -O0, -O1, -O2, -O3

🏗️ **Modern Architecture**
- Modular design with clear separation of concerns
- Visitor pattern for AST traversal
- Type-safe symbol tables with scope management
- Smart pointer-based memory management
- CMake-based build system
- Pass manager infrastructure with analysis caching

📚 **Educational Value**
- Clean, readable codebase (~5000 lines)
- Comprehensive comments and documentation
- Step-by-step compilation phases
- Pretty-printed IR for learning
- Working SSA-form transformations

## Quick Start

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, or MSVC 2017+)
- CMake 3.15 or higher

### Building

```bash
# Clone the repository
git clone https://github.com/thromel/Yet-Another-C-Compiler.git
cd Yet-Another-C-Compiler

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make -j4

# Run the compiler
./tools/yac ../test/fixtures/simple.c
```

### Quick Test

```bash
# Basic compilation (no optimization)
./tools/yac ../test/fixtures/simple.c

# With optimizations
./tools/yac -O1 ../test/fixtures/simple.c          # Basic optimizations
./tools/yac -O2 ../test/fixtures/const_test.c      # Aggressive optimizations
./tools/yac -O3 ../test/fixtures/loop_test.c       # Maximum optimizations

# Show optimized IR
./tools/yac -O1 --dump-ir ../test/fixtures/loop_test.c

# Verify IR correctness
./tools/yac -O1 --verify ../test/fixtures/loop_test.c

# Show control flow graph
./tools/yac -O1 --dump-cfg ../test/fixtures/loop_test.c

# Run test suite
../test_optimizer.sh
```

### Optimization Example

Input (loop_test.c):
```c
int main() {
    int sum = 0;
    int i = 0;
    while (i < 10) {
        sum = sum + i;
        i = i + 1;
    }
    return sum;
}
```

Output with `-O1`:
```
function main() -> int {
entry:
  br while_cond0
while_cond0:
  %phi_sum = phi [%t6, while_body1], [0, entry]
  %phi_i = phi [%t8, while_body1], [0, entry]
  %t3 = lt %phi_i, 10
  br %t3, while_body1, while_end2
while_body1:
  %t6 = add %phi_sum, %phi_i  // sum = sum + i
  %t8 = add %phi_i, 1          // i = i + 1
  br while_cond0
while_end2:
  ret %phi_sum
}
```

✨ **Result**: Converted to perfect SSA form with phi nodes, eliminated all memory operations!

## Project Structure

```
Yet-Another-C-Compiler/
├── include/yac/          # Public headers
│   ├── AST/              # Abstract syntax tree definitions
│   │   ├── AST.h         # AST node hierarchy (20+ node types)
│   │   └── ASTVisitor.h  # Visitor pattern for AST traversal
│   ├── Basic/            # Core utilities
│   │   ├── Diagnostic.h  # Error/warning reporting
│   │   └── SourceLocation.h # Source position tracking
│   ├── CodeGen/          # Code generation
│   │   ├── IR.h          # Intermediate representation
│   │   └── IRBuilder.h   # AST to IR converter
│   ├── Parse/            # Lexing and parsing
│   │   ├── Lexer.h       # Hand-written lexer
│   │   ├── Parser.h      # Recursive descent parser
│   │   └── Token.h       # Token definitions
│   ├── Sema/             # Semantic analysis
│   │   ├── Sema.h        # Type checking and validation
│   │   └── SymbolTable.h # Symbol table with scopes
│   └── Type/             # Type system
│       └── Type.h        # Type hierarchy and context
├── lib/                  # Implementation files
│   ├── AST/              # AST implementation
│   ├── Basic/            # Diagnostics implementation
│   ├── CodeGen/          # IR generation
│   ├── Parse/            # Lexer and parser
│   ├── Sema/             # Semantic analysis
│   └── Type/             # Type system
├── tools/yac/            # Compiler driver
│   └── main.cpp          # Main entry point
├── test/                 # Test suite
│   ├── fixtures/         # Test programs
│   └── unit/             # Unit tests (optional)
├── CMakeLists.txt        # Root build configuration
└── README.md             # This file
```

## Language Support

YAC supports a practical subset of C:

### Data Types
- **Primitives**: `int`, `float`, `char`, `void`
- **Pointers**: `int*`, `char*`, etc.
- **Arrays**: `int arr[10]`, multi-dimensional arrays
- **Functions**: Function types with parameters

### Declarations
```c
int x;                    // Variable declaration
int y = 42;              // With initialization
int arr[10];             // Array
int* ptr;                // Pointer
int add(int a, int b);   // Function declaration
```

### Control Flow
```c
// Conditionals
if (condition) { ... }
if (condition) { ... } else { ... }

// Loops
while (condition) { ... }
for (init; condition; increment) { ... }
do { ... } while (condition);

// Jumps
return expression;
break;
continue;
```

### Expressions
```c
// Arithmetic
x + y, x - y, x * y, x / y, x % y

// Relational
x < y, x <= y, x > y, x >= y, x == y, x != y

// Logical
x && y, x || y, !x

// Bitwise
x & y, x | y, x ^ y, ~x, x << y, x >> y

// Assignment
x = y, x += y, x -= y, x *= y, x /= y

// Increment/Decrement
++x, --x, x++, x--

// Function Calls
func(arg1, arg2)

// Array Access
arr[index]

// Pointer Operations
&x, *ptr
```

### Functions
```c
int factorial(int n) {
    if (n <= 1)
        return 1;
    return n * factorial(n - 1);
}

int main() {
    int result = factorial(5);
    return result;
}
```

## Compilation Pipeline

YAC implements a classic multi-pass compiler architecture:

### Phase 1: Lexical Analysis
**Input**: Source code
**Output**: Token stream
**Component**: Hand-written lexer (`Lexer.cpp`)

Tokenizes the source code into a sequence of tokens:
- Keywords: `int`, `if`, `while`, `return`, etc.
- Identifiers: variable and function names
- Literals: integers, floats, characters, strings
- Operators and punctuation: `+`, `==`, `{`, `;`, etc.

```
int x = 42;
↓
[int] [x] [=] [42] [;]
```

### Phase 2: Syntax Analysis
**Input**: Token stream
**Output**: Abstract Syntax Tree (AST)
**Component**: Recursive descent parser (`Parser.cpp`)

Parses tokens into a tree structure representing the program's syntax:
- Declarations: functions, variables, parameters
- Statements: if/else, loops, returns, expressions
- Expressions: binary ops, unary ops, calls, subscripts

```c
int x = 42;
↓
VarDecl: x : int
  Init:
    IntegerLiteral: 42
```

### Phase 3: Semantic Analysis
**Input**: AST
**Output**: Type-checked AST + symbol table
**Component**: Semantic analyzer (`Sema.cpp`)

Performs comprehensive validation:
- **Type checking**: Ensures type safety
- **Symbol resolution**: Looks up variable and function references
- **Scope management**: Handles nested scopes
- **Error detection**: Reports semantic errors

Checks include:
- Variable declarations before use
- Type compatibility in assignments and operations
- Function call argument types and counts
- Return type matching
- Break/continue only in loops

### Phase 4: IR Generation
**Input**: Type-checked AST
**Output**: Intermediate representation (IR)
**Component**: IR builder (`IRBuilder.cpp`)

Generates three-address code (TAC) IR:
- Temporaries for intermediate results
- Basic blocks for control flow
- Explicit labels for jumps
- Stack-based calling convention

```c
int x = 1 + 2 * 3;
↓
%t0 = mul 2, 3
%t1 = add 1, %t0
%t2 = alloca int
store %t1, %t2
```

## Example Compilation

**Input (test/fixtures/simple.c):**
```c
int main() {
    int x = 42;
    return x;
}
```

**Compilation Output:**
```
YAC Compiler v0.2.0
Compiling: test/fixtures/simple.c

--- Lexical Analysis ---
Generated 15 tokens
✓ Lexical analysis: OK

--- Parsing ---
✓ Parsing successful!

--- Semantic Analysis ---
✓ Semantic analysis successful!

--- IR Generation ---
✓ IR generation successful!

--- Intermediate Representation ---
=== IR Module ===

function main() -> int {
entry:
  %t0 = alloca int
  store 42, %t0
  %t1 = load %t0
  ret %t1
}

--- Compilation Summary ---
✓ Lexical analysis: OK
✓ Syntax analysis: OK
✓ Semantic analysis: OK
✓ IR generation: OK
  - Declarations: 1
  - Functions: 1
```

## More Complex Example

**Input (test/fixtures/control_flow.c):**
```c
int factorial(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

int main() {
    int result = factorial(5);
    return result;
}
```

**Generated IR:**
```
function factorial(%n: int) -> int {
entry:
  %t0 = alloca int
  store %n, %t0
  %t0 = load %t0
  %t1 = le %t0, 1
  br %t1, then0, else1
then0:
  ret 1
  br endif2
else1:
  %t2 = load %t0
  %t3 = load %t0
  %t4 = sub %t3, 1
  %t5 = call factorial(%t4)
  %t6 = mul %t2, %t5
  ret %t6
  br endif2
endif2:
}

function main() -> int {
entry:
  %t0 = alloca int
  %t1 = call factorial(5)
  store %t1, %t0
  %t2 = load %t0
  ret %t2
}
```

## Error Detection

YAC provides comprehensive error detection with helpful messages:

**Input (semantic error):**
```c
int main() {
    int x = "hello";  // Type error!
    return x;
}
```

**Output:**
```
Semantic analysis failed:
test.c:2:13: error: Incompatible types in assignment:
cannot convert 'char*' to 'int'

1 error generated.
```

## Architecture Highlights

### AST Design
- 20+ node types organized in a clear hierarchy
- RTTI (Run-Time Type Information) for safe downcasting
- Source location tracking for all nodes
- Type information attached to expressions

### Type System
- Canonical types managed by `TypeContext`
- Type compatibility checking
- Support for primitives, pointers, arrays, and functions
- Explicit type conversions

### Symbol Tables
- Scope-based symbol resolution
- Separate storage for variables, parameters, and functions
- Redeclaration detection
- Hierarchical scope chains

### IR Design
- Three-address code format
- SSA-like temporary management
- Explicit control flow with basic blocks
- Easy to analyze and optimize

### Error Handling
- Clang-style diagnostic system
- Source location tracking
- Color-coded output
- Error recovery in parser

## Building and Testing

### Build Options

```bash
# Default build
cmake ..

# Debug build with symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build with optimizations
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build with verbose output
make VERBOSE=1
```

### Running Tests

```bash
# Simple test
./tools/yac ../test/fixtures/simple.c

# All test cases
for file in ../test/fixtures/*.c; do
    echo "Testing $file"
    ./tools/yac "$file"
done
```

### Command-Line Options

```bash
# Show help
./tools/yac --help

# Dump AST
./tools/yac --dump-ast input.c

# Check syntax only
./tools/yac -fsyntax-only input.c

# Specify output file
./tools/yac -o output.ir input.c
```

## Code Statistics

| Component | Files | Lines |
|-----------|-------|-------|
| AST | 2 | ~600 |
| Parser | 3 | ~900 |
| Semantic Analysis | 2 | ~700 |
| IR Generation | 2 | ~800 |
| Type System | 1 | ~200 |
| Diagnostics | 1 | ~150 |
| **Total** | **11** | **~3350** |

## Development

### Adding a New Feature

1. **Add to AST**: Define new node type in `AST.h`
2. **Add to Parser**: Parse new syntax in `Parser.cpp`
3. **Add to Sema**: Type check in `Sema.cpp`
4. **Add to IRBuilder**: Generate IR in `IRBuilder.cpp`
5. **Add Tests**: Create test cases in `test/fixtures/`

### Code Style

- C++17 standard
- Descriptive variable and function names
- Comments for complex logic
- Visitor pattern for extensibility
- Smart pointers for memory safety

## Limitations

Current limitations (future work):

- **Arrays**: Basic support, no multi-dimensional
- **Strings**: String literals not fully implemented
- **Structs**: Not yet supported
- **Floating Point**: Treated as integers in IR
- **Preprocessor**: No `#include`, `#define`, etc.
- **Standard Library**: No standard library functions
- **Backend**: IR generation only, no assembly output yet

## Future Enhancements

Potential additions:

1. **Backend**: x86-64 or 8086 assembly generation
2. **Optimizations**: Constant folding, dead code elimination
3. **More Types**: Structs, unions, enums
4. **Preprocessor**: Basic `#include` and `#define`
5. **Standard Library**: Subset of libc functions
6. **Debugger Support**: DWARF debug information
7. **JIT**: LLVM backend for JIT compilation

## Contributing

Contributions welcome! Areas to help:

- **Tests**: Add more test cases
- **Documentation**: Improve comments and docs
- **Features**: Implement new language features
- **Optimizations**: Add IR optimization passes
- **Backend**: Implement assembly code generation

## Resources

### Learning Compiler Design
- [**Crafting Interpreters**](https://craftinginterpreters.com/) - Excellent introduction
- [**Engineering a Compiler**](https://www.elsevier.com/books/engineering-a-compiler/cooper/978-0-12-088478-0) - Comprehensive textbook
- [**LLVM Tutorial**](https://llvm.org/docs/tutorial/) - Building compilers with LLVM

### Related Projects
- [**Clang**](https://clang.llvm.org/) - Production C compiler
- [**TinyCC**](https://bellard.org/tcc/) - Small C compiler
- [**8cc**](https://github.com/rui314/8cc) - Educational C compiler

## License

MIT License - see LICENSE file for details

## Author

**Romel**
Department of Computer Science and Engineering
Bangladesh University of Engineering and Technology

## Acknowledgments

This project demonstrates modern compiler construction techniques, drawing inspiration from:
- Clang's diagnostic system
- LLVM's IR design
- Academic compiler textbooks
- Industry best practices

Built as an educational project to learn and teach compiler design principles.
