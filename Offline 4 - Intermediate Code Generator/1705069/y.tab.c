/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "1705069.y"


#ifndef PARSER
#define PARSER
#include "ParserUtils.h"
#endif

int yyparse(void);
int yylex(void);
extern FILE *yyin;
extern SymbolTable st;


#line 84 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 15 "1705069.y"

SymbolInfo* symbol;

#line 224 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  11
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   167

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  43
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  28
/* YYNRULES -- Number of rules.  */
#define YYNRULES  75
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  134

#define YYUNDEFTOK  2
#define YYMAXUTOK   297


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    67,    67,    85,    93,   103,   109,   115,   123,   130,
     139,   139,   146,   146,   155,   163,   170,   177,   184,   190,
     197,   204,   214,   221,   228,   237,   244,   251,   258,   265,
     273,   282,   288,   296,   303,   309,   315,   321,   327,   333,
     339,   346,   354,   360,   369,   379,   385,   393,   399,   407,
     413,   420,   426,   435,   441,   449,   455,   461,   469,   475,
     482,   489,   495,   501,   508,   514,   522,   528,   536,   536,
     544,   544,   552,   560,   573,   587
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IF", "FOR", "DO", "INT", "FLOAT",
  "VOID", "SWITCH", "DEFAULT", "ELSE", "WHILE", "BREAK", "CHAR", "DOUBLE",
  "RETURN", "CASE", "CONTINUE", "INCOP", "DECOP", "NOT", "LPAREN",
  "RPAREN", "LCURL", "RCURL", "LTHIRD", "RTHIRD", "COMMA", "SEMICOLON",
  "PRINTLN", "STRING", "ID", "CONST_INT", "CONST_FLOAT", "CONST_CHAR",
  "ADDOP", "MULOP", "LOGICOP", "RELOP", "BITOP", "ASSIGNOP", "second_prec",
  "$accept", "start", "program", "unit", "func_declaration",
  "func_definition", "$@1", "$@2", "parameter_list", "var_declaration",
  "type_specifier", "declaration_list", "statements", "statement",
  "expression_statement", "expression", "logic_expression",
  "rel_expression", "simple_expression", "term", "unary_expression",
  "factor", "variable", "compound_statement", "$@3", "$@4",
  "argument_list", "arguments", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297
};
# endif

#define YYPACT_NINF (-77)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-71)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     140,   -77,   -77,   -77,    14,   140,   -77,   -77,   -77,   -77,
     -17,   -77,   -77,    -5,     4,    53,     5,   -77,    -3,   -77,
      26,    48,   -10,    40,    51,    54,    56,   -77,   -77,    62,
      58,   113,   -77,   -77,   -77,    11,    65,   -77,   -77,    62,
      61,    66,    75,    79,   101,    69,   -77,   -77,   -77,   -77,
     -77,    88,    90,    93,   123,   123,   123,   -77,    94,     1,
     -77,   -77,   123,   -77,    86,    67,   -77,   -77,    21,   -77,
      89,    17,    87,   -77,   -77,    43,   -77,   -77,   123,   107,
     123,    97,   -77,    45,    29,   100,   123,   123,   -77,   112,
     -77,   -77,   -77,   -77,   123,   123,   123,   123,   -77,   -77,
     123,   119,   107,   126,   -77,   -77,   -77,   127,   -77,   128,
     124,   131,   -77,    87,   117,   -77,   -77,   101,   123,   101,
     125,   -77,    15,   -77,   149,   138,   -77,   -77,   -77,   -77,
     101,   101,   -77,   -77
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    22,    23,    24,     0,     2,     4,     7,     6,     5,
       0,     1,     3,    27,     0,     0,     0,    21,     0,    20,
       0,    10,     0,    17,     0,     0,    25,    19,     9,     0,
      12,     0,    16,    29,    28,     0,    68,    11,     8,     0,
       0,    15,     0,     0,     0,     0,    13,    18,    14,    30,
      26,     0,     0,     0,     0,     0,     0,    42,     0,    66,
      59,    60,     0,    33,     0,     0,    31,    34,     0,    45,
      47,    49,    51,    53,    55,    58,    35,    71,     0,     0,
       0,     0,    57,    58,     0,     0,     0,     0,    56,    27,
      69,    32,    44,    43,     0,     0,     0,     0,    61,    62,
       0,     0,     0,     0,    41,    64,    63,     0,    74,     0,
      72,     0,    48,    52,    50,    54,    46,     0,     0,     0,
       0,    65,     0,    67,    37,     0,    39,    40,    75,    73,
       0,     0,    38,    36
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -77,   -77,   -77,   157,   -77,   -77,   -77,   -77,   -77,    20,
      80,   -77,   -77,   -62,   -68,   -52,   -76,    70,    71,    68,
     -54,   -77,   -55,   -20,   -77,   -77,   -77,   -77
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     4,     5,     6,     7,     8,    29,    39,    22,    63,
      64,    14,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    44,    45,   109,   110
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      83,    82,    81,    91,    84,    17,    24,    83,    88,    37,
     108,   102,    42,    30,    11,    13,   128,    15,    31,    46,
       9,    16,    92,    86,   116,     9,   101,    87,   103,    26,
     105,    83,    18,    19,   118,   111,    55,    56,    25,    83,
      83,    83,    83,   115,    43,    83,   129,    59,    60,    61,
      93,    62,   106,    95,    20,   124,    96,   126,    27,     1,
       2,     3,    98,    99,    98,    99,   125,    83,   132,   133,
      51,    52,    32,     1,     2,     3,    21,    28,    33,    53,
      10,    34,    35,    54,   100,    10,    36,    38,    55,    56,
     -70,    36,    90,    47,    77,    23,    57,    58,    48,    59,
      60,    61,    49,    62,    51,    52,    50,     1,     2,     3,
      78,    41,    79,    53,    40,    80,    85,    54,    89,     1,
       2,     3,    55,    56,    97,    36,   104,    94,    55,    56,
      57,    58,   107,    59,    60,    61,    57,    62,    16,    59,
      60,    61,   117,    62,    55,    56,     1,     2,     3,   119,
     120,   121,   122,    95,   127,    59,    60,    61,   123,    62,
     130,   131,    12,   113,   112,     0,     0,   114
};

static const yytype_int16 yycheck[] =
{
      55,    55,    54,    65,    56,     1,     1,    62,    62,    29,
      86,    79,     1,    23,     0,    32,     1,    22,    28,    39,
       0,    26,     1,    22,   100,     5,    78,    26,    80,    32,
       1,    86,    28,    29,   102,    87,    21,    22,    33,    94,
      95,    96,    97,    97,    33,   100,   122,    32,    33,    34,
      29,    36,    23,    36,     1,   117,    39,   119,    32,     6,
       7,     8,    19,    20,    19,    20,   118,   122,   130,   131,
       3,     4,    32,     6,     7,     8,    23,    29,    27,    12,
       0,    27,    26,    16,    41,     5,    24,    29,    21,    22,
      25,    24,    25,    32,    25,    15,    29,    30,    32,    32,
      33,    34,    27,    36,     3,     4,    27,     6,     7,     8,
      22,    31,    22,    12,     1,    22,    22,    16,    32,     6,
       7,     8,    21,    22,    37,    24,    29,    38,    21,    22,
      29,    30,    32,    32,    33,    34,    29,    36,    26,    32,
      33,    34,    23,    36,    21,    22,     6,     7,     8,    23,
      23,    23,    28,    36,    29,    32,    33,    34,    27,    36,
      11,    23,     5,    95,    94,    -1,    -1,    96
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     6,     7,     8,    44,    45,    46,    47,    48,    52,
      53,     0,    46,    32,    54,    22,    26,     1,    28,    29,
       1,    23,    51,    53,     1,    33,    32,    32,    29,    49,
      23,    28,    32,    27,    27,    26,    24,    66,    29,    50,
       1,    53,     1,    33,    67,    68,    66,    32,    32,    27,
      27,     3,     4,    12,    16,    21,    22,    29,    30,    32,
      33,    34,    36,    52,    53,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    25,    22,    22,
      22,    58,    63,    65,    58,    22,    22,    26,    63,    32,
      25,    56,     1,    29,    38,    36,    39,    37,    19,    20,
      41,    58,    57,    58,    29,     1,    23,    32,    59,    69,
      70,    58,    60,    62,    61,    63,    59,    23,    57,    23,
      23,    23,    28,    27,    56,    58,    56,    29,     1,    59,
      11,    23,    56,    56
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    43,    44,    45,    45,    46,    46,    46,    47,    47,
      49,    48,    50,    48,    51,    51,    51,    51,    51,    51,
      52,    52,    53,    53,    53,    54,    54,    54,    54,    54,
      54,    55,    55,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    57,    57,    57,    58,    58,    59,    59,    60,
      60,    61,    61,    62,    62,    63,    63,    63,    64,    64,
      64,    64,    64,    64,    64,    64,    65,    65,    67,    66,
      68,    66,    69,    70,    70,    70
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     6,     5,
       0,     6,     0,     7,     4,     3,     2,     1,     4,     2,
       3,     3,     1,     1,     1,     3,     6,     1,     4,     4,
       6,     1,     2,     1,     1,     1,     7,     5,     7,     5,
       5,     3,     1,     2,     2,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     2,     2,     1,     1,
       1,     2,     2,     3,     3,     4,     1,     4,     0,     4,
       0,     3,     1,     3,     1,     3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 68 "1705069.y"
        {
		if (!error_count){
			cout<<"No error, generating assembly code"<<endl;
			addDataSegment();
			startCodeSegment();
			printCode((yyval.symbol));
			addPrintFunc();
			endCodeSegment();
		}
		
		st.printAll();
		printRule("start : program");
		printSymbol((yyvsp[0].symbol));
		
	}
#line 1515 "y.tab.c"
    break;

  case 3:
#line 86 "1705069.y"
        {
		(yyval.symbol) = new SymbolInfo((yyvsp[-1].symbol)->getName() + "\n" + (yyvsp[0].symbol)->getName(), "NON_TERMINAL");
		(yyval.symbol)->setCode((yyvsp[-1].symbol)->getCode() + "\n" +(yyvsp[0].symbol)->getCode());
		printRule("program : program unit");
		printSymbol((yyval.symbol));
		
	}
#line 1527 "y.tab.c"
    break;

  case 4:
#line 94 "1705069.y"
        {
		(yyval.symbol) = (yyvsp[0].symbol);
		(yyval.symbol)->setCode((yyvsp[0].symbol)->getCode());
		printRule("program : unit");
		printSymbol((yyval.symbol));
		st.printAll();
	}
#line 1539 "y.tab.c"
    break;

  case 5:
#line 104 "1705069.y"
        {
		(yyval.symbol) = (yyvsp[0].symbol);
		printRule("unit : var_declaration");
		printSymbol((yyval.symbol));
	}
#line 1549 "y.tab.c"
    break;

  case 6:
#line 110 "1705069.y"
        {
		(yyval.symbol) = (yyvsp[0].symbol);
		printRule("unit : func_definition");
		printSymbol((yyval.symbol));
	}
#line 1559 "y.tab.c"
    break;

  case 7:
#line 116 "1705069.y"
        {
		(yyval.symbol) = (yyvsp[0].symbol);
		printRule("unit : func_declaration");
		printSymbol((yyval.symbol));
	}
#line 1569 "y.tab.c"
    break;

  case 8:
#line 124 "1705069.y"
                {
			addFuncDecl((yyvsp[-4].symbol), (yyvsp[-5].symbol));
			(yyval.symbol) = new SymbolInfo((yyvsp[-5].symbol)->getName() + " " + (yyvsp[-4].symbol)->getName() + "(" + (yyvsp[-2].symbol)->getName() + ");", "NON_TERMINAL" );
			printRule("func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON");
			printSymbol((yyval.symbol));
		}
#line 1580 "y.tab.c"
    break;

  case 9:
#line 131 "1705069.y"
                {
			addFuncDecl((yyvsp[-3].symbol), (yyvsp[-4].symbol));
			(yyval.symbol) = new SymbolInfo((yyvsp[-4].symbol)->getName() + " " + (yyvsp[-3].symbol)->getName() + "( );", "NON_TERMINAL" );
			printRule("func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON");
			printSymbol((yyval.symbol));
		}
#line 1591 "y.tab.c"
    break;

  case 10:
#line 139 "1705069.y"
                                                  {addFuncDef((yyvsp[-2].symbol), (yyvsp[-3].symbol));}
#line 1597 "y.tab.c"
    break;

  case 11:
#line 140 "1705069.y"
                {
			(yyval.symbol) = new SymbolInfo((yyvsp[-5].symbol)->getName() + " " + (yyvsp[-4].symbol)->getName() + " ( ) " + (yyvsp[0].symbol)->getName(), "NON_TERMINAL" );
			(yyval.symbol)->setCode((yyvsp[-4].symbol)->getFuncStart() + "\n" + (yyvsp[0].symbol)->getCode() + "\n" + (yyvsp[-4].symbol)->getFuncEnd());
			printRule("func_definition : type_specifier ID LPAREN RPAREN compound_statement");
			printSymbol((yyval.symbol));
		}
#line 1608 "y.tab.c"
    break;

  case 12:
#line 146 "1705069.y"
                                                                  {addFuncDef((yyvsp[-3].symbol),(yyvsp[-4].symbol));}
#line 1614 "y.tab.c"
    break;

  case 13:
#line 147 "1705069.y"
                {
			(yyval.symbol) = new SymbolInfo((yyvsp[-6].symbol)->getName() + " " + (yyvsp[-5].symbol)->getName() + " ( " + (yyvsp[-3].symbol)->getName() +" ) "+ (yyvsp[0].symbol)->getName(), "NON_TERMINAL" );
			(yyval.symbol)->setCode((yyvsp[-5].symbol)->getFuncStart() + "\n" + (yyvsp[0].symbol)->getCode() + "\n" + (yyvsp[-5].symbol)->getFuncEnd());
			printRule("func_definition : type_specifier ID LPAREN parameter_list RPAREN");
			printSymbol((yyval.symbol));
		}
#line 1625 "y.tab.c"
    break;

  case 14:
#line 156 "1705069.y"
                {	

			(yyval.symbol) = new SymbolInfo((yyvsp[-3].symbol)->getName() + "," + (yyvsp[-1].symbol)->getName() + " " + (yyvsp[0].symbol)->getName(), "NON_TERMINAL");
			printRule("parameter_list : parameter_list COMMA type_specifier ID");
			printSymbol((yyval.symbol));
			addParam((yyvsp[0].symbol)->getName(), (yyvsp[-1].symbol)->getName());
		}
#line 1637 "y.tab.c"
    break;

  case 15:
#line 164 "1705069.y"
                {
			(yyval.symbol) = new SymbolInfo((yyvsp[-2].symbol)->getName() + "," + (yyvsp[0].symbol)->getName(), "NON_TERMINAL");
			printRule("parameter_list : parameter_list COMMA type_specifier");
			printSymbol((yyval.symbol));
			addParam("", (yyvsp[0].symbol)->getName());
		}
#line 1648 "y.tab.c"
    break;

  case 16:
#line 171 "1705069.y"
                {
			(yyval.symbol) = new SymbolInfo((yyvsp[-1].symbol)->getName() + " " + (yyvsp[0].symbol)->getName(), "NON_TERMINAL");
			printRule("parameter_list : type_specifier ID");
			printSymbol((yyval.symbol));
			addParam((yyvsp[0].symbol)->getName(), (yyvsp[-1].symbol)->getName());
		}
#line 1659 "y.tab.c"
    break;

  case 17:
#line 178 "1705069.y"
                {
			(yyval.symbol) = (yyvsp[0].symbol);
			printRule("parameter_list : type_specifier");
			printSymbol((yyval.symbol));
			addParam("", (yyvsp[0].symbol)->getName());
		}
#line 1670 "y.tab.c"
    break;

  case 18:
#line 184 "1705069.y"
                                                {
			(yyval.symbol) = new SymbolInfo((yyvsp[-3].symbol)->getName() + "," + " " + "ERROR", "NON_TERMINAL");
			printRule("parameter_list : parameter_list COMMA error ID");
			printSymbol((yyval.symbol));
			printError("Missing type specifier before ID");
		}
#line 1681 "y.tab.c"
    break;

  case 19:
#line 190 "1705069.y"
                           {
			(yyval.symbol) = new SymbolInfo("ERROR " + (yyvsp[0].symbol)->getName(), "NON_TERMINAL");
			printRule("parameter_list : error ID");
			printSymbol((yyval.symbol));
			printError("Missing type specifier before ID");
		}
#line 1692 "y.tab.c"
    break;

  case 20:
#line 198 "1705069.y"
                {
			(yyval.symbol) = new SymbolInfo((yyvsp[-2].symbol)->getName() + " " + (yyvsp[-1].symbol)->getName() + ";","NON_TERMINAL");
			printRule("var_declaration : type_specifier declaration_list SEMICOLON");
			printSymbol((yyval.symbol));
			type = "";
		}
#line 1703 "y.tab.c"
    break;

  case 21:
#line 205 "1705069.y"
                {
			(yyval.symbol) = new SymbolInfo((yyvsp[-2].symbol)->getName() + " " + (yyvsp[-1].symbol)->getName(),"NON_TERMINAL");
			printRule("var_declaration : type_specifier declaration_list error");
			printSymbol((yyval.symbol));
			type = "";
			printError("Missing SEMICOLON");	
		}
#line 1715 "y.tab.c"
    break;

  case 22:
#line 215 "1705069.y"
                {
			(yyval.symbol) = new SymbolInfo("int", "NON_TERMINAL");
			printRule("type_specifier	: INT");
			printSymbol((yyval.symbol));
			type = "INT";
		}
#line 1726 "y.tab.c"
    break;

  case 23:
#line 222 "1705069.y"
                {
			(yyval.symbol) = new SymbolInfo("float", "NON_TERMINAL");
			printRule("type_specifier	: FLOAT");
			printSymbol((yyval.symbol));
			type = "FLOAT";
		}
#line 1737 "y.tab.c"
    break;

  case 24:
#line 229 "1705069.y"
                {
			(yyval.symbol) = new SymbolInfo("void", "NON_TERMINAL");
			printRule("type_specifier	: VOID");
			printSymbol((yyval.symbol));
			type = "VOID";
		}
#line 1748 "y.tab.c"
    break;

  case 25:
#line 238 "1705069.y"
                        {
				(yyval.symbol) = new SymbolInfo((yyvsp[-2].symbol)->getName() + ", " + (yyvsp[0].symbol)->getName(), "NON_TERMINAL");
				printRule("declaration_list : declaration_list COMMA ID");
				printSymbol((yyval.symbol));
				insertVar((yyvsp[0].symbol));
			}
#line 1759 "y.tab.c"
    break;

  case 26:
#line 245 "1705069.y"
                        {
				(yyval.symbol) = new SymbolInfo((yyvsp[-5].symbol)->getName() + ", " + (yyvsp[-3].symbol)->getName() + "[" + (yyvsp[-1].symbol)->getName() + "]", "NON_TERMINAL");
				printRule("declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD ");
				printSymbol((yyval.symbol));
				insertArray((yyvsp[-3].symbol), (yyvsp[-1].symbol));
			}
#line 1770 "y.tab.c"
    break;

  case 27:
#line 252 "1705069.y"
                   {
			   printRule("declaration_list : ID");
			   printSymbol((yyval.symbol));
			   (yyvsp[0].symbol) = insertVar((yyvsp[0].symbol));
			   (yyval.symbol) = (yyvsp[0].symbol);
		   }
#line 1781 "y.tab.c"
    break;

  case 28:
#line 259 "1705069.y"
                        {
				printRule("declaration_list :ID LTHIRD CONST_INT RTHIRD ");
				(yyval.symbol) = new SymbolInfo((yyvsp[-3].symbol)->getName() + "[" + (yyvsp[-1].symbol)->getName() + "]", "NON_TERMINAL" );
				printSymbol((yyval.symbol));
				insertArray((yyvsp[-3].symbol), (yyvsp[-1].symbol));
			}
#line 1792 "y.tab.c"
    break;

  case 29:
#line 266 "1705069.y"
                        {
				printRule("declaration_list :ID LTHIRD error RTHIRD ");
				(yyval.symbol) = new SymbolInfo((yyvsp[-3].symbol)->getName() + "[" + "ERROR" + "]", "NON_TERMINAL" );
				printSymbol((yyval.symbol));
				printError("Missing array size");
				
			}
#line 1804 "y.tab.c"
    break;

  case 30:
#line 274 "1705069.y"
                        {
				printRule("declaration_list : declaration_list COMMA ID LTHIRD error RTHIRD ");
				(yyval.symbol) = new SymbolInfo((yyvsp[-5].symbol)->getName() + ", " + (yyvsp[-3].symbol)->getName() + "[" + "ERROR" + "]", "NON_TERMINAL");
				printSymbol((yyval.symbol));
				printError("Missing array size");
			}
#line 1815 "y.tab.c"
    break;

  case 31:
#line 283 "1705069.y"
                        {
				(yyval.symbol) = (yyvsp[0].symbol);
				printRule("statements : statement");
				printSymbol((yyval.symbol));
			}
#line 1825 "y.tab.c"
    break;

  case 32:
#line 289 "1705069.y"
                        {
				(yyval.symbol) = new SymbolInfo((yyvsp[-1].symbol)->getName() + "\n" + (yyvsp[0].symbol)->getName(), "NON_TERMINAL");
				(yyval.symbol)->setCode((yyvsp[-1].symbol)->getCode() + "\n" + (yyvsp[0].symbol)->getCode());
				printRule("statements : statements statement");
				printSymbol((yyval.symbol));
			}
#line 1836 "y.tab.c"
    break;

  case 33:
#line 297 "1705069.y"
                        {
				(yyval.symbol) = (yyvsp[0].symbol);
				printRule("statement : var_declaration");
				printSymbol((yyval.symbol));

			}
#line 1847 "y.tab.c"
    break;

  case 34:
#line 304 "1705069.y"
                        {
				(yyval.symbol) = (yyvsp[0].symbol);
				printRule("statement : expression_statement");
				printSymbol((yyval.symbol));
			}
#line 1857 "y.tab.c"
    break;

  case 35:
#line 310 "1705069.y"
                        {
				(yyval.symbol) = (yyvsp[0].symbol);
				printRule("statement : compound_statement");
				printSymbol((yyval.symbol));
			}
#line 1867 "y.tab.c"
    break;

  case 36:
#line 316 "1705069.y"
                        {
				(yyval.symbol) = handle_for((yyvsp[-4].symbol), (yyvsp[-3].symbol), (yyvsp[-2].symbol), (yyvsp[0].symbol));
				printRule("FOR LPAREN expression_statement expression_statement expression RPAREN statement");
				printSymbol((yyval.symbol));
			}
#line 1877 "y.tab.c"
    break;

  case 37:
#line 322 "1705069.y"
                        {
				(yyval.symbol) = handle_if((yyvsp[-2].symbol), (yyvsp[0].symbol));
				printRule("IF LPAREN expression RPAREN statement");
				printSymbol((yyval.symbol));
			}
#line 1887 "y.tab.c"
    break;

  case 38:
#line 328 "1705069.y"
                        {
				(yyval.symbol) = handle_if_else((yyvsp[-4].symbol), (yyvsp[-2].symbol), (yyvsp[0].symbol));
				printRule("IF LPAREN expression RPAREN statement ELSE statement");
				printSymbol((yyval.symbol));
			}
#line 1897 "y.tab.c"
    break;

  case 39:
#line 334 "1705069.y"
                        {
				(yyval.symbol) = handle_while((yyvsp[-2].symbol), (yyvsp[0].symbol));
				printRule("WHILE LPAREN expression RPAREN statement");
				printSymbol((yyval.symbol));
			}
#line 1907 "y.tab.c"
    break;

  case 40:
#line 340 "1705069.y"
                        {
				(yyval.symbol) = new SymbolInfo("printf("+ (yyvsp[-2].symbol)->getName() + ");", "NON_TERMINAL");
				printRule("PRINTLN LPAREN ID RPAREN SEMICOLON");
				printSymbol((yyval.symbol));
				handle_print((yyvsp[-2].symbol), (yyval.symbol));
			}
#line 1918 "y.tab.c"
    break;

  case 41:
#line 347 "1705069.y"
                        {
				(yyval.symbol) = handle_return((yyvsp[-1].symbol));
				printRule("RETURN expression SEMICOLON");
				printSymbol((yyval.symbol));
			}
#line 1928 "y.tab.c"
    break;

  case 42:
#line 355 "1705069.y"
                        {
				(yyval.symbol) = new SymbolInfo(" ; ", "NON_TERMINAL");
				printRule("expression_statement : SEMICOLON");
				printSymbol((yyval.symbol));
			}
#line 1938 "y.tab.c"
    break;

  case 43:
#line 361 "1705069.y"
                        {
				(yyval.symbol) = new SymbolInfo((yyvsp[-1].symbol)->getName()+";", "NON_TERMINAL");
				(yyval.symbol)->setAsmVar((yyvsp[-1].symbol)->getAsmVar());
				(yyval.symbol)->setCode((yyvsp[-1].symbol)->getCode());
				vm.freeTempVar((yyvsp[-1].symbol)->getAsmVar());
				printRule("expression_statement : expression SEMICOLON");
				printSymbol((yyval.symbol));
			}
#line 1951 "y.tab.c"
    break;

  case 44:
#line 370 "1705069.y"
                        {
				(yyval.symbol) = new SymbolInfo((yyvsp[-1].symbol)->getName()+"", "NON_TERMINAL");
				(yyval.symbol)->setCode((yyvsp[-1].symbol)->getCode());
				printRule("expression_statement : expression error");
				printSymbol((yyval.symbol));
				printError("Missing SEMICOLON");
			}
#line 1963 "y.tab.c"
    break;

  case 45:
#line 380 "1705069.y"
                        {
				(yyval.symbol) = (yyvsp[0].symbol);
				printRule("expression : logic_expression");
				printSymbol((yyval.symbol));
			}
#line 1973 "y.tab.c"
    break;

  case 46:
#line 386 "1705069.y"
                        {
				printRule("expression : variable ASSIGNOP logic_expression");
				(yyval.symbol) = handle_assign((yyvsp[-2].symbol), (yyvsp[0].symbol));
				printSymbol((yyval.symbol));
			}
#line 1983 "y.tab.c"
    break;

  case 47:
#line 394 "1705069.y"
                        {
				(yyval.symbol) = (yyvsp[0].symbol);
				printRule("logic_expression : rel_expression");
				printSymbol((yyval.symbol));
			}
#line 1993 "y.tab.c"
    break;

  case 48:
#line 400 "1705069.y"
                        {
				(yyval.symbol) = handle_LOGICOP((yyvsp[-2].symbol), (yyvsp[-1].symbol), (yyvsp[0].symbol));
				printRule("logic_expression : rel_expression LOGICOP rel_expression");
				printSymbol((yyval.symbol));
			}
#line 2003 "y.tab.c"
    break;

  case 49:
#line 408 "1705069.y"
                        {
				(yyval.symbol) = (yyvsp[0].symbol);
				printRule("rel_expression : simple_expression");
				printSymbol((yyval.symbol));
			}
#line 2013 "y.tab.c"
    break;

  case 50:
#line 414 "1705069.y"
                        {
				(yyval.symbol) = handle_RELOP((yyvsp[-2].symbol), (yyvsp[-1].symbol), (yyvsp[0].symbol));
				printRule("rel_expression : simple_expression RELOP simple_expression");
				printSymbol((yyval.symbol));
			}
#line 2023 "y.tab.c"
    break;

  case 51:
#line 421 "1705069.y"
                        {
				(yyval.symbol) = (yyvsp[0].symbol);
				printRule("simple_expression : term");
				printSymbol((yyval.symbol));
			}
#line 2033 "y.tab.c"
    break;

  case 52:
#line 427 "1705069.y"
                        {
				(yyval.symbol) = handleADDOP((yyvsp[-2].symbol), (yyvsp[-1].symbol), (yyvsp[0].symbol));
				// $$->setCode($1->getCode() + "\n" + $3->getCode());
				printRule("simple expression : simple_expression ADDOP term");
				printSymbol((yyval.symbol));
			}
#line 2044 "y.tab.c"
    break;

  case 53:
#line 436 "1705069.y"
                        {
				(yyval.symbol) = (yyvsp[0].symbol);
				printRule("term : unary_expression");
				printSymbol((yyval.symbol));
			}
#line 2054 "y.tab.c"
    break;

  case 54:
#line 442 "1705069.y"
                        {
				printRule("term : term MULOP unary_expression");
				(yyval.symbol) = handle_MULOP((yyvsp[-2].symbol), (yyvsp[-1].symbol), (yyvsp[0].symbol));
				printSymbol((yyval.symbol));
			}
#line 2064 "y.tab.c"
    break;

  case 55:
#line 450 "1705069.y"
                        {
				(yyval.symbol) = (yyvsp[0].symbol);
				printRule("unary_expression : factor");
				printSymbol((yyval.symbol));
			}
#line 2074 "y.tab.c"
    break;

  case 56:
#line 456 "1705069.y"
                        {
				(yyval.symbol) = handle_unary_ADDOP((yyvsp[-1].symbol), (yyvsp[0].symbol));
				printRule("unary_expression : ADDOP unary_expression");
				printSymbol((yyval.symbol));
			}
#line 2084 "y.tab.c"
    break;

  case 57:
#line 462 "1705069.y"
                        {
				(yyval.symbol) = handle_NOT((yyvsp[0].symbol));
				printRule("unary_expression : NOT unary_expression");
				printSymbol((yyval.symbol));
			}
#line 2094 "y.tab.c"
    break;

  case 58:
#line 470 "1705069.y"
                        {
				(yyval.symbol) = (yyvsp[0].symbol);
				printRule("factor : variable");
				printSymbol((yyval.symbol));
			}
#line 2104 "y.tab.c"
    break;

  case 59:
#line 476 "1705069.y"
                        {
				printRule("factor : CONST_INT");
				(yyval.symbol) = getConstVal((yyvsp[0].symbol), "INT");
				printSymbol((yyval.symbol));
				
			}
#line 2115 "y.tab.c"
    break;

  case 60:
#line 483 "1705069.y"
                        {
				printRule("factor : CONST_FLOAT");
				(yyval.symbol) = getConstVal((yyvsp[0].symbol), "FLOAT");
				printSymbol((yyval.symbol));
					
			}
#line 2126 "y.tab.c"
    break;

  case 61:
#line 490 "1705069.y"
                        {
				printRule("factor : variable INCOP");
				(yyval.symbol) = handle_INCOP((yyvsp[-1].symbol));
				printSymbol((yyval.symbol));
			}
#line 2136 "y.tab.c"
    break;

  case 62:
#line 496 "1705069.y"
                        {
				printRule("factor: variable DECOP");
				(yyval.symbol) = handle_DECOP((yyvsp[-1].symbol));
				printSymbol((yyval.symbol));
			}
#line 2146 "y.tab.c"
    break;

  case 63:
#line 502 "1705069.y"
                        {
				(yyval.symbol) = (yyvsp[-1].symbol);
				(yyval.symbol)->setName("("+(yyvsp[-1].symbol)->getName()+")");
				printRule("factor : LPAREN expression RPAREN");
				printSymbol((yyval.symbol));
			}
#line 2157 "y.tab.c"
    break;

  case 64:
#line 509 "1705069.y"
                        {
				(yyval.symbol) = new SymbolInfo("(" + (yyvsp[-1].symbol)->getName() + "ERROR", "NON_TERMINAL");
				printRule("factor : LPAREN expression error");
				printError("Missing RPAREN");
			}
#line 2167 "y.tab.c"
    break;

  case 65:
#line 515 "1705069.y"
                        {
				(yyval.symbol) = handle_function((yyvsp[-3].symbol), (yyvsp[-1].symbol));
				printRule("factor : ID LPAREN argument_list RPAREN");
				printSymbol((yyval.symbol));
			}
#line 2177 "y.tab.c"
    break;

  case 66:
#line 523 "1705069.y"
                        {
				(yyval.symbol) = getVariable((yyvsp[0].symbol));
				printRule("variable : ID");
				printSymbol((yyval.symbol));
			}
#line 2187 "y.tab.c"
    break;

  case 67:
#line 529 "1705069.y"
                        {
				printRule("variable : ID LTHIRD expression RTHIRD");
				(yyval.symbol) = getArrayIndexVar((yyvsp[-3].symbol), (yyvsp[-1].symbol));
				printSymbol((yyval.symbol));
			}
#line 2197 "y.tab.c"
    break;

  case 68:
#line 536 "1705069.y"
                           {enterScope();}
#line 2203 "y.tab.c"
    break;

  case 69:
#line 537 "1705069.y"
                        {
				(yyval.symbol) = new SymbolInfo("{\n" + (yyvsp[-1].symbol)->getName() + "\n}\n", "NON_TERMINAL");
				(yyval.symbol)->setCode((yyvsp[-1].symbol)->getCode());
				printRule("compound_statement : LCURL statements RCURL");
				printSymbol((yyval.symbol));
				exitScope();
			}
#line 2215 "y.tab.c"
    break;

  case 70:
#line 544 "1705069.y"
                                {enterScope();}
#line 2221 "y.tab.c"
    break;

  case 71:
#line 545 "1705069.y"
                        {
				(yyval.symbol) = new SymbolInfo("{}", "NON_TERMINAL");
				printRule("compound_statement : LCURL RCURL");
				printSymbol((yyval.symbol));
				exitScope();
			}
#line 2232 "y.tab.c"
    break;

  case 72:
#line 553 "1705069.y"
                        {
				(yyval.symbol) = (yyvsp[0].symbol);
				printRule("argument_list : arguments");
				printSymbol((yyval.symbol));
			}
#line 2242 "y.tab.c"
    break;

  case 73:
#line 561 "1705069.y"
                        {
				(yyval.symbol) = new SymbolInfo((yyvsp[-2].symbol)->getName() + ", " + (yyvsp[0].symbol)->getName(), "NON_TERMINAL");
				printRule("arguments : arguments COMMA logic_expression");
				printSymbol((yyval.symbol));

				if ((yyvsp[0].symbol)->getVarType() == "VOID"){
					printError("Argument cannot be void");
					(yyvsp[0].symbol)->setVarType("FLOAT");
				}
				argTypeList.push_back((yyvsp[0].symbol)->getVarType()); 
				asmArgList.push_back((yyvsp[0].symbol)->getAsmVar());
			}
#line 2259 "y.tab.c"
    break;

  case 74:
#line 574 "1705069.y"
                        {
				(yyval.symbol) = (yyvsp[0].symbol);
				printRule("arguments : logic_expression");
				printSymbol((yyval.symbol));
				
				if ((yyvsp[0].symbol)->getVarType() == "VOID"){
					printError("Argument cannot be void");
					(yyvsp[0].symbol)->setVarType("FLOAT");
				}
				argTypeList.push_back((yyvsp[0].symbol)->getVarType());
				asmArgList.push_back((yyvsp[0].symbol)->getAsmVar()); 

			}
#line 2277 "y.tab.c"
    break;

  case 75:
#line 588 "1705069.y"
                        {
				(yyval.symbol) = new SymbolInfo((yyvsp[-2].symbol)->getName() + ", " + "ERROR", "NON_TERMINAL");
				printRule("arguments : arguments COMMA error");
				printError("Unfinished argument list");
				printSymbol((yyval.symbol));	
			}
#line 2288 "y.tab.c"
    break;


#line 2292 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 597 "1705069.y"

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

