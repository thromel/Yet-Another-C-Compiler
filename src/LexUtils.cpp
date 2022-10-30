#include "../include/LexUtils.h"
#include <iostream>
#include <algorithm>

// Global variables
int line_count = 1;
int keyword_count = 0;
int lex_err_count = 0;

void handle_keyword(char *str) {
    std::string s(str);
    printLog(line_count, toUpper(s), s);
    keyword_count++;
}

void handle_const_int(char *str) {
    std::string s(str);
    assignSymbol("CONST_INT");
}

void handle_const_float(char *str) {
    std::string s(str);
    assignSymbol("CONST_FLOAT");
}

void handle_const_char(char *str, std::string type) {
    std::string s(str);

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

void handle_operator(char *str, std::string opr) {
    std::string s(str);
    assignSymbol(s, "OPERATOR");
}

void handle_id(char *str) {
    std::string s(str);
    assignSymbol("ID");
}

void handle_error(char *str, std::string msg) {
    std::string s(str);
    log << "\n"
        << "Lexical Error at line no: " << line_count << ". " << msg << ": " << s << std::endl;
    lex_err_count++;

    size_t n = std::count(s.begin(), s.end(), '\n');
    line_count += (int)n;
}

void handle_comment(char *str) {
    std::string s(str);
    size_t n = std::count(s.begin(), s.end(), '\n');
    line_count += (int)n;
} 