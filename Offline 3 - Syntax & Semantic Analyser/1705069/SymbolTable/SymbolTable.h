#ifndef SYMBOLTABLE
#define SYMBOLTABLE

#include "ScopeTable.h"
#include <iostream>
using namespace std;
#define SYMBOL_TABLE_SIZE 7
class SymbolTable
{
    ScopeTable *current = NULL;
    ofstream *log;

public:
    SymbolTable(ofstream *log)
    {
        this->log = log;
        enterScope();
    }

    void enterScope(int buckets = SYMBOL_TABLE_SIZE)
    {
        ScopeTable *st = new ScopeTable(buckets, current, log);
        current = st;

        *log << "\nNew ScopeTable #" << st->getID() << " created" << endl;
    }

    void exitScope()
    {
        if (current == NULL)
        {
            return;
        }
        ScopeTable *temp = current;
        current = current->getParentScope();

        delete temp;
    }

    bool insertSymbol(string name, string type)
    {
        if (current == NULL)
        {
            return false;
        }

        return current->insertSymbol(name, type);
    }

    bool deleteSymbol(string name)
    {
        if (current == NULL)
        {
            return false;
        }

        return current->deleteSymbol(name);
    }

    SymbolInfo *lookup(string name)
    {
        if (current == NULL)
        {
            return NULL;
        }

        ScopeTable *temp = current;
        SymbolInfo *symbol = NULL;

        while (temp != NULL)
        {
            symbol = temp->lookUp(name);

            if (symbol != NULL)
            {
                return symbol;
            }
            temp = temp->getParentScope();
        }

        return NULL;
    }

    void printCurrent()
    {
        if (current == NULL)
        {
            // *log << "\nNo ScopeTable in the SymbolTable" << endl;
            return;
        }

        current->print();
    }

    void printAll()
    {
        if (current == NULL)
        {
            // *log << "\nNo ScopeTable in the SymbolTable" << endl;
            return;
        }

        ScopeTable *itr = current;

        while (itr != NULL)
        {
            itr->print();
            *log << endl;
            itr = itr->getParentScope();
        }
    }
};

#endif