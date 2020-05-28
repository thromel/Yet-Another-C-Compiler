#include <string>
#include<iostream>
using namespace std;

class SymbolInfo
{
    string name;
    string type;

    SymbolInfo *next;

public:
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

    void setNext(SymbolInfo *next)
    {
        this->next = next;
    }

    SymbolInfo *getNext()
    {
        return this->next;
    }

    friend ostream &operator<<(ostream &, SymbolInfo &);

    ~SymbolInfo()
    {
    }
};

ostream &operator<<(ostream &out, SymbolInfo &ref)
{
    out << "< " << ref.name << " : " << ref.type << " >";
    return out;
}