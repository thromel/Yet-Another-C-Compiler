# Compiler Architecture

This document explains the internal design of the "Yet Another C Compiler" project. It complements `PROJECT_OVERVIEW.md` by describing how each component works and how the pieces fit together.

## Compilation Pipeline

1. **Lexical Analysis** – Flex processes the `.l` files to create a scanner (`lex.yy.c`). This scanner reads characters from the source program and groups them into tokens.
2. **Syntax Analysis** – Bison reads the `.y` grammar to produce a parser (`y.tab.c`). The parser consumes the tokens produced by the scanner and checks that they follow the grammar of the language.
3. **Semantic Analysis** – During parsing, semantic checks are performed. A scope-aware symbol table is updated to ensure that identifiers are declared, types are compatible, and function calls match their definitions.
4. **Intermediate Code Generation** – The final offline phase generates a simple three‑address intermediate representation (IR). Each parser action emits instructions that later phases could use for optimization or code generation.

## Source Layout and Data Flow

The project is organized into four separate directories (`Offline 1` through `Offline 4`). Each directory builds on the previous one:

- `Offline 1` implements the stand‑alone symbol table. It provides the data structures for later phases.
- `Offline 2` adds the scanner using Flex. Tokens are printed to log files for debugging.
- `Offline 3` introduces the Bison parser. Semantic actions manipulate the symbol table and generate detailed logs.
- `Offline 4` extends the parser to output IR code in textual form.

Scripts in each directory generate the scanner and parser, compile the C/C++ sources, and run the tests provided in `Test Cases` or `sample io`.

### Symbol Table

The symbol table uses hash buckets for fast lookup and maintains a stack of scopes. Each entry records type information, storage class, and (for functions) a list of parameters. The parser opens a new scope when it encounters a compound statement or function definition and closes it when leaving that block.

### Error Reporting

Both the scanner and parser produce human‑readable logs. Lexical errors (such as invalid tokens) are reported with line numbers. Semantic errors (such as undeclared identifiers or type mismatches) are also recorded, allowing developers to debug each compilation stage separately.

### Intermediate Representation

The IR generated in `Offline 4` resembles three‑address code. Each instruction is written to a text file along with temporary variable names and labels. Although the project does not include a full optimizer or code generator, this IR is a foundation for future enhancements.

## Example Build Flow

Below is a simplified sequence of commands that illustrates how the components work together:

```bash
# Generate scanner and parser
bison -d -y 1705069.y      # creates y.tab.c and y.tab.h
flex 1705069.l             # creates lex.yy.c

# Compile
g++ -w -c -o y.o y.tab.c
g++ -w -c -o l.o lex.yy.c

# Link and run
g++ y.o l.o -lfl -o compiler.out
./compiler.out sample_input.c
```

During execution, tokens are produced by the scanner, parsed into a syntax tree, checked semantically, and finally emitted as IR instructions.

For additional details on running each phase individually, see `PROJECT_OVERVIEW.md`.
