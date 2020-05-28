#ifndef LEXUTILS_H
#define LEXUTILS_H

#include <fstream>
#include <string>
#include <algorithm>
#include "./SymbolTable/SymbolInfo.h"
#include "y.tab.h"
using namespace std;

int line_count = 1;
int keyword_count = 0;
int lex_err_count = 0;

extern ofstream log;
extern char *yytext;
extern YYSTYPE yylval;
//Utility functions

string toUpper(string str)
{
    for (auto &c : str)
        c = toupper(c);
    return str;
}

string replaceAll(string str, const string &from, const string &to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

string replaceFirst(string &source, string const &find, string const &replace)
{
    int i = static_cast<int>(source.find(find, 0));
    if (i != string::npos)
        source.replace(static_cast<unsigned long>(i), find.length(), replace);
    return source;
}

// not checked
string replaceLast(string &source, string const &find, string const &replace)
{
    int i = static_cast<int>(source.rfind(find));
    if (i != string::npos)
        source.replace(static_cast<unsigned long>(i), find.length(), replace);
    return source;
}

string tokenGenerator(string s){
    return "<" + toUpper(s) + ">";
}

string tokenGenerator(string type, string symbol){
    return "<" + type + "," + symbol + ">";
}

void printLog(int line, string token, string lexeme)
{
    log << "Line no " << line << ": Token "
        << tokenGenerator(token)<<" Lexeme "<<lexeme<<" found\n";
}


//added for syntax analyzer

void assignSymbol(string name, string type)
{
    yylval.symbol = new SymbolInfo(name, type);
}

void assignSymbol(string type)
{
    assignSymbol((string)yytext, type);
}



//------------------------------------------------------------------

void handle_keyword(char *str)
{
    string s(str);
    printLog(line_count, toUpper(s), s);
    keyword_count++;
}

void handle_const_int(char *str)
{
    string s(str);
    // printLog(line_count, "CONST_INT", s);
    assignSymbol("CONST_INT");
}

void handle_const_float(char *str)
{
    string s(str);
    // printLog(line_count, "CONST_FLOAT", s);
    assignSymbol("CONST_FLOAT");
}

void handle_const_char(char *str, string type)
{
    string s(str);

    // printLog(line_count, type, s);

    size_t n = std::count(s.begin(), s.end(), '\n');
    line_count += (int)n;

    s = replaceFirst(s, "\'", "");
    s = replaceLast(s, "\'", "");

    s = replaceFirst(s, "\"", "");
    s = replaceLast(s, "\"", "");

    s = replaceAll(s, "\\\"", "\"");
    s = replaceAll(s, "\\\n", "\t");
    s = replaceAll(s, "\\n", "\n");
    s = replaceAll(s, "\\t", "\t");
    s = replaceAll(s, "\\r", "\r");
    s = replaceAll(s, "\\0", "\n");
    s = replaceAll(s, "\\a", "\a");
    s = replaceAll(s, "\\b", "\b");
    s = replaceAll(s, "\\f", "\f");
    s = replaceAll(s, "\\v", "\v");

    assignSymbol(s, type);

}

void handle_operator(char *str, string opr)
{
    string s(str);

    assignSymbol(s, "OPERATOR");
}

void handle_id(char *str)
{
    string s(str);
    assignSymbol("ID");
    // printLog(line_count, "ID", s);
}

void handle_error(char *str, string msg)
{
    string s(str);
    log << "\n"
        << "Lexical Error at line no: " << line_count << ". " << msg << ": " << s << endl;
    lex_err_count++;

    size_t n = std::count(s.begin(), s.end(), '\n');
    line_count += (int)n;
}

void handle_comment(char *str)
{
    string s(str);
    // printLog(line_count, "COMMENT", s);
    size_t n = std::count(s.begin(), s.end(), '\n');
    line_count += (int)n;
}



#endif