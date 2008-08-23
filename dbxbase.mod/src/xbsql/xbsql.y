%{
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

%}


%token	T_COMMA
%token	T_SELECT
%token	T_INSERT
%token	T_UPDATE
%token	T_DELETE
%token	T_CREATE
%token	T_DROP
%token	T_FROM
%token	T_WHERE
%token	T_GROUP
%token	T_HAVING
%token	T_BY
%token	T_AND
%token	T_OR
%token	T_INTO
%token	T_VALUES
%token	T_SET
%token	T_AS
%token	T_TABLE
%token	T_PLACE
%token	T_STRING
%token	T_NAME
%token	T_NUMBER
%token	T_REAL
%token	T_PLUS
%token	T_MINUS
%token	T_MULTIPLY
%token	T_DIVIDE
%token	T_REM
%token	T_EQUALS
%token	T_NOTEQUAL
%token	T_LT
%token	T_LE
%token	T_GT
%token	T_GE
%token	T_CONCAT
%token	T_BRA
%token	T_KET
%token	T_PERIOD
%token	T_INTEGER
%token	T_FLOAT
%token	T_CHAR
%token	T_BLOB
%token	T_DATE
%token	T_UNIQUE
%token	T_INDEX
%token	T_ORDER
%token	T_BY
%token	T_ASCENDING
%token	T_DESCENDING
%token	T_NULL
%token	T_LIKE

%token	T_MIN
%token	T_MAX
%token	T_SUM
%token	T_COUNT

%token	T_UPPER
%token	T_LOWER
%token	T_NULLIF

%token	T_TOCHAR

%right	T_COMMA
%left	T_OR
%left	T_AND
%left	T_LIKE T_EQUALS T_NOTEQUAL T_LT T_LE T_GT T_GE
%left	T_CONCAT
%left	T_PLUS T_MINUS
%left	T_MULTIPLY T_DIVIDE T_REM

%union
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

%type	<_str>		T_STRING
%type	<_str>		T_NAME
%type	<_num>		T_NUMBER
%type	<_dbl>		T_REAL
%type	<elist>		selectlist
%type	<elist>		exprlist
%type	<elist>		arglist1
%type	<elist>		arglist2
%type	<elist>		arglistN
%type	<flist>		fieldlist
%type	<enode>		expr
%type	<enode>		binaryexpr
%type	<enode>		funcexpr
%type	<elist>		wherelist
%type	<table>		tablelist
%type	<alist>		assignlist
%type	<elist>		wherepart
%type	<elist>		orderpart
%type	<elist>		orderlist
%type	<elist>		grouppart
%type	<elist>		havingpart
%type	<columns>	columns
%type	<coltype>	coltype
%type	<coltype>	colsize
%type	<_idx>		indexed		
%type	<_num>		ascdesc
%type	<sqlsel>	select
%type	<sqlins>	insert
%type	<sqlupd>	update
%type	<sqldel>	delete
%type	<query>		create
%type	<query>		drop
%type	<query>		query
%%

query:		select	{ $$ = $1 ; } |
		insert	{ $$ = $1 ; } |
		update	{ $$ = $1 ; } |
		delete	{ $$ = $1 ; } |
		create	{ $$ = $1 ; } |
		drop	
		;

select:		T_SELECT selectlist T_FROM tablelist wherepart grouppart havingpart orderpart
		{
			XBSQLSelect *s = new XBSQLSelect (xbXBaseSQL, $2, $4, $5, $6, $7, $8) ;
			xbQuery = s ;
			$$	= s ;
		}
		;

insert:		T_INSERT T_INTO T_NAME T_BRA fieldlist T_KET T_VALUES T_BRA exprlist T_KET
		{
			XBSQLInsert *i = new XBSQLInsert (xbXBaseSQL, $3, $5, $9) ;
			xbQuery = i ;
			$$	= i ;
		}
		|
		T_INSERT T_INTO T_NAME T_VALUES T_BRA exprlist T_KET
		{
			XBSQLInsert *i = new XBSQLInsert (xbXBaseSQL, $3, 0,  $6) ;
			xbQuery = i ;
			$$	= i ;
		}
		|
		T_INSERT T_INTO T_NAME T_BRA fieldlist T_KET select
		{
			XBSQLInsert *i = new XBSQLInsert (xbXBaseSQL, $3, $5, $7) ;
			xbQuery = i ;
			$$	= i ;
		}
		|
		T_INSERT T_INTO T_NAME select
		{
			XBSQLInsert *i = new XBSQLInsert (xbXBaseSQL, $3, 0,  $4) ;
			xbQuery = i ;
			$$	= i ;
		}
		;

delete:		T_DELETE T_FROM T_NAME wherepart
		{
			XBSQLDelete *d = new XBSQLDelete (xbXBaseSQL, $3, $4) ;
			xbQuery = d ;
			$$	= d ;
		}
		;

update:		T_UPDATE T_NAME T_SET assignlist wherepart
		{
			XBSQLUpdate *u = new XBSQLUpdate (xbXBaseSQL, $2, $4, $5) ;
			xbQuery = u ;
			$$	= u ;
		}
		;

create:		T_CREATE T_TABLE T_NAME T_BRA columns T_KET
		{
			xbQuery	= new XBSQLCreate (xbXBaseSQL, $3, $5) ;
			$$	= 0 ;
		}
		;

drop:		T_DROP T_TABLE T_NAME
		{
			xbQuery	= new XBSQLDrop   (xbXBaseSQL, $3) ;
			$$	= 0 ;
		}
		;

columns:	T_NAME coltype
		{
			$$	= new XBSQLColumnList ($1, $2.t, $2.w, $2.p, $2.i,  0) ;
		}
		|
		T_NAME coltype T_COMMA columns
		{
			$$	= new XBSQLColumnList ($1, $2.t, $2.w, $2.p, $2.i, $4) ;
		}	
		;

coltype:	T_INTEGER colsize indexed
		{
			$$.t	= 'N' 	;
			$$.w	= $2.w	;
			$$.p	= $2.p	;
			$$.i	= $3	;
		}
		|
		T_FLOAT colsize indexed
		{
			$$.t	= 'F' 	;
			$$.w	= $2.w	;
			$$.p	= $2.p	;
			$$.i	= $3	;
		}
		|
		T_CHAR colsize indexed
		{
			$$.t	= 'C' 	;
			$$.w	= $2.w	;
			$$.p	= $2.p	;
			$$.i	= $3	;
		}
		|
		T_BLOB colsize
		{
			$$.t	= 'M' 	;
			$$.w	= 10	;
			$$.p	= 0	;
			$$.i	= XBSQL::IndexNone ;
		}
		|
		T_DATE colsize indexed
		{
			$$.t	= 'D' 	;
			$$.w	= $2.w	;
			$$.p	= $2.p	;
			$$.i	= $3	;
		}
		|
		T_DATE colsize
		{
			$$.t	= 'D' 	;
			$$.w	= $2.w	== 0 ? 8 : $2.w ;
			$$.p	= $2.p	;
			$$.i	= XBSQL::IndexNone	;
		}
		;

indexed:	T_INDEX
		{
			$$	= XBSQL::IndexNotUnique	;
		}
		|
		T_UNIQUE T_INDEX
		{
			$$	= XBSQL::IndexUnique	;
		}
		|
		{
			$$	= XBSQL::IndexNone	;
		}
		;

colsize:	T_BRA T_NUMBER T_COMMA T_NUMBER T_KET
		{
			$$.t	= 0	;
			$$.w	= $2	;
			$$.p	= $4	;
		}
		|
		T_BRA T_NUMBER T_KET
		{
			$$.t	= 0	;
			$$.w	= $2	;
			$$.p	= 0	;
		}
		|
		{
			$$.t	= 0	;
			$$.w	= 0	;
			$$.p	= 0	;
		}
		;


wherepart:	T_WHERE  wherelist
		{
			$$ = $2 ;
		}
		|
		{
			$$ =  0 ;
		}
		;

wherelist:	expr
		{
			$$ = new XBSQLExprList ($1,  (const char *)0, 0) ;
		}
		|
		expr T_AND wherelist
		{
			$$ = new XBSQLExprList ($1,  (const char *)0, $3) ;
		}
		;

grouppart:	T_GROUP T_BY arglistN
		{
			$$ = $3 ;
		}
		|
		{
			$$ =  0 ;
		}
		;

havingpart:	T_HAVING expr
		{
			$$ = new XBSQLExprList ($2,  (const char *)0,  0) ;
		}
		|
		{
			$$ =  0 ;
		}
		;

orderpart:	T_ORDER T_BY orderlist
		{
			$$ = $3 ;
		}
		|
		{
			$$ =  0 ;
		}
		;

orderlist:	expr ascdesc
		{
			$$ = new XBSQLExprList ($1, $2,  0) ;
		}
		|
		expr ascdesc T_COMMA orderlist
		{
			$$ = new XBSQLExprList ($1, $2, $4) ;
		}
		;

ascdesc:	T_ASCENDING
		{
			$$ = 1 ;
		}
		|
		T_DESCENDING
		{
			$$ = 0 ;
		}
		|
		{
			$$ = 1 ;
		}
		;

fieldlist:	T_NAME
		{
			$$ = new XBSQLFieldList ($1,  0) ;
		}
		|
		T_NAME T_COMMA fieldlist
		{
			$$ = new XBSQLFieldList ($1, $3) ;
		}
		;

tablelist:	T_NAME
		{
			$$ = new XBSQLTableList ($1, 0,  0) ;
		}
		|
		T_NAME T_NAME
		{
			$$ = new XBSQLTableList ($1, $2, 0) ;
		}
		|
		T_NAME T_COMMA tablelist
		{
			$$ = new XBSQLTableList ($1, 0, $3) ;
		}
		|
		T_NAME T_NAME T_COMMA tablelist
		{
			$$ = new XBSQLTableList ($1, $2, $4) ;
		}
		;

assignlist:	T_NAME T_EQUALS expr
		{
			$$ = new XBSQLAssignList ($1, $3,  0) ;
		}
		|
		T_NAME T_EQUALS expr T_COMMA assignlist
		{
			$$ = new XBSQLAssignList ($1, $3, $5) ;
		}
		;

selectlist:	T_MULTIPLY
		{
			$$ = new XBSQLExprList ( 0,  (const char *)0, 0) ;
		}
		|
		T_MULTIPLY T_COMMA selectlist
		{
			$$ = new XBSQLExprList ( 0,  (const char *)0, $3) ;
		}
		|
		expr
		{
			$$ = new XBSQLExprList ($1,  (const char *)0, 0) ;
		}
		|
		expr T_AS T_NAME
		{
			$$ = new XBSQLExprList ($1,  $3, 0) ;
		}
		|
		expr T_COMMA selectlist
		{
			$$ = new XBSQLExprList ($1,  (const char *)0, $3) ;
		}
		|
		expr T_AS T_NAME T_COMMA selectlist
		{
			$$ = new XBSQLExprList ($1,  $3, $5) ;
		}
		;

exprlist:	expr
		{
			$$ = new XBSQLExprList ($1,  (const char *)0, 0) ;
		}
		|
		expr T_AS T_NAME
		{
			$$ = new XBSQLExprList ($1,  $3, 0) ;
		}
		|
		expr T_COMMA exprlist
		{
			$$ = new XBSQLExprList ($1,  (const char *)0, $3) ;
		}
		|
		expr T_AS T_NAME T_COMMA exprlist
		{
			$$ = new XBSQLExprList ($1,  $3, $5) ;
		}
		;

expr:		binaryexpr
		|
		funcexpr
		|
		T_NUMBER
		{
			$$ = new XBSQLExprNode ($1, XBSQL::ENumber) ;
		}
		|
		T_REAL
		{
			$$ = new XBSQLExprNode ($1, XBSQL::EDouble) ;
		}
		|
		T_STRING
		{
			$$ = new XBSQLExprNode ($1, true ) ;
		}
		|
		T_NAME
		{
			$$ = new XBSQLExprNode ($1, false) ;
		}
		|
		T_NAME T_PERIOD T_NAME
		{
			$$ = new XBSQLExprNode ($3, $1, false) ;
		}
		|
		T_NULL
		{
			$$ = new XBSQLExprNode ("", true ) ;
		}
		|
		T_PLACE
		{	$$ = new XBSQLExprNode (placeNo, XBSQL::EPlace) ;
			placeNo += 1 ;
		}
		|
		T_BRA expr T_KET
		{
			$$ = $2 ;
		}
		;


binaryexpr:	expr T_PLUS expr
		{
			$$ = new XBSQLExprNode ($1, $3, XBSQL::EPlus) ;
		}
		|
		expr T_MINUS expr
		{
			$$ = new XBSQLExprNode ($1, $3, XBSQL::EMinus) ;
		}
		|
		expr T_MULTIPLY expr
		{
			$$ = new XBSQLExprNode ($1, $3, XBSQL::EMultiply) ;
		}
		|
		expr T_DIVIDE expr
		{
			$$ = new XBSQLExprNode ($1, $3, XBSQL::EDivide) ;
		}
		|
		expr T_REM expr
		{
			$$ = new XBSQLExprNode ($1, $3, XBSQL::ERem) ;
		}
		|
		expr T_CONCAT expr
		{
			$$ = new XBSQLExprNode ($1, $3, XBSQL::EConcat) ;
		}
		|
		expr T_LIKE expr
		{
			$$ = new XBSQLExprNode ($1, $3, XBSQL::ELike  ) ;
		}
		|
		expr T_EQUALS expr
		{
			$$ = new XBSQLExprNode ($1, $3, XBSQL::EEquals) ;
		}
		|
		expr T_NOTEQUAL expr
		{
			$$ = new XBSQLExprNode ($1, $3, XBSQL::ENotEqual) ;
		}
		|
		expr T_LT expr
		{
			$$ = new XBSQLExprNode ($1, $3, XBSQL::ELT	) ;
		}
		|
		expr T_LE expr
		{
			$$ = new XBSQLExprNode ($1, $3, XBSQL::ELTEqual	) ;
		}
		|
		expr T_GT expr
		{
			$$ = new XBSQLExprNode ($1, $3, XBSQL::EGT	) ;
		}
		|
		expr T_GE expr
		{
			$$ = new XBSQLExprNode ($1, $3, XBSQL::EGTEqual	) ;
		}
		|
		expr T_AND expr
		{
			$$ = new XBSQLExprNode ($1, $3, XBSQL::EAnd	) ;
		}
		|
		expr T_OR expr
		{
			$$ = new XBSQLExprNode ($1, $3, XBSQL::EOr	) ;
		}
		;

funcexpr:	T_MIN T_BRA arglist1 T_KET
		{
			$$ = new XBSQLExprNode ($3, XBSQL::EFNMin	) ;
		}
		|
		T_MAX T_BRA arglist1 T_KET
		{
			$$ = new XBSQLExprNode ($3, XBSQL::EFNMax	) ;
		}
		|
		T_SUM T_BRA arglist1 T_KET
		{
			$$ = new XBSQLExprNode ($3, XBSQL::EFNSum	) ;
		}
		|
		T_UPPER T_BRA arglist1 T_KET
		{
			$$ = new XBSQLExprNode ($3, XBSQL::EFNUpper	) ;
		}
		|
		T_LOWER T_BRA arglist1 T_KET
		{
			$$ = new XBSQLExprNode ($3, XBSQL::EFNLower	) ;
		}
		|
		T_TOCHAR T_BRA arglist1 T_KET
		{
			$$ = new XBSQLExprNode ($3, XBSQL::EFNToChar	) ;
		}
		|
		T_NULLIF T_BRA arglist2 T_KET
		{
			$$ = new XBSQLExprNode ($3, XBSQL::EFNNullIF	) ;
		}
		|
		T_COUNT T_BRA T_MULTIPLY T_KET
		{
			$$ = new XBSQLExprNode (0,  XBSQL::EFNCount	) ;
		}
		;

arglist1:	expr
		{
			$$ = new XBSQLExprList ($1,  (const char *)0,  0) ;
		}
		;

arglist2:	expr T_COMMA arglist1
		{
			$$ = new XBSQLExprList ($1,  (const char *)0, $3) ;
		}
		;

arglistN:	expr
		{
			$$ = new XBSQLExprList ($1,  (const char *)0,  0) ;
		}
		|
		expr T_COMMA arglistN
		{
			$$ = new XBSQLExprList ($1,  (const char *)0, $3) ;
		}
		;
%%

#include	"lex.yy.c"
