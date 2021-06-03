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

    bool isDummy = false;
    SymbolInfo *real = NULL;


    SymbolInfo(string name, string type)
    {
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

		this->paramList = symbolInfo.paramList;
        this->isDummy = symbolInfo.isDummy;
        this->real = symbolInfo.real;
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

    int getIntValue()
    {
        if (!isDummy){
            if (intData.size() == 0) return defaultInt;
            else if (idType == "VARIABLE") return intData[0];
            else if (idType == "ARRAY" && arrIndex < arrSize) return intData[arrIndex];
        } else {
            return real->getIntValue();
        }
        
    }

    void setIntValue(int value)
    {
        if (!isDummy){
            if (intData.size() == 0) intData.push_back(value);
            else if (idType == "VARIABLE") intData[0]=value;
            else if (idType == "ARRAY" || arrIndex < arrSize) intData[arrIndex] = value;
        } else {
            real->setIntValue(value);
        }

        
    }

    float getFloatValue()
    {
        if (!isDummy){
            if (floatData.size() == 0) return defaultFloat;
            else if (idType == "VARIABLE") return floatData[0];
            else if (idType == "ARRAY" && arrIndex < arrSize) return floatData[arrIndex];
        } else {
            return real->getFloatValue();
        }
        
    }

    void setFloatValue(float value)
    {
        if (!isDummy){
            if (floatData.size() == 0) floatData.push_back(value);
            else if (idType == "VARIABLE") floatData[0]=value;
            else if (idType == "ARRAY" || arrIndex < arrSize) floatData[arrIndex] = value;
        } else {
            real->setFloatValue(value);
        }
        
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

    void setReal(SymbolInfo *actual)
    {
        isDummy = true;
        this->real = actual;
    }


};


#endif