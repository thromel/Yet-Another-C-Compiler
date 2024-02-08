# thromel – Yet Another C Compiler

## This project is a complete compiler for a subset of the C language. It is organized into several offline assignments that demonstrate the major phases of compiler construction:
	
-	Symbol Table Management
-	Lexical Analysis
-	Syntax and Semantic Analysis

### The compiler uses industry-standard tools—Flex for lexical analysis and Bison for parsing—and is written in C/C++. It includes detailed error handling, scope‐aware symbol table management, and support for common C constructs.

## Project Structure

```thromel-yet-another-c-compiler/
├── Offline 1 - Symbol Table/
│   └── [Symbol table implementation and tests]
├── Offline 2 - Lexical Analyser/
│   ├── 1705069/
│   │   ├── LexAnalyzer.l          # Flex source file for lexical analysis
│   │   ├── LexUtils.h             # Utility functions for the scanner
│   │   └── ...                    # Additional files (e.g. command.sh)
│   ├── Lex Resources/
│   │   └── [Documentation and sample lex files]
│   └── Test Cases/
│       ├── error.txt              # Input designed to generate lexical errors
│       ├── no_error.txt           # Valid input files
│       └── cases/                 # Additional test case files (with corresponding logs/tokens)
├── Offline 3 - Syntax & Semantic Analyser/
│   ├── 1705069/
│   │   ├── 1705069.l              # Flex file used by the parser
│   │   ├── 1705069.y              # Bison grammar file (syntax/semantic rules)
│   │   ├── LexUtils.h             # Shared utilities for parser actions
│   │   ├── ParserUtils.h          # Helper functions for error reporting, symbol handling, etc.
│   │   ├── script.sh              # Shell script to build and run the parser
│   │   ├── log.txt                # Generated log file for parsing
│   │   └── error.txt              # Errors found during parsing
│   └── Test Cases/
│       ├── input.txt              # Sample input for syntax analysis
│       ├── log.txt                # Parser log output
│       └── ...                    # Additional test cases and sample outputs
└── sample io/
    ├── sample_input1.txt          # Example source files for demonstration
    ├── sample_log1.txt            # Corresponding log outputs
    ├── sample_token1.txt          # Token stream output
    └── ...                        # More sample files
```

### Tools and Technologies
	
-	Flex: Used to generate the lexical analyzer (scanner) from .l files.
-	Bison: Used to generate the parser from .y files.
-	C/C++: Implementation language for the compiler’s components.
-	GNU Compiler Collection (g++): Used for compiling the generated C/C++ code.
-	Unix Shell Scripting: Scripts (e.g., command.sh, script.sh) automate the build and run processes.

### How to Build and Run

#### Building the Lexical Analyzer
	
 1.	Navigate to the Lexical Analyser directory:

```bash
cd "Offline 2 - Lexical Analyser/1705069"
```
2.	Build and run using the provided shell script:
Make sure the script is executable:

```bash
chmod +x command.sh
```

Then run it with a test file:

```bash
./command.sh <input_file>
```

#### This script invokes Flex to generate lex.yy.c, compiles it with g++, and executes the resulting scanner.

Building the Syntax & Semantic Analyzer
	1.	Navigate to the Syntax & Semantic Analyser directory:

```bash
cd "Offline 3 - Syntax & Semantic Analyser/1705069"
```

	
 2.	Run the provided shell script to build the parser:
Make sure the script is executable:

```bash
chmod +x script.sh
```

Then run it with an input file:

```bash
./script.sh <input_file>
```

This script performs the following steps:
	-	Uses Bison to process the grammar file (1705069.y), generating y.tab.c and y.tab.h.
	-	Compiles the generated parser and the scanner (lex.yy.c) into object files.
	-	Links the object files with the Flex library (-lfl) to create the final executable (a.out).
	-	Executes the final executable with the provided input file.

### Manual Compilation (Optional)

You can compile the generated files manually if needed:

#### Generate parser files using Bison
```bash
bison -d -y -v 1705069.y
```

#### Compile the parser
```bash
g++ -w -c -o y.o y.tab.c
```

#### Generate scanner files using Flex
```bash
flex 1705069.l
```

#### Compile the scanner
```bash
g++ -w -c -o l.o lex.yy.c
```

#### Link the object files to produce the executable
```bash
g++ y.o l.o -lfl -o compiler.out
```

#### Run the compiler with an input source file
```bash
./compiler.out <source_file.c>
```

## Testing

### The project includes several test cases:
	
-	Test Cases (Offline 2 – Lexical Analyser):
-	error.txt / error_log.txt / error_token.txt: Files with input examples that cause lexical errors (e.g., unterminated strings, invalid number formats).
-	no_error.txt / no_error_log.txt / no_error_token.txt: Files with valid input demonstrating correct tokenization.
-	Cases directory: Additional input files (1.txt, 2.txt, 3.txt, etc.) along with corresponding log and token output.
-	Sample I/O (sample io/ directory):
-	Sample source files and their expected log and token outputs.
-	These files demonstrate various language constructs (variable declarations, expressions, control structures, comments, strings, etc.).
-	Test Cases (Offline 3 – Syntax & Semantic Analyser):
-	input.txt provides a sample C-like program.
-	log.txt and error.txt contain the results of syntax and semantic analysis, including scope and symbol table outputs.

#### Run the shell scripts to build and run the tests. The output log files and token files will help you verify the correctness of the compiler’s phases.

### Features
	
- Lexical Analysis: The scanner recognizes keywords, identifiers, constants (integers, floats, characters, strings), operators, comments (single- and multi-line), and handles errors (e.g., unterminated literals).
-	Syntax & Semantic Analysis: The parser validates the syntax and performs semantic checks (e.g., type compatibility, proper array indexing, function declaration vs. definition matching) while building and updating a scope-aware symbol table.
-	Symbol Table Management: A robust symbol table supports multiple scopes and tracks variable and function declarations, types, and parameters.
-	Error and Warning Reporting: The system produces detailed logs with line numbers and error messages for both lexical and semantic issues.
-	Scope Handling: Nested scopes are managed during parsing (e.g., in compound statements), and the symbol table reflects scope changes.

### Future Enhancements
	
-	Intermediate Code Generation: Extend the compiler to generate an intermediate representation (IR) for further optimizations.
-	Target Code Generation: Implement a backend to generate assembly or machine code for a target architecture.
-	Optimization Passes: Add optimization phases to improve the performance of the generated code.
-	IDE Integration: Create tools or plugins to integrate the compiler with an IDE for improved debugging and development.

Credits

This compiler project was developed as part of the [CSE 309: Compilers] course. It leverages Flex and Bison, both open-source tools provided under the GNU Public License. Special thanks to [Dr. Muhammad Mashroor Ali] and all project contributors.

License

This project is licensed under the MIT License. See the LICENSE file for details.

Feel free to modify or expand upon this documentation to suit your project’s specifics and additional features.
