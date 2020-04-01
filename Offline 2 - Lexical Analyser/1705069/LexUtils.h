// #include "./SymbolTable/SymbolTable.h"
#include <fstream>
#include <string>
// #include <iostream>
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

string tokenGenerator(string type, string symbol){
    return "<" + type + "," + symbol + ">";
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

void printToken(string tkn, string lexeme){
    token<<tokenGenerator(tkn, lexeme);
}

//------------------------------------------------------------------

void handle_Keyword(char *str)
{
    string s(str);
    printLog(line_count,toUpper(s), s);
    printToken(s);
}

void handle_CONST_INT(char *str){
    string s(str);
    printLog(line_count, "CONST_INT", s);
    printToken("CONST_INT", s);
}

void handle_CONST_FLOAT(char *str){
    string s(str);
    printLog(line_count, "CONST_FLOAT", s);
    printToken("CONST_FLOAT", s);
}

void handle_CONST_CHAR(char *str){
    string s(str);
    string refined = "";

    for(int i = 1; i < s.length() - 1; ++i){
        refined += s[i];
    }

    printLog(line_count, "CONST_CHAR", refined);
    printToken("CONST_CHAR", refined);
}

//-------------------------------------------------------------------




void insertToken(string symbol, string type)
{
}