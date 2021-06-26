#ifndef ASMUTILS
#define ASMUTILS

#include<stack>
#include<set>
#include<fstream>

extern vector<string> asmVarList;
extern vector<pair<string, int>> asmArrList;
extern ofstream code;

int label_count=0;

class VarManager {
    int size = 0;

    stack<string> free;
    set<string> used;

public:
    string getTempVar(){
        string tempVar;
        if (free.empty()){
            tempVar = "temp"+to_string(size);
            size++;
            used.insert(tempVar);
            asmVarList.push_back(tempVar);
        } else {
            tempVar = free.top();
            free.pop();
        }
        return tempVar;
    }

    void freeTempVar(string tempVar){
        if (tempVar.substr(0, 4) == "temp"){
            used.erase(tempVar);
            free.push(tempVar);
        }
    }


};

void printCode(SymbolInfo *sym){
    code << sym->getCode() <<endl;
}

string newLabel(){
    return "L"+to_string(label_count++);
}


void addDataSegment(){
    code << ".MODEL MEDIUM \n.STACK 100H \n.DATA"<<endl<<endl;

    asmVarList.push_back("return_loc");
    for(string s : asmVarList){
        code << s << " DW ?" << endl;
    }

    for(auto p : asmArrList){
        code << p.first <<	" DW "	<< p.second <<" DUP (?)"<<endl; 
    }
}



void startCodeSegment(){
    code <<endl<< ".CODE" <<endl;

}

void endCodeSegment(){
    code <<endl<<"END MAIN\n";
}

string memToMem(SymbolInfo *lhs, SymbolInfo *rhs){
    string asmCode = "MOV AX, ";
    asmCode +=  rhs->getAsmVar() + "\n";
    asmCode += "MOV " + lhs->getAsmVar() + ", AX \n";
    return asmCode;
}


string constToMem(SymbolInfo *lhs, SymbolInfo *constVal){
    string asmCode = "MOV AX, " ;
    asmCode += constVal->getName() + "\n";
    asmCode += "MOV " + lhs->getAsmVar() + ", AX \n";
    return asmCode;
}

void addPrintFunc(){
    ifstream srcFile("./AsmLibraries/outdec.h");
    string line;
    while( std::getline( srcFile, line, '.' ) ) code << line << '\n' ;
    srcFile.close();
}



#endif