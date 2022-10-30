#ifndef COMPILER_GLOBALS_H
#define COMPILER_GLOBALS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <stack>
#include <regex>

// Global variables declaration
extern int line_count;
extern int error_count;
extern int lex_err_count;
extern std::ofstream log;
extern std::ofstream code;
extern std::ofstream optimized;
extern std::vector<std::string> asmVarList;
extern std::vector<std::pair<std::string, int>> asmArrList;

// Define compiler version
#define COMPILER_VERSION "1.0.0"
#define COMPILER_NAME "YACC - Yet Another C Compiler"

// Common utility functions can be added here

#endif // COMPILER_GLOBALS_H 