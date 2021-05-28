#ifndef SYMBOLINFO
#define SYMBOLINFO

#include <string>
#include <iostream>
#include <vector>
using namespace std;

class SymbolInfo
{
    string name = "";
    string type = "";

    string idType = ""; //FUNCTION, VARIABLE, ARRAY
    string varType = ""; //INT, FLOAT, VOID

    string returnType = ""; //INT, FLOAT, VOID
    bool funcDefined = false;

    int arrSize = 0;
    int arrIndex = 0;

    int defaultInt = 0;
    float defaultFloat = 0.0;

    SymbolInfo *next;

public:
    struct param
    {
        string type;
        string name;
    };

    vector<param> paramList;

    vector<int> intData;
    vector<float> floatData;

    SymbolInfo(string name, string type)
    {
        this->name = name;
        this->type = type;
        next = NULL;
        fill(intData.begin(), intData.end(), 0);
        fill(floatData.begin(), floatData.end(), 0.0);
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

		this->paramList = symbolInfo.paramList;
	}

    string getName() const
    {
        return this->name;
    }

    string getType() const
    {
        return this->type;
    }

    void setName(string name)
    {
        this->name = name;
    }

    void setType(string type)
    {
        this->type = type;
    }

    void setNext(SymbolInfo *next)
    {
        this->next = next;
    }

    SymbolInfo *getNext()
    {
        return this->next;
    }


    ~SymbolInfo()
    {
        paramList.clear();
    }

    //Added for parser

    void addParam(string name, string type)
    {
        param temp;
        temp.name = name;
        temp.type = type;

        paramList.push_back(temp);
    }

    param getParam(int index)
    {
        return paramList[index];
    }

    void setIdType(string idType)
    {
        this->idType = idType;
    }

    string getIdType()
    {
        return idType;
    }

    void setVarType(string vt)
    {
        this->varType = vt;
    }

    string getVarType()
    {
        return this->varType;
    }

    void setArrSize(int size)
    {
        arrSize = size;
    }

    int getArrSize()
    {
        return arrSize;
    }

    bool isFunction()
    {
        return idType == "FUNCTION";
    }

    bool isVariable()
    {
        return idType == "VARIABLE";
    }

    bool isArray()
    {
        return idType == "ARRAY";
    }

    int getIntValue(int index = 0)
    {
        if (intData.size() == 0) return defaultInt;
        else return intData[index];
        
    }

    void setIntValue(int value)
    {
        if (intData.size() == 0) intData.push_back(value);
        else intData[0] = value;
    }

    float getFloatValue(int index = 0)
    {
        if (floatData.size() == 0) return defaultFloat;
        return floatData[index];
    }

    void addIntValue(int value)
    {
        intData.push_back(value);
    }

    void addFloatValue(float value)
    {
        floatData.push_back(value);
    }

    void setFloatValue(float value)
    {
        if (floatData.size() == 0) floatData.push_back(value);
        else floatData[0]=value;
    }



    void setArrIndex(int arrIndex)
    {
        if (!isArray()){
            return;
        }
        this->arrIndex = arrIndex;
    }

    void setReturnType(string ret)
    {
        this->returnType = ret;
    }

};


#endif