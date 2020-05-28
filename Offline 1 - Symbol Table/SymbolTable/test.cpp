#include <iostream>
#include "SymbolTable.h"

using namespace std;

int main()
{
    // SymbolTable st;

    // st.enterScope(1, 10);
    // st.printCurrent();
    // st.insertSymbol("a", "a");
    // st.lookup("a");
    // st.lookup("b");
    // st.deleteSymbol("b");
    // st.deleteSymbol("a");
    // st.exitScope();

    ScopeTable st(1, 10, NULL);
    st.insertSymbol("a", "a");
    st.print();
    st.lookUp("a");
    st.deleteSymbol("a");
    st.insertSymbol("b", "int");
    st.insertSymbol("c", "float");
    st.print();
}