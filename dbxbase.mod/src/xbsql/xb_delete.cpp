/***************************************************************************
    file                 : xb_delete.cpp
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


/*  XBSQLDelete								*/
/*  XBSQLDelete	: Constructor for delete query class			*/
/*  xbase	: XBaseSQL *	: Database object			*/
/*  tabname	: const char *	: Table name				*/
/*  where	: XBSQLEList *	: Where expression list			*/
/*  (returns)	: XBSQLDelete	:					*/

XBSQLDelete::XBSQLDelete
	(	XBaseSQL	*xbase,
		const char	*tabname,
		XBSQLExprList	*where
	)
	:
	XBSQLMulti (xbase, new XBSQLTableList (tabname, 0, 0), where)
{
	delAll		= false	;
	realDelete	= false	;

	xbase->addClosePack (tabname) ;
}

/*  XBSQLDelete								*/
/*  ~XBSQLDelete: Destructor for delete query class			*/
/*  (returns)	:		:					*/

XBSQLDelete::~XBSQLDelete ()
{
}

/*  XBSQLDelete								*/
/*  linkDatabase: Link to database					*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLDelete::linkDatabase ()
{
	/* Make a note of whether thete are any where conditions; if	*/
	/* not then we will be able to grossly optimise the deletion.	*/
	delAll	= where == 0 ;

	if (!XBSQLMulti::linkDatabase ())
		return	false	;

//	if (realDelete)
//		tables->getTable()->RealDeleteOn () ;
//	else	tables->getTable()->RealDeleteOff() ;

	return	true	;
}

/*  XBSQLDelete								*/
/*  processRow	: Process a row set from the database			*/
/*  recordNo	: xbLong	: Record number				*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLDelete::processRow
	(	xbLong
	)
{
	XBSQLTable *table = tables->getTable() ;
	xbShort	   rc	  ;

	if ((rc = table->DeleteRecord()) != XB_NO_ERROR)
	{	xbase->setError (rc) ;
		return	false	;
	}

	numRows += 1	;
	return	true	;
}

/*  XBSQLDelete								*/
/*  runQuery	: Execute the query					*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLDelete::runQuery ()
{
	/* If we are to delete all records then we can shortcut by	*/
	/* getting the number of records currently in the table, then	*/
	/* using the "DeleteAllRecords" method.				*/
	if (delAll)
	{
		XBSQLTable *table = tables->table ;
		int	   rc	  ;

		numRows	= table->NoOfRecords	  () ;
		rc	= table->DeleteAllRecords () ;

		if (rc != XB_NO_ERROR)
		{	xbase->setError (rc) ;
			return	false ;
		}

		return	true	;		
	}

	numRows = 0 ;
	return	tables->scanRows (this) ;
}

/*  XBSQLDelete								*/
/*  getNumRows	: Get number of rows updated				*/
/*  (returns)	: int		:					*/

int	XBSQLDelete::getNumRows ()
{
	return	numRows	;
}

void	XBSQLDelete::setRealDelete
	(	bool		on
	)
{
	realDelete	= on ;
}

XBSQLTable
	*XBSQLDelete::getTable ()
{
	return	tables->table	;
}
