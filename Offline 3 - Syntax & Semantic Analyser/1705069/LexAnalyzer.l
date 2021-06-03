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

{STRING} {handle_const_char(yytext, "STRING"); return STRING;}
{UNFINISHED_STRING} {handle_error(yytext, "Unfinished string");}

{INTEGER} {handle_const_int(yytext); return CONST_INT;}
{FLOAT} {handle_const_float(yytext); return CONST_FLOAT;}
{TOO_DECPT} {handle_error(yytext, "Too many decimal points");}
{ILL_NUM} {handle_error(yytext, "Ill formed number");}


if {return IF;}
else {return ELSE;}
for {return FOR;}
while {return WHILE;}
do {return DO;}
break {return BREAK;}
int {return INT;}
char {return CHAR;}
float {return FLOAT;}
double {return DOUBLE;}
void {return VOID;}
return {return RETURN;}
switch {return SWITCH;}
case {return CASE;}
default {return DEFAULT;}
continue {return CONTINUE;}


{ADDOP} {handle_operator(yytext, "ADDOP"); return ADDOP;}
("++"|"--") {handle_operator(yytext, "INCOP"); return INCOP;}
("*"|"/"|"%") {handle_operator(yytext, "MULOP"); return MULOP;}
("<="|">="|"=="|"!="|"<"|">") {handle_operator(yytext, "RELOP"); return RELOP;}
("&&"|"||") {handle_operator(yytext, "LOGICOP"); return LOGICOP;}
("&"|"|"|"^"|"<<"|">>") {handle_operator(yytext, "BITOP"); return BITOP;}
";" {return SEMICOLON;}
"," {return COMMA;}
"[" {return LTHIRD;}
"]" {return RTHIRD;}
"{" {return LCURL;}
"}" {return RCURL;}
"(" {return LPAREN;}
")" {return RPAREN;}
"=" {handle_operator(yytext, "ASSIGNOP"); return ASSIGNOP;}
"!" {return NOT;}
"PRINT" {return PRINT;}

{ID} {handle_id(yytext); return ID;}
{NOT_ID} {handle_error(yytext, "Invalid Suffix on numeric constant or invalid prefix on identifier for character sequence");}


{NEWLINE} {line_count++;}
{WHITESPACE} {}
. {handle_error(yytext, "Unrecognized character");}
<<EOF>> {log << "\nTotal lines: " << line_count<<"\nTotal errors: "<<lex_err_count<<endl; log.close(); return 0;}
%%


