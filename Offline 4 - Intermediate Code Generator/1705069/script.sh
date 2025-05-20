#!/bin/bash

# rm -f *.o *.c *.hh y.tab.h *.out *.output

helpFunction()
{
    echo "Usage: ./script.sh filename.c"
    exit 1 # Exit script after printing help
}

if [ -z "$1" ]
then
    echo "File name is not given";
    helpFunction
fi

bison -d -y -v ./17./sc05069.y
if [ $? -ne 0 ]; then
    echo 'Error: Bison parser generation failed'
    exit 1
fi
echo 'Generated the parser C file as well the header file'

g++ -w -c -o y.o y.tab.c
if [ $? -ne 0 ]; then
    echo 'Error: Parser compilation failed'
    exit 1
fi
echo 'Generated the parser object file'

flex 1705069.l
if [ $? -ne 0 ]; then
    echo 'Error: Flex scanner generation failed'
    exit 1
fi
echo 'Generated the scanner C file'

g++ -w -c -o l.o lex.yy.c
if [ $? -ne 0 ]; then
    echo 'Error: Scanner compilation failed'
    exit 1
fi
echo 'Generated the scanner object file'

g++ y.o l.o -lfl
if [ $? -ne 0 ]; then
    echo 'Error: Final linking failed'
    exit 1
fi
echo 'All ready, running'
./a.out $1

