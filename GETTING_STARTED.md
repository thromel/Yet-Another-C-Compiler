# Getting Started with YACC

This guide will help you set up your development environment and get started with the Yet Another C Compiler project.

## Prerequisites

Before you begin, ensure you have the following installed:

- **GCC/G++** (version 7.0+) 
- **Flex** (version 2.6+)
- **Bison** (version 3.0+)
- **Make** (version 4.0+)
- **Docker** (optional, for containerized development)

## Setting Up the Development Environment

### Option 1: Local Setup

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd Yet-Another-C-Compiler
   ```

2. Build the compiler:
   ```bash
   cd "Offline 4 - Intermediate Code Generator/1705069"
   make
   ```

3. Run a test compilation:
   ```bash
   make run
   ```

### Option 2: Docker Setup

1. Build the Docker image:
   ```bash
   cd "Offline 4 - Intermediate Code Generator/1705069"
   docker build -t yacc .
   ```

2. Run the compiler using Docker:
   ```bash
   docker run -v $(pwd):/app yacc input.c
   ```

## Project Directory Structure

- **include/**: Header files
  - **CompilerGlobals.h**: Global definitions
  - **AsmUtils.h**: Assembly code utilities
  - **LexUtils.h**: Lexical analysis utilities

- **src/**: Implementation files
  - **AsmUtils.cpp**: Assembly utilities implementation
  - **LexUtils.cpp**: Lexical utilities implementation
  - **Globals.cpp**: Global variables initialization
  - **main.cpp**: Main entry point

- **SymbolTable/**: Symbol table implementation
  - **SymbolInfo.h**: Symbol information
  - **ScopeTable.h**: Scope management
  - **SymbolTable.h**: Symbol table

- **AsmLibraries/**: Assembly libraries
  - **outdec.h**: Assembly output routines

- **Test Cases/**: Test input files

## Development Workflow

1. **Understanding the Code**: Start by reviewing the README.md for a high-level overview, then study the codebase structure.

2. **Making Changes**: When modifying the code, follow these steps:
   - Update header files in `include/`
   - Implement functionality in `src/`
   - Update the grammar in `1705069.y` for parser changes
   - Update the token definitions in `1705069.l` for lexer changes

3. **Building and Testing**:
   - Use `make` to build the project
   - Use `make run` to test with the default input file
   - Use `make test` to run all test cases

4. **Debugging**:
   - Check `log.txt` for compilation logs
   - Examine `code.asm` for the unoptimized assembly output
   - Examine `optimized.asm` for the optimized assembly output

## Common Tasks

### Adding a New Feature

1. Identify which component needs modification (lexer, parser, code generator)
2. Update the corresponding files:
   - Lexer feature: `1705069.l` and `include/LexUtils.h`
   - Parser feature: `1705069.y`
   - Code generation: `include/AsmUtils.h` and `src/AsmUtils.cpp`

3. Add test cases in `Test Cases/`

### Fixing a Bug

1. Reproduce the issue using a test case
2. Identify the component causing the issue
3. Fix the issue in the corresponding file
4. Verify the fix with the test case

## Contributing Guidelines

1. **Code Style**: Follow the existing code style
2. **Documentation**: Add comments to explain complex logic
3. **Testing**: Add test cases for new features or bug fixes
4. **Commits**: Write clear commit messages

## Resources

- [Flex Manual](https://westes.github.io/flex/manual/)
- [Bison Manual](https://www.gnu.org/software/bison/manual/)
- [8086 Assembly Language Reference](https://en.wikipedia.org/wiki/X86_assembly_language)

## Troubleshooting

### Common Issues

1. **Flex/Bison errors**:
   - Check the syntax in your .l and .y files
   - Verify that tokens are properly defined

2. **Compilation errors**:
   - Check for missing includes
   - Verify function signatures match between declarations and definitions

3. **Runtime errors**:
   - Check file paths and permissions
   - Verify that input files are properly formatted

### Getting Help

If you encounter issues not covered in this guide, please:
1. Check the documentation
2. Look at existing issues in the repository
3. Contact the project maintainers

Happy coding! 