%option	noyywrap

alpha	[_a-zA-Z]
alnum	[_a-zA-Z0-9]
oct	[0-7]
dec	[0-9]
hex	[0-9a-fA-F]
white	[ \t\r\n]
anyname	[_a-zA-Z0-9 ]
%%

\'			{
			string	('\'')		;
			return	T_STRING	;
			}


"?"			return	T_PLACE		;
","			return	T_COMMA		;
"+"			return	T_PLUS		;
"-"			return	T_MINUS		;
"*"			return	T_MULTIPLY	;
"/"			return	T_DIVIDE	;
"%"			return	T_REM		;
"="			return	T_EQUALS	;
"<>"			return	T_NOTEQUAL	;
"!="			return	T_NOTEQUAL	;
"||"			return	T_CONCAT	;
"("			return	T_BRA		;
")"			return	T_KET		;
"."			return	T_PERIOD	;
"<"			return	T_LT		;
"<="			return	T_LE		;
">"			return	T_GT		;
">="			return	T_GE		;

{alnum}*{alpha}{alnum}*	return	xbLookup (yytext) ;

"["{anyname}+"]"	return	xbWord	 (yytext) ;

{dec}+\.{dec}*		{
			yylval._dbl = atof (yytext) ;
			return	T_REAL	 ;
			}
{dec}+			{
			yylval._num = atoi (yytext) ;
			return	T_NUMBER ;
			}

" "
"\t"
%%

typedef	struct	keyword	KEYWORD	;

struct	keyword
{	KEYWORD	*next	;	/* Link to next in hash chain		*/
	char	*word	;	/* Pointer at the word			*/
	int	code	;	/* Code for the word			*/
}	;

/*  keywds	: Initial table of keywords. Note that this table is	*/
/*		  inserted into the hash table on initialisation.	*/
static	KEYWORD	keywds[] =
{
	{	0,	"select",	T_SELECT	},
	{	0,	"insert",	T_INSERT	},
	{	0,	"update",	T_UPDATE	},
	{	0,	"delete",	T_DELETE	},
	{	0,	"create",	T_CREATE	},
	{	0,	"drop",		T_DROP		},
	{	0,	"where",	T_WHERE		},
	{	0,	"group",	T_GROUP		},
	{	0,	"by",		T_BY		},
	{	0,	"having",	T_HAVING	},
	{	0,	"from",		T_FROM		},
	{	0,	"and",		T_AND		},
	{	0,	"or",		T_OR		},
	{	0,	"into",		T_INTO		},
	{	0,	"values",	T_VALUES	},
	{	0,	"set",		T_SET		},
	{	0,	"as",		T_AS		},
	{	0,	"table",	T_TABLE		},
	{	0,	"integer",	T_INTEGER	},
	{	0,	"int",		T_INTEGER	},
	{	0,	"float",	T_FLOAT		},
	{	0,	"double",	T_FLOAT		},
	{	0,	"char",		T_CHAR		},
	{	0,	"varchar",	T_CHAR		},
	{	0,	"blob",		T_BLOB		},
	{	0,	"date",		T_DATE		},
	{	0,	"index",	T_INDEX		},
	{	0,	"unique",	T_UNIQUE	},
	{	0,	"order",	T_ORDER		},
	{	0,	"by",		T_BY		},
	{	0,	"asc",		T_ASCENDING	},
	{	0,	"desc",		T_DESCENDING	},
	{	0,	"null",		T_NULL		},
	{	0,	"like",		T_LIKE		},

	{	0,	"min",		T_MIN		},
	{	0,	"max",		T_MAX		},
	{	0,	"sum",		T_SUM		},
	{	0,	"count",	T_COUNT		},

	{	0,	"upper",	T_UPPER		},
	{	0,	"lower",	T_LOWER		},
	{	0,	"nullif",	T_NULLIF	},

	{	0,	"tochar",	T_TOCHAR	},

	{	0,	0,		0		}
}	;

#define	HASHSIZ	64
#define	HASHMSK	(HASHSIZ-1)
static	KEYWORD	*hashtab[HASHSIZ] ;

static	void	string
	(	int	qch
	)
{
	int	ch		;
	int	ptr		;
	char	str[10240]	;

	extern	int	yyinput (void) ;

	for (ptr = 0, ch = yyinput () ; (ch != EOF) && (ch != 0) && (ch != qch) ; ch = yyinput ())
	{
		if (ch == '\\')
		{
			ch = yyinput() ;
			if ((ch == EOF) || (ch == 0)) break ;
			switch (ch)
			{	case 'n' : ch = '\n' ; break ;
				case 'r' : ch = '\r' ; break ;
				case 'b' : ch = '\b' ; break ;
				default	 : break ;
			}
		}

		str[ptr++] = ch ;
	}

	str[ptr]    = 0	;


	yylval._str = xbStoreText (str) ;
}


static	int	hashval
	(	const char	*str
	)
{
	int	hval	;
	for (hval = 0 ; *str ; str += 1) hval ^= tolower(*str) * 13 ;
	return	hval & HASHMSK ;
}

static	int	xbLookup
	(	const char	*word
	)
{
	int	hval	= hashval (word) ;
	KEYWORD	*wptr	;

#if	_WIN32
	extern	int	strcasecmp (const char *, const char *) ;
#endif
	/* Scan hash table to see of the word is already there. If so	*/
	/* then this entry will be returned.				*/
	for (wptr = hashtab[hval] ; wptr != NULL ; wptr = wptr->next)
		if (strcasecmp (word, wptr->word) == 0)
			return	wptr->code ;

	yylval._str = xbStoreText (word) ;
	return	T_NAME	 ;

}

static	int	xbWord
	(	const char	*word
	)
{
	/* This allows non-standard names, a. la. Abcess, by enclosing	*/
	/* in [...] brackets, for alphanumerics and space.		*/
	yylval._str = xbStoreText (&word[1]) ;
	yylval._str[strlen(word)-2] = 0 ;
	return	T_NAME	 ;
}


static	int	nextChar ()
{
	return	*qp == 0 ? EOF : *qp++ ;
}

static	int	nextData
	(	char	*buff,
		int	size,
		int	nmemb
	)
{
	int	left	= strlen (qp)  ;
	int	reqd	= size * nmemb ;

	if (left < reqd) reqd = left   ;
	memcpy	(buff, qp, reqd) ;
	qp	+= reqd ;

	return	reqd / size ;
}

/*  initParser	: Initialise parser for new query			*/
/*  xb		: XBaseSQL *	: Database object			*/
/*  query	: const char *	: Query					*/

void	initParser
	(	XBaseSQL	*xb,
		const char	*query
	)
{
	static	int	hInit	;
	if (!hInit)
	{
		int	hval	;
		KEYWORD	*wptr	;

		for (wptr = keywds ; wptr->word ; wptr += 1)
		{
			hval		= hashval (wptr->word) ;
			wptr->next	= hashtab[hval] ;
			hashtab[hval]	= wptr ;
		}

		hInit	= 1	;
	}

	/* To make it easy to store text (names and strings) allocate	*/
	/* a buffer into which such text is stored. If the query is	*/
	/* successfully parsed, the buffer pointer will be saved in the	*/
	/* query object (and cleared here), and freed only when the	*/
	/* query is destroyed.						*/
	if (xbTextb != 0) free ((void *)xbTextb) ;
	xbTextb	= (char *) malloc (strlen(query) * 2 + 2048) ;

	textp		= xbTextb;	/* Buffer allocation pointer	*/
	qp		= query	 ;	/* Query parse pointer		*/
	xbXBaseSQL	= xb	 ;	/* Database object		*/
	xbQuery		= 0	 ;	/* No query yet!		*/
	placeNo		= 0	 ;	/* Place holder number		*/

	/* Reset bison/flex (not that we are actually using stdin, this	*/
	/* just keeps the code happy).					*/
	yyrestart (stdin) ;
}


void	yyerror
	(	const char	*fmt,
		...
	)
{
	printf ("======>%s\n", fmt) ;
}

bool	xbIsKeyword
	(	const char	*word
	)
{
	int	hval	= hashval (word) ;
	KEYWORD	*wptr	;

#if	_WIN32
	extern	int	strcasecmp (const char *, const char *) ;
#endif
	/* Scan hash table to see of the word is already there. If so	*/
	/* then check whether it is a keywork or not.			*/
	for (wptr = hashtab[hval] ; wptr != NULL ; wptr = wptr->next)
		if (strcasecmp (word, wptr->word) == 0)
			return	wptr->code != T_NAME ;

	/* Not in the table so it cannot be a keyword, since these are	*/
	/* all preloaded.						*/
	return	false	;
}

