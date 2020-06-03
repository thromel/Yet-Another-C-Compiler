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
%token INCOP DECOP NOT
%token LPAREN RPAREN LCURL RCURL LTHIRD RTHIRD COMMA SEMICOLON
%token PRINT
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
%token <symbol> ASSIGNOP

%type <symbol> program
%type <symbol> unit
%type <symbol> start
%type <symbol> type_specifier 
%type <symbol> expression 
%type <symbol> expression_statement
%type <symbol>logic_expression
%type <symbol>rel_expression
%type <symbol>simple_expression
%type <symbol>term
%type <symbol>unary_expression
%type <symbol>factor
%type <symbol>variable
%type <symbol>parameter_list
%type <symbol> declaration_list
// %type <symbol>func_declaration
%type <symbol>func_definition
%type <symbol> compound_statement
%type <symbol> var_declaration
%type <symbol> statement
%type <symbol> statements
%type <symbol> print

// %error-verbose

%%

start : program
	{
		$$ = $1;
		printRule("start : program");
		printSymbol($$);
		st.printAll();
	}
	;

program : program unit 
	{
		$1->setName($1->getName() + "\n" + $2->getName());
		$$ = $1;
		printRule("program : program unit");
		printSymbol($$);
		
	}
	| unit
	{
		$$ = $1;
		printRule("program : unit");
		printSymbol($$);
		st.printAll();
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
		printRule("unit : func_definition");
		printSymbol($$);
	}
     ;

func_definition : type_specifier ID LPAREN RPAREN {addFuncDef($2, $1);} compound_statement
		{
			$$ = new SymbolInfo($1->getName() + " " + $2->getName() + " ( ) " + $6->getName(), "NON_TERMINAL" );
			printRule("func_definition : type_specifier ID LPAREN RPAREN compound_statement");
			printSymbol($$);
		}
		| type_specifier ID LPAREN parameter_list RPAREN {addFuncDef($2, $1);} compound_statement
		{
			$$ = new SymbolInfo($1->getName() + " " + $2->getName() + " ( " + $4->getName() +" ) "+ $7->getName(), "NON_TERMINAL" );
			printRule("func_definition : type_specifier ID LPAREN parameter_list RPAREN");
			printSymbol($$);
		}
		;

parameter_list: parameter_list COMMA type_specifier ID
		{	

			$$ = new SymbolInfo($1->getName() + "," + $3->getName() + " " + $4->getName(), "NON_TERMINAL");
			printRule("parameter_list : parameter_list COMMA type_specifier ID");
			printSymbol($$);
			addParam($4->getName(), $3->getName());
		}
		| parameter_list COMMA type_specifier
		{
			$$ = new SymbolInfo($1->getName() + "," + $3->getName(), "NON_TERMINAL");
			printRule("parameter_list : parameter_list COMMA type_specifier");
			printSymbol($$);
			addParam("", $3->getName());
		}
		| type_specifier ID
		{
			$$ = new SymbolInfo($1->getName() + " " + $2->getName(), "NON_TERMINAL");
			printRule("parameter_list : type_specifier ID");
			printSymbol($$);
			addParam($2->getName(), $1->getName());
		}
		| type_specifier
		{
			$$ = $1;
			printRule("parameter_list : type_specifier");
			printSymbol($$);
			addParam("", $1->getName());
		}
		| parameter_list COMMA error ID {
			//Error Recovery
		}
		| error ID {
			//Error Recovery
		}	
     		
var_declaration : type_specifier declaration_list SEMICOLON
		{
			$$ = new SymbolInfo($1->getName() + " " + $2->getName() + ";","NON_TERMINAL");
			printRule("var_declaration : type_specifier declaration_list SEMICOLON");
			printSymbol($$);
			type = "";
		}
		| type_specifier declaration_list error 
		{
			//Error Recovery
		}
 		 ;
 		 
type_specifier	: INT
		{
			$$ = new SymbolInfo("int", "NON_TERMINAL");
			printRule("type_specifier	: INT");
			printSymbol($$);
			type = "INT";
		}
		| FLOAT
		{
			$$ = new SymbolInfo("float", "NON_TERMINAL");
			printRule("type_specifier	: FLOAT");
			printSymbol($$);
			type = "FLOAT";
		}
		| VOID
		{
			$$ = new SymbolInfo("void", "NON_TERMINAL");
			printRule("type_specifier	: VOID");
			printSymbol($$);
			type = "VOID";
		}
 		;
 		
declaration_list : declaration_list COMMA ID
			{
				$$ = new SymbolInfo($1->getName() + ", " + $3->getName(), "NON_TERMINAL");
				printRule("declaration_list : declaration_list COMMA ID");
				printSymbol($$);
				insertVar($3);
			}
			| declaration_list COMMA ID LTHIRD CONST_INT RTHIRD 
			{
				$$ = new SymbolInfo($1->getName() + ", " + $3->getName() + "[" + $5->getName() + "]", "NON_TERMINAL");
				printRule("declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD ");
				printSymbol($$);
				insertArray($3, $5);
			}
 		  	| ID
		   {
			   printRule("declaration_list : ID");
			   printSymbol($$);
			   $1 = insertVar($1);
			   $$ = $1;
		   }
			| ID LTHIRD CONST_INT RTHIRD
			{
				printRule("declaration_list :ID LTHIRD CONST_INT RTHIRD ");
				$$ = new SymbolInfo($1->getName() + "[" + $3->getName() + "]", "NON_TERMINAL" );
				printSymbol($$);
				insertArray($1, $3);
			}
			| ID LTHIRD error RTHIRD
			{
				//Error Recovery
			}
			| declaration_list COMMA ID LTHIRD error RTHIRD 
			{
				//Error Recovery
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
			| expression error 
			{
				//Error Recovery
			}
			| print
			{
				$$ = $1;
			}
			;

expression : logic_expression
			{
				$$ = $1;
				printRule("expression : logic_expression");
				printSymbol($$);
			}
			| variable ASSIGNOP logic_expression
			{
				printRule("expression : variable ASSIGNOP logic_expression");
				$$ = handle_assign($1, $3);
				printSymbol($$);
			}
			;

logic_expression : rel_expression
			{
				$$ = $1;
				printRule("logic_expression : simple_expression");
				printSymbol($$);
			}
			| rel_expression LOGICOP rel_expression
			{
				$$ = new SymbolInfo($1->getName(), "NON_TERMINAL");
				printRule("logic_expression : simple_expression RELOP simple_expression");
				printSymbol($$);

				//TODO
			}
			;

rel_expression : simple_expression
			{
				$$ = $1;
				printRule("rel_expression : simple_expression");
				printSymbol($$);
			}
			| simple_expression RELOP simple_expression
			{
				$$ = handle_RELOP($1, $2, $3);
				printRule("rel_expression : simple_expression RELOP simple_expression");
				printSymbol($$);
			}

simple_expression : term
			{
				$$ = $1;
				printRule("simple_expression : term");
				printSymbol($$);
			}
			| simple_expression ADDOP term
			{
				$$ = handleADDOP($1, $2, $3);
				printRule("simple expression : simple_expression ADDOP term");
				printSymbol($$);
			}
			;

term : unary_expression
			{
				$$ = $1;
				printRule("term : unary_expression");
				printSymbol($$);
			}
			| term MULOP unary_expression
			{

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
			| CONST_INT
			{
				printRule("factor : CONST_INT");
				$$ = getConstVal($1, "INT");
				printSymbol($$);
				
			}
			| CONST_FLOAT
			{
				printRule("factor : CONST_FLOAT");
				$$ = getConstVal($1, "FLOAT");
				printSymbol($$);
					
			}
			| variable INCOP
			{

			}
			| variable DECOP
			{

			}
			| LPAREN expression error
			{

			}
			;

variable : ID
			{
				$$ = getVariable($1);
				printRule("variable : ID");
				printSymbol($$);
			}
			| ID LTHIRD expression RTHIRD
			{
				printRule("variable : ID LTHIRD expression RTHIRD");
				$$ = getArrayIndexVar($1, $3);
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
			};

print: PRINT variable
			{
				$$ = new SymbolInfo ("PRINT" + $2->getName(), "NON_TERMINAL");
				cout<<$2->getIntValue();
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
	st.printAll();
    return 0;

}

