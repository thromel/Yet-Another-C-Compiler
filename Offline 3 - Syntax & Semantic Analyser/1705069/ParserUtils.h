#ifndef PARSERUTILS
#define PARSERUTILS

#include "./SymbolTable/SymbolTable.h"
#include "./SymbolTable/SymbolInfo.h"
#include <fstream>
#include <iostream>
using namespace std;
ofstream log, error;

extern int line_count;
int error_count = 0;

SymbolTable st(&log);

void yyerror(const char *s)
{
    log << "Error at line no: "<<line_count<<endl;
}

void printRule(string rule)
{
    log << "At line no: " << line_count <<" " << rule << "\n"  << endl;
}

void printSymbol(SymbolInfo *sym)
{
    log << sym->getName()<<endl<<endl;
}

void printSymbol(SymbolInfo sym1, SymbolInfo sym2){
}

void printLog(string msg)
{
    log << msg << endl;
}

void printError(string msg)
{
    error << "Error at line no: " << line_count << msg << endl;
    error_count++;
}

bool insertSymbol(SymbolInfo *sym)
{
    return st.insertSymbol(sym->getName(), sym->getType());
}

void enterScope(){
    st.enterScope();
    st.printAll();
}

void exitScope()
{
    st.exitScope();
    st.printAll();
}

SymbolInfo* getVariable(SymbolInfo* sym)
{
    SymbolInfo *var = st.lookup(sym->getName());

    if (var == NULL){
        printError(sym->getName() + " doesn't exist");
    } else {
        return var;
    }
}

void addFuncDef (SymbolInfo *funcVal, SymbolInfo *returnType)
{
    SymbolInfo *func = st.lookup(funcVal->getName());

    log << "TODO : Function definiton";
}

#endif