#include "SymbolInfo.h"
#include<iostream>
#include<fstream>
using namespace std;

class ScopeTable
{
    SymbolInfo **symbols;
    ScopeTable *parentScope = NULL;
    int total_buckets = 0;
    string id;
    int childCount = 0;
    ofstream *log;

public:
    ScopeTable(int total_buckets, ScopeTable *parentScope, ofstream *log)
    {
        this->id = id;
        this->total_buckets = total_buckets;
        this->log = log;

        symbols = new SymbolInfo *[total_buckets];

        for (int i = 0; i < total_buckets; ++i)
        {
            symbols[i] = NULL;
        }

        this->parentScope = parentScope;

        if (parentScope == NULL)
        {
            id = "1";
        }
        else
        {
            parentScope->setChildCount(parentScope->getChildCount() + 1);
            id = parentScope->getID() + "." + to_string(parentScope->getChildCount());
        }
    }

    int hashFunction(string name)
    {
        int hashValue = 0;

        for (int i = 0; i < name.length(); ++i)
        {
            hashValue += name[i];
        }

        return hashValue % total_buckets;
    }

    string getID() const
    {
        return id;
    }

    int getLength() const
    {
        return total_buckets;
    }

    ScopeTable *getParentScope()
    {
        return parentScope;
    }

    SymbolInfo *lookUp(string name)
    {
        int index = hashFunction(name);
        int pos = 0;

        SymbolInfo *symbol = symbols[index];

        while (symbol != NULL)
        {
            if (name == symbol->getName())
            {
                // *log << "\nFound in ScopeTable #" << id << " at position " << index << ", " << pos << endl;
                return symbol;
            }
            pos++;
            symbol = symbol->getNext();
        }
        // *log << "\nNot found in ScopeTable #" << id << endl;

        return NULL;
    }

    bool insertSymbol(string name, string type)
    {
        SymbolInfo *symbol = new SymbolInfo(name, type);

        SymbolInfo *searched = lookUp(symbol->getName());
        if (searched != NULL)
        {
            // *log << *searched << " already exists in the current ScopeTable" << endl;
            return false;
        }

        int index = hashFunction(symbol->getName());
        int pos = 0;

        SymbolInfo *temp = symbols[index];

        //If there is no symbol at that position
        if (temp == NULL)
        {
            symbols[index] = symbol;
            symbol->setNext(NULL);
        }
        else
        {
            //if there is already a symbol at that position
            while (temp->getNext() != NULL)
            {
                temp = temp->getNext();
                pos++;
            }
            temp->setNext(symbol);
            symbol->setNext(NULL);
        }
        // *log << "Inserted in ScopeTable #" << id << " at position " << index << ", " << pos << endl;
        return true;
    }

    bool deleteSymbol(string name)
    {
        if (lookUp(name) == NULL)
        {
            *log << name << " not found" << endl;
            return false;
        }

        int index = hashFunction(name);
        int pos = 0;

        SymbolInfo *temp = symbols[index];
        SymbolInfo *previous = NULL;

        while (temp != NULL)
        {
            if (name == temp->getName())
            {
                break;
            }

            pos++;
            previous = temp;
            temp = temp->getNext();
        }

        if (previous == NULL)
        {
            symbols[index] = temp->getNext();
        }
        else
        {
            previous->setNext(temp->getNext());
        }

        delete temp; //Deletes the symbolInfo object

        // *log << "Deleted entry at " << index << ", " << pos << " from current scopeTable" << endl;

        return true;
    }

    void print()
    {
        *log << "\nScopeTable #" << this->id << endl;

        for (int i = 0; i < total_buckets; ++i)
        {
            *log << i << " -->";

            SymbolInfo *temp = symbols[i];

            while (temp != NULL)
            {
                *log << " " << *temp;

                temp = temp->getNext();
            }

            *log << "\n";
        }
    }

    void setChildCount(int i)
    {
        this->childCount = i;
    }

    int getChildCount()
    {
        return this->childCount;
    }

    ~ScopeTable()
    {
        *log << "\nDeleted ScopeTable #" << id << endl;
        delete[] symbols;
        parentScope = NULL;
    }
};