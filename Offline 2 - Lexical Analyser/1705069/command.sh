flex -o LexAnalyzer.cpp LexAnalyzer.l
g++ LexAnalyzer.cpp -lfl -o LexAnalyzer.out
./LexAnalyzer.out "input.txt"
