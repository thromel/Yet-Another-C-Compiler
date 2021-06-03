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
    id->setVarType(type);

    for(int i = 0; i < id->getArrSize(); ++i){
        id->intData.push_back(0);
        id->floatData.push_back(0);
    }

}

SymbolInfo* getConstVal(SymbolInfo *val, string varType)
{   

    
    val->setIdType("VARIABLE");
    val->setVarType(varType);

    if (varType == "FLOAT")
    {
        val->setFloatValue(stof(val->getName()));
    } else if (varType == "INT")
    {
        val->setIntValue(stoi(val->getName()));
    }
    
    return val;
}

SymbolInfo* getArrayIndexVar(SymbolInfo *arr, SymbolInfo *index)
{
    SymbolInfo *arrIdxVar = st.lookup(arr->getName());
    SymbolInfo *var;
    if (arrIdxVar == NULL)
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
             var->setName(arr->getName()+"["+to_string(index->getIntValue())+"]");
             var->setReal(arrIdxVar);
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
        } 
        else if (sym1->getVarType() == "INT")
        {
            sym1->setIntValue(sym2->getIntValue());
            result = new SymbolInfo(*sym1);
        }
        
        
        
    } 
    else if (sym2->getVarType() == "FLOAT")
    {
        if (sym1->getVarType() == "FLOAT") 
        {
            sym1->setFloatValue(sym2->getFloatValue());
            result = new SymbolInfo(*sym1);
        } 
        else if (sym1->getVarType() == "INT")
        {
            printWarning("Assigning float to an integer variable");
            sym1->setIntValue(sym2->getFloatValue());
            result = new SymbolInfo(*sym1);
        }
        
        
    }
    result->setName(sym1->getName() + "=" + sym2->getName());
    result->setIdType("VARIABLE");

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
    result->setIdType("VARIABLE");


    string ADDOP = op->getName();
    if (ADDOP == "+"){
        if (sym1->isVariable() || sym1->isArray()){
            if (sym2->isVariable() || sym2->isArray()){
                if (sym1->getVarType() == "FLOAT"){
                    if (sym2->getVarType() == "INT"){
                        result->setFloatValue(sym1->getFloatValue()+sym2->getIntValue());
                    } else {
                        result->setFloatValue(sym1->getFloatValue()+sym2->getFloatValue());
                    }                    
                } else if (sym1->getVarType() == "INT"){
                    if (sym2->getVarType() == "INT"){
                        result->setIntValue(sym1->getIntValue()+sym2->getIntValue());
                        // cout<<result->getIntValue()<<" "<<result->getVarType()<<endl;
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
        if (sym1->isVariable() || sym1->isArray()){
            if (sym2->isVariable() || sym2->isArray()){
                if (sym1->getVarType() == "FLOAT"){
                    if (sym2->getVarType() == "INT"){
                        result->setFloatValue(sym1->getFloatValue()-sym2->getIntValue());
                    } else {
                        result->setFloatValue(sym1->getFloatValue()-sym2->getFloatValue());
                    }                    
                } else if (sym1->getVarType() == "INT"){
                    if (sym2->getVarType() == "INT"){
                        result->setIntValue(sym1->getIntValue()-sym2->getIntValue());
                    } else {
                        result->setFloatValue(sym1->getIntValue()-sym2->getFloatValue());
                    }
                }
            }
        }
    }
    result->setName(sym1->getName() + ADDOP + sym2->getName());
    return result;
}

SymbolInfo* handle_RELOP (SymbolInfo *sym1, SymbolInfo *op, SymbolInfo *sym2)
{
    if (sym1->getVarType() == "VOID" || sym2->getVarType() == "VOID"){
        printError("Void type expressions are not comparable");
        return nullSym();
    }

    if (sym1->getVarType() != sym2->getVarType()){
        printWarning("Comparison between different types");
    }

    string relop = op->getName();



    SymbolInfo *result = new SymbolInfo("","");
    result->setVarType("INT");
    result->setIdType("VARIABLE");

    int leftInt = 0, rightInt = 0;
    float leftFloat = 0.0, rightFloat = 0.0;
    int resultValue = 0;

    if (sym1->getVarType() == "INT" && sym2->getVarType() == "INT"){
        leftInt = sym1->getIntValue();
        rightInt = sym2->getIntValue();
        
        if (relop == "=="){
            resultValue = leftInt == rightInt;

        } else if (relop == "!="){
            resultValue = leftInt != rightInt;

        } else if (relop == "<="){
            resultValue = leftInt <= rightInt;

        } else if (relop == ">="){
            resultValue = leftInt >= rightInt;
        } else if (relop == ">"){
            resultValue = leftInt > rightInt;
        } else if (relop == "<"){
            resultValue = leftInt < rightInt;
        } 
    } else if (sym1->getVarType() == "INT" && sym2->getVarType() == "FLOAT"){
        leftInt = sym1->getIntValue();
        rightFloat = sym2->getFloatValue();
        if (relop == "=="){
            resultValue = leftInt == rightFloat;

        } else if (relop == "!="){
            resultValue = leftInt != rightFloat;

        } else if (relop == "<="){
            resultValue = leftInt <= rightFloat;

        } else if (relop == ">="){
            resultValue = leftInt >= rightFloat;
        } else if (relop == ">"){
            resultValue = leftInt > rightFloat;
        } else if (relop == "<"){
            resultValue = leftInt < rightFloat;
        } 
    } else if (sym1->getVarType() == "FLOAT" && sym2->getVarType() == "INT"){
        leftFloat = sym1->getFloatValue();
        rightInt = sym2->getIntValue();
        if (relop == "=="){
            resultValue = leftFloat == rightInt;

        } else if (relop == "!="){
            resultValue = leftFloat != rightInt;

        } else if (relop == "<="){
            resultValue = leftFloat <= rightInt;

        } else if (relop == ">="){
            resultValue = leftFloat >= rightInt;
        } else if (relop == ">"){
            resultValue = leftFloat > rightInt;
        } else if (relop == "<"){
            resultValue = leftFloat < rightInt;
        } 
    } else if (sym1->getVarType() == "FLOAT" && sym2->getVarType() == "FLOAT"){
        leftFloat = sym1->getFloatValue();
        rightFloat = sym2->getFloatValue();
        if (relop == "=="){
            resultValue = leftFloat == rightFloat;

        } else if (relop == "!="){
            resultValue = leftFloat != rightFloat;

        } else if (relop == "<="){
            resultValue = leftFloat <= rightFloat;

        } else if (relop == ">="){
            resultValue = leftFloat >= rightFloat;
        } else if (relop == ">"){
            resultValue = leftFloat > rightFloat;
        } else if (relop == "<"){
            resultValue = leftFloat < rightFloat;
        } 
    }
    result->setIntValue(resultValue);
    result->setName(sym1->getName() + relop + sym2->getName());
    cout<<resultValue<<endl;
    return result; 
}

#endif