/***************************************************************************
    file                 : xb_tablelist.cpp
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

/*  XBSQLTableList							*/
/*  XBSQLTableList: Constructor for parsed table item			*/
/*  name	  : const char *     : Table name			*/
/*  alias	  : const char *     : Table alias			*/
/*  next	  : XBSQLTabelItem * : Next item in list		*/

XBSQLTableList::XBSQLTableList
	(	const char	*name,
		const char	*alias,
		XBSQLTableList	*next
	)
	:
	name	(name ),
	alias	(alias),
	next	(next)
{
	table		= 0 	;
	where		= 0 	;

	useindex	= 0 	;
	idxexprlist	= 0 	;
	idxexprnode	= 0 	;
	ftype		= 'X'	;
}

/*  XBSQLTableList							*/
/*  ~XBSQLTableList: Destructor for parsed table item			*/
/*  (returns)	   :		:					*/

XBSQLTableList::~XBSQLTableList ()
{
	if (next	!= 0) delete next	 ;
	if (table	!= 0) delete table	 ;
	if (where  	!= 0) delete where  	 ;
	if (idxexprlist != 0) delete idxexprlist ;
}

/*  XBSQLTableList							*/
/*  linkDatabase : Link table item to database				*/
/*  _xbase	 : XBaseSQL *	: Database				*/
/*  _tabidx	 : int &	: Table index for this table		*/
/*  (returns)	 : bool		: Success				*/

bool	XBSQLTableList::linkDatabase
	(	XBaseSQL	*_xbase,
		int		&_tabidx
	)
{
	xbase	= _xbase ;

	/* Request the database to open the table. If this is OK then	*/
	/* continue over any further tables in the list.		*/
	if ((table = xbase->openTable (name)) == 0)
		return	false	;

	tabidx	 = _tabidx ;
	_tabidx += 1	   ;
	
	return	next == 0 ? true : next->linkDatabase (xbase, _tabidx) ;
}

/*  XBSQLTableList							*/
/*  scanRowsSimple							*/
/*		: Simple linear scan of rows in the database		*/
/*  query	: XBSQLMulti *	: Associated query			*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLTableList::scanRowsSimple
	(	XBSQLMulti	*query
	)
{
	bool	ok 	;
	int	rowno	= 0 ;

	table->rewind () ;

	/* The outer loop iterates over the records in the table. The	*/
	/* nextRecord method returns true until the table is exhausted	*/
	/* or there is an error; the "ok" parameter is the real result.	*/
	while (table->nextRecord (ok))
	{	
		rowno	+= 1 ;

		/* See if the record just retrieved from the database	*/
		/* is acceptable given any where expressions that can	*/
		/* be evaluated at thie table (ie., which don't depend	*/
		/* on values from tables further down the list).	*/
		if (where != 0)
		{	if (!where->acceptable (ok)) return false ;
			if (!ok) continue ;
		}

		/* If this is not the last table in the list then	*/
		/* continue on into it ....				*/
		if (next != 0)
			if (!next->scanRows (query))
				return false ;
			else	continue ;


		/* OK, we have a complete acceptable row, so process it	*/
		/* This will get the expressions, or update or whatever	*/
		if (!query->processRow (table->GetCurRecNo()))
			return	false ;
	}

	return	ok	;
}

/*  XBSQLTableList							*/
/*  scanRowsIndexed							*/
/*		: Indexed scan of rows in the database			*/
/*  query	: XBSQLMulti *	: Associated query			*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLTableList::scanRowsIndexed
	(	XBSQLMulti	*query
	)
{
	XBSQLValue	key 	;
	xbShort		rc	;
//	int		rowno	;
	bool		ok	;

	/* Evaluate the expression. We know that this only depends on	*/
	/* prior tables, so it is constant for this table.		*/
	if (!idxexprnode->evaluate (key, 0))
		return false ;

	/* Tedious bit, xbase keeps text indexed as text, and numbers	*/
	/* as doubles.							*/
	switch (ftype)
	{
		case 'L' :
		case 'N' :
		case 'F' :
			rc	= useindex->FindKey (atof(key.getText())) ;
			break	;

		case 'D' :
		case 'C' :
		case 'M' :
			rc	= useindex->FindKey (key.getText()) ;
			break	;

		default	:
			xbase->setError ("Unknown field type %c", ftype) ;
			return	false	;
	}

	switch (rc)
	{
		case XB_FOUND	  :
			/* Row found, Continue and scan forward for	*/
			/* suitable rows.				*/
			break	;

		case XB_NOT_FOUND : 
		case XB_EOF	  :
			/* Not found/end of file. In this case there	*/
			/* are no matching rows, so return. Not an	*/
			/* error though.				*/
			return true ;

		default	:
			/* Anything else *is* an error.			*/
			xbase->setError (rc) ;
			return false ;
	}

	do
	{
		table->NoteCurRecNo () ;

//		rowno	= table->GetCurRecNo () ;
//		fprintf (stderr, "index scan : %*s%5d\n", tabidx *3, "" , rowno) ;

		/* Check out this row. If the index expression is not	*/
		/* true then, since we are scanning in index order, no	*/
		/* further rows can match.				*/
		if (!idxexprlist->acceptable (ok))
			return false ;

		if (!ok)
		{	rc	= XB_EOF ;
			break	;
		}

		/* See if the record just retrieved from the database	*/
		/* is acceptable given any where expressions that can	*/
		/* be evaluated at thie table (ie., which don't depend	*/
		/* on values from tables further down the list).	*/
		if (where != 0)
		{	if (!where->acceptable (ok)) return false ;
			if (!ok) continue ;
		}

		/* If this is not the last table in the list then	*/
		/* continue on into it ....				*/
		if (next != 0)
			if (!next->scanRows (query))
				return false ;
			else	continue ;


		/* OK, we have a complete acceptable row, so process it	*/
		/* This will get the expressions, or update or whatever	*/
		if (!query->processRow (table->GetCurRecNo()))
			return	false ;

		
	}
	while ((rc = useindex->GetNextKey ()) == XB_NO_ERROR) ;

        if ((rc != XB_EOF) && (rc != XB_NOT_FOUND))
	{
		xbase->setError (rc) ;
		return	false	;
	}

	return	true	;
}

/*  XBSQLTableList							*/
/*  scanRows	: Scan rows in the database				*/
/*  query	: XBSQLMulti *	: Associated query			*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLTableList::scanRows
	(	XBSQLMulti	*query
	)
{
	/* Two cases. If no index has been found then we do a brute-	*/
	/* force scan, otherwise we can work from the index.		*/
	return	useindex == 0 ?
			  scanRowsSimple  (query) :
			  scanRowsIndexed (query) ;
}


/*  XBSQLTableList							*/
/*  findField	: Locate field in associated tables			*/
/*  tabname	: const char *	: Table name				*/
/*  fldname	: const char *	: Field name				*/
/*  field	: XBSQLField &	: Field object				*/
/*  maxtab	: int &		: Maximum table index			*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLTableList::findField
	(	const char	*tabname,
		const char	*fldname,
		XBSQLField	&field,
		int		&maxtab
	)
{
	if ((tabname == 0) || (strcmp (tabname, alias == 0 ? name : alias) == 0))
		if (table->findField (fldname, field))
		{	if (tabidx > maxtab) maxtab = tabidx ;
			return true ;
		}

	return	next == 0 ? false : next->findField (tabname, fldname, field, maxtab) ;
}

/*  XBSQLTableList							*/
/*  getTable	: Get the table for this list entry			*/
/*  (returns)	: XBSQLTable *	:					*/

XBSQLTable *XBSQLTableList::getTable ()
{
	return	table	;
}

/*  XBSQLTableList							*/
/*  attachExpr	: Attach a where expression				*/
/*  expr	: XBSQLExprList * : Expression				*/
/*  _tabidx	: int		  : Required table index		*/
/*  (returns)	: bool		  : Success				*/

bool	XBSQLTableList::attachExpr
	(	XBSQLExprList	*expr,
		int		_tabidx
	)
{
	if (_tabidx == tabidx)
	{
		/* If no usable index has been found then see if this	*/
		/* expression yields one. If so, the "indexable" method	*/
		/* will set the associated expression and field type.	*/
		if (useindex == 0)
			if ((useindex = expr->expr->indexable
				        (	table,
						tabidx,
						idxexprnode,
						ftype
				        )) != 0)
			{
				xbString etext ;
				fprintf	(stderr, "attachExpr %3d: %s\n",
						 tabidx,
						 expr->expr->getExprName (etext)) ;

				idxexprlist  = expr ;
				expr->next   = 0    ;
				return true  ;
			}

		expr->next = where ;
		where	   = expr  ;
		return	true	   ;
	}

	if (next == 0)
	{
		xbase->setError ("Table index %d is invalid", _tabidx) ;
		return	false	;
	}

	return	next->attachExpr (expr, _tabidx) ;
}

/*  XBSQLTableList							*/
/*  getAllColumns: Get expression list for all columns			*/
/*  xbase	 : XBaseSQL *	   : Database object			*/
/*  tail	 : XBSQLExprList * : List tail				*/
/*  (returns)	 : XBSQLExprList * : List with tail appended		*/

XBSQLExprList *XBSQLTableList::getAllColumns
	(	XBaseSQL	*xbase,
		XBSQLExprList	*tail
	)
{
	if (next != 0) tail = next->getAllColumns (xbase, tail) ;

	XBSQLFieldSet	*fields	= new XBSQLFieldSet (xbase, table) ;
	const char	*fname	;
	XBSQLExprNode	*expr	;

	for (int fno = fields->getNumFields() - 1 ; fno >= 0 ; fno -= 1)
	{
		fname	= xbStoreText (fields->getFieldName (fno)) ;

//		if (alias == 0)
//			expr = new XBSQLExprNode (fname, name,  false) ;
//		else	expr = new XBSQLExprNode (fname, alias, false) ;

		expr	= new XBSQLExprNode (fname, false) ;
		tail	= new XBSQLExprList (expr,  (const char *)0, tail) ;
	}
	delete	fields	;

	return	tail	;
}

/*  XBSQLTableList							*/
/*  setRecordNos: Set record numbers from current records		*/
/*  recNos	: xbLong	: Record number vector			*/
/*  (returns)	: void		:					*/

void	XBSQLTableList::setRecordNos
	(	xbLong	*recNos
	)
{
	recNos[tabidx] = table->GetCurRecNo() ;
	if (next != 0) next->setRecordNos (recNos) ;
}

/*  XBSQLTableList							*/
/*  loadRecords	: Load records for current row				*/
/*  qrySet	: XBSQLQuerySet & : Associated query set		*/
/*  row		: int		  : Query row number			*/
/*  (returns)	: bool		  : Success				*/

bool	XBSQLTableList::loadRecords
	(	XBSQLQuerySet	&qrySet,
		int		row
	)
{
	/* This routine is used in slow query mode. From the query set	*/
	/* we get the record number in the XBase file corresponding to	*/
	/* the current query row, and then load that record.		*/
	if (table->GetRecord (qrySet.getRecordNo(row, tabidx)) != XB_NO_ERROR)
		return	false	;

	return	next == 0 ? true : next->loadRecords (qrySet, row) ;
}
