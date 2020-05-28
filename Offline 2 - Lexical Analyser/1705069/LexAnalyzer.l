%option noyywrap


%{
// #include <stdlib.h>
#include "LexUtils.h"
#include<iostream>
#include<string.h>
#include<stdio.h>
using namespace std;


FILE *input;

%}

NEWLINE \r?\n
NOT_NEWLINE ([^\r\n])
WHITESPACE [ \t\f\r\v]+

LETTER [a-zA-Z]
DIGIT [0-9]

ALPHABET {LETTER}|_
ALPHANUM {ALPHABET}|{DIGIT}

ID {ALPHABET}({ALPHANUM})*
NOT_ID {DIGIT}({ALPHANUM})*

DIGITS {DIGIT}+
DECPT (\.)*
FRAC (\.{DIGITS})

INTEGER {DIGITS}
FLOAT ({INTEGER})?(\.?{DIGITS}*)([Ee][+-]?{INTEGER})?
ILL_NUM ({DIGITS}|{FLOAT})[Ee]*{FLOAT}*
TOO_DECPT {DIGIT}*((\.)+{DIGITS})+([Ee]{DECPT}[+-]?{DECPT}{DIGITS})?{DECPT}{DIGITS}*

CHAR_CONST ((\\[^\r\n])|[^\\'\r\n])
CHARACTER [\']{CHAR_CONST}[\']
EMPTY_CHARACTER ('')
MULTI_CHARACTER [\']{CHAR_CONST}{2,}[\']
UNFINISHED_CHARACTER [\']({CHAR_CONST})*[;]?


STRING [\"](\\[^\r\n]|[^\\\r\n]|([\\](\r?\n)))*[\"]
UNFINISHED_STRING \"([^"\\]|\\['"?\\a-zA-Z0-9]|([\\](\r?\n)))*[;]?

STAR_COMMENT [\/][*][^*]*[*]+([^*\/][^*]*[*]+)*[\/]
COMMENT "//"([^\r\n]*|([\\][\r\n]))*
UNFINISHED_STAR_COMMENT [\/][*][^*]*([^*\/][^*]*)*(\r?\n)

ADDOP ("+"|"-")

%%

{COMMENT} |
{STAR_COMMENT} {handle_comment(yytext);}
{UNFINISHED_STAR_COMMENT} {
    handle_error(yytext, "Unfinished comment");
    }

  

{CHARACTER} {handle_const_char(yytext, "CONST_CHAR");}
{UNFINISHED_CHARACTER} {handle_error(yytext, "Unfinished character");}
{EMPTY_CHARACTER} {handle_error(yytext, "Empty character");}
{MULTI_CHARACTER} {handle_error(yytext, "Multi character constant");}

{STRING} {handle_const_char(yytext, "STRING");}
{UNFINISHED_STRING} {handle_error(yytext, "Unfinished string");}

{INTEGER} {handle_const_int(yytext);}
{FLOAT} {handle_const_float(yytext);}
{TOO_DECPT} {handle_error(yytext, "Too many decimal points");}
{ILL_NUM} {handle_error(yytext, "Ill formed number");}


if |
else |
for |
while |
do |
break |
int |
char |
float |
double |
void |
return |
switch |
case |
default |
continue {
        handle_keyword(yytext);
    }


{ADDOP} {handle_operator(yytext, "ADDOP");}
("++"|"--") {handle_operator(yytext, "INCOP");}
("*"|"/"|"%") {handle_operator(yytext, "MULOP");}
("<="|">="|"=="|"!="|"<"|">") {handle_operator(yytext, "RELOP");}
("&&"|"||") {handle_operator(yytext, "LOGICOP");}
("&"|"|"|"^"|"<<"|">>") {handle_operator(yytext, "BITOP");}
";" {handle_operator(yytext, "SEMICOLON");}
"," {handle_operator(yytext, "COMMA");}
"[" {handle_operator(yytext, "LTHIRD");}
"]" {handle_operator(yytext, "RTHIRD");}
"{" {handle_operator(yytext, "LCURL");}
"}" {handle_operator(yytext, "RCURL");}
"(" {handle_operator(yytext, "LPAREN");}
")" {handle_operator(yytext, "RPAREN");}
"=" {handle_operator(yytext, "ASSIGNOP");}
"!" {handle_operator(yytext, "NOT");}

{ID} {handle_id(yytext);}
{NOT_ID} {handle_error(yytext, "Invalid Suffix on numeric constant or invalid prefix on identifier for character sequence");}


{NEWLINE} {line_count++;}
{WHITESPACE} {}
. {handle_error(yytext, "Unrecognized character");}
<<EOF>> {log << "\nTotal lines: " << line_count<<"\nTotal errors: "<<err_count<<endl; log.close(); return 0;}
%%

int main(int argc, char *argv[]){
    if (argc != 2){
        cout<<"Please provide a file name\n";
    }

    log.open("log.txt", ios::out);
    token.open("token.txt", ios::out);

    input = fopen(argv[1], "r");
    yyin = input;
    yylex();

    fclose(yyin);
    return 0;


}

