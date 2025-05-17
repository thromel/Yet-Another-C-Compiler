# Project Documentation

## Overview

This repository contains a teaching compiler that demonstrates the fundamental phases of a C compiler. The project is divided into four "offline" assignments that gradually extend the functionality from a simple symbol table to an intermediate code generator.

The source code is written in C and C++ and makes heavy use of Flex and Bison. Shell scripts are provided in each offline directory to automate the build process.

## Directory Layout

```
.
├── Offline 1 - Symbol Table/
│   └── SymbolTable/       # Standalone symbol table implementation
├── Offline 2 - Lexical Analyser/
│   ├── 1705069/           # Flex scanner and support files
│   └── Test Cases/        # Example input programs
├── Offline 3 - Syntax & Semantic Analyser/
│   └── 1705069/           # Bison grammar and parser utilities
└── Offline 4 - Intermediate Code Generator/
    └── 1705069/           # Intermediate code generation
```

Each assignment folder contains its own scripts, inputs and outputs. Below is a brief summary of what can be found in each step.

## Offline 1 – Symbol Table

This assignment implements a scope-aware symbol table. The `SymbolTable` directory contains C++ headers and a small driver program (`main.cpp`). To build and run the symbol table tester:

```bash
cd "Offline 1 - Symbol Table/SymbolTable"
g++ -std=c++11 -o main main.cpp
./main input0.txt > output.txt
```

The input format and output produced demonstrate how scopes are entered and exited while identifiers are stored in hash buckets.

## Offline 2 – Lexical Analyser

The lexical analyser is implemented using Flex. Navigate to the `1705069` directory and run the provided script:

```bash
cd "Offline 2 - Lexical Analyser/1705069"
chmod +x command.sh
./command.sh input.txt
```

This generates `lex.yy.c`, compiles it using `g++`, and executes the scanner on the provided input file. Token streams and log files for various test cases are available in the `Test Cases` folder.

## Offline 3 – Syntax & Semantic Analyser

This stage adds a Bison grammar and semantics. The `1705069` directory again contains a script to automate the build process:

```bash
cd "Offline 3 - Syntax & Semantic Analyser/1705069"
chmod +x script.sh
./script.sh input.txt
```

The parser performs syntax checking, manages nested scopes and produces detailed log output describing the discovered tokens and semantic information. Errors are written to `error.txt`.

## Offline 4 – Intermediate Code Generator

The final step extends the parser to produce a simple three-address style intermediate representation. To build and run:

```bash
cd "Offline 4 - Intermediate Code Generator/1705069"
chmod +x script.sh
./script.sh input.c
```

Intermediate code is written to files in the directory and the output can be compared against the sample results under `Sample Output`.

## Building Tools Manually

While the shell scripts are the recommended way to build, you can compile the generated files manually. The commands are very similar across Offline 2, 3 and 4:

```bash
bison -d -y 1705069.y        # generate parser
flex 1705069.l               # generate scanner
g++ -w -c -o y.o y.tab.c
g++ -w -c -o l.o lex.yy.c
g++ y.o l.o -lfl -o a.out    # link
./a.out <input-file>
```

Replace `<input-file>` with an appropriate test program for the phase you are running.

## Additional Resources

* `sample io/` – various example programs and their expected token/log outputs.
* PDF specifications – each offline directory includes the original assignment PDF that describes the requirements in detail.
* [ARCHITECTURE.md](ARCHITECTURE.md) – detailed explanation of the compiler pipeline and internal data structures.

For more information about the background and goals of this project, see the main [README](../README.md).

