/***************************************************************************
    file                 : xb_update.cpp
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


/*  XBSQLUpdate								*/
/*  XBSQLUpdate	: Constructor for update query class			*/
/*  xbase	: XBaseSQL *	    : Database object			*/
/*  tabname	: const char *	    : Table name			*/
/*  assigns	: XBSQLAssignList * : List of update assignments	*/
/*  where	: XBSQLEList *	: Where expression list			*/
/*  (returns)	: XBSQLUpdate	    :					*/

XBSQLUpdate::XBSQLUpdate
	(	XBaseSQL	*xbase,
		const char	*tabname,
		XBSQLAssignList	*assigns,
		XBSQLExprList	*where
	)
	:
	XBSQLMulti (xbase, new XBSQLTableList (tabname, 0, 0), where),
	assigns	   (assigns)
{
}

/*  XBSQLUpdate								*/
/*  ~XBSQLUpdate: Destructor for delete query class			*/
/*  (returns)	:		:					*/

XBSQLUpdate::~XBSQLUpdate ()
{
	if (assigns != 0) delete assigns ;
}

/*  XBSQLUpdate								*/
/*  processRow	: Process a row set from the database			*/
/*  recordNo	: xbLong	: Record number				*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLUpdate::processRow
	(	xbLong
	)
{
	if (!assigns->assignValues ()) return false ;

	XBSQLTable *table = tables->getTable() ;
	xbShort	   rc	  ;

	if ((rc = table->PutRecord()) != XB_NO_ERROR)
	{	xbase->setError (rc) ;
		return	false	;
	}

	numRows	+= 1	;
	return	true	;
}

/*  XBSQLUpdate								*/
/*  linkDatabase: Link into database					*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLUpdate::linkDatabase ()
{
	if (!XBSQLMulti::linkDatabase ()) return false ;

	return	assigns->linkDatabase (this) ;
}

/*  XBSQLUpdate								*/
/*  runQuery	: Execute the query					*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLUpdate::runQuery ()
{
	numRows	= 0 ;
	return	tables->scanRows (this) ;
}

/*  XBSQLUpdate								*/
/*  getNumRows	: Get number of rows updated				*/
/*  (returns)	: int		:					*/

int	XBSQLUpdate::getNumRows ()
{
	return	numRows	;
}
