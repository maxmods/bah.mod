/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse xbsql_parse
#define yylex   xbsql_lex
#define yyerror xbsql_error
#define yylval  xbsql_lval
#define yychar  xbsql_char
#define yydebug xbsql_debug
#define yynerrs xbsql_nerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_COMMA = 258,
     T_SELECT = 259,
     T_INSERT = 260,
     T_UPDATE = 261,
     T_DELETE = 262,
     T_CREATE = 263,
     T_DROP = 264,
     T_FROM = 265,
     T_WHERE = 266,
     T_GROUP = 267,
     T_HAVING = 268,
     T_BY = 269,
     T_AND = 270,
     T_OR = 271,
     T_INTO = 272,
     T_VALUES = 273,
     T_SET = 274,
     T_AS = 275,
     T_TABLE = 276,
     T_PLACE = 277,
     T_STRING = 278,
     T_NAME = 279,
     T_NUMBER = 280,
     T_REAL = 281,
     T_PLUS = 282,
     T_MINUS = 283,
     T_MULTIPLY = 284,
     T_DIVIDE = 285,
     T_REM = 286,
     T_EQUALS = 287,
     T_NOTEQUAL = 288,
     T_LT = 289,
     T_LE = 290,
     T_GT = 291,
     T_GE = 292,
     T_CONCAT = 293,
     T_BRA = 294,
     T_KET = 295,
     T_PERIOD = 296,
     T_INTEGER = 297,
     T_FLOAT = 298,
     T_CHAR = 299,
     T_BLOB = 300,
     T_DATE = 301,
     T_UNIQUE = 302,
     T_INDEX = 303,
     T_ORDER = 304,
     T_ASCENDING = 305,
     T_DESCENDING = 306,
     T_NULL = 307,
     T_LIKE = 308,
     T_MIN = 309,
     T_MAX = 310,
     T_SUM = 311,
     T_COUNT = 312,
     T_UPPER = 313,
     T_LOWER = 314,
     T_NULLIF = 315,
     T_TOCHAR = 316
   };
#endif
/* Tokens.  */
#define T_COMMA 258
#define T_SELECT 259
#define T_INSERT 260
#define T_UPDATE 261
#define T_DELETE 262
#define T_CREATE 263
#define T_DROP 264
#define T_FROM 265
#define T_WHERE 266
#define T_GROUP 267
#define T_HAVING 268
#define T_BY 269
#define T_AND 270
#define T_OR 271
#define T_INTO 272
#define T_VALUES 273
#define T_SET 274
#define T_AS 275
#define T_TABLE 276
#define T_PLACE 277
#define T_STRING 278
#define T_NAME 279
#define T_NUMBER 280
#define T_REAL 281
#define T_PLUS 282
#define T_MINUS 283
#define T_MULTIPLY 284
#define T_DIVIDE 285
#define T_REM 286
#define T_EQUALS 287
#define T_NOTEQUAL 288
#define T_LT 289
#define T_LE 290
#define T_GT 291
#define T_GE 292
#define T_CONCAT 293
#define T_BRA 294
#define T_KET 295
#define T_PERIOD 296
#define T_INTEGER 297
#define T_FLOAT 298
#define T_CHAR 299
#define T_BLOB 300
#define T_DATE 301
#define T_UNIQUE 302
#define T_INDEX 303
#define T_ORDER 304
#define T_ASCENDING 305
#define T_DESCENDING 306
#define T_NULL 307
#define T_LIKE 308
#define T_MIN 309
#define T_MAX 310
#define T_SUM 311
#define T_COUNT 312
#define T_UPPER 313
#define T_LOWER 314
#define T_NULLIF 315
#define T_TOCHAR 316




/* Copy the first part of user declarations.  */
#line 1 "xbsql.y"

#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>

#ifdef 		_WIN32
#include 	<io.h>
#endif

#include	"xbsql.h"

extern	void		yyerror	(const char *, ...) ;
extern	int		yylex	()	;
static	void		string	(int)	;
static	int		nextChar()	;
static	int		nextData(char *,  int, int) ;
static	char		*textp		;
static	const char	*qp		;
static	XBaseSQL	*xbXBaseSQL	;
static	int		placeNo		;
static	int		xbLookup(const char *) ;
static	int		xbWord	(const char *) ;

char	*xbTextb	;

char	*xbStoreText
	(	const char	*text
	)
{
	if (text != 0)
	{
		char	*res	= textp	;
		strcpy	(textp, text)	;
		textp	+= strlen (text) + 1 ;
		return	res	;
	}

	return	0 ;
}

typedef	struct
{
	int		t	;
	int		w	;
	int		p	;
	XBSQL::Index	i	;
}	ColType	;

#undef	getc
#define	getc(f)		nextChar()
#define	fread(b,n,l,f)	nextData(b,n,l)	
#define	orzel_read(f,b,l)	nextData(b,1,l)

XBSQLCommand	*xbQuery	;



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 132 "xbsql.y"
{
	XBSQLSelect	*sqlsel	;
	XBSQLInsert	*sqlins	;
	XBSQLUpdate	*sqlupd	;
	XBSQLDelete	*sqldel	;
	XBSQLQuery	*query	;
	XBSQLExprList	*elist	;
	XBSQLExprNode	*enode	;
	XBSQLFieldList	*flist	;
	XBSQLAssignList	*alist	;
	XBSQLTableList	*table	;
	XBSQLColumnList	*columns;
	char		*_str	;
	int		_num	;
	double		_dbl	;
	XBSQL::Index	_idx	;
	ColType		coltype	;
	void		*dummy	;
}
/* Line 193 of yacc.c.  */
#line 303 "xbsql.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 316 "xbsql.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  39
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   492

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  62
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  30
/* YYNRULES -- Number of rules.  */
#define YYNRULES  100
/* YYNRULES -- Number of states.  */
#define YYNSTATES  211

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   316

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    15,    24,
      35,    43,    51,    56,    61,    67,    74,    78,    81,    86,
      90,    94,    98,   101,   105,   108,   110,   113,   114,   120,
     124,   125,   128,   129,   131,   135,   139,   140,   143,   144,
     148,   149,   152,   157,   159,   161,   162,   164,   168,   170,
     173,   177,   182,   186,   192,   194,   198,   200,   204,   208,
     214,   216,   220,   224,   230,   232,   234,   236,   238,   240,
     242,   246,   248,   250,   254,   258,   262,   266,   270,   274,
     278,   282,   286,   290,   294,   298,   302,   306,   310,   314,
     319,   324,   329,   334,   339,   344,   349,   354,   356,   360,
     362
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      63,     0,    -1,    64,    -1,    65,    -1,    67,    -1,    66,
      -1,    68,    -1,    69,    -1,     4,    84,    10,    82,    74,
      76,    77,    78,    -1,     5,    17,    24,    39,    81,    40,
      18,    39,    85,    40,    -1,     5,    17,    24,    18,    39,
      85,    40,    -1,     5,    17,    24,    39,    81,    40,    64,
      -1,     5,    17,    24,    64,    -1,     7,    10,    24,    74,
      -1,     6,    24,    19,    83,    74,    -1,     8,    21,    24,
      39,    70,    40,    -1,     9,    21,    24,    -1,    24,    71,
      -1,    24,    71,     3,    70,    -1,    42,    73,    72,    -1,
      43,    73,    72,    -1,    44,    73,    72,    -1,    45,    73,
      -1,    46,    73,    72,    -1,    46,    73,    -1,    48,    -1,
      47,    48,    -1,    -1,    39,    25,     3,    25,    40,    -1,
      39,    25,    40,    -1,    -1,    11,    75,    -1,    -1,    86,
      -1,    86,    15,    75,    -1,    12,    14,    91,    -1,    -1,
      13,    86,    -1,    -1,    49,    14,    79,    -1,    -1,    86,
      80,    -1,    86,    80,     3,    79,    -1,    50,    -1,    51,
      -1,    -1,    24,    -1,    24,     3,    81,    -1,    24,    -1,
      24,    24,    -1,    24,     3,    82,    -1,    24,    24,     3,
      82,    -1,    24,    32,    86,    -1,    24,    32,    86,     3,
      83,    -1,    29,    -1,    29,     3,    84,    -1,    86,    -1,
      86,    20,    24,    -1,    86,     3,    84,    -1,    86,    20,
      24,     3,    84,    -1,    86,    -1,    86,    20,    24,    -1,
      86,     3,    85,    -1,    86,    20,    24,     3,    85,    -1,
      87,    -1,    88,    -1,    25,    -1,    26,    -1,    23,    -1,
      24,    -1,    24,    41,    24,    -1,    52,    -1,    22,    -1,
      39,    86,    40,    -1,    86,    27,    86,    -1,    86,    28,
      86,    -1,    86,    29,    86,    -1,    86,    30,    86,    -1,
      86,    31,    86,    -1,    86,    38,    86,    -1,    86,    53,
      86,    -1,    86,    32,    86,    -1,    86,    33,    86,    -1,
      86,    34,    86,    -1,    86,    35,    86,    -1,    86,    36,
      86,    -1,    86,    37,    86,    -1,    86,    15,    86,    -1,
      86,    16,    86,    -1,    54,    39,    89,    40,    -1,    55,
      39,    89,    40,    -1,    56,    39,    89,    40,    -1,    58,
      39,    89,    40,    -1,    59,    39,    89,    40,    -1,    61,
      39,    89,    40,    -1,    60,    39,    90,    40,    -1,    57,
      39,    29,    40,    -1,    86,    -1,    86,     3,    89,    -1,
      86,    -1,    86,     3,    91,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   187,   187,   188,   189,   190,   191,   192,   195,   203,
     210,   217,   224,   232,   240,   248,   255,   262,   267,   273,
     281,   289,   297,   305,   313,   322,   327,   332,   337,   344,
     351,   359,   364,   369,   374,   380,   385,   390,   395,   400,
     405,   410,   415,   421,   426,   431,   436,   441,   447,   452,
     457,   462,   468,   473,   479,   484,   489,   494,   499,   504,
     510,   515,   520,   525,   531,   533,   535,   540,   545,   550,
     555,   560,   565,   570,   577,   582,   587,   592,   597,   602,
     607,   612,   617,   622,   627,   632,   637,   642,   647,   653,
     658,   663,   668,   673,   678,   683,   688,   694,   700,   706,
     711
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_COMMA", "T_SELECT", "T_INSERT",
  "T_UPDATE", "T_DELETE", "T_CREATE", "T_DROP", "T_FROM", "T_WHERE",
  "T_GROUP", "T_HAVING", "T_BY", "T_AND", "T_OR", "T_INTO", "T_VALUES",
  "T_SET", "T_AS", "T_TABLE", "T_PLACE", "T_STRING", "T_NAME", "T_NUMBER",
  "T_REAL", "T_PLUS", "T_MINUS", "T_MULTIPLY", "T_DIVIDE", "T_REM",
  "T_EQUALS", "T_NOTEQUAL", "T_LT", "T_LE", "T_GT", "T_GE", "T_CONCAT",
  "T_BRA", "T_KET", "T_PERIOD", "T_INTEGER", "T_FLOAT", "T_CHAR", "T_BLOB",
  "T_DATE", "T_UNIQUE", "T_INDEX", "T_ORDER", "T_ASCENDING",
  "T_DESCENDING", "T_NULL", "T_LIKE", "T_MIN", "T_MAX", "T_SUM", "T_COUNT",
  "T_UPPER", "T_LOWER", "T_NULLIF", "T_TOCHAR", "$accept", "query",
  "select", "insert", "delete", "update", "create", "drop", "columns",
  "coltype", "indexed", "colsize", "wherepart", "wherelist", "grouppart",
  "havingpart", "orderpart", "orderlist", "ascdesc", "fieldlist",
  "tablelist", "assignlist", "selectlist", "exprlist", "expr",
  "binaryexpr", "funcexpr", "arglist1", "arglist2", "arglistN", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    62,    63,    63,    63,    63,    63,    63,    64,    65,
      65,    65,    65,    66,    67,    68,    69,    70,    70,    71,
      71,    71,    71,    71,    71,    72,    72,    72,    73,    73,
      73,    74,    74,    75,    75,    76,    76,    77,    77,    78,
      78,    79,    79,    80,    80,    80,    81,    81,    82,    82,
      82,    82,    83,    83,    84,    84,    84,    84,    84,    84,
      85,    85,    85,    85,    86,    86,    86,    86,    86,    86,
      86,    86,    86,    86,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    88,
      88,    88,    88,    88,    88,    88,    88,    89,    90,    91,
      91
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     8,    10,
       7,     7,     4,     4,     5,     6,     3,     2,     4,     3,
       3,     3,     2,     3,     2,     1,     2,     0,     5,     3,
       0,     2,     0,     1,     3,     3,     0,     2,     0,     3,
       0,     2,     4,     1,     1,     0,     1,     3,     1,     2,
       3,     4,     3,     5,     1,     3,     1,     3,     3,     5,
       1,     3,     3,     5,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     4,
       4,     4,     4,     4,     4,     4,     4,     1,     3,     1,
       3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     2,     3,
       5,     4,     6,     7,    72,    68,    69,    66,    67,    54,
       0,    71,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    56,    64,    65,     0,     0,     0,     0,     0,     1,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    32,     0,    16,    70,    55,    73,    97,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    48,    32,    58,
      87,    88,    57,    74,    75,    76,    77,    78,    81,    82,
      83,    84,    85,    86,    79,    80,     0,     0,    12,     0,
      32,     0,    13,     0,    89,    90,    91,    96,    92,    93,
       0,    95,    94,     0,    49,    36,     0,     0,    46,     0,
       0,    14,    31,    33,     0,     0,    98,    50,     0,     0,
      38,    59,     0,    60,     0,     0,    52,     0,    30,    30,
      30,    30,    30,    17,    15,    51,     0,     0,    40,    10,
       0,     0,    47,     0,    11,     0,    34,    33,     0,    27,
      27,    27,    22,    24,     0,    99,    35,    37,     0,     8,
      62,    61,     0,    53,     0,     0,    25,    19,    20,    21,
      23,    18,     0,     0,     0,     0,     0,    29,    26,   100,
      39,    45,    63,     9,     0,    43,    44,    41,    28,     0,
      42
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     7,     8,     9,    10,    11,    12,    13,   135,   153,
     187,   169,   112,   132,   140,   158,   179,   200,   207,   129,
      88,   110,    30,   142,    77,    32,    33,    78,    85,   176
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -146
static const yytype_int16 yypact[] =
{
      68,   261,   -10,    12,    36,    58,    60,    71,  -146,  -146,
    -146,  -146,  -146,  -146,  -146,  -146,    44,  -146,  -146,    85,
     284,  -146,    69,    70,    77,    78,    79,    80,    82,    88,
      96,    67,  -146,  -146,    83,   104,   100,   111,   118,  -146,
     119,   261,   319,   284,   284,   284,    99,   284,   284,   284,
     284,   120,   261,   284,   284,   121,   284,   284,   284,   284,
     284,   284,   284,   284,   284,   284,   284,   284,   284,    50,
     123,   139,   112,  -146,  -146,  -146,  -146,   346,   114,   117,
     134,   135,   136,   137,   169,   138,   140,    11,   139,  -146,
     439,   427,   149,   -13,   -13,  -146,  -146,  -146,    84,    84,
      84,    84,    84,    84,   103,    84,   143,   155,  -146,   126,
     139,   284,  -146,   159,  -146,  -146,  -146,  -146,  -146,  -146,
     284,  -146,  -146,   120,   170,   175,   261,   284,   185,   150,
     284,  -146,  -146,   373,    95,   154,  -146,  -146,   120,   181,
     176,  -146,   172,   133,   155,    16,   208,   284,   171,   171,
     171,   171,   171,   206,  -146,  -146,   284,   284,   164,  -146,
     284,   190,  -146,   177,  -146,   123,  -146,   400,   192,    -5,
      -5,    -5,  -146,    -5,   159,   244,  -146,   346,   201,  -146,
    -146,   215,   284,  -146,    10,   173,  -146,  -146,  -146,  -146,
    -146,  -146,   284,   284,   284,   179,   195,  -146,  -146,  -146,
    -146,    -6,  -146,  -146,   186,  -146,  -146,   222,  -146,   284,
    -146
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -146,  -146,   -65,  -146,  -146,  -146,  -146,  -146,    53,  -146,
    -132,   -59,   -77,    81,  -146,  -146,  -146,    20,  -146,    86,
     -54,    66,   -40,  -145,    -1,  -146,  -146,   -42,  -146,    40
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -88
static const yytype_int16 yytable[] =
{
      31,    75,    79,    80,   108,    82,    83,    34,    86,    53,
      54,   125,    89,   196,   123,   180,    58,    59,    60,    42,
       1,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,   131,   163,   124,    35,   195,   188,   189,
      31,   190,   185,   186,   205,   206,    36,    68,    84,   202,
     197,    31,    90,    91,     1,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   137,
      52,    39,     1,     2,     3,     4,     5,     6,   136,    37,
     164,    38,    53,    54,   155,    40,   141,    55,    41,   107,
     170,   171,   172,   173,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    51,    69,    43,    44,
     133,    56,    57,    58,    59,    60,    45,    46,    47,    48,
      68,    49,    67,    70,    71,    31,   143,    50,    81,   146,
      56,    57,    58,    59,    60,    72,   160,   148,   149,   150,
     151,   152,    73,    74,    87,    92,   167,   109,    53,    54,
     111,   113,   126,   161,   114,   175,   177,   115,   130,   143,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,   120,   138,   116,   117,   118,   119,   121,   128,
     122,   143,   127,   134,    53,    54,    68,   139,   144,   157,
     145,   175,   201,   143,   154,   156,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,   201,   174,
     168,   165,   159,   178,   181,   193,   182,   184,   194,   203,
     204,   198,    68,    53,    54,   209,   208,   191,   166,   210,
     162,   183,   199,     0,     0,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,   192,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    53,
      54,    68,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    14,    15,    16,    17,    18,     0,     0,
      19,     0,     0,     0,     0,     0,     0,    68,     0,     0,
      20,     0,     0,     0,     0,     0,    14,    15,    16,    17,
      18,     0,     0,    21,     0,    22,    23,    24,    25,    26,
      27,    28,    29,    20,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    53,    54,    21,     0,    22,    23,
      24,    25,    26,    27,    28,    29,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,     0,    76,
       0,    53,    54,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    68,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,     0,     0,     0,   147,    54,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    68,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,     0,     0,     0,   -87,   -87,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    68,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,     0,
       0,     0,    53,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    68,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,     0,     0,
      68,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    68
};

static const yytype_int16 yycheck[] =
{
       1,    41,    44,    45,    69,    47,    48,    17,    50,    15,
      16,    88,    52,     3,     3,   160,    29,    30,    31,    20,
       4,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,   110,    18,    24,    24,   182,   170,   171,
      41,   173,    47,    48,    50,    51,    10,    53,    49,   194,
      40,    52,    53,    54,     4,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    18,   123,
       3,     0,     4,     5,     6,     7,     8,     9,   120,    21,
     145,    21,    15,    16,   138,    41,   126,    20,     3,    39,
     149,   150,   151,   152,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    10,    24,    39,    39,
     111,    27,    28,    29,    30,    31,    39,    39,    39,    39,
      53,    39,    38,    19,    24,   126,   127,    39,    29,   130,
      27,    28,    29,    30,    31,    24,     3,    42,    43,    44,
      45,    46,    24,    24,    24,    24,   147,    24,    15,    16,
      11,    39,     3,    20,    40,   156,   157,    40,    32,   160,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,     3,     3,    40,    40,    40,    40,    40,    24,
      40,   182,    39,    24,    15,    16,    53,    12,     3,    13,
      40,   192,   193,   194,    40,    14,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,   209,     3,
      39,     3,    40,    49,    24,    14,    39,    25,     3,    40,
      25,    48,    53,    15,    16,     3,    40,   174,   147,   209,
     144,   165,   192,    -1,    -1,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,     3,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,
      16,    53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    22,    23,    24,    25,    26,    -1,    -1,
      29,    -1,    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    22,    23,    24,    25,
      26,    -1,    -1,    52,    -1,    54,    55,    56,    57,    58,
      59,    60,    61,    39,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    15,    16,    52,    -1,    54,    55,
      56,    57,    58,    59,    60,    61,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    -1,    40,
      -1,    15,    16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    53,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    -1,    -1,    -1,    15,    16,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    -1,    -1,    -1,    15,    16,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    53,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    -1,
      -1,    -1,    15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    53,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    -1,    -1,
      53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    53
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     5,     6,     7,     8,     9,    63,    64,    65,
      66,    67,    68,    69,    22,    23,    24,    25,    26,    29,
      39,    52,    54,    55,    56,    57,    58,    59,    60,    61,
      84,    86,    87,    88,    17,    24,    10,    21,    21,     0,
      41,     3,    86,    39,    39,    39,    39,    39,    39,    39,
      39,    10,     3,    15,    16,    20,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    53,    24,
      19,    24,    24,    24,    24,    84,    40,    86,    89,    89,
      89,    29,    89,    89,    86,    90,    89,    24,    82,    84,
      86,    86,    24,    86,    86,    86,    86,    86,    86,    86,
      86,    86,    86,    86,    86,    86,    18,    39,    64,    24,
      83,    11,    74,    39,    40,    40,    40,    40,    40,    40,
       3,    40,    40,     3,    24,    74,     3,    39,    24,    81,
      32,    74,    75,    86,    24,    70,    89,    82,     3,    12,
      76,    84,    85,    86,     3,    40,    86,    15,    42,    43,
      44,    45,    46,    71,    40,    82,    14,    13,    77,    40,
       3,    20,    81,    18,    64,     3,    75,    86,    39,    73,
      73,    73,    73,    73,     3,    86,    91,    86,    49,    78,
      85,    24,    39,    83,    25,    47,    48,    72,    72,    72,
      72,    70,     3,    14,     3,    85,     3,    40,    48,    91,
      79,    86,    85,    40,    25,    50,    51,    80,    40,     3,
      79
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

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
#ifndef	YYINITDEPTH
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
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
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
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
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
	    /* Fall through.  */
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

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

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
#line 187 "xbsql.y"
    { (yyval.query) = (yyvsp[(1) - (1)].sqlsel) ; ;}
    break;

  case 3:
#line 188 "xbsql.y"
    { (yyval.query) = (yyvsp[(1) - (1)].sqlins) ; ;}
    break;

  case 4:
#line 189 "xbsql.y"
    { (yyval.query) = (yyvsp[(1) - (1)].sqlupd) ; ;}
    break;

  case 5:
#line 190 "xbsql.y"
    { (yyval.query) = (yyvsp[(1) - (1)].sqldel) ; ;}
    break;

  case 6:
#line 191 "xbsql.y"
    { (yyval.query) = (yyvsp[(1) - (1)].query) ; ;}
    break;

  case 8:
#line 196 "xbsql.y"
    {
			XBSQLSelect *s = new XBSQLSelect (xbXBaseSQL, (yyvsp[(2) - (8)].elist), (yyvsp[(4) - (8)].table), (yyvsp[(5) - (8)].elist), (yyvsp[(6) - (8)].elist), (yyvsp[(7) - (8)].elist), (yyvsp[(8) - (8)].elist)) ;
			xbQuery = s ;
			(yyval.sqlsel)	= s ;
		;}
    break;

  case 9:
#line 204 "xbsql.y"
    {
			XBSQLInsert *i = new XBSQLInsert (xbXBaseSQL, (yyvsp[(3) - (10)]._str), (yyvsp[(5) - (10)].flist), (yyvsp[(9) - (10)].elist)) ;
			xbQuery = i ;
			(yyval.sqlins)	= i ;
		;}
    break;

  case 10:
#line 211 "xbsql.y"
    {
			XBSQLInsert *i = new XBSQLInsert (xbXBaseSQL, (yyvsp[(3) - (7)]._str), 0,  (yyvsp[(6) - (7)].elist)) ;
			xbQuery = i ;
			(yyval.sqlins)	= i ;
		;}
    break;

  case 11:
#line 218 "xbsql.y"
    {
			XBSQLInsert *i = new XBSQLInsert (xbXBaseSQL, (yyvsp[(3) - (7)]._str), (yyvsp[(5) - (7)].flist), (yyvsp[(7) - (7)].sqlsel)) ;
			xbQuery = i ;
			(yyval.sqlins)	= i ;
		;}
    break;

  case 12:
#line 225 "xbsql.y"
    {
			XBSQLInsert *i = new XBSQLInsert (xbXBaseSQL, (yyvsp[(3) - (4)]._str), 0,  (yyvsp[(4) - (4)].sqlsel)) ;
			xbQuery = i ;
			(yyval.sqlins)	= i ;
		;}
    break;

  case 13:
#line 233 "xbsql.y"
    {
			XBSQLDelete *d = new XBSQLDelete (xbXBaseSQL, (yyvsp[(3) - (4)]._str), (yyvsp[(4) - (4)].elist)) ;
			xbQuery = d ;
			(yyval.sqldel)	= d ;
		;}
    break;

  case 14:
#line 241 "xbsql.y"
    {
			XBSQLUpdate *u = new XBSQLUpdate (xbXBaseSQL, (yyvsp[(2) - (5)]._str), (yyvsp[(4) - (5)].alist), (yyvsp[(5) - (5)].elist)) ;
			xbQuery = u ;
			(yyval.sqlupd)	= u ;
		;}
    break;

  case 15:
#line 249 "xbsql.y"
    {
			xbQuery	= new XBSQLCreate (xbXBaseSQL, (yyvsp[(3) - (6)]._str), (yyvsp[(5) - (6)].columns)) ;
			(yyval.query)	= 0 ;
		;}
    break;

  case 16:
#line 256 "xbsql.y"
    {
			xbQuery	= new XBSQLDrop   (xbXBaseSQL, (yyvsp[(3) - (3)]._str)) ;
			(yyval.query)	= 0 ;
		;}
    break;

  case 17:
#line 263 "xbsql.y"
    {
			(yyval.columns)	= new XBSQLColumnList ((yyvsp[(1) - (2)]._str), (yyvsp[(2) - (2)].coltype).t, (yyvsp[(2) - (2)].coltype).w, (yyvsp[(2) - (2)].coltype).p, (yyvsp[(2) - (2)].coltype).i,  0) ;
		;}
    break;

  case 18:
#line 268 "xbsql.y"
    {
			(yyval.columns)	= new XBSQLColumnList ((yyvsp[(1) - (4)]._str), (yyvsp[(2) - (4)].coltype).t, (yyvsp[(2) - (4)].coltype).w, (yyvsp[(2) - (4)].coltype).p, (yyvsp[(2) - (4)].coltype).i, (yyvsp[(4) - (4)].columns)) ;
		;}
    break;

  case 19:
#line 274 "xbsql.y"
    {
			(yyval.coltype).t	= 'N' 	;
			(yyval.coltype).w	= (yyvsp[(2) - (3)].coltype).w	;
			(yyval.coltype).p	= (yyvsp[(2) - (3)].coltype).p	;
			(yyval.coltype).i	= (yyvsp[(3) - (3)]._idx)	;
		;}
    break;

  case 20:
#line 282 "xbsql.y"
    {
			(yyval.coltype).t	= 'F' 	;
			(yyval.coltype).w	= (yyvsp[(2) - (3)].coltype).w	;
			(yyval.coltype).p	= (yyvsp[(2) - (3)].coltype).p	;
			(yyval.coltype).i	= (yyvsp[(3) - (3)]._idx)	;
		;}
    break;

  case 21:
#line 290 "xbsql.y"
    {
			(yyval.coltype).t	= 'C' 	;
			(yyval.coltype).w	= (yyvsp[(2) - (3)].coltype).w	;
			(yyval.coltype).p	= (yyvsp[(2) - (3)].coltype).p	;
			(yyval.coltype).i	= (yyvsp[(3) - (3)]._idx)	;
		;}
    break;

  case 22:
#line 298 "xbsql.y"
    {
			(yyval.coltype).t	= 'M' 	;
			(yyval.coltype).w	= 10	;
			(yyval.coltype).p	= 0	;
			(yyval.coltype).i	= XBSQL::IndexNone ;
		;}
    break;

  case 23:
#line 306 "xbsql.y"
    {
			(yyval.coltype).t	= 'D' 	;
			(yyval.coltype).w	= (yyvsp[(2) - (3)].coltype).w	;
			(yyval.coltype).p	= (yyvsp[(2) - (3)].coltype).p	;
			(yyval.coltype).i	= (yyvsp[(3) - (3)]._idx)	;
		;}
    break;

  case 24:
#line 314 "xbsql.y"
    {
			(yyval.coltype).t	= 'D' 	;
			(yyval.coltype).w	= (yyvsp[(2) - (2)].coltype).w	== 0 ? 8 : (yyvsp[(2) - (2)].coltype).w ;
			(yyval.coltype).p	= (yyvsp[(2) - (2)].coltype).p	;
			(yyval.coltype).i	= XBSQL::IndexNone	;
		;}
    break;

  case 25:
#line 323 "xbsql.y"
    {
			(yyval._idx)	= XBSQL::IndexNotUnique	;
		;}
    break;

  case 26:
#line 328 "xbsql.y"
    {
			(yyval._idx)	= XBSQL::IndexUnique	;
		;}
    break;

  case 27:
#line 332 "xbsql.y"
    {
			(yyval._idx)	= XBSQL::IndexNone	;
		;}
    break;

  case 28:
#line 338 "xbsql.y"
    {
			(yyval.coltype).t	= 0	;
			(yyval.coltype).w	= (yyvsp[(2) - (5)]._num)	;
			(yyval.coltype).p	= (yyvsp[(4) - (5)]._num)	;
		;}
    break;

  case 29:
#line 345 "xbsql.y"
    {
			(yyval.coltype).t	= 0	;
			(yyval.coltype).w	= (yyvsp[(2) - (3)]._num)	;
			(yyval.coltype).p	= 0	;
		;}
    break;

  case 30:
#line 351 "xbsql.y"
    {
			(yyval.coltype).t	= 0	;
			(yyval.coltype).w	= 0	;
			(yyval.coltype).p	= 0	;
		;}
    break;

  case 31:
#line 360 "xbsql.y"
    {
			(yyval.elist) = (yyvsp[(2) - (2)].elist) ;
		;}
    break;

  case 32:
#line 364 "xbsql.y"
    {
			(yyval.elist) =  0 ;
		;}
    break;

  case 33:
#line 370 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ((yyvsp[(1) - (1)].enode),  (const char *)0, 0) ;
		;}
    break;

  case 34:
#line 375 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ((yyvsp[(1) - (3)].enode),  (const char *)0, (yyvsp[(3) - (3)].elist)) ;
		;}
    break;

  case 35:
#line 381 "xbsql.y"
    {
			(yyval.elist) = (yyvsp[(3) - (3)].elist) ;
		;}
    break;

  case 36:
#line 385 "xbsql.y"
    {
			(yyval.elist) =  0 ;
		;}
    break;

  case 37:
#line 391 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ((yyvsp[(2) - (2)].enode),  (const char *)0,  0) ;
		;}
    break;

  case 38:
#line 395 "xbsql.y"
    {
			(yyval.elist) =  0 ;
		;}
    break;

  case 39:
#line 401 "xbsql.y"
    {
			(yyval.elist) = (yyvsp[(3) - (3)].elist) ;
		;}
    break;

  case 40:
#line 405 "xbsql.y"
    {
			(yyval.elist) =  0 ;
		;}
    break;

  case 41:
#line 411 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ((yyvsp[(1) - (2)].enode), (yyvsp[(2) - (2)]._num),  0) ;
		;}
    break;

  case 42:
#line 416 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ((yyvsp[(1) - (4)].enode), (yyvsp[(2) - (4)]._num), (yyvsp[(4) - (4)].elist)) ;
		;}
    break;

  case 43:
#line 422 "xbsql.y"
    {
			(yyval._num) = 1 ;
		;}
    break;

  case 44:
#line 427 "xbsql.y"
    {
			(yyval._num) = 0 ;
		;}
    break;

  case 45:
#line 431 "xbsql.y"
    {
			(yyval._num) = 1 ;
		;}
    break;

  case 46:
#line 437 "xbsql.y"
    {
			(yyval.flist) = new XBSQLFieldList ((yyvsp[(1) - (1)]._str),  0) ;
		;}
    break;

  case 47:
#line 442 "xbsql.y"
    {
			(yyval.flist) = new XBSQLFieldList ((yyvsp[(1) - (3)]._str), (yyvsp[(3) - (3)].flist)) ;
		;}
    break;

  case 48:
#line 448 "xbsql.y"
    {
			(yyval.table) = new XBSQLTableList ((yyvsp[(1) - (1)]._str), 0,  0) ;
		;}
    break;

  case 49:
#line 453 "xbsql.y"
    {
			(yyval.table) = new XBSQLTableList ((yyvsp[(1) - (2)]._str), (yyvsp[(2) - (2)]._str), 0) ;
		;}
    break;

  case 50:
#line 458 "xbsql.y"
    {
			(yyval.table) = new XBSQLTableList ((yyvsp[(1) - (3)]._str), 0, (yyvsp[(3) - (3)].table)) ;
		;}
    break;

  case 51:
#line 463 "xbsql.y"
    {
			(yyval.table) = new XBSQLTableList ((yyvsp[(1) - (4)]._str), (yyvsp[(2) - (4)]._str), (yyvsp[(4) - (4)].table)) ;
		;}
    break;

  case 52:
#line 469 "xbsql.y"
    {
			(yyval.alist) = new XBSQLAssignList ((yyvsp[(1) - (3)]._str), (yyvsp[(3) - (3)].enode),  0) ;
		;}
    break;

  case 53:
#line 474 "xbsql.y"
    {
			(yyval.alist) = new XBSQLAssignList ((yyvsp[(1) - (5)]._str), (yyvsp[(3) - (5)].enode), (yyvsp[(5) - (5)].alist)) ;
		;}
    break;

  case 54:
#line 480 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ( 0,  (const char *)0, 0) ;
		;}
    break;

  case 55:
#line 485 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ( 0,  (const char *)0, (yyvsp[(3) - (3)].elist)) ;
		;}
    break;

  case 56:
#line 490 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ((yyvsp[(1) - (1)].enode),  (const char *)0, 0) ;
		;}
    break;

  case 57:
#line 495 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ((yyvsp[(1) - (3)].enode),  (yyvsp[(3) - (3)]._str), 0) ;
		;}
    break;

  case 58:
#line 500 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ((yyvsp[(1) - (3)].enode),  (const char *)0, (yyvsp[(3) - (3)].elist)) ;
		;}
    break;

  case 59:
#line 505 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ((yyvsp[(1) - (5)].enode),  (yyvsp[(3) - (5)]._str), (yyvsp[(5) - (5)].elist)) ;
		;}
    break;

  case 60:
#line 511 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ((yyvsp[(1) - (1)].enode),  (const char *)0, 0) ;
		;}
    break;

  case 61:
#line 516 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ((yyvsp[(1) - (3)].enode),  (yyvsp[(3) - (3)]._str), 0) ;
		;}
    break;

  case 62:
#line 521 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ((yyvsp[(1) - (3)].enode),  (const char *)0, (yyvsp[(3) - (3)].elist)) ;
		;}
    break;

  case 63:
#line 526 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ((yyvsp[(1) - (5)].enode),  (yyvsp[(3) - (5)]._str), (yyvsp[(5) - (5)].elist)) ;
		;}
    break;

  case 66:
#line 536 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (1)]._num), XBSQL::ENumber) ;
		;}
    break;

  case 67:
#line 541 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (1)]._dbl), XBSQL::EDouble) ;
		;}
    break;

  case 68:
#line 546 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (1)]._str), true ) ;
		;}
    break;

  case 69:
#line 551 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (1)]._str), false) ;
		;}
    break;

  case 70:
#line 556 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(3) - (3)]._str), (yyvsp[(1) - (3)]._str), false) ;
		;}
    break;

  case 71:
#line 561 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ("", true ) ;
		;}
    break;

  case 72:
#line 566 "xbsql.y"
    {	(yyval.enode) = new XBSQLExprNode (placeNo, XBSQL::EPlace) ;
			placeNo += 1 ;
		;}
    break;

  case 73:
#line 571 "xbsql.y"
    {
			(yyval.enode) = (yyvsp[(2) - (3)].enode) ;
		;}
    break;

  case 74:
#line 578 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (3)].enode), (yyvsp[(3) - (3)].enode), XBSQL::EPlus) ;
		;}
    break;

  case 75:
#line 583 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (3)].enode), (yyvsp[(3) - (3)].enode), XBSQL::EMinus) ;
		;}
    break;

  case 76:
#line 588 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (3)].enode), (yyvsp[(3) - (3)].enode), XBSQL::EMultiply) ;
		;}
    break;

  case 77:
#line 593 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (3)].enode), (yyvsp[(3) - (3)].enode), XBSQL::EDivide) ;
		;}
    break;

  case 78:
#line 598 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (3)].enode), (yyvsp[(3) - (3)].enode), XBSQL::ERem) ;
		;}
    break;

  case 79:
#line 603 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (3)].enode), (yyvsp[(3) - (3)].enode), XBSQL::EConcat) ;
		;}
    break;

  case 80:
#line 608 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (3)].enode), (yyvsp[(3) - (3)].enode), XBSQL::ELike  ) ;
		;}
    break;

  case 81:
#line 613 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (3)].enode), (yyvsp[(3) - (3)].enode), XBSQL::EEquals) ;
		;}
    break;

  case 82:
#line 618 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (3)].enode), (yyvsp[(3) - (3)].enode), XBSQL::ENotEqual) ;
		;}
    break;

  case 83:
#line 623 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (3)].enode), (yyvsp[(3) - (3)].enode), XBSQL::ELT	) ;
		;}
    break;

  case 84:
#line 628 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (3)].enode), (yyvsp[(3) - (3)].enode), XBSQL::ELTEqual	) ;
		;}
    break;

  case 85:
#line 633 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (3)].enode), (yyvsp[(3) - (3)].enode), XBSQL::EGT	) ;
		;}
    break;

  case 86:
#line 638 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (3)].enode), (yyvsp[(3) - (3)].enode), XBSQL::EGTEqual	) ;
		;}
    break;

  case 87:
#line 643 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (3)].enode), (yyvsp[(3) - (3)].enode), XBSQL::EAnd	) ;
		;}
    break;

  case 88:
#line 648 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(1) - (3)].enode), (yyvsp[(3) - (3)].enode), XBSQL::EOr	) ;
		;}
    break;

  case 89:
#line 654 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(3) - (4)].elist), XBSQL::EFNMin	) ;
		;}
    break;

  case 90:
#line 659 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(3) - (4)].elist), XBSQL::EFNMax	) ;
		;}
    break;

  case 91:
#line 664 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(3) - (4)].elist), XBSQL::EFNSum	) ;
		;}
    break;

  case 92:
#line 669 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(3) - (4)].elist), XBSQL::EFNUpper	) ;
		;}
    break;

  case 93:
#line 674 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(3) - (4)].elist), XBSQL::EFNLower	) ;
		;}
    break;

  case 94:
#line 679 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(3) - (4)].elist), XBSQL::EFNToChar	) ;
		;}
    break;

  case 95:
#line 684 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode ((yyvsp[(3) - (4)].elist), XBSQL::EFNNullIF	) ;
		;}
    break;

  case 96:
#line 689 "xbsql.y"
    {
			(yyval.enode) = new XBSQLExprNode (0,  XBSQL::EFNCount	) ;
		;}
    break;

  case 97:
#line 695 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ((yyvsp[(1) - (1)].enode),  (const char *)0,  0) ;
		;}
    break;

  case 98:
#line 701 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ((yyvsp[(1) - (3)].enode),  (const char *)0, (yyvsp[(3) - (3)].elist)) ;
		;}
    break;

  case 99:
#line 707 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ((yyvsp[(1) - (1)].enode),  (const char *)0,  0) ;
		;}
    break;

  case 100:
#line 712 "xbsql.y"
    {
			(yyval.elist) = new XBSQLExprList ((yyvsp[(1) - (3)].enode),  (const char *)0, (yyvsp[(3) - (3)].elist)) ;
		;}
    break;


/* Line 1267 of yacc.c.  */
#line 2463 "xbsql.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
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

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
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

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
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
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 716 "xbsql.y"


#include	"lex.yy.c"
