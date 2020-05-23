%{

#ifndef PARSER
#define PARSER
#include "ParserUtils.h"
#endif

int yyparse(void);
int yylex(void);
extern FILE *yyin;
extern SymbolTable st;

%}

%union{
SymbolInfo* symbol;
}


%token IF FOR DO INT FLOAT VOID SWITCH DEFAULT ELSE WHILE BREAK CHAR DOUBLE RETURN CASE CONTINUE
%token INCOP DECOP ASSIGNOP NOT
%token LPAREN RPAREN LCURL RCURL LTHIRD RTHIRD COMMA SEMICOLON
%token PRINTLN
%token STRING


%token <symbol> ID
%token <symbol> CONST_INT
%token <symbol> CONST_FLOAT
%token <symbol> CONST_CHAR
%token <symbol> ADDOP
%token <symbol> MULOP
%token <symbol> LOGICOP
%token <symbol> RELOP
%token <symbol> BITOP


%type <symbol> type_specifier 
%type <symbol> expression 
%type <symbol> expression_statement
%type <symbol>logic_expression
// %type <symbol>rel_expression
%type <symbol>simple_expression
%type <symbol>term
%type <symbol>unary_expression
%type <symbol>factor
%type <symbol>variable
// %type <symbol>parameter_list
%type <symbol> declaration_list
%type <symbol> program
%type <symbol> unit
%type <symbol> start
// %type <symbol>func_declaration
%type <symbol>func_definition
%type <symbol> compound_statement
%type <symbol> var_declaration
%type <symbol> statement
%type <symbol> statements

// %error-verbose

%%

start : program
	{
		printRule("start : program");
		$$ = $1;
		printSymbol($$);
		st.printAll();
	}
	;

program : program unit 
	{
		$1->setName($1->getName() + "\n" + $2->getName());
		printRule("program : program unit");
		printSymbol($1);
		$$ = $1;
	}
	| unit
	{
		$$ = $1;
		printRule("program : unit");
		printSymbol($$);
	}
	;
	
unit : var_declaration
	{
		$$ = $1;
		printRule("unit : var_declaration");
		printSymbol($$);
	}
	| func_definition
	{
		$$ = $1;
		printError("unit : func_definition");
		printSymbol($$);
	}
     ;

func_definition : type_specifier ID LPAREN RPAREN {addFuncDef($1, $2);} compound_statement
		{
			$$ = new SymbolInfo($1->getName() + " " + $2->getName() + " ( ) " + $6->getName(), "NON_TERMINAL" );
			printRule("func_definition : type_specifier ID LPAREN RPAREN compound_statement");
			printSymbol($$);
		}
		;
     		
var_declaration : type_specifier declaration_list SEMICOLON
		{
			$$ = new SymbolInfo($1->getName() + " " + $2->getName() + ";","NON_TERMINAL");
			printRule("var_declaration : type_specifier declaration_list SEMICOLON");
			printSymbol($$);
		}
 		 ;
 		 
type_specifier	: INT
		{
			$$ = new SymbolInfo("int", "NON_TERMINAL");
			printRule("type_specifier	: INT");
			printSymbol($$);
		}
		| FLOAT
		{
			$$ = new SymbolInfo("float", "NON_TERMINAL");
			printRule("type_specifier	: FLOAT");
			printSymbol($$);
		}
		| VOID
		{
			$$ = new SymbolInfo("void", "NON_TERMINAL");
			printRule("type_specifier	: VOID");
			printSymbol($$);
		}
 		;
 		
declaration_list : declaration_list COMMA ID
			{
				$$ = new SymbolInfo($1->getName() + ", " + $3->getName(), "NON_TERMINAL");
				printRule("declaration_list : declaration_list COMMA ID");
				printSymbol($$);
				if (!insertSymbol($1)){
					printError(" multiple declaration of " + $1->getName());
				}
			}
			| declaration_list COMMA ID LTHIRD CONST_INT RTHIRD 
			{
				$$ = new SymbolInfo($1->getName() + ", " + $3->getName() + "[" + $5->getName() + "]", "NON_TERMINAL");
				printRule("declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD ");
				printSymbol($$);
				if (!insertSymbol($$)) {
					printError(" multiple declaration of " + $3->getName() );
				}
			}
 		  	| ID
		   {
			   $$ = $1;
			   printRule("declaration_list : ID");
			   printSymbol($$);
			   if (!insertSymbol($1)){
				   printError(" multiple declaration of " + $1->getName() );
			   }
		   }
			| ID LTHIRD CONST_INT RTHIRD
			{
				printRule("declaration_list :ID LTHIRD CONST_INT RTHIRD ");
				$$ = new SymbolInfo($1->getName() + "[" + $3->getName() + "]", "NON_TERMINAL" );
				printSymbol($$);
				if (!insertSymbol($$)) {
					printError(" multiple declaration of " + $3->getName() );
				}
			}
 		  ;

statements : statement
			{
				$$ = $1;
				printRule("statements : statement");
				printSymbol($$);
			}
			| statements statement
			{
				$$ = new SymbolInfo($1->getName() + " " + $2->getName(), "NON_TERMINAL");
				printRule("statements : statements statement");
				printSymbol($$);
			}
			;
statement : var_declaration
			{
				$$ = $1;
				printRule("statement : var_declaration");
				printSymbol($$);

			}
			| expression_statement
			{
				$$ = $1;
				printRule("statement : expression_statement");
				printSymbol($$);
			}
			| compound_statement
			{
				$$ = $1;
				printRule("statement : compound_statement");
				printSymbol($$);
			}
			;

expression_statement : SEMICOLON
			{
				$$ = new SymbolInfo(" ; ", "NON_TERMINAL");
				printRule("expression_statement : SEMICOLON");
				printSymbol($$);
			}
			| expression SEMICOLON
			{
				$$ = new SymbolInfo($1->getName()+";", "NON_TERMINAL");
				printRule("expression_statement : expression SEMICOLON");
				printSymbol($$);
			}
			;

expression : logic_expression
			{
				$$ = $1;
				printRule("expression : logic_expression");
				printSymbol($$);
			}
			
			;

logic_expression : simple_expression
			{
				$$ = $1;
				printRule("logic_expression : simple_expression");
				printSymbol($$);
			}
			| simple_expression RELOP simple_expression
			{
				$$ = new SymbolInfo($1->getName(), "NON_TERMINAL");
				printRule("logic_expression : simple_expression RELOP simple_expression");
				printSymbol($$);

				//TODO
			}
			;

simple_expression : term
			{
				$$ = $1;
				printRule("simple_expression : term");
				printSymbol($$);
			}
			| simple_expression ADDOP term
			{

			}
			;

term : unary_expression
			{
				$$ = $1;
				printRule("unary_expression : factor");
				printSymbol($$);
			}
			;

unary_expression : factor
			{
				$$ = $1;
				printRule("unary_expression : factor");
				printSymbol($$);
			}
			;

factor : variable
			{
				$$ = $1;
				printRule("factor : variable");
				printSymbol($$);
			}
			;

variable : ID
			{
				$$ = getVariable($1);
				printRule("variable : ID");
				printSymbol($$);
			}
			;
compound_statement : LCURL {enterScope();} statements RCURL
			{
				$$ = new SymbolInfo("{\n" + $3->getName() + "}\n", "NON_TERMINAL");
				printRule("compound_statement : LCURL statements RCURL");
				printSymbol($$);
			}
			| LCURL {enterScope();} RCURL
			{
				$$ = new SymbolInfo("{}", "NON_TERMINAL");
				printRule("compound_statement : LCURL RCURL");
				printSymbol($$);
			}
 

%%
int main(int argc,char *argv[])
{

    if (argc != 2){
        cout<<"Please provide a file name\n";
    }

    log.open("log.txt", ios::out);
	error.open("error.txt", ios::out);

    yyin = fopen(argv[1], "r");

    if (yyin == NULL){
        cout<<"Cannot open input file\n";
        exit(1);
    }

    yyparse();

    return 0;

}

