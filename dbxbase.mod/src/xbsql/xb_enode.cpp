/***************************************************************************
    file                 : xb_enode.cpp
    begin                : Wed 07-Mar-01
    copyright            : (C) 2001 by Mike Richardson
    email                : mike@quaking.demon.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; you may use the current    *
 *   version as of 07-Mar-2001, or at your discretion, any later version.  *
 *                                                                         *
 ***************************************************************************/

#include	<stdio.h>
#include	<ctype.h>

#include	"xbsql.h"

extern	const char	*XBSQLConvDate (const char *, char *) ;


bool	xbCaseSensitive = false ;
bool	xbUseWildcard	= false	;

static	bool	sameChar
	(	int	ch1,
		int	ch2
	)
{
	if (!xbCaseSensitive)
	{
		if (isupper(ch1)) ch1 = tolower(ch1) ;
		if (isupper(ch2)) ch2 = tolower(ch2) ;
	}

	return	ch1 == ch2 ;
}

static	char	*operText
	(	XBSQL::EType	oper
	)
{
	switch (oper)
	{
		case XBSQL::EPlus	: return " + "	   ;
		case XBSQL::EMinus	: return " - "     ;
		case XBSQL::EMultiply	: return " * "     ;
		case XBSQL::EDivide	: return " / "     ;
		case XBSQL::ERem	: return " % "     ;
		case XBSQL::ELike	: return " like "  ;
		case XBSQL::EEquals	: return " = "     ;
		case XBSQL::ENotEqual	: return " != "    ;
		case XBSQL::EConcat	: return " || "    ;
		case XBSQL::ELT		: return " < "     ;
		case XBSQL::ELTEqual	: return " <= "    ;
		case XBSQL::EGT		: return " > "     ;
		case XBSQL::EGTEqual	: return " >= "    ;
		case XBSQL::EAnd	: return " and "   ;
		case XBSQL::EOr		: return " or "    ;

		case XBSQL::EFNMin	: return "min"	   ;
		case XBSQL::EFNMax	: return "max"	   ;
		case XBSQL::EFNSum	: return "sum"	   ;
		case XBSQL::EFNCount	: return "count"   ;
		case XBSQL::EFNLower	: return "lower"   ;
		case XBSQL::EFNUpper	: return "upper"   ;
		case XBSQL::EFNNullIF	: return "nullif"  ;
		case XBSQL::EFNToChar	: return "tochar"  ;

		default			: break ;
	}

	return	"<op?>" ;
}

/*  islikeWildcard
/*		: Like match routine with wildcards			*/
/*  str		: const char *	: String to be matched			*/
/*  pat		: const char *	: Pattern string			*/
/*  (returns)	: int		: Non-zero if matched			*/

static	int	islikeWildcard
	(	const char	*str,
		const char	*pat
	)
{
	while (*pat) switch (*pat)
	{
		case '?' :
			/* Match any single character. Fail if the	*/
			/* string is exhausted, otherwise advance the	*/
			/* string and the pattern.			*/
			if (*str++ == 0) return 0 ;
			pat += 1 ;
			break	 ;

		case '*' :
			/* Match (a possibly empty) string. First try	*/
			/* a match with a null string (just advance the	*/
			/* pattern), otherwise advance ths string (but	*/
			/* check for the end).				*/
			if (islikeWildcard (str, &pat[1])) return 1 ;
			if (*str++ == 0) return 0 ;
			break	 ;

		case '[' :
		{	int	mat	= 0	;
			int	cmp	= 0	;
			int	lch	= 256	;
			int	sch	= *str++;

			if (sch == 0) return 0 ;

			/* Advance the patter pointer past the open	*/
			/* square bracket. Then check for the inverse	*/
			/* operator and a special case for the close	*/
			/* bracket.					*/
			pat += 1 ;
			if (*pat == '^') pat += 1, cmp = 1 ;
			if (*pat == ']') pat += 1, mat = sch == ']' ;

			while (*pat && (*pat != ']'))
				if ((*pat == '-') && (pat[1] != ']'))
				{	mat |= (sch >= lch   ) &&
					       (sch <= pat[1]) ;
					pat += 2   ;
					lch  = 256 ;
				}
				else	mat |= sameChar (sch, lch = *pat++) ;

			/* If the match result is the same as the	*/
			/* inverse flag then the match has failed. If	*/
			/* OK then advance the pattern pointer past a	*/
			/* close bracket.				*/
			if (mat == cmp) return 0 ;
			if (*pat) pat += 1 ;
		}
		break	;

		default	 :
			/* Anything else requires an exact match	*/
			/* between the string and the pattern.		*/
			if (!sameChar (*str++, *pat++)) return 0 ;
			break	 ;
	}

	/* End of tne pattern. There is a match if the string is also	*/
	/* exhausted.							*/
	return	*str == 0 ;
}

/*  islikeLike	: Like match routine using conventional like		*/
/*  str		: const char *	: String to be matched			*/
/*  pat		: const char *	: Pattern string			*/
/*  (returns)	: int		: Non-zero if matched			*/

static	int	islikeLike
	(	const char	*str,
		const char	*pat
	)
{
	while (*pat) switch (*pat)
	{
		case '%' :
			/* Match (a possibly empty) string. First try	*/
			/* a match with a null string (just advance the	*/
			/* pattern), otherwise advance ths string (but	*/
			/* check for the end).				*/
			if (islikeLike (str, &pat[1])) return 1 ;
			if (*str++ == 0) return 0 ;
			break	 ;

		default	 :
			/* Anything else requires an exact match	*/
			/* between the string and the pattern.		*/
			if (!sameChar (*str, *pat)) return 0 ;
			str += 1 ;
			pat += 1 ;
			break	 ;
	}

	/* End of tne pattern. There is a match if the string is also	*/
	/* exhausted.							*/
	return	*str == 0 ;
}

/*  islike	: Like match routine					*/
/*  str		: const char *	: String to be matched			*/
/*  pat		: const char *	: Pattern string			*/
/*  (returns)	: int		: Non-zero if matched			*/

static	int	islike
	(	const char	*str,
		const char	*pat
	)
{
	return	xbUseWildcard ?
			islikeWildcard (str, pat) :
			islikeLike     (str, pat) ;
}


/*  XBSQLExprNode							*/
/*  XBSQLExprNode: Constructor for an expression node for name/string	*/
/*  nors	 : const char *	: Field name				*/
/*  string	 : bool		: True for strings			*/
/*  (returns)	 : XBSQLExprNode:					*/

XBSQLExprNode::XBSQLExprNode
	(	const char	*nors,
		bool		string
	)
{
	oper	= string ? XBSQL::EString : XBSQL::EField ;
	text	= nors	 ;
	tabname	= 0	 ;
	left	= 0	 ;
	right	= 0	 ;
	alist	= 0	 ;
	maxtab	= 0	 ;
}

/*  XBSQLExprNode							*/
/*  XBSQLExprNode: Constructor for a alias.name term			*/
/*  name	 : const char *	: Field name				*/
/*  table	 : const char * : Table/alias				*/
/*  string	 : bool		: True for strings			*/
/*  (returns)	 : XBSQLExprNode:					*/

XBSQLExprNode::XBSQLExprNode
	(	const char	*name,
		const char	*table,
		bool		string
	)
{
	oper	= XBSQL::EField ;
	text	= name	 ;
	tabname	= table	 ;
	left	= 0	 ;
	right	= 0	 ;
	alist	= 0	 ;
}

/*  XBSQLExprNode							*/
/*  XBSQLExprNode: Constructor for an binary expression node		*/
/*  left	 : XBSQLExprNode * : Left operand			*/
/*  right	 : XBSQLExprNode * : Right operand			*/
/*  toper	 : XBSQL::EType	   : Operator				*/
/*  (returns)	 : XBSQLExprNode   :					*/

XBSQLExprNode::XBSQLExprNode
	(	XBSQLExprNode	*left,
		XBSQLExprNode	*right,
		XBSQL::EType	oper
	)
	:
	oper	(oper),
	left	(left),
	right	(right),
	alist	(0)
{
	text	= 0	;
	tabname	= 0	;
}

/*  XBSQLExprNode							*/
/*  XBSQLExprNode: Constructor for a function node			*/
/*  alist	 : XBSQLExprList * : Argument list			*/
/*  toper	 : XBSQL::EType	   : Operator				*/
/*  (returns)	 : XBSQLExprNode   :					*/

XBSQLExprNode::XBSQLExprNode
	(	XBSQLExprList	*alist,
		XBSQL::EType	oper
	)
	:
	oper	(oper),
	left	(0),
	right	(0),
	alist	(alist)
{
	text	= 0	;
	tabname	= 0	;
}

/*  XBSQLExprNode							*/
/*  XBSQLExprNode: Constructor for an number or similar node		*/
/*  num		 : int		: Number				*/
/*  oper	 : XBSQL::EType	: Operator				*/
/*  (returns)	 : XBSQLExprNode:					*/

XBSQLExprNode::XBSQLExprNode
	(	int		num,
		XBSQL::EType	oper
	)
	:
	oper	(oper),
	num	(num)
{
	text	= 0 ;
	tabname	= 0 ;
	left	= 0 ;
	right	= 0 ;
	alist	= 0 ;
}

/*  XBSQLExprNode							*/
/*  XBSQLExprNode: Constructor for an number or similar node		*/
/*  dbl		 : double	: Number				*/
/*  oper	 : XBSQL::EType	: Operator				*/
/*  (returns)	 : XBSQLExprNode:					*/

XBSQLExprNode::XBSQLExprNode
	(	double		dbl,
		XBSQL::EType	oper
	)
	:
	oper	(oper),
	dbl	(dbl)
{
	text	= 0 ;
	tabname	= 0 ;
	left	= 0 ;
	right	= 0 ;
	alist	= 0 ;
}

/*  XBSQLExprNode							*/
/*  XBSQLExprNode: Destructor for an expression node			*/
/*  (returns)	 :		:					*/

XBSQLExprNode::~XBSQLExprNode ()
{
	if (left   != 0) delete left   ;
	if (right  != 0) delete right  ;
	if (alist  != 0) delete alist  ;
}

/*  XBSQLExprNode							*/
/*  linkDatabase: Link expression node into the database		*/
/*  _query	: XBSQLQuery *	: Parent query				*/
/*  _hasaggr	: bool &	: Has aggregate functions		*/
/*  _maxtab	: int &		: Return maximum table index		*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLExprNode::linkDatabase
	(	XBSQLQuery	*_query,
		bool		&_hasaggr,
		int		&_maxtab
	)
{
	/* Record the query. This is needed for place holders and for	*/
	/* error reporting.						*/
	query	= _query ;
	maxtab	= -1	 ;

	if (oper == XBSQL::EField)
		if (query->findField (tabname, text, field, maxtab))
		{	if (maxtab > _maxtab) _maxtab = maxtab ;
			return	true	 ;
		}
		else	return	false	 ;


	if ((left  != 0) && !left ->linkDatabase (query, _hasaggr, maxtab))
		return false ;
	if ((right != 0) && !right->linkDatabase (query, _hasaggr, maxtab))
		return false ;

	if ((alist != 0) && !alist->linkDatabase (query, _hasaggr, maxtab))
		return false ;

	if (maxtab > _maxtab) _maxtab = maxtab ;

	switch (oper)
	{
		case XBSQL::EFNMin	:
		case XBSQL::EFNMax	:
		case XBSQL::EFNSum	:
		case XBSQL::EFNCount	:
			_hasaggr	= true	;
			break		;

		default	:
			break		;
	}

	return	true ;
}

/*  XBSQLExprNode							*/
/*  functionSum	: Calculation for sum function				*/
/*  lval	: XBSQLValue &	: Left value				*/
/*  rval	: XBSQLValue &	: Right value				*/
/*  resv	: XBSQLValue &	: Sum value				*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLExprNode::functionSum
	(	XBSQLValue	&lval,
		XBSQLValue	&rval,
		XBSQLValue	&resv
	)
{
	/* If either argument is null then then sum is just the other	*/
	/* one ....							*/
	if (lval.tag == XBSQL::VNull)
	{	resv	= rval	;
		return	true	;
	}
	if (rval.tag == XBSQL::VNull)
	{	resv	= lval	;
		return	true	;
	}

	/* ... otherwise the types must be the same. Note that we will	*/
	/* not get to this point in the code until we have hit at least	*/
	/* one non-null value, at which point the result type is known.	*/
	if (lval.tag != rval.tag)
	{	query->getXBase()->setError ("Type mismatch in sum") ;
		return	false	;
	}

	/* Now work through all the possibilities. The code is such	*/
	/* that, if control reaches the end of the method, then there	*/
	/* is a bug somewhere; we don't assume default cases.		*/
	switch (lval.tag)
	{
		case XBSQL::VNum 	:
			resv	= lval.num + rval.num ;
			return	true	;

		case XBSQL::VDouble	:
			resv	= lval.dbl + rval.dbl ;
			return	true	;

		default	:
			break	;
	}

	/* Ooops. Control should never get here ...			*/
	query->getXBase()->setError ("Unexpected failure in function: sum") ;
	return	false	;
}

/*  XBSQLExprNode							*/
/*  functionMM	: Calculation for min and max functions			*/
/*  lval	: XBSQLValue &	: Left value				*/
/*  rval	: XBSQLValue &	: Right value				*/
/*  resv	: XBSQLValue &	: Sum value				*/
/*  oper	: XBSQL::EType	: Operation (min/max)			*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLExprNode::functionMM
	(	XBSQLValue	&lval,
		XBSQLValue	&rval,
		XBSQLValue	&resv,
		XBSQL::EType	oper
	)
{
	/* If either argument is null then then result is just the	*/
	/* other one ....						*/
	if (lval.tag == XBSQL::VNull)
	{	resv	= rval	;
		return	true	;
	}
	if (rval.tag == XBSQL::VNull)
	{	resv	= lval	;
		return	true	;
	}

	/* ... otherwise the types must be the same. Note that we will	*/
	/* not get to this point in the code until we have hit at least	*/
	/* one non-null value, at which point the result type is known.	*/
	if (lval.tag != rval.tag)
	{	query->getXBase()->setError ("Type mismatch in min/max") ;
		return	false	;
	}

	bool	rLeft	;

	switch (lval.tag)
	{
		case XBSQL::VNum 	:
			rLeft	= lval.num > rval.num ;
			break	;

		case XBSQL::VDouble	:
			rLeft	= lval.dbl > rval.dbl ;
			break	;

		case XBSQL::VText	:
		case XBSQL::VDate	:
			rLeft	= strcmp (lval.text, rval.text) > 0 ;
			break	;

		default	:
			query->getXBase()->setError ("Unexpected failure in function: min/max") ;
			return	false	;
	}

	if (oper == XBSQL::EFNMin) rLeft = !rLeft  ;
	resv = rLeft ? lval : rval ;
	return	true ;
}

/*  XBSQLExprNode							*/
/*  functionArg1: Handle single argument functions			*/
/*  arg		: XBSQLValue &	: Argument				*/
/*  res		: XBSQLValue &	: Result				*/
/*  oper	: XBSQL::EType	: Function				*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLExprNode::functionArg1
	(	XBSQLValue	&arg,
		XBSQLValue	&res,
		XBSQL::EType	oper
	)
{
	if ((oper & arg.tag) == 0)
	{
		query->getXBase()->setError
		(	"Illegal function argument: %s(%C)",
			operText     (oper),
			VTypeToXType (arg.tag)
		)	;
		return	false ;
	}

	switch (oper)
	{
		case XBSQL::EFNUpper	:
			res	= arg.text ;
			for (char *cp0 = &res.text[0] ; *cp0 != 0 ; cp0 += 1)
				if (islower(*cp0)) *cp0 = toupper(*cp0) ;
			return	true	;

		case XBSQL::EFNLower	:
			res	= arg.text ;
			for (char *cp1 = &res.text[0] ; *cp1 != 0 ; cp1 += 1)
				if (isupper(*cp1)) *cp1 = tolower(*cp1) ;
			return	true	;

		default	:
			break	;
	}

	query->getXBase()->setError
	(	"Unrecognised function: %08x",
		oper
	)	;
	return	false	;
}

/*  XBSQLExprNode							*/
/*  evaluate	: Evaluate an expression				*/
/*  value	: XBValue &	: Result location			*/
/*  int		: rowno		: Result row number			*/
/*  (retunrns)	: bool		: Success				*/

bool	XBSQLExprNode::evaluate
	(	XBSQLValue	&value,
		int		rowno
	)
{
	XBSQLValue	temp	;

	/* First set of cases are leaf nodes, which are either		*/
	/* constants (numbers or strings), a field valie from a table,	*/
	/* or a value from a placeholder.				*/
	switch (oper)
	{
		case XBSQL::ENumber	:
			value	= num	;
			return	true	;

		case XBSQL::EDouble	:
			value	= dbl	;
			return	true	;

		case XBSQL::EString	:
			value	= text	;
			return	true	;

		case XBSQL::EField	:
			return	field.getField (value) ;

		case XBSQL::EPlace	:
			return	query->getPlaceValue (num, value) ;

		default	:
			break	;
	}

	/* Next check for aggregate functions. The row number is used	*/
	/* as an aggregate index, so that we can keep separate values	*/
	/* for each row-group of a "select ... group by ..." query.	*/
	switch (oper)
	{
		case XBSQL::EFNMin	:
		case XBSQL::EFNMax	:
			if (!alist->evaluate (temp, rowno))
				return false ;

			if (!functionMM  (agvals.at(rowno), temp, value, oper))
				return	false	;

			agvals.at(rowno)	= value	;
			return	true	;			

		case XBSQL::EFNSum	:
			if (!alist->evaluate (temp, rowno))
				return false ;

			if (!functionSum (agvals.at(rowno), temp, value))
				return	false	;

			agvals.at(rowno)= value	;
			return	true	;

		case XBSQL::EFNCount	:
			value	= agvals.at(rowno) = agvals.at(rowno).num + 1 ;
			return	true	;

		default	:
			break	;
	}

	/* Next handle the non-aggregate function ...			*/
	switch (oper)
	{
		case XBSQL::EFNUpper	:
		case XBSQL::EFNLower	:
			if (!alist->evaluate (temp, rowno))
				return false ;

			return	functionArg1 (temp, value, oper) ;

		case XBSQL::EFNNullIF	:
			if (!alist->evaluate (value, rowno))
				return false ;

			if ((value.tag == XBSQL::VText) && (value.text[0] == 0))
				if (!alist->next->evaluate (value, rowno))
					return false ;

			return	true	;

		case XBSQL::EFNToChar	:
			if (!alist->evaluate (temp, rowno))
				return false ;

			value	= temp.getText() ;
			return	true	;

		default	:
			break	;
	}


	/* Remaining cases are binary operators where we evaluate one	*/
	/* or operands. Start by evaluating each side, then check for	*/
	/* nulls, in which case the result will always be null.		*/
	XBSQLValue lval	; if (!left ->evaluate (lval, rowno)) return false ;

	switch (oper)
	{
		case XBSQL::EAnd :
			if (!lval.isTRUE())
			{	value	= false	;
				return	true	;
			}
			break	;

		case XBSQL::EOr  :
			if ( lval.isTRUE())
			{	value	= true	;
				return	true	;
			}
			break	;

		default	:
			break	;

	}

	XBSQLValue rval	; if (!right->evaluate (rval, rowno)) return false ;

	switch (oper)
	{
		case XBSQL::EAnd :
		case XBSQL::EOr  :
			value	= rval.isTRUE() ;
			return	true ;

		default	:
			break	;

	}

	if ((lval.tag == XBSQL::VNull) || (rval.tag == XBSQL::VNull))
	{
		value	= (void *)0 ;
		return	true ;
	}


	switch (oper)
	{
		case XBSQL::ELT		:
		case XBSQL::ELTEqual	:
		case XBSQL::EGT		:
		case XBSQL::EGTEqual	:
		case XBSQL::EEquals	:
		case XBSQL::ENotEqual	:

			if ((lval.tag == XBSQL::VDate) && (rval.tag == XBSQL::VText))
			{
				char	   temp[10] ;
				const char *e	= XBSQLConvDate (rval.text, temp) ;

				if (e != 0)
				{
					query->getXBase()->setError (e) ;
					return	false ;
				}

				free	((void *)rval.text)	;
				rval.text = strdup (temp)	;
				lval.tag  = XBSQL::VText	;
				break	;
			}

			if ((lval.tag == XBSQL::VText) && (rval.tag == XBSQL::VDate))
			{
				char	   temp[10] ;
				const char *e	= XBSQLConvDate (lval.text, temp) ;

				if (e != 0)
				{
					query->getXBase()->setError (e) ;
					return	false ;
				}

				free	((void *)lval.text)	;
				lval.text = strdup (temp)	;
				rval.tag  = XBSQL::VText	;
				break	;
			}

			break	;

		default	:
			break	;
	}

	/* Next decide on the target type. This is the upper of the	*/
	/* two operand tags, with a few special operator related cases.	*/
	XBSQL::VType	ttag = lval.tag > rval.tag ? lval.tag : rval.tag ;

	switch (oper)
	{
		case XBSQL::EConcat	:
		case XBSQL::ELike	:
			if (ttag < XBSQL::VText) ttag = XBSQL::VText ;
			break	;

		default	:
			break	;
	}

	if ((oper & ttag) == 0)
	{
		extern	xbShort	VTypeToXType (XBSQL::VType) ;
		
		query->getXBase()->setError
		(	"Illegal use of operator: %s on %C",
			operText     (oper),
			VTypeToXType (ttag)
		)	;
		return	false ;
	}

	/* Promote one or other of the operands as neccessary to the	*/
	/* target type.							*/
	if (ttag > lval.tag) lval.promote (ttag) ;
	if (ttag > rval.tag) rval.promote (ttag) ;

	/* Equality and non-equality. Since the types are now the same	*/
	/* we can handle this quite easily.				*/
	if ((oper == XBSQL::EEquals) || (oper == XBSQL::ENotEqual))
	{
		bool	res	= false	;

		switch (ttag)
		{
			case XBSQL::VNum    :
				res = lval.num == rval.num ;
				break	;

			case XBSQL::VDouble :
				res = lval.dbl == rval.dbl ;
				break	;

			case XBSQL::VDate   :
			case XBSQL::VText   :
			case XBSQL::VMemo   :
				if (xbCaseSensitive)
					res = strcmp     (lval.text, rval.text) == 0 ;
				else	res = strcasecmp (lval.text, rval.text) == 0 ;
				break	;

			default		    :
				break	;
		}

		if (oper == XBSQL::ENotEqual) res = !res ;

		value	= res	;
		return	true	;
	}

	/* Now work through all the possibilities. The code is such	*/
	/* that, if control reaches the end of the method, then there	*/
	/* is a bug somewhere; we don't assume default cases.		*/
	if (ttag == XBSQL::VNum)
		switch (oper)
		{
			case XBSQL::EPlus 	:
				value	= lval.num +  rval.num ;
				return	true	;

			case XBSQL::EMinus	:
				value	= lval.num -  rval.num ;
				return	true	;

			case XBSQL::EMultiply	:
				value	= lval.num *  rval.num ;
				return	true	;

			case XBSQL::EDivide	:
				if (rval.num == 0)
				{	query->getXBase()->setError ("Integer division by zero") ;
					return	false ;
				}
				value	= lval.num /  rval.num ;
				return	true	;

			case XBSQL::ERem	:
				if (rval.num == 0)
				{	query->getXBase()->setError ("Integer remainded by zero") ;
					return	false ;
				}
				value	= lval.num %  rval.num ;
				return	true	;

			case XBSQL::ELT		:
				value	= lval.num <  rval.num ;
				return	true	;

			case XBSQL::ELTEqual	:
				value	= lval.num <= rval.num ;
				return	true	;

			case XBSQL::EGT		:
				value	= lval.num >  rval.num ;
				return	true	;

			case XBSQL::EGTEqual	:
				value	= lval.num >= rval.num ;
				return	true	;

			default	:
				break	;
		}
	
	if (ttag == XBSQL::VDouble)
		switch (oper)
		{
			case XBSQL::EPlus 	:
				value	= lval.dbl + rval.dbl ;
				return	true	;

			case XBSQL::EMinus	:
				value	= lval.dbl - rval.dbl ;
				return	true	;

			case XBSQL::EMultiply	:
				value	= lval.dbl * rval.dbl ;
				return	true	;

			case XBSQL::EDivide	:
				if (rval.dbl == 0)
				{	query->getXBase()->setError ("Integer division by zero") ;
					return	false ;
				}
				value	= lval.dbl / rval.dbl ;
				return	true	;

			case XBSQL::ELT		:
				value	= lval.dbl <  rval.dbl ;
				return	true	;

			case XBSQL::ELTEqual	:
				value	= lval.dbl <= rval.dbl ;
				return	true	;

			case XBSQL::EGT		:
				value	= lval.dbl >  rval.dbl ;
				return	true	;

			case XBSQL::EGTEqual	:
				value	= lval.dbl >= rval.dbl ;
				return	true	;

			default	:
				break	;
		}

	if (ttag == XBSQL::VText)
		switch (oper)
		{
			case XBSQL::ELT		:
				value	= strcmp (lval.text, rval.text) < 0  ;
				return	true	;

			case XBSQL::ELTEqual	:
				value	= strcmp (lval.text, rval.text) <= 0 ;
				return	true	;

			case XBSQL::EGT		:
				value	= strcmp (lval.text, rval.text) >  0 ;
				return	true	;

			case XBSQL::EGTEqual	:
				value	= strcmp (lval.text, rval.text) >= 0 ;
				return	true	;

			case XBSQL::ELike	:
				value	= islike (lval.text, rval.text) ;
				return	true	;

			default	:
				break	;
		}

	if (oper == XBSQL::EConcat)
	{
		if (ttag == XBSQL::VText)
		{
			int	llen	= strlen (lval.text) ;
			int	rlen	= strlen (rval.text) ;
			char	*tmp	= (char *) malloc (llen + rlen + 2) ;

			strcpy	(tmp, lval.text) ;
			strcat	(tmp, rval.text) ;
			value	= tmp		 ;
			free	((void *)tmp   ) ;

			return	true ;
		}

		if (ttag == XBSQL::VMemo)
		{
			char	*tmp	= (char *) malloc (lval.len + rval.len + 2) ;
			memcpy	(&tmp[	     0], lval.text, lval.len) ;
			memcpy	(&tmp[lval.len], rval.text, rval.len) ;
			tmp[lval.len + rval.len] = 0 ;
			value	= tmp	;
			free	((void *)tmp   ) ;

			return	true ;
		}
	}

	/* Ooops. Control should never get here ...			*/
	query->getXBase()->setError
	(	"Unexpected failure in expression: %c %s %c",
		VTypeToXType (lval.tag),
		operText     (oper),
		VTypeToXType (rval.tag)
	)	;
	return	false	;
}

/*  XBSQLExprNode							*/
/*  getExprType	: Get expression type					*/
/*  type	: XBSQL::VType &: Result location			*/
/*  (retunrns)	: bool		: Success				*/

bool	XBSQLExprNode::getExprType
	(	XBSQL::VType	&type
	)
{
	XBSQL::VType	type2	;

	/* This is rather like ::evaluate, above, but without any of	*/
	/* the values.							*/
	switch (oper)
	{
		case XBSQL::ENumber	:
			type	= XBSQL::VNum	;
			return	true	;

		case XBSQL::EDouble	:
			type	= XBSQL::VDouble;
			return	true	;

		case XBSQL::EString	:
			type	= XBSQL::VText	;
			return	true	;

		case XBSQL::EField	:
			type	= field.getFieldType () ;
			return	true	;

		case XBSQL::EPlace	:
			type	= query->getPlaceType (num) ;
			return	true	;

		default	:
			break	;
	}

	switch (oper)
	{
		case XBSQL::EFNMin	:
		case XBSQL::EFNMax	:
		case XBSQL::EFNSum	:
			return	alist->getExprType (type) ;

		case XBSQL::EFNCount	:
			type	= XBSQL::VNum	;
			return	true	;

		case XBSQL::EFNUpper	:
		case XBSQL::EFNLower	:
		case XBSQL::EFNToChar	:
			type	= XBSQL::VText	;
			return	true	;

		case XBSQL::EFNNullIF	:
			if (!alist	->getExprType (type )) return false ;
			if (!alist->next->getExprType (type2)) return false ;

			if (type2 != type)
			{
				query->getXBase()->setError
				(	"Mismatched types in nullif(%C,%C)",
					VTypeToXType (type ),
					VTypeToXType (type2)
				)	;
				return	false	;
			}

			return	true	;

		default	:
			break	;
	}

	/* Remaining cases are binary operators where we evaluate both	*/
	/* operands. Start by gettting the types for each.		*/
	XBSQL::VType ltype ; if (!left ->getExprType (ltype)) return false ;
	XBSQL::VType rtype ; if (!right->getExprType (rtype)) return false ;


	/* Next decide on the target type. This is the upper of the	*/
	/* two operand tags, with a few special operator related cases.	*/
	XBSQL::VType ttype = ltype > rtype ? ltype : rtype ;

	switch (oper)
	{
		case XBSQL::EConcat	:
			if (ttype < XBSQL::VText) ttype = XBSQL::VText ;
			break	;

		default	:
			break	;
	}

	if ((oper & ttype) == 0)
	{
		extern	xbShort	VTypeToXType (XBSQL::VType) ;
		
		query->getXBase()->setError
		(	"Illegal use of operator: %s on %C",
			operText     (oper),
			VTypeToXType (ttype)
		)	;
		return	false ;
	}

	type	= ttype	;
	return	true	;
}

/*  XBSQLExprNode							*/
/*  getExprLength: Get expression length				*/
/*  length	 : int &	: Result location			*/
/*  (retunrns)	 : bool		: Success				*/

bool	XBSQLExprNode::getExprLength
	(	int	&length
	)
{
	/* We are actually only interested in field lengths. Anything	*/
	/* else will be returned as zero.				*/
	if (oper == XBSQL::EField)
		length	= field.getFieldLength () ;
	else	length	= 0 ;

	return	true	;
}


/*  XBSQLExprNode							*/
/*  getExprName	 : Get expression name					*/
/*  buff	 : xbString &	: Buffer				*/
/*  (returns)	 : const char *	: Expression name			*/

const char *XBSQLExprNode::getExprName
	(	xbString	&buff
	)
{
	char	t[33]	;

	switch (oper)
	{
		case XBSQL::EField :
			if (tabname != 0)
			{	buff	+= tabname ;
				buff	+= "."	   ;
			}
			buff	+= text	;
			return	buff.getData ()	;

		case XBSQL::ENumber	:
			sprintf	(t, "%d", num) ;
			buff	+= t	;
			return	buff.getData () ;

		case XBSQL::EDouble	:
			buff	+= t	;
			sprintf	(t, "%f", dbl) ;
			return	buff.getData () ;

		case XBSQL::EString	:
			buff	+= "'"	;
			buff	+= text	;
			buff	+= "'"	;
			return	buff.getData () ;

		case XBSQL::EPlace	:
			buff	+= "?"	;
			return	buff.getData () ;

		case XBSQL::EFNMin	:
		case XBSQL::EFNMax	:
		case XBSQL::EFNSum	:
		case XBSQL::EFNLower	:
		case XBSQL::EFNUpper	:
		case XBSQL::EFNToChar	:
		case XBSQL::EFNNullIF	:
			alist->getFuncName (buff, operText (oper)) ;
			return	buff.getData () ;

		case XBSQL::EFNCount	:
			buff	+= "count(*)"	;
			return	buff.getData () ;

		default	:
			break	;
	}

	if (left  != 0) left ->getExprName (buff) ;
	buff	+= operText (oper) ;
	if (right != 0) right->getExprName (buff) ;

	return	buff.getData () ;
}

/*  XBSQLExprNode							*/
/*  indexable	: See if expression can be handled via an index		*/
/*  table	: XBSQLTable *	   : Table being scanned		*/
/*  tabidx	: int		   : Index of the table			*/
/*  expr	: XBSQLExprNode *& : Return location for sub-expression	*/
/*  ftype	: int &		   : XBase field type			*/
/*  (returns)	: XBSQLIndex *	   : Appropriate index or NULL		*/

XBSQLIndex *XBSQLExprNode::indexable
	(	XBSQLTable	*table,
		int		tabidx,
		XBSQLExprNode	*&expr,
		int		&ftype
	)
{
	/* We currently only allow indexing on enquality relations	*/
	/* between table expressions.					*/
	if (oper != XBSQL::EEquals) return 0 ;

	/* On side of the equality must be a field in the table. See if	*/
	/* the right-hand side fits, and if so swap left and right.	*/
	{
		XBSQLExprNode	*l	= left	;
		XBSQLExprNode	*r	= right	;

		if ((r->oper == XBSQL::EField) && (r->field.getTable() == table))
		{
			left	= r	;
			right	= l	;
		}
	}

	/* Now see if the left-hand side is a field in the table. If	*/
	/* not then we cannot index.					*/
	if ((left->oper != XBSQL::EField) || (left->field.getTable() != table))
		return	0 ;

	/* The right-hand side must be an expression which does not	*/
	/* involve a field in this table (ie., it must only get fields	*/
	/* from tables higher up the chain).				*/
	if (right->maxtab >= tabidx)
		return	0 ;

	/* Expression is suitable. We can then index provided that the	*/
	/* left-hand field is indexed, in which case the expression is	*/
	/* set to the right-hand side and we note the field type.	*/
	expr	= right ;
	ftype	= table->GetFieldType (table->GetFieldNo (left->text)) ;

	return	table->indexForField (left->text) ;
}

void	XBSQLExprNode::print
	(	FILE		*fd,
		int		indent
	)
{
	fprintf	(fd, "%*s", indent, "") ;

	/* First set of cases are leaf nodes, which are either		*/
	/* constants (numbers or strings), a field valie from a table,	*/
	/* or a value from a placeholder.				*/
	switch (oper)
	{
		case XBSQL::ENumber	:
			fprintf	(fd, "%d\n", num) ;
			return	;

		case XBSQL::EDouble	:
			fprintf	(fd, "%f\n", dbl) ;
			return	;

		case XBSQL::EString	:
			if (strlen(text) > 32)
				fprintf	(fd, "'%.32s ...'\n", text) ;
			else	fprintf	(fd, "'%s'\n",	    text) ;
			return	;

		case XBSQL::EField	:
			if (tabname != 0) fprintf (fd, "%s.", tabname) ;
			fprintf	(fd, "%s\n", text) ;
			return	;

		case XBSQL::EPlace	:
			fprintf	(fd, "?") ;
			return	;

		default	:
			break	;
	}

	/* Next check for functions ....				*/
	switch (oper)
	{
		case XBSQL::EFNMin	:
			fprintf	(fd, "fn_min\n") ;
			alist->print (fd, indent + 2) ;
			return	;

		case XBSQL::EFNMax	:
			fprintf	(fd, "fn_max\n") ;
			alist->print (fd, indent + 2) ;
			return	;

		case XBSQL::EFNSum	:
			fprintf	(fd, "fn_sum\n") ;
			alist->print (fd, indent + 2) ;
			return	;

		case XBSQL::EFNCount	:
			fprintf	(fd, "fn_count(*)\n") ;
			return	;

		case XBSQL::EFNUpper	:
			fprintf	(fd, "fn_upper\n") ;
			alist->print (fd, indent + 2) ;
			return	;

		case XBSQL::EFNLower	:
			fprintf	(fd, "fn_lower\n") ;
			alist->print (fd, indent + 2) ;
			return	;

		case XBSQL::EFNNullIF	:
			fprintf	(fd, "fn_nullif\n") ;
			alist->print (fd, indent + 2) ;
			return	;

		case XBSQL::EFNToChar	:
			fprintf	(fd, "fn_tochar\n") ;
			alist->print (fd, indent + 2) ;
			return	;

		default	:
			break	;
	}


	/* Remaining cases are binary operators  ...			*/
	fprintf	(fd, "operator%s\n", operText (oper)) ;
	left ->print (fd, indent + 2) ;
	right->print (fd, indent + 2) ;
}
