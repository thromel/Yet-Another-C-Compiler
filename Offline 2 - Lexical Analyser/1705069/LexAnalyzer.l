%option noyywrap

%x MYSTATE

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
NOT_NEWLINE [^\r\n]

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
FLOAT ({INTEGER})?(\.?{DIGITS})(E[+-]?{INTEGER})?
TOO_DECPOINT {DIGIT}*((\.)+{DIGITS})+(E{DECPT}[+-]?{DECPT}{DIGITS})?{DECPOINT}{DIGITS}*
ILL_NUM ({DIGITS}|{FLOATING_POINT})[E]*{FLOATING_POINT}*


ILLEGAL_CHAR ([^\n\t\\\a\f\r\b\0\v])
CHAR_CONST ('{ILLEGAL_CHAR}')

EMPTY_CHARACTER ('')
MULTI_CHARACTER '{CHAR_CONST}{2,}'
UNFINISHED_CHARACTER '({CHAR_CONST}|\\)*


ANYTHING_EXCEPT_SLASH [^\\]
SLASH [\\]
NOT_QUOTE_SLASH_OR_NEWLINE [^"\\\r\n]
ANYTHING ({ANYTHING_EXCEPT_NEWLINE}|{NEWLINE})


UNFINISHED_STRING \"({NOT_QUOTE_SLASH_OR_NEWLINE}|{SLASH}{ANYTHING})*
STRING {UNFINISHED_STRING}\"




STAR_COMMENT_START "/*"
STAR_COMMENT_END "*/"
ANYTHING_EXCEPT_STAR [^\*]
STAR_NOT_FOLLOWED_BY_SLASH [\*][^/]

SLASH_COMMENT "//"({ANYTHING_EXCEPT_NEWLINE}*({SLASH}({NEWLINE}))?{ANYTHING_EXCEPT_NEWLINE}*)*

/* UNFINISHED_STAR_COMMENT {STAR_COMMENT_START}({ANYTHING_EXCEPT_STAR}|{STAR_NOT_FOLLOWED_BY_SLASH})* */
UNFINISHED_STAR_COMMENT {STAR_COMMENT_START}({ANYTHING_EXCEPT_STAR}|([\*]+[^*/]))*
/* STAR_COMMENT {UNFINISHED_STAR_COMMENT}{STAR_COMMENT_END} */
STAR_COMMENT {UNFINISHED_STAR_COMMENT}[\*]*{STAR_COMMENT_END}


ADDOP ("+"|"-")

%%

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
        handle_Keyword(yytext);
    }


{INTEGER} {handle_CONST_INT(yytext);}
{FLOAT} {handle_CONST_FLOAT(yytext);}
{CHAR_CONST} {handle_CONST_CHAR(yytext);}
{NEWLINE} {line_count++;}
{WHITESPACE} {}
<<EOF>> {log << "\nTotal Line number: " << line_count<<"\nTotal Errors: "<<err_count<<endl; log.close(); return 0;}
%%

int main(int argc, char *argv[]){
    if (argc != 2){
        cout<<"Please provide a file name\n";
    }

    log.open("log.txt", ios::out);
    token.open("token.txt", ios::out);

    

    cout<<argv[1]<<endl;
    input = fopen(argv[1], "r");
    yyin = input;
    yylex();

    fclose(yyin);
    return 0;


}

