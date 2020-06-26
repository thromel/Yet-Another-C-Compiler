#ifndef ASMUTILS
#define ASMUTILS

#include "SymbolTable/SymbolInfo.h"
#include <fstream>
#include <stack>
#include <string>
extern vector<string> asmVarList;
extern vector<pair<string, int>> asmArrList;
extern ofstream code;
using namespace std;
int label_count = 0;

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

#endif