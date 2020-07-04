#ifndef SYMBOLINFO
#define SYMBOLINFO

#include <iostream>
#include <string>
#include <vector>
using namespace std;

class SymbolInfo {
  string name = "";
  string type = "";

  string idType = "";  // FUNCTION, VARIABLE, ARRAY
  string varType = ""; // INT, FLOAT, VOID

  string returnType = ""; // INT, FLOAT, VOID
  bool funcDefined = false;

  int arrSize = 0;
  int arrIndex = 0;

  int defaultInt = -1;
  float defaultFloat = -1.0;

  SymbolInfo *next;

  // asm
  string code = " ";
  string asmVar = "";
  bool isConst = false;
  string funcStart;
  string funcEnd;
  vector<string> asmVarList;
  //

public:
  vector<int> intData;
  vector<float> floatData;

  bool isDummy = false;
  SymbolInfo *real = NULL;

  vector<SymbolInfo *> paramSymList;
  string funcEndLabel = "";
  string arrAsmVar = "";

  void addParam(string name, string type) {
    SymbolInfo *sym = new SymbolInfo(name, "ID");
    sym->setIdType("VARIABLE");
    for (auto &c : type)
      c = toupper(c);
    sym->setVarType(type);
    this->paramSymList.push_back(sym);
  }

  SymbolInfo(string name, string type) {
    this->name = name;
    this->type = type;
    next = NULL;
    fill(intData.begin(), intData.end(), 0);
    fill(floatData.begin(), floatData.end(), 0.0);
    isDummy = false;
  }

  SymbolInfo(const SymbolInfo &symbolInfo) {
    this->name = symbolInfo.name;
    this->type = symbolInfo.type;
    this->arrSize = symbolInfo.arrSize;
    this->arrIndex = symbolInfo.arrIndex;

    this->varType = symbolInfo.varType;
    this->idType = symbolInfo.idType;

    this->funcDefined = symbolInfo.funcDefined;
    this->returnType = symbolInfo.returnType;

    this->intData = symbolInfo.intData;
    this->floatData = symbolInfo.floatData;

    this->isDummy = symbolInfo.isDummy;
    this->real = symbolInfo.real;
    this->code = symbolInfo.code;
    this->asmVar = symbolInfo.asmVar;
    this->isConst = symbolInfo.isConst;
    this->paramSymList = symbolInfo.paramSymList;
  }

  void setFuncDefined(bool funcDefined) { this->funcDefined = funcDefined; }

  void setFuncEnd(string funcEnd) { this->funcEnd = funcEnd; }

  string getFuncEnd() { return this->funcEnd; }

  void setFuncStart(string funcStart) { this->funcStart = funcStart; }

  string getFuncStart() { return this->funcStart; }

  string getCode() { return this->code; }

  void setCode(string code) { this->code = code; }

  void addCode(string code) { this->code += "\n" + code; }

  string getAsmVar() { return this->asmVar; }

  void setAsmVar(string asmVar) { this->asmVar = asmVar; }

  bool getIsConst() { return isConst; }

  void setIsConst(bool isConst) { this->isConst = isConst; }

  bool isFuncDefined() { return funcDefined; }

  string getName() const { return this->name; }

  string getType() const { return this->type; }

  void setName(string name) { this->name = name; }

  void setType(string type) { this->type = type; }

  void setNext(SymbolInfo *next) { this->next = next; }

  SymbolInfo *getNext() { return this->next; }

  ~SymbolInfo() { paramSymList.clear(); }

  // Added for parser

  void setIdType(string idType) { this->idType = idType; }

  string getIdType() { return idType; }

  void setVarType(string vt) { this->varType = vt; }

  string getVarType() { return this->varType; }

  void setArrSize(int size) { arrSize = size; }

  int getArrSize() { return arrSize; }

  bool isFunction() { return idType == "FUNCTION"; }

  bool isVariable() { return idType == "VARIABLE"; }

  bool isArray() { return idType == "ARRAY"; }

  int getIntValue() {
    if (!isDummy) {
      if (intData.size() == 0)
        return defaultInt;
      else if (idType == "VARIABLE")
        return intData[0];
      else if (idType == "ARRAY" && arrIndex < arrSize)
        return intData[arrIndex];
    } else {
      return real->getIntValue();
    }
  }

  void setIntValue(int value) {
    if (!isDummy) {
      if (intData.size() == 0)
        intData.push_back(value);
      else if (idType == "VARIABLE")
        intData[0] = value;
      else if (idType == "ARRAY" || arrIndex < arrSize)
        intData[arrIndex] = value;
    } else {
      real->setIntValue(value);
    }
  }

  float getFloatValue() {
    if (!isDummy) {
      if (floatData.size() == 0)
        return defaultFloat;
      else if (idType == "VARIABLE")
        return floatData[0];
      else if (idType == "ARRAY" && arrIndex < arrSize)
        return floatData[arrIndex];
    } else {
      return real->getFloatValue();
    }
  }

  void setFloatValue(float value) {
    if (!isDummy) {
      if (floatData.size() == 0)
        floatData.push_back(value);
      else if (idType == "VARIABLE")
        floatData[0] = value;
      else if (idType == "ARRAY" || arrIndex < arrSize)
        floatData[arrIndex] = value;
    } else {
      real->setFloatValue(value);
    }
  }

  int getArrIndex() { return arrIndex; }

  void setArrIndex(int arrIndex) {
    if (!isArray()) {
      return;
    }
    this->arrIndex = arrIndex;
  }

  void setReturnType(string ret) { this->returnType = ret; }

  string getReturnType() { return returnType; }

  void setReal(SymbolInfo *actual) {
    isDummy = true;
    this->real = actual;
  }
};

#endif