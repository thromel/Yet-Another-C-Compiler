#!/bin/bash

# Clean build artifacts if they exist
clean_artifacts() {
    echo "Cleaning up previous build artifacts..."
    rm -f *.o lex.yy.c a.out code.asm optimized.asm
}

print_help() {
    echo "Usage: ./script.sh [filename.c]"
    echo "Compiles and runs the C to assembly code translator"
    exit 1
}

# Check if filename is provided
if [ -z "$1" ]; then
    echo "Error: Input file name is not provided"
    print_help
fi

# Clean previous build artifacts
clean_artifacts

# Generate parser from Bison grammar
echo "Generating parser..."
bison -d -y -v ./1705069.y
if [ $? -ne 0 ]; then
    echo 'Error: Bison parser generation failed'
    exit 1
fi
echo 'Successfully generated parser C file and header file'

# Compile the parser
echo "Compiling parser..."
g++ -w -c -o y.o y.tab.c
if [ $? -ne 0 ]; then
    echo 'Error: Parser compilation failed'
    exit 1
fi
echo 'Successfully compiled parser'

# Generate lexer from Flex grammar
echo "Generating lexer..."
flex 1705069.l
if [ $? -ne 0 ]; then
    echo 'Error: Flex scanner generation failed'
    exit 1
fi
echo 'Successfully generated lexer C file'

# Compile the lexer
echo "Compiling lexer..."
g++ -w -c -o l.o lex.yy.c
if [ $? -ne 0 ]; then
    echo 'Error: Scanner compilation failed'
    exit 1
fi
echo 'Successfully compiled lexer'

# Link everything together
echo "Linking objects..."
g++ y.o l.o -lfl -o compiler
if [ $? -ne 0 ]; then
    echo 'Error: Final linking failed'
    exit 1
fi
echo 'Build completed successfully'

# Run the compiler with the provided input file
echo "Running compiler on input file: $1"
./compiler $1

# Check if optimized.asm was generated
if [ -f "optimized.asm" ]; then
    echo "Compilation successful. Generated assembly output in optimized.asm"
else
    echo "Warning: No optimized assembly output was generated"
fi

