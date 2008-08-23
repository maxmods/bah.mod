/***************************************************************************
    file                 : xb_insert.cpp
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


/*  XBSQLInsert								*/
/*  XBSQLInsert	: Constructor for parsed insert object			*/
/*  xbase	: XBaseSQL *	   : Database				*/
/*  tabname	: const char *	   : Table name				*/
/*  fields	: XBSQLFieldList * : List of fields or null		*/
/*  exprs	: XBSQLExprList  * : Expression list			*/
/*  (returns)	: XBSQLInsert	   :					*/

XBSQLInsert::XBSQLInsert
	(	XBaseSQL	*xbase,
		const char	*tabname,
		XBSQLFieldList	*fields,
		XBSQLExprList	*exprs
	)
	:
	XBSQLQuery (xbase, new XBSQLTableList (tabname, 0, 0)),
	fields	   (fields),
	exprs	   (exprs),
	select	   (0)
{
}

/*  XBSQLInsert	: Constructor for parsed insert object			*/
/*  xbase	: XBaseSQL *	   : Database				*/
/*  tabname	: const char *	   : Table name				*/
/*  fields	: XBSQLFieldList * : List of fields or null		*/
/*  select	: XBSQLSelect *    : Select query			*/
/*  (returns)	: XBSQLInsert	   :					*/

XBSQLInsert::XBSQLInsert
	(	XBaseSQL	*xbase,
		const char	*tabname,
		XBSQLFieldList	*fields,
		XBSQLSelect	*select
	)
	:
	XBSQLQuery (xbase, new XBSQLTableList (tabname, 0, 0)),
	fields	   (fields),
	exprs	   (0),
	select	   (select)
{
}

/*  XBSQLInsert								*/
/*  XBSQLInsert	: Destructor for parsed insert object			*/
/*  (returns)	:		:					*/

XBSQLInsert::~XBSQLInsert ()
{
	if (select != 0) delete select	;
	if (exprs  != 0) delete	exprs	;
	if (fields != 0) delete	fields	;
}

/*  XBSQLInsert								*/
/*  linkDatabase: Link query into database				*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLInsert::linkDatabase ()
{
	int	nExprs	;
	int	nFields	;

	/* First do the common stuff, in particular that will link the	*/
	/* table to the database.					*/
	if (!XBSQLQuery::linkDatabase ()) return false ;

	/* If there is no field list then construct one. Note that we	*/
	/* work in reverse field order so that the list is assembled	*/
	/* in the correct order.					*/
	if (fields == 0)
	{
		XBSQLTable *table = tables->getTable () ;

		for (int idx = table->FieldCount() - 1 ; idx >= 0 ; idx -= 1)
			fields = new XBSQLFieldList
				 (	xbStoreText (table->GetFieldName (idx)),
					fields
				 )	;
	}


	/* Verify that the fields can be linked into the database. This	*/
	/* could be shortcut if we have assembled our own list but it	*/
	/* is easire this way.						*/
	if (!fields->linkDatabase (this)) return false ;


	/* Field are accessible, but we will also make sure that we	*/
	/* have the same number of fields as expressions, so count them	*/
	nFields  = 0 ;
	for (XBSQLFieldList *f = fields ; f != 0 ; f = f->next)
		nFields += 1 ;


	/* There are now two distinct (and exclusive) cases. In the	*/
	/* first we have a set of expressions; in the second this is	*/
	/* and "insert into .... select ...." form of insert.		*/
	if (select == 0)
	{
		nExprs	= 0 ;
		for (XBSQLExprList *e = exprs ; e != 0 ; e = e->next)
		{	e->index = nExprs ;
			nExprs	+= 1 	  ;
		}
	}
	else
	{
		if (!select->linkDatabase()) return false ;
		nExprs	= select->getNumExprs() ;
	}


	if (nFields != nExprs)
	{	xbase->setError ("Mismatched number of fields and expressions") ;
		return	false	;
	}

	/* OK, now just the expressions. There must be some, or the	*/
	/* query would not have parsed, except if this is an insert	*/
	/* from a select.						*/
	bool	dummy  ;
	return	select == 0 ? exprs->linkDatabase (this, dummy) : true ;
}

/*  XBSQLInsert								*/
/*  copySelect	: Copy results from embedded select			*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLInsert::copySelect ()
{
	if (!select->runQuery()) return false ;

	int		nRows	= select->getNumRows  () ;
	int		nCols	= select->getNumFields() ;
	XBSQLTable	*table	= tables->getTable () ;
	xbShort		rc	;

	for (int rowNo = 0 ; rowNo < nRows ; rowNo += 1)
	{
		XBSQLFieldList	*f	= fields ;

		table->BlankRecord ()	;

		for (int colNo = 0 ; colNo < nCols ; colNo += 1)
		{
			if (f == 0)
			{	xbase->setError ("Internal field/expression mismatch") ;
				return	false	;
			}

			XBSQLValue value = select->getField (rowNo, colNo) ;
			if (!f->saveValue (value)) return false	;

			f = f->next ;
		}

		if (f != 0)
		{	xbase->setError ("Internal field/expression mismatch") ;
			return	false	;
		}

		if ((rc = table->AppendRecord ()) != XB_NO_ERROR)
		{	xbase->setError (rc) ;
fprintf(stderr, "Append failed [%d]\n", rc) ;
			return	false	;
		}
fprintf(stderr, "Append OK\n") ;
	}

	numRows	= select->getNumRows () ;
	return	true ;
}

/*  XBSQLInsert								*/
/*  runQuery	: Execute the query					*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLInsert::runQuery ()
{
	numRows	= 0 ;

	/* Processing depends on whether or not this is the "insert ...	*/
	/* select ...." form:						*/
	if (select == 0)
	{
		XBSQLTable	*table	= tables->getTable () ;
		xbShort		rc	;

		table->BlankRecord ()	;

		/* Run through the expressions and fields, evaluating	*/
		/* the former and saving the value through the latter	*/
		/* to the table.					*/
		XBSQLExprList	*e	= exprs  ;
		XBSQLFieldList	*f	= fields ;

		while ((e != 0) && (f != 0))
		{
			XBSQLValue value  ;

			if (!e->evaluate  (value, 0)) return false ;
			if (!f->saveValue (value   )) return false ;

			e	= e->next ;
			f	= f->next ;
		}

		if ((e != 0) || (f != 0))
		{	xbase->setError ("Internal field/expression mismatch") ;
			return	false	;
		}

		if ((rc = table->AppendRecord ()) != XB_NO_ERROR)
		{	xbase->setError (rc) ;
			return	false	;
		}

		numRows	= 1	;
	}
	else	if (!copySelect ()) return false ;

	return	true	;
}

/*  XBSQLInsert								*/
/*  getNumRows	: Get number of rows updated				*/
/*  (returns)	: int		:					*/

int	XBSQLInsert::getNumRows ()
{
	return	numRows	;
}
