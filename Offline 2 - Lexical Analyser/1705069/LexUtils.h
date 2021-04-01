// #include "./SymbolTable/SymbolTable.h"
#include <fstream>
#include <string>
using namespace std;

// SymbolTable st;
int line_count = 1;
int keyword_count = 0;
int err_count = 0;

ofstream log, token;
ifstream in;

string toUpper(string str){
    for (auto & c: str) c = toupper(c);
    return str;

}

string tokenGenerator(string s){
    return "<" + toUpper(s) + ">";
}

string tokenGenerator(string symbol, string type){
    //TODO : token generator for non-keyword lexemes
    return " ";
}

void printLog(int line, string token, string lexeme)
{
    log << "\nLine no " << line << ": Token "
        << tokenGenerator(token)<<" Lexeme "<<lexeme<<" found";
}

void printToken(string lexeme){
    //Prints token for keywords;
    token<<tokenGenerator(lexeme);
}



void handleKeyword(char *str)
{
    string s(str);
    printLog(line_count,toUpper(s), s);
    printToken(s);
}






void insertToken(string symbol, string type)
{
}