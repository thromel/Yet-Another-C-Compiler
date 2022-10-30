#ifndef ASMUTILS
#define ASMUTILS

#include "SymbolTable/SymbolInfo.h"
#include <algorithm>
#include <fstream>
#include <regex>
#include <stack>
#include <string>

extern vector<string> asmVarList;
extern vector<pair<string, int>> asmArrList;
extern ofstream code, optimized, log;
using namespace std;
int label_count = 0;

string prevMovLhs = "", prevMovRhs = "";
string BXval = "";
string currentLabel = "";

class VarManager {
  int size = 0;

  stack<string> free;

public:
  string getTempVar() {
    string tempVar;
    if (free.empty()) {
      tempVar = "temp" + to_string(size);
      size++;
      asmVarList.push_back(tempVar);
    } else {
      tempVar = free.top();
      free.pop();
    }
    return tempVar;
  }

  void freeTempVar(string tempVar) {
    if (tempVar.substr(0, 4) == "temp") {
      free.push(tempVar);
      // cout<<"freed " + tempVar<<endl;
    }
  }

  int getSize() { return size; }
};

inline void printCode(SymbolInfo *sym) { code << sym->getCode() << endl; }

inline string newLabel() { return "L" + to_string(label_count++); }

inline void addDataSegment() {
  code << ".MODEL MEDIUM \n.STACK 100H \n.DATA" << endl << endl;

  asmVarList.push_back("return_loc");
  for (string s : asmVarList) {
    code << s << " DW ?" << endl;
  }

  for (auto p : asmArrList) {
    code << p.first << " DW " << p.second << " DUP (?)" << endl;
  }
}

inline void startCodeSegment() { code << endl << ".CODE" << endl; }

inline void endCodeSegment() { code << endl << "END MAIN\n"; }

inline string memToMem(SymbolInfo *lhs, SymbolInfo *rhs) {
  string asmCode = "MOV AX, ";
  asmCode += rhs->getAsmVar() + "\n";
  asmCode += "MOV " + lhs->getAsmVar() + ", AX \n";
  return asmCode;
}

inline string constToMem(SymbolInfo *lhs, SymbolInfo *constVal) {
  string asmCode = "MOV AX, ";
  asmCode += constVal->getName() + "\n";
  asmCode += "MOV " + lhs->getAsmVar() + ", AX \n";
  return asmCode;
}

inline void addPrintFunc() {
  ifstream srcFile("./AsmLibraries/outdec.h");
  string line;
  while (std::getline(srcFile, line, '.'))
    code << line << '\n';
  srcFile.close();
}

inline vector<string> split(const string &str, const string &delim) {
  vector<string> tokens;
  size_t prev = 0, pos = 0;
  do {
    pos = str.find(delim, prev);
    if (pos == string::npos)
      pos = str.length();
    string token = str.substr(prev, pos - prev);
    if (!token.empty())
      tokens.push_back(token);
    prev = pos + delim.length();
  } while (pos < str.length() && prev < str.length());
  return tokens;
}

inline bool optimize_mov(string inst) {
  inst.erase(std::remove(inst.begin(), inst.end(), ','), inst.end());
  vector<string> tokens = split(inst, " ");
  if (tokens.size() == 3 && tokens[0] == "MOV") {
    // cout << "MOV operation: " << tokens[1] << " " << tokens[2] << endl;

    string movLhs = tokens[1];
    string movRhs = tokens[2];

    if (movLhs == prevMovRhs && movRhs == prevMovLhs) {
      return true;
    } else if (movLhs == "BX") {
      BXval = movRhs;
    } else if (movLhs == movRhs) {
      return true;
    }

    prevMovLhs = movLhs;
    prevMovRhs = movRhs;
  }
  return false;
}

inline bool optimize_arithmetic(string inst) {
  inst.erase(std::remove(inst.begin(), inst.end(), ','), inst.end());
  vector<string> tokens = split(inst, " ");

  if (tokens.size() == 3 && (tokens[0] == "ADD" || tokens[0] == "SUB") &&
      tokens[2] == "0") {
    return true;
  } else if (tokens.size() == 2 &&
             (tokens[0] == "IMUL" || tokens[0] == "IDIV") && BXval == "1") {
    return true;
  }

  return false;
}

inline void optimize() {
  ifstream srcFile("code.asm");
  string line;
  int line_count = 0, line_removed = 0;
  vector<string> lines;
  while (std::getline(srcFile, line, '\n')) {
    lines.push_back(line);
  }

  log << "-------------------------------------" << endl;
  log << "Optimizer log: " << endl;

  for (string s : lines) {
    line_count++;

    if (s.substr(0, 1) == "L") {
      // Clears prevMovLhs and prevMovRhs when entering a new Label so that MOV
      // optimization doesn't happen

      prevMovLhs = "";
      prevMovRhs = "";
    }

    if (s == " ") {
      log << "Removed blank line : " << line_count << endl;
      line_removed++;
    } else if (optimize_mov(s)) {
      log << "Optimized redundant MOV operation: " << line_count << endl;
      line_removed++;
    } else if (optimize_arithmetic(s)) {
      log << "Optimized redundant arithmetic operation : " << line_count
          << endl;
      line_removed++;
    } else {
      optimized << s << endl;
    }
  }
  log << "Line removed:" << line_removed << endl;
  log << "-------------------------------------" << endl;
  optimized << "END MAIN" << endl;
  srcFile.close();
}

#endif