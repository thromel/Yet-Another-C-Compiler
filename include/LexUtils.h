#ifndef LEX_UTILS_H
#define LEX_UTILS_H

#include "CompilerGlobals.h"
#include "SymbolTable/SymbolInfo.h"
#include <fstream>
#include <string>
#include <algorithm>

// Forward declarations
extern int yylex();
extern char *yytext;
extern YYSTYPE yylval;

/**
 * Converts a string to uppercase
 * 
 * @param str The string to convert
 * @return The uppercase version of the string
 */
std::string toUpper(std::string str) {
    for (auto &c : str)
        c = toupper(c);
    return str;
}

/**
 * Replaces all occurrences of a substring with another
 * 
 * @param str The original string
 * @param from The substring to replace
 * @param to The replacement substring
 * @return The modified string
 */
std::string replaceAll(std::string str, const std::string &from, const std::string &to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

/**
 * Replaces the first occurrence of a substring with another
 * 
 * @param source The original string
 * @param find The substring to replace
 * @param replace The replacement substring
 * @return The modified string
 */
std::string replaceFirst(std::string &source, std::string const &find, std::string const &replace) {
    int i = static_cast<int>(source.find(find, 0));
    if (i != std::string::npos)
        source.replace(static_cast<unsigned long>(i), find.length(), replace);
    return source;
}

/**
 * Replaces the last occurrence of a substring with another
 * 
 * @param source The original string
 * @param find The substring to replace
 * @param replace The replacement substring
 * @return The modified string
 */
std::string replaceLast(std::string &source, std::string const &find, std::string const &replace) {
    int i = static_cast<int>(source.rfind(find));
    if (i != std::string::npos)
        source.replace(static_cast<unsigned long>(i), find.length(), replace);
    return source;
}

/**
 * Generates a token string format
 * 
 * @param s The token name
 * @return The formatted token string
 */
std::string tokenGenerator(std::string s) {
    return "<" + toUpper(s) + ">";
}

/**
 * Generates a token string format with type and symbol
 * 
 * @param type The token type
 * @param symbol The token symbol
 * @return The formatted token string
 */
std::string tokenGenerator(std::string type, std::string symbol) {
    return "<" + type + "," + symbol + ">";
}

/**
 * Logs a token to the log file
 * 
 * @param line The line number
 * @param token The token type
 * @param lexeme The lexeme
 */
void printLog(int line, std::string token, std::string lexeme) {
    log << "Line no " << line << ": Token "
        << tokenGenerator(token) << " Lexeme " << lexeme << " found\n";
}

/**
 * Assigns the current lexeme to the symbol attribute
 * 
 * @param name The symbol name
 * @param type The symbol type
 */
void assignSymbol(std::string name, std::string type) {
    yylval.symbol = new SymbolInfo(name, type);
}

/**
 * Assigns the current lexeme to the symbol attribute with default name
 * 
 * @param type The symbol type
 */
void assignSymbol(std::string type) {
    assignSymbol(yytext, type);
}

/**
 * Handles a keyword token
 * 
 * @param str The keyword string
 */
void handle_keyword(char *str);

/**
 * Handles a constant integer token
 * 
 * @param str The integer string
 */
void handle_const_int(char *str);

/**
 * Handles a constant float token
 * 
 * @param str The float string
 */
void handle_const_float(char *str);

/**
 * Handles a constant character token
 * 
 * @param str The character string
 * @param type The token type
 */
void handle_const_char(char *str, std::string type);

/**
 * Handles an operator token
 * 
 * @param str The operator string
 * @param opr The operator type
 */
void handle_operator(char *str, std::string opr);

/**
 * Handles an identifier token
 * 
 * @param str The identifier string
 */
void handle_id(char *str);

/**
 * Handles an error in the lexical analysis
 * 
 * @param str The erroneous string
 * @param msg The error message
 */
void handle_error(char *str, std::string msg);

/**
 * Handles a comment token
 * 
 * @param str The comment string
 */
void handle_comment(char *str);

#endif // LEX_UTILS_H 