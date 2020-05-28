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

string type = "";

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
    error << "Error at line no: " << line_count << " - "<<msg << endl;
    error_count++;
}

void printWarning(string msg)
{
    error << "Warning at line no: " << line_count <<" " << msg << endl;
}

bool insertSymbol(SymbolInfo *sym)
{
    return st.insertSymbol(sym);
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

SymbolInfo* nullSym()
{
    SymbolInfo *ns = new SymbolInfo("NULL", "NULL");
    ns->setVarType("INT");
    ns->setIdType("VARIABLE");
    ns->intData.push_back(0);
    ns->floatData.push_back(0);

    return ns;
}

SymbolInfo* getVariable(SymbolInfo* sym)
{
    
    SymbolInfo *var = st.lookup(sym->getName());

    if (var == NULL){
        printError(sym->getName() + " doesn't exist");
    } else if (!var->isVariable())
    {
        if(var->isArray()){
            printError(var->getName() + " is an array and must be accessed with an index");
        } else {
            printError(var->getName() + " is not a variable");
        }
    } 
    else {
        return var;
    }
    return nullSym();
}


void addFuncDef (SymbolInfo *funcVal, SymbolInfo *returnType)
{
    SymbolInfo *func = st.lookup(funcVal->getName());

    if (func != NULL){
        printError(funcVal->getName() + " is already declared");
        paramList.clear();
        return;
    } else {
        for(param p : paramList){
            funcVal->addParam(p.name, p.type);
        }
        funcVal->setIdType("FUNCTION");
        funcVal->setReturnType(returnType->getName());
        st.insertSymbol(funcVal);
        paramList.clear();
    }
    
}

SymbolInfo *insertVar(SymbolInfo *var)
{   
    st.printAll();
    if (st.lookup(var->getName())){
        printError(var->getName() + " already exists inside the scope");
    } else {
        var->setVarType(type);
        var->setIdType("VARIABLE");
        st.insertSymbol(var);
        return var;
    }
    return nullSym();
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
    id->setIdType("ARRAY");
    id->setArrSize(stoi(size->getName()));

    for(int i = 0; i < id->getArrSize(); ++i){
        id->intData.push_back(0);
        id->floatData.push_back(0);
    }

}

SymbolInfo* getConstVal(SymbolInfo *val, string varType)
{   

    SymbolInfo *const_val = new SymbolInfo(val->getName(), val->getType());
    
    const_val->setIdType("VARIABLE");
    const_val->setVarType(varType);

    if (varType == "FLOAT")
    {
        const_val->setFloatValue(stof(val->getName()));
    } else if (varType == "INT")
    {
        const_val->setIntValue(stoi(val->getName()));
    }

    return const_val;
}

SymbolInfo* getArrayIndexVar(SymbolInfo *arr, SymbolInfo *index)
{
    SymbolInfo *arrIdxVar = st.lookup(arr->getName());
    SymbolInfo *var;

    if (arrIdxVar = NULL)
    {
        printError(arr->getName() + " doesn't exist");
        return nullSym();
    }
    else
    {
        if (!arrIdxVar->isArray())
        {
            printError(arrIdxVar->getName() + " is not an array");
        }
        else if (index->getVarType() != "INT")
        {
            printError(arrIdxVar->getName() + " array index must be an integer");
        }
         else {
             var = new SymbolInfo(*arrIdxVar);
             var->setArrIndex(index->getIntValue());
        }

    }
    return var;
}

SymbolInfo* handle_assign(SymbolInfo *sym1, SymbolInfo *sym2)
{
    SymbolInfo *result;

    if(sym1->getVarType() == "VOID" || sym2->getVarType() == "VOID")
    {
        printError("Operand of void type");
        return nullSym();
    }

    else if (sym2->getVarType() == "INT")
    {
        if (sym1->getVarType() == "FLOAT") 
        {
            printWarning("Assigning integer to a float variable");
            sym1->setFloatValue(sym2->getIntValue());
            result = new SymbolInfo(*sym1);
            result->setName(sym1->getName() + "=" + to_string(sym1->getFloatValue()));
        } 
        else if (sym1->getVarType() == "INT")
        {
            sym1->setIntValue(sym2->getIntValue());
            result = new SymbolInfo(*sym1);
            result->setName(sym1->getName() + "=" + to_string(sym1->getIntValue()));
        }
        
        
        
    } 
    else if (sym2->getVarType() == "FLOAT")
    {
        if (sym1->getVarType() == "FLOAT") 
        {
            sym1->setFloatValue(sym2->getFloatValue());
            result = new SymbolInfo(*sym1);
            result->setName(sym1->getName() + "=" + to_string(sym1->getFloatValue()));
        } 
        else if (sym1->getVarType() == "INT")
        {
            printWarning("Assigning float to an integer variable");
            sym1->setIntValue(sym2->getFloatValue());
            result = new SymbolInfo(*sym1);
            result->setName(sym1->getName() + "=" + to_string(sym1->getIntValue()));
        }
        
        
    }
    return result;
    
}

SymbolInfo* handleADDOP(SymbolInfo* sym1, SymbolInfo* op, SymbolInfo* sym2)
{
    if(sym1->getVarType() == "VOID" || sym2->getVarType() == "VOID")
    {
        printError("Operand of void type");
        return nullSym();
    }

    SymbolInfo *result = new SymbolInfo("", "");
    if (sym1->getVarType() == "FLOAT" || sym2->getVarType() == "FLOAT" )
    {
        result->setVarType("FLOAT");
    }
    else 
    {
        result->setVarType("INT");
    }


    string ADDOP = op->getName();
    if (ADDOP == "+"){
        if (sym1->isVariable()){
            if (sym2->isVariable()){
                if (sym1->getVarType() == "FLOAT"){
                    if (sym2->getVarType() == "INT"){
                        result->setFloatValue(sym1->getFloatValue()+sym2->getIntValue());
                    } else {
                        result->setFloatValue(sym1->getFloatValue()+sym2->getFloatValue());
                    }                    
                } else if (sym1->getVarType() == "INT"){
                    if (sym2->getVarType() == "INT"){
                        result->setIntValue(sym1->getIntValue()+sym2->getIntValue());
                    } else {
                        result->setFloatValue(sym1->getIntValue()+sym2->getFloatValue());
                    }
                }
            } else if (sym2->isArray()){

            }
        }
    } 
    else if (ADDOP == "-")
    {

    }
    result->setName(sym1->getName() + ADDOP + sym2->getName());
    return result;
}

#endif