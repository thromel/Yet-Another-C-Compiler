/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IF = 258,
     FOR = 259,
     DO = 260,
     INT = 261,
     FLOAT = 262,
     VOID = 263,
     SWITCH = 264,
     DEFAULT = 265,
     ELSE = 266,
     WHILE = 267,
     BREAK = 268,
     CHAR = 269,
     DOUBLE = 270,
     RETURN = 271,
     CASE = 272,
     CONTINUE = 273,
     INCOP = 274,
     DECOP = 275,
     NOT = 276,
     LPAREN = 277,
     RPAREN = 278,
     LCURL = 279,
     RCURL = 280,
     LTHIRD = 281,
     RTHIRD = 282,
     COMMA = 283,
     SEMICOLON = 284,
     PRINTLN = 285,
     STRING = 286,
     ID = 287,
     CONST_INT = 288,
     CONST_FLOAT = 289,
     CONST_CHAR = 290,
     ADDOP = 291,
     MULOP = 292,
     LOGICOP = 293,
     RELOP = 294,
     BITOP = 295,
     ASSIGNOP = 296,
     second_prec = 297
   };
#endif
/* Tokens.  */
#define IF 258
#define FOR 259
#define DO 260
#define INT 261
#define FLOAT 262
#define VOID 263
#define SWITCH 264
#define DEFAULT 265
#define ELSE 266
#define WHILE 267
#define BREAK 268
#define CHAR 269
#define DOUBLE 270
#define RETURN 271
#define CASE 272
#define CONTINUE 273
#define INCOP 274
#define DECOP 275
#define NOT 276
#define LPAREN 277
#define RPAREN 278
#define LCURL 279
#define RCURL 280
#define LTHIRD 281
#define RTHIRD 282
#define COMMA 283
#define SEMICOLON 284
#define PRINTLN 285
#define STRING 286
#define ID 287
#define CONST_INT 288
#define CONST_FLOAT 289
#define CONST_CHAR 290
#define ADDOP 291
#define MULOP 292
#define LOGICOP 293
#define RELOP 294
#define BITOP 295
#define ASSIGNOP 296
#define second_prec 297




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 15 "./1705069.y"
{
SymbolInfo* symbol;
}
/* Line 1529 of yacc.c.  */
#line 137 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

