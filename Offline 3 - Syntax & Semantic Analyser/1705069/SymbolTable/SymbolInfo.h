#ifndef SYMBOLINFO
#define SYMBOLINFO

#include <string>
#include <iostream>
#include <vector>
using namespace std;

class SymbolInfo
{
    string name;
    string type;

    string idType; //FUNCTION, VARIABLE, ARRAY
    string varType; //INT, FLOAT, VOID

    string returnType; //INT, FLOAT, VOID
    bool funcDefined = false;

    int arrSize;
    int arrIndex;

    int defaultInt = 0;
    float defaultFloat = 0.0;

    SymbolInfo *next;

public:
    struct param
    {
        string type;
        string name;
    };

    vector<param> params;

    vector<int> intData;
    vector<float> floatData;

    SymbolInfo(string name, string type)
    {
        this->name = name;
        this->type = type;
        next = NULL;
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
        params.clear();
    }

    //Added for parser

    void addParam(string type, string name)
    {
        param temp;
        temp.name = name;
        temp.type = type;

        params.push_back(temp);
    }

    param getParam(int index)
    {
        return params[index];
    }

    void set_idType(string idtype)
    {
        this->idType = idType;
    }

    string get_idType()
    {
        return idType;
    }

    void set_arrSize(int size)
    {
        arrSize = size;
    }

    int get_arrSize()
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

    int intValue(int index = 0)
    {
        if (idType == "VARIABLE"  && varType == "INT"){
            if (intData.size() == 0) return 0;
            else intData[index];
        } else if (idType == "ARRAY" && varType == "FLOAT"){
            return intData[arrIndex];    
        }
        return defaultInt;
    }

    void setArrIndex(int arrIndex)
    {
        if (!isArray()){
            return;
        }
        this->arrIndex = arrIndex;
    }

    void setArrSize (int arrSize)
    {
        if (!isArray()){
            return;
        }
        this->arrSize = arrSize;
    }


};


#endif