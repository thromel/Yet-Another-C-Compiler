#ifndef PARSERUTILS
#define PARSERUTILS

#include "./SymbolTable/SymbolTable.h"
#include "./SymbolTable/SymbolInfo.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
using namespace std;
ofstream log, error;

extern int line_count;
int error_count = 0;

SymbolTable st(&log);

struct param { 
    public:
    string type;
    string name;
    param(string name, string type){
        this->name = name;
        this->type = type;
    }
    
};

vector<param> paramList;


void yyerror(const char *s)
{
    error << "Error at line no: "<<line_count<<endl;
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

    if (func != NULL){
        printError(funcVal->getName() + " is already declared");
    } else {
        for(param p : paramList){
            funcVal->addParam(p.name, p.type);
            funcVal->set_idType("FUNCTION");
            funcVal->setReturnType(returnType->getName());
            st.insertSymbol(funcVal);
        }
        paramList.clear();
    }
    
}

void addParam (string name, string type)
{
    param p(name, type);
    paramList.push_back(p);
}

void insertArray(SymbolInfo *id, SymbolInfo *size)
{
    if (!insertSymbol(id)){
        printError(id->getName() + " already exists");
        return;
    }
    id->set_idType("ARRAY");
    id->set_arrSize(stoi(size->getName()));

    for(int i = 0; i < id->get_arrSize(); ++i){
        id->intData.push_back(0);
        id->floatData.push_back(0);
    }

}

#endif