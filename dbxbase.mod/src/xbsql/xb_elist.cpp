/***************************************************************************
    file                 : xb_elist.cpp
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

#include	"xbsql.h"


/*  XBSQLExprList							*/
/*  XBSQLExprList: Constructor for expression list class		*/
/*  expr	 : XBSQLExprNode * : Actual expression			*/
/*  asname	 : const char *	   : AS name				*/
/*  next	 : XBSQLExprList * : Further expressions		*/
/*  (returns)	 : XBSQLExprList   :					*/

XBSQLExprList::XBSQLExprList
	(	XBSQLExprNode	*expr,
		const char	*asname,
		XBSQLExprList	*next
	)
	:
	expr	(expr),
	asname	(asname),
	ascend	(false),
	next	(next)
{
	index	= 0 ;
}

/*  XBSQLExprList							*/
/*  XBSQLExprList: Constructor for expression list class		*/
/*  expr	 : XBSQLExprNode * : Actual expression			*/
/*  ascend	 : bool		   : Sort ascending			*/
/*  next	 : XBSQLExprList * : Further expressions		*/
/*  (returns)	 : XBSQLExprList   :					*/

XBSQLExprList::XBSQLExprList
	(	XBSQLExprNode	*expr,
		bool		ascend,
		XBSQLExprList	*next
	)
	:
	expr	(expr),
	asname	(0),
	ascend	(ascend),
	next	(next)
{
	/* This constructor is used for expression in an "order by"	*/
	/* list, whence we dont have "expr AS name" but can have an	*/
	/* ascending or descending ordering.				*/
	index	= 0 ;
}

/*  XBSQLExprList 							*/
/*  ~XBSQLExprList: Destructor for expression list class		*/
/*  (returns)	  : XBSQLExprList :					*/

XBSQLExprList::~XBSQLExprList ()
{
	if (expr != 0) delete expr ;
	if (next != 0) delete next ;
}

/*  XBSQLExprList							*/
/*  linkDatabase: Link expressions into database			*/
/*  query	: XBSQLQuery *	: Parent query				*/
/*  _hasaggr	: bool &	: Has aggregate functions		*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLExprList::linkDatabase
	(	XBSQLQuery	*query,
		bool		&_hasaggr
	)
{
	/* This method is used when traversing the list of expressions	*/
	/* in, say, a select query.					*/
	if (expr != 0)
	{	maxtab	= -1 ;
		if (!expr->linkDatabase (query, _hasaggr, maxtab)) return false ;
#if	0
		xbString   eName ;
		const char *ep = expr->getExprName (eName) ;
		fprintf	(stderr, "%3d: %s\n", maxtab, eName.getData()) ;
#endif
	}
	else	next	= query->getAllColumns (next) ;

	return	next == 0 ? true : next->linkDatabase (query, _hasaggr) ;
}

/*  XBSQLExprList							*/
/*  linkDatabase: Link expressions into database			*/
/*  query	: XBSQLQuery *	: Parent query				*/
/*  _hasaggr	: bool &	: Has aggregate functions		*/
/*  _maxtab	: int &		: Return maximum table index		*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLExprList::linkDatabase
	(	XBSQLQuery	*query,
		bool		&_hasaggr,
		int		&_maxtab
	)
{
	/* This method is used when traversing the list of expressions	*/
	/* that are arguments to a function.				*/
	if (expr != 0)
		if (!expr->linkDatabase (query, _hasaggr, _maxtab)) return false ;

	return	next == 0 ? true : next->linkDatabase (query, _hasaggr, _maxtab) ;
}

/*  XBSQLExprList							*/
/*  moveTotables: Move expressions to earliest table for evaluation	*/
/*  tablist	: XBSQLTableList * : Table list				*/
/*  (returns)	: bool		   : Success				*/

bool	XBSQLExprList::moveToTables
	(	XBSQLTableList	*tablist
	)
{
//	xbString etext ;
//	fprintf	(stderr, "expr %3d: %s\n", maxtab, expr->getExprName(etext)) ;

	XBSQLExprList *_next	= next	;
	if (!tablist->attachExpr (this, maxtab)) return false ;
	return	_next == 0 ? true : _next->moveToTables (tablist) ;
}

/*  XBSQLExprList							*/
/*  fetchValues	 : Fetch and store database values			*/
/*  querySet	 : XBSQLQuerySet & : Query result set			*/
/*  int		 : rowno	   : Result row number			*/
/*  (returns)	 : bool		   : Success				*/

bool	XBSQLExprList::fetchValues
	(	XBSQLQuerySet	&querySet,
		int		rowno
	)
{
	/* Evaluate the expression, storing the result directly into	*/
	/* the query set (which will minimise data copying).		*/
	if (expr != 0)
		if (!expr->evaluate (querySet.getValue (rowno, index), rowno))
			return	false	;

	/* If this is the last list element then return true, otherwise	*/
	/* continue down the list.					*/
	return	next == 0 ? true : next->fetchValues (querySet, rowno) ;
}

/*  XBSQLExprList							*/
/*  fetchValues	 : Fetch and store database values			*/
/*  values	 : XBSQLValue *	   : Result value vector		*/
/*  int		 : rowno	   : Result row number			*/
/*  (returns)	 : bool		   : Success				*/

bool	XBSQLExprList::fetchValues
	(	XBSQLValue	*values,
		int		rowno
	)
{
	/* Evaluate the expression, storing the result directly into	*/
	/* the query set (which will minimise data copying).		*/
	if (expr != 0)
		if (!expr->evaluate (values[index], rowno))
			return	false	;

	/* If this is the last list element then return true, otherwise	*/
	/* continue down the list.					*/
	return	next == 0 ? true : next->fetchValues (values, rowno) ;
}

/*  XBSQLExprList							*/
/*  setTypeNames : Get and store database types and names		*/
/*  querySet	 : XBSQLQuerySet & : Query result set			*/
/*  (returns)	 : bool		   : Success				*/

bool	XBSQLExprList::setTypeNames
	(	XBSQLQuerySet	&querySet
	)
{
	if (expr != 0)
	{
		/* Get the type that the expression will return, and	*/
		/* store this in the query set.				*/
		XBSQL::VType 	vtype	;
		int		length	;
		xbString	eName	;

		if (!expr->getExprType   (vtype )) return false	;
		if (!expr->getExprLength (length)) return false	;

		querySet.setFieldInfo (index,
				       vtype,
				       length, getFieldName (eName)) ;
	}

	/* If this is the last list element then return true, otherwise	*/
	/* continue down the list.					*/
	return	next == 0 ? true : next->setTypeNames (querySet) ;
}

/*  XBSQLExprList							*/
/*  acceptable	 : Evaluate expressions from a WHERE clause		*/
/*  ok		 : bool &	: True if all expressions are true	*/
/*  (returns)	 : bool		: Success (ie., no error occurred)	*/

bool	XBSQLExprList::acceptable
	(	bool	&ok
	)
{
	if (expr != 0)
	{
		XBSQLValue	value	;

#if	0
		xbString	ename	;
		fprintf	(stderr, "accept(%s)\n", expr->getExprName (ename)) ;
#endif
		/* Evaluate the expression; if this fails then return	*/
		/* false indicating an actual error. Otherwise, see if	*/
		/* the result is not true; if so return true (no	*/
		/* error) with the "ok" flag set false (where		*/
		/* expressions false)					*/
		if (!expr->evaluate (value, 0)) return false ;
		if (!(ok = value.isTRUE()))    return true  ;
	}

	/* This expression accepts the row, so return true (and "ok"	*/
	/* will be true) at the end of the list, otherwise process the	*/
	/* remaining expressions.					*/
	return	next == 0 ? true : next->acceptable (ok) ;
}

/*  XBSQLExprList							*/
/*  evaluate	: Evaluate associated expression			*/
/*  value	: XBSQLValue &	: Result				*/
/*  rowno	: int		: Row number				*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLExprList::evaluate
	(	XBSQLValue	&value,
		int		rowno
	)
{
	if (expr == 0)
	{	value	= XBSQLValue() ;
		return	true	;
	}

	return	expr->evaluate (value, rowno) ;
}

/*  XBSQLExprList							*/
/*  getExprType	: Get type of associated expression			*/
/*  type	: XBSQL::VType & : Type					*/
/*  (returns)	: bool		 : Success				*/

bool	XBSQLExprList::getExprType
	(	XBSQL::VType	&type
	)
{
	if (expr == 0)
	{	type	= XBSQL::VNum	;
		return	true	;
	}

	return	expr->getExprType (type) ;
}

/*  XBSQLExprList							*/
/*  getFuncName	: Assemble column name for a function			*/
/*  buff	: xbString &	: Result buffer				*/
/*  func	: char *	: Function name on first call		*/
/*  (returns)	: void		:					*/

void	XBSQLExprList::getFuncName
	(	xbString	&buff,
		char		*func
	)
{
	/* The method is called intially on the object which is the	*/
	/* first argument, in which case "func" is the function name.	*/
	/* Thereafter, the function name is null.			*/
	if (func != 0)
	{	buff	+= func ;
		buff	+= "("  ;
	}
	else	buff	+= ","	;
	
	expr->getExprName (buff) ;

	if (next == 0)
		buff	+= ")" ;
	else	next->getFuncName (buff, 0) ;
}

/*  XBSQLExprList							*/
/*  getFieldName: Get field name for the expression			*/
/*  buff	: xbString &	: Buffer for name			*/
/*  (returns)	: const char *	: Name					*/

const char *XBSQLExprList::getFieldName
	(	xbString	&buff
	)
{
	if (expr != 0)
		if (asname != 0)
			buff	+= asname ;
		else	expr->getExprName (buff) ;

	return	buff.getData () ;
}

/*  XBSQLExprList							*/
/*  getExprname	: Get list expression					*/
/*  buff	: xbString &	: Buffer for expression			*/
/*  (returns)	: const char *	: Name					*/

const char *XBSQLExprList::getExprName
	(	xbString	&buff
	)
{
	if (!buff.isEmpty()) buff += "," ;

	if (expr != 0) expr->getExprName (buff) ;
	if (next != 0) next->getExprName (buff) ;

	return	buff.getData() ;
}

/*  XBSQLExprList							*/
/*  concatValues: Concatenate values from list of expressions		*/
/*  concat	: xbString &	: Result string				*/
/*  index	: int		: Index into expression list		*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLExprList::concatValues
	(	xbString	&concat,
		int		index
	)
{
	/* This method is useg by the select query class to generate a	*/
	/* string for a list of expression which comprise a group by	*/
	/* clause. Prefix each value (converted to text) by __N__ where	*/
	/* N is the expression index.					*/
	char		b[32]	;
	XBSQLValue	val	;

	if (!expr->evaluate (val, 0)) return false ;

	sprintf	(b, "__%02d__", index) ;

	concat	+= b	;
	concat	+= val.getText() ;

	return	next == 0 ?
			true :
			next->concatValues (concat, index + 1) ;
}

void	XBSQLExprList::print
	(	FILE		*fd,
		int		indent
	)
{
	if (expr == 0)
		fprintf	(fd, "%*s<no expression>\n", indent, "") ;
	else	expr->print (fd, indent) ;

	if (next != 0)
		next->print (fd, indent) ;
}
