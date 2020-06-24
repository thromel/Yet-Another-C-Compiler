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
%token <symbol> ASSIGNOP

%type <symbol> program
%type <symbol> unit
%type <symbol> start
%type <symbol> type_specifier 
%type <symbol> expression 
%type <symbol> expression_statement
%type <symbol> logic_expression
%type <symbol> rel_expression
%type <symbol> simple_expression
%type <symbol> term
%type <symbol> unary_expression
%type <symbol> factor
%type <symbol> variable
%type <symbol> parameter_list
%type <symbol> declaration_list
%type <symbol> func_declaration
%type <symbol> func_definition
%type <symbol> compound_statement
%type <symbol> var_declaration
%type <symbol> statement
%type <symbol> statements
%type <symbol> argument_list
%type <symbol> arguments

%nonassoc second_prec
%nonassoc ELSE

%%

start : program
	{
		if (!error_count){
			cout<<"No error, generating assembly code"<<endl;
			addDataSegment();
			startCodeSegment();
			printCode($$);
			addPrintFunc();
			endCodeSegment();
		}
		
		st.printAll();
		printRule("start : program");
		printSymbol($1);
		
	}
	;

program : program unit 
	{
		$$ = new SymbolInfo($1->getName() + "\n" + $2->getName(), "NON_TERMINAL");
		$$->setCode($1->getCode() + "\n" +$2->getCode());
		printRule("program : program unit");
		printSymbol($$);
		
	}
	| unit
	{
		$$ = $1;
		$$->setCode($1->getCode());
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
	| func_declaration
	{
		$$ = $1;
		printRule("unit : func_declaration");
		printSymbol($$);
	}
    ;

func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON
		{
			addFuncDecl($2, $1);
			$$ = new SymbolInfo($1->getName() + " " + $2->getName() + "(" + $4->getName() + ");", "NON_TERMINAL" );
			printRule("func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON");
			printSymbol($$);
		}
		| type_specifier ID LPAREN RPAREN SEMICOLON
		{
			addFuncDecl($2, $1);
			$$ = new SymbolInfo($1->getName() + " " + $2->getName() + "( );", "NON_TERMINAL" );
			printRule("func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON");
			printSymbol($$);
		}
		;

func_definition : type_specifier ID LPAREN RPAREN {addFuncDef($2, $1);} compound_statement 
		{
			$$ = new SymbolInfo($1->getName() + " " + $2->getName() + " ( ) " + $6->getName(), "NON_TERMINAL" );
			$$->setCode($2->getFuncStart() + "\n" + $6->getCode() + "\n" + $2->getFuncEnd());
			printRule("func_definition : type_specifier ID LPAREN RPAREN compound_statement");
			printSymbol($$);
		}
		| type_specifier ID LPAREN parameter_list RPAREN  {addFuncDef($2,$1);} compound_statement 
		{
			$$ = new SymbolInfo($1->getName() + " " + $2->getName() + " ( " + $4->getName() +" ) "+ $7->getName(), "NON_TERMINAL" );
			$$->setCode($2->getFuncStart() + "\n" + $7->getCode() + "\n" + $2->getFuncEnd());
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
			$$ = new SymbolInfo($1->getName() + "," + " " + "ERROR", "NON_TERMINAL");
			printRule("parameter_list : parameter_list COMMA error ID");
			printSymbol($$);
			printError("Missing type specifier before ID");
		}
		| error ID {
			$$ = new SymbolInfo("ERROR " + $2->getName(), "NON_TERMINAL");
			printRule("parameter_list : error ID");
			printSymbol($$);
			printError("Missing type specifier before ID");
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
			$$ = new SymbolInfo($1->getName() + " " + $2->getName(),"NON_TERMINAL");
			printRule("var_declaration : type_specifier declaration_list error");
			printSymbol($$);
			type = "";
			printError("Missing SEMICOLON");	
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
				printRule("declaration_list :ID LTHIRD error RTHIRD ");
				$$ = new SymbolInfo($1->getName() + "[" + "ERROR" + "]", "NON_TERMINAL" );
				printSymbol($$);
				printError("Missing array size");
				
			}
			| declaration_list COMMA ID LTHIRD error RTHIRD 
			{
				printRule("declaration_list : declaration_list COMMA ID LTHIRD error RTHIRD ");
				$$ = new SymbolInfo($1->getName() + ", " + $3->getName() + "[" + "ERROR" + "]", "NON_TERMINAL");
				printSymbol($$);
				printError("Missing array size");
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
				$$ = new SymbolInfo($1->getName() + "\n" + $2->getName(), "NON_TERMINAL");
				$$->setCode($1->getCode() + "\n" + $2->getCode());
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
			| FOR LPAREN expression_statement expression_statement expression RPAREN statement
			{
				$$ = new SymbolInfo("for("+$3->getName()+$4->getName()+$5->getName()+")"+$7->getName(), "NON_TERMINAL");
				printRule("FOR LPAREN expression_statement expression_statement expression RPAREN statement");
				printSymbol($$);
			}
			| IF LPAREN expression RPAREN statement %prec second_prec
			{
				$$ = new SymbolInfo("if(" + $3->getName() + ")"+ $5->getName(), "NON_TERMINAL");
				printRule("IF LPAREN expression RPAREN statement");
				printSymbol($$);
			}
			| IF LPAREN expression RPAREN statement ELSE statement
			{
				$$ = new SymbolInfo("if(" + $3->getName() + ")"+ $5->getName() + " else " + $7->getName(), "NON_TERMINAL");
				printRule("IF LPAREN expression RPAREN statement ELSE statement");
				printSymbol($$);
			}
			| WHILE LPAREN expression RPAREN statement
			{
				$$ = new SymbolInfo("while(" + $3->getName() + ") " + $5->getName(), "NON_TERMINAL");
				printRule("WHILE LPAREN expression RPAREN statement");
				printSymbol($$);
			}
			| PRINTLN LPAREN ID RPAREN SEMICOLON
			{
				$$ = new SymbolInfo("printf("+ $3->getName() + ");", "NON_TERMINAL");
				printRule("PRINTLN LPAREN ID RPAREN SEMICOLON");
				printSymbol($$);
				handle_print($3, $$);
			}
			| RETURN expression SEMICOLON
			{
				$$ = new SymbolInfo("return " + $2->getName() + ";", "NON_TERMINAL");
				printRule("RETURN expression SEMICOLON");
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
				$$->setCode($1->getCode());
				printRule("expression_statement : expression SEMICOLON");
				printSymbol($$);
			}
			| expression error 
			{
				$$ = new SymbolInfo($1->getName()+"", "NON_TERMINAL");
				$$->setCode($1->getCode());
				printRule("expression_statement : expression error");
				printSymbol($$);
				printError("Missing SEMICOLON");
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
				printRule("logic_expression : rel_expression");
				printSymbol($$);
			}
			| rel_expression LOGICOP rel_expression
			{
				$$ = handle_LOGICOP($1, $2, $3);
				printRule("logic_expression : rel_expression LOGICOP rel_expression");
				printSymbol($$);
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
				// $$->setCode($1->getCode() + "\n" + $3->getCode());
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
				printRule("term : term MULOP unary_expression");
				$$ = handle_MULOP($1, $2, $3);
				printSymbol($$);
			}
			;

unary_expression : factor
			{
				$$ = $1;
				printRule("unary_expression : factor");
				printSymbol($$);
			}
			| ADDOP unary_expression
			{
				$$ = handle_unary_ADDOP($1, $2);
				printRule("unary_expression : ADDOP unary_expression");
				printSymbol($$);
			}
			| NOT unary_expression
			{
				$$ = handle_NOT($2);
				printRule("unary_expression : NOT unary_expression");
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
				printRule("factor : variable INCOP");
				$$ = handle_INCOP($1);
				printSymbol($$);
			}
			| variable DECOP
			{
				printRule("factor: variable DECOP");
				$$ = handle_DECOP($1);
				printSymbol($$);
			}
			| LPAREN expression error
			{
				$$ = new SymbolInfo("(" + $2->getName() + "ERROR", "NON_TERMINAL");
				printRule("factor : LPAREN expression error");
				printError("Missing RPAREN");
			}
			| ID LPAREN argument_list RPAREN
			{
				$$ = handle_function($1, $3);
				printRule("factor : ID LPAREN argument_list RPAREN");
				printSymbol($$);
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
				$$ = new SymbolInfo("{\n" + $3->getName() + "\n}\n", "NON_TERMINAL");
				$$->setCode($3->getCode());
				printRule("compound_statement : LCURL statements RCURL");
				printSymbol($$);
				exitScope();
			}
			| LCURL {enterScope();} RCURL
			{
				$$ = new SymbolInfo("{}", "NON_TERMINAL");
				printRule("compound_statement : LCURL RCURL");
				printSymbol($$);
				exitScope();
			};

argument_list : arguments
			{
				$$ = $1;
				printRule("argument_list : arguments");
				printSymbol($$);
			}
			;

arguments: arguments COMMA logic_expression
			{
				$$ = new SymbolInfo($1->getName() + ", " + $3->getName(), "NON_TERMINAL");
				printRule("arguments : arguments COMMA logic_expression");
				printSymbol($$);

				if ($3->getVarType() == "VOID"){
					printError("Argument cannot be void");
					$3->setVarType("FLOAT");
				}
				argTypeList.push_back($3->getVarType()); 
			}
			| logic_expression
			{
				$$ = $1;
				printRule("arguments : logic_expression");
				printSymbol($$);
				
				if ($1->getVarType() == "VOID"){
					printError("Argument cannot be void");
					$1->setVarType("FLOAT");
				}
				argTypeList.push_back($1->getVarType()); 

			}
			| arguments COMMA error 
			{
				$$ = new SymbolInfo($1->getName() + ", " + "ERROR", "NON_TERMINAL");
				printRule("arguments : arguments COMMA error");
				printError("Unfinished argument list");
				printSymbol($$);	
			}
			;
 

%%
int main(int argc,char *argv[])
{

    if (argc != 2){
        cout<<"Please provide a file name\n";
    }

    log.open("log.txt", ios::out);
	error.open("error.txt", ios::out);
	code.open("code.asm", ios::out);

    yyin = fopen(argv[1], "r");

    if (yyin == NULL){
        cout<<"Cannot open input file\n";
        exit(1);
    }

    yyparse();
	st.printAll();
    return 0;

}

