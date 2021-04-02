#include "./SymbolTable/SymbolTable.h"
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;


int line_count = 1;
int keyword_count = 0;
int err_count = 0;

ofstream log, token;
ifstream in;
SymbolTable st(&log);

//Utility functions

string toUpper(string str){
    for (auto & c: str) c = toupper(c);
    return str;

}

string replaceAll(string str, const string& from, const string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

string tokenGenerator(string s){
    return "<" + toUpper(s) + ">";
}

string tokenGenerator(string type, string symbol){
    return "<" + type + "," + symbol + ">";
}

void printLog(int line, string token, string lexeme)
{
    log << "\nLine no " << line << ": Token "
        << tokenGenerator(token)<<" Lexeme "<<lexeme<<" found";
}

void printToken(string lexeme){
    //Prints token for keywords;
    token<<tokenGenerator(lexeme);
}

void printToken(string tkn, string lexeme){
    token<<tokenGenerator(tkn, lexeme);
}

void insertToken(string symbol, string type)
{
    if (!st.insertSymbol(symbol, type)){
        log << endl<< symbol << " already exists in current scopeTable\n";
    } else {
        st.printAll();
    }
    
}

//------------------------------------------------------------------

void handle_keyword(char *str)
{
    string s(str);
    printLog(line_count,toUpper(s), s);
    printToken(s);
    keyword_count++;
}

void handle_const_int(char *str){
    string s(str);
    insertToken(s, "CONST_INT");
    printLog(line_count, "CONST_INT", s);
    printToken("CONST_INT", s);
}

void handle_const_float(char *str){
    string s(str);
    insertToken(s, "CONST_FLOAT");
    printLog(line_count, "CONST_FLOAT", s);
    printToken("CONST_FLOAT", s);
}

void handle_const_char(char *str, string type){
    string s(str);

    printLog(line_count, type, s);
    insertToken(s, type);

    size_t n = std::count(s.begin(), s.end(), '\n');
    line_count += (int)n;
    

    s = replaceAll(s, "\'", "");
    s = replaceAll(s,"\"", "" );
    s = replaceAll(s, "\\n", "\n");
    s = replaceAll(s, "\\t", "\t");
    printToken(type, s);
}

void handle_operator(char *str, string opr){
    string s(str);

    printLog(line_count, opr, s);
    printToken(opr, s);

    if (opr == "LCURL"){
        st.enterScope();
    } 

    if (opr == "RCURL"){
        st.exitScope();
    }    
}

void handle_id (char *str){
    string s(str);
    printLog(line_count, "ID", s);
    printToken("ID", s);
    insertToken(s, "ID");
}

void handle_error(char *str, string msg){
    string s(str);
    log <<"\n"<<"Error at line no: "<<line_count<<". "<<msg<<": "<<s<<endl;
    err_count++;
}

void handle_comment(char *str){
    string s(str);
    printLog(line_count, "COMMENT", s);
    size_t n = std::count(s.begin(), s.end(), '\n');
    line_count += (int)n;
}


//-------------------------------------------------------------------