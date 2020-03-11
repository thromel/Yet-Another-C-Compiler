#include <iostream>
#include "SymbolTable.h"

using namespace std;

int main()
{
    SymbolTable st;

    int length, number, current;
    current = number = 0;

    cin >> length;

    st.enterScope(++number, length);
    current++;

    string choice, name, type;

    while (true)
    {
        cin >> choice;

        if (choice == "I")
        {

            cin >> name >> type;

            cout << choice << " " << name << " " << type << "\n\n";

            st.insertSymbol(name, type);
        }
        else if (choice == "L")
        {
            cin >> name;
            cout << choice << " " << name << "\n\n";
            st.lookup(name);
        }
        else if (choice == "D")
        {
            cin >> name;
            cout << choice << " " << name << "\n\n";
            st.deleteSymbol(name);
        }
        else if (choice == "P")
        {
            cin >> type;
            if (type == "A" || type == "C")
            {
                cout << choice << " " << type << "\n\n";
                if (type == "A")
                    st.printAll();
                else
                    st.printCurrent();
            }

            else
                cout << "Invalid operation\n\n";
        }
        else if (choice == "S")
        {
            cout << choice << "\n";
            st.enterScope(++number, length);
            current++;
        }
        else if (choice == "E")
        {
            cout << choice << "\n";

            st.exitScope();
            current--;
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i < current; ++i)
    {
        st.exitScope();
    }
}