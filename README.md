# Yet Another C Compiler (YACC)

A complete compiler that translates a subset of C language into optimized 8086 assembly code. Features lexical analysis, parsing, semantic checking, code generation, and peephole optimization.

## Overview

This compiler implements a full compilation pipeline from C source code to assembly:

1. **Lexical Analysis**: Tokenizes source code using Flex
2. **Syntax Analysis**: Parses tokens and builds an abstract syntax tree using Bison
3. **Semantic Analysis**: Performs type checking and scope management
4. **Code Generation**: Generates 8086 assembly instructions
5. **Optimization**: Applies peephole optimizations for improved efficiency

## Quick Start

### Using Docker

```bash
# Build the Docker image
docker build -t yacc .

# Run the compiler
docker run -v $(pwd):/app yacc test.c
```

### Using Make

```bash
# Build the compiler
make

# Compile a C file
./compiler "Test Cases/loop.c"

# Run tests
make test

# Clean build artifacts
make clean
```

### Using the Build Script

```bash
# Build and run
./script.sh "Test Cases/recursion.c"
```

## Project Structure

```
├── 1705069.l           # Flex lexical analyzer specification
├── 1705069.y           # Bison parser grammar
├── AsmLibraries/       # Assembly runtime libraries
│   └── outdec.h        # Decimal output procedures
├── SymbolTable/        # Hash-based symbol table
│   ├── ScopeTable.h    # Hierarchical scope management
│   ├── SymbolInfo.h    # Symbol metadata storage
│   └── SymbolTable.h   # Main symbol table interface
├── include/            # Compiler header files
│   ├── AsmUtils.h      # Assembly generation utilities
│   ├── CompilerGlobals.h # Global definitions
│   └── LexUtils.h      # Lexical analysis helpers
├── src/                # Implementation files
│   ├── AsmUtils.cpp    # Assembly code generation
│   ├── Globals.cpp     # Global variable initialization
│   ├── LexUtils.cpp    # Lexical utilities
│   └── main.cpp        # Compiler entry point
├── Test Cases/         # Comprehensive test suite
│   ├── loop.c          # Loop constructs
│   ├── func.c          # Function calls
│   ├── recursion.c     # Recursive functions
│   └── exp.c           # Expression evaluation
├── docs/               # Documentation
│   ├── ARCHITECTURE.md # Compiler architecture overview
│   ├── PROJECT_OVERVIEW.md # Development guide
│   └── deep-dive.md    # Technical deep dive
├── Dockerfile          # Container configuration
├── Makefile            # Build system
├── script.sh           # Quick build script
└── GETTING_STARTED.md  # Development setup guide
```

## Language Support

The compiler supports a subset of C including:

### Data Types
- `int`, `float`, `char`, `void`
- Single and multi-dimensional arrays

### Control Flow
- `if-else` statements
- `while`, `for`, `do-while` loops
- Function calls (including recursion)
- `return` statements

### Expressions
- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Relational: `<`, `<=`, `>`, `>=`, `==`, `!=`
- Logical: `&&`, `||`, `!`
- Bitwise: `&`, `|`, `^`, `~`, `<<`, `>>`
- Assignment operators
- Increment/decrement: `++`, `--`

### Functions
- Declaration and definition
- Parameter passing
- Return values
- Recursive calls

## Assembly Code Generation

The compiler generates 8086 assembly with:

- **Data Segment**: Variable and array declarations
- **Code Segment**: Translated instructions
- **Function Prologues/Epilogues**: Stack frame management
- **Register Allocation**: Efficient use of AX, BX, CX, DX
- **Memory Management**: Temporary variables for complex expressions

## Optimization

The peephole optimizer eliminates:

1. **Redundant MOV instructions**: `MOV AX, BX` followed by `MOV BX, AX`
2. **Zero-operation elimination**: `ADD AX, 0`, `SUB AX, 0`, `IMUL AX, 0`
3. **Identity operations**: `IMUL AX, 1`, `IDIV AX, 1`
4. **Unnecessary operations**: Operations with no effect on result

## Example

**Input (recursion.c):**
```c
int factorial(int n) {
    if (n <= 1)
        return 1;
    return n * factorial(n - 1);
}

int main() {
    int result;
    result = factorial(5);
    printf(result);
    return 0;
}
```

**Output (optimized assembly):**
```assembly
.MODEL MEDIUM
.STACK 100H
.DATA
    temp0 DW ?
    temp1 DW ?

.CODE
factorial PROC
    PUSH BP
    MOV BP, SP

    MOV AX, [BP+4]    ; Load n
    CMP AX, 1
    JG L1

    MOV AX, 1         ; return 1
    JMP L2

L1:
    MOV AX, [BP+4]
    SUB AX, 1
    PUSH AX
    CALL factorial
    ADD SP, 2

    MOV BX, AX
    MOV AX, [BP+4]
    IMUL BX           ; n * factorial(n-1)

L2:
    POP BP
    RET
factorial ENDP

main PROC
    MOV AX, @DATA
    MOV DS, AX

    PUSH 5
    CALL factorial
    ADD SP, 2
    MOV result, AX

    CALL print_output

    MOV AX, 4CH
    INT 21H
main ENDP
END main
```

## Output Files

Running the compiler generates:

- `log.txt` - Parsing log with symbol table dumps
- `error.txt` - Compilation errors and warnings
- `code.asm` - Generated assembly code
- `optimized.asm` - Optimized assembly code

## Documentation

- **[ARCHITECTURE.md](docs/ARCHITECTURE.md)** - Multi-pass compilation pipeline
- **[PROJECT_OVERVIEW.md](docs/PROJECT_OVERVIEW.md)** - Step-by-step development walkthrough
- **[deep-dive.md](docs/deep-dive.md)** - Technical implementation details
- **[GETTING_STARTED.md](GETTING_STARTED.md)** - Development environment setup

## Building from Source

### Prerequisites
- Flex (lexical analyzer generator)
- Bison (parser generator)
- GCC/G++ compiler
- Make

### Build Steps
```bash
# Generate lexer
flex 1705069.l

# Generate parser
bison -d -y -v 1705069.y

# Compile
g++ -o compiler y.tab.c lex.yy.c src/*.cpp -lfl

# Run
./compiler input.c
```

## Testing

The `Test Cases/` directory contains comprehensive tests:

```bash
# Test all cases
make test

# Test individual cases
./compiler "Test Cases/loop.c"
./compiler "Test Cases/func.c"
./compiler "Test Cases/recursion.c"
./compiler "Test Cases/exp.c"
```

## Technical Details

### Symbol Table
- Hash table with separate chaining (SDBMHash function)
- Hierarchical scope management
- Type information tracking
- Function signature validation

### Lexer
- Regular expression-based token recognition
- Error recovery for malformed tokens
- Line and column tracking
- Comment handling (single and multi-line)

### Parser
- LR(1) parsing with Bison
- Error recovery and reporting
- Attribute-based semantic actions
- AST construction during parsing

### Code Generator
- Three-address code intermediate representation
- Register allocation with spilling to temporaries
- Label generation for control flow
- Function calling convention (stack-based)

### Optimizer
- Single-pass peephole optimization
- Pattern matching for redundant instructions
- Algebraic simplification
- Constant folding

## License

MIT License - See LICENSE file for details

## Author

**Romel**
Department of Computer Science and Engineering
Bangladesh University of Engineering and Technology

## Acknowledgments

Built with Flex and Bison, leveraging decades of compiler construction research and best practices.
