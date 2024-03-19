
---
**Title:** Development of a Compiler for a Subset of the C Language

**Duration:** January 2021 - July 2021

**Technologies Used:** Flex (Fast Lexical Analyzer Generator), Bison (GNU Parser Generator), C/C++, 8086 Assembly

**Project Overview:**
This project was part of the Compiler Sessional course at Bangladesh University of Engineering & Technology, where we aimed to build a compiler for a subset of the C language. The project was divided into four main assignments, each tackling a crucial component of compiler development: Symbol Table Implementation, Lexical Analysis, Syntax and Semantic Analysis, and Intermediate Code Generation.

**Achievements:**

1. **Symbol Table Implementation (Assignment 1):** Developed a dynamic, scope-aware symbol table using hash tables. This involved creating data structures for storing identifiers, type information, and scope level, crucial for supporting nested scopes in C programs.

2. **Lexical Analysis (Assignment 2):** Implemented a lexical analyzer using Flex. The analyzer converts a source program into a stream of tokens, identifying keywords, constants, identifiers, and handling errors such as unrecognized symbols and malformed numbers.

3. **Syntax and Semantic Analysis (Assignment 3):** Utilized Bison to construct a parser that performs syntax and semantic analysis. The parser validates the source code's grammar and checks for semantic errors like type inconsistencies and undeclared variables, ensuring the correct use of variables and function calls.

4. **Intermediate Code Generation (Assignment 4):** Extended the compiler to generate 8086 assembly language code from the analyzed source code. Implemented optimizations such as peephole to enhance the efficiency of the generated code. This step also included generating assembly code for conditional statements, loops, and function calls, and handling function call stack management.

**Challenges Overcome:**

- Designing and integrating a flexible symbol table to support complex scoping rules.
- Ensuring accurate error detection and reporting throughout lexical and semantic analysis.
- Generating optimized intermediate code that runs efficiently on the EMU8086 emulator.

**Project Impact:**
This comprehensive project not only solidified my understanding of compiler design principles but also honed my skills in using compiler construction tools like Flex and Bison. Through practical implementation, I gained insights into the intricacies of lexical, syntax, semantic analysis, and code optimization, laying a strong foundation for future projects in compiler design and development.

---

This summary captures the essence of the compiler project while highlighting your role and the technologies used. Adjust the details based on your specific contributions and learnings to personalize your LinkedIn profile.

## Asignment 1 - SymbolTable

Relevant commands are described in the specs file.

How to run:
```bash
g++ main.cpp
./main.out filename.txt
```

## Assignment 2 - Lexical Analyser

How to run:

```bash
chmod +x command.sh
./command.sh filename.txt
```

## Assignment 3 - Parser

How to run:

```bash
chmod +x script.sh
./script.sh filename.txt
```

##### log.txt will contain relevant parser logs
##### error.txt will contain syntax and semantic errors if any

## Assignment 4 - Intermediate Code Generator (.c to 8086 asm)

```
chmod +x script.sh
./script.sh filename.c
```

##### log.txt will contain relevant parser logs
##### error.txt will contain syntax and semantic errors if any
##### code.asm will contain assembly code
##### optimized-code.asm will contain optimized assembly code 

###
