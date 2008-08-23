/***************************************************************************
    file                 : xb_select.cpp
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
#include	<stdlib.h>

#include	"xbsql.h"


/*  XBSQLSelect	:							*/
/*  XBSQLSelect	: Constructor for parsed select object			*/
/*  xbase	: XBaseSQL *	: Database				*/
/*  exprs	: XBSQLEList *	: Field (strictly, expression) list	*/
/*  tables	: XBSQLTable *	: Table list				*/
/*  where	: XBSQLEList *	: Where expression list			*/
/*  group	: XBSQLEList *	: Group by expression list		*/
/*  having	: XBSQLEList *	: Having expression list		*/
/*  order	: XBSQLEList *	: Order expression list			*/
/*  (returns)	: XBSQLSelect	:					*/

XBSQLSelect::XBSQLSelect
	(	XBaseSQL	*xbase,
		XBSQLExprList	*exprs,
		XBSQLTableList	*tables,
		XBSQLExprList	*where,
		XBSQLExprList	*group,
		XBSQLExprList	*having,
		XBSQLExprList	*order
	)
	:
	XBSQLMulti (xbase, tables, where),
	exprs	   (exprs),
	group	   (group),
	having	   (having),
	order	   (order),
	qrySet	   (xbase->goSlow)
{
	hasAggr		= false	;
	goSlow		= xbase->goSlow ;
	goRow		= -1	;
	goValues	= 0	;
}

/*  XBSQLSelect								*/
/*  ~XBSQLSelect: Destructor for parsed select object			*/
/*  (returns)	:		:					*/

XBSQLSelect::~XBSQLSelect ()
{
	if (exprs    != 0) delete exprs	 ;
	if (order    != 0) delete order	 ;
	if (group    != 0) delete group	 ;
	if (having   != 0) delete having ;
	if (goValues != 0) delete [] goValues ;
}

/*  XBSQLSelect								*/
/*  linkDatabase: Link select query into a database			*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLSelect::linkDatabase ()
{
	xbString	eName	;
	bool		dummy1	= false ;
	int		dummy2	= 0	;

	if (getenv ("XBSQL_PRINT") != 0)
	{
		fprintf	(stderr, "Expressions:\n") ;
		exprs->print (stderr, 2) ;
		fprintf	(stderr, "Where:\n") ;
		if (where != 0) where->print (stderr, 2) ;
		fprintf	(stderr, "Group By:\n") ;
		if (group != 0) group->print (stderr, 2) ;
		fprintf	(stderr, "Order By:\n") ;
		if (order != 0) order->print (stderr, 2) ;
	}

	/* Process stuff common to all multi-row queries. This must be	*/
	/* done first.							*/
	if (!XBSQLMulti::linkDatabase ()) return false ;

	/* Process the group by, having, and order terms, if present.	*/
	if (group  != 0)
		if (!group ->linkDatabase (this, dummy1, dummy2)) return false ;

	if (having != 0)
		if (!having->linkDatabase (this, dummy1, dummy2)) return false ;

	if (order != 0)
		if (!order ->linkDatabase (this, dummy1, dummy2)) return false ;

	/* Link the expressions. This will associate each field node	*/
	/* with a table and a field number in that table; it will also	*/
	/* expand "select * from ...." into a series of expressions.	*/
	if (!exprs->linkDatabase (this, hasAggr))
		return	false	;

	/* Count the number of expressions (for value fetching and	*/
	/* ordering), assigning each its index into the query set. Then	*/
	/* pass the expression counts to the query set, and tell it	*/
	/* the ordering for each sort expression.			*/
	nGetExprs = 0 ;
	nQryExprs = 0 ;
	nAllExprs = 0 ;

	int	exprIdx	= 0 ;

	for (XBSQLExprList *e0 = order  ; e0 != 0 ; e0 = e0->next)
		if (e0->expr != 0)
		{	e0->index  = exprIdx ;
			nAllExprs += 1 ;
			nQryExprs += 1 ;
			exprIdx	  += 1 ;
		}

	for (XBSQLExprList *e1 = having ; e1 != 0 ; e1 = e1->next)
		if (e1->expr != 0)
		{	e1->index  = exprIdx ;
			nAllExprs += 1 ;
			nQryExprs += 1 ;
			exprIdx	  += 1 ;
		}

	for (XBSQLExprList *e2 = exprs  ; e2 != 0 ; e2 = e2->next)
		if (e2->expr != 0)
		{	e2->index  = exprIdx ;
			nAllExprs += 1 ;
			nGetExprs += 1 ;
			exprIdx	  += 1 ;
		}

	qrySet.setNumFields
	(	nGetExprs,
		nQryExprs,
		nAllExprs,
		nTables
	)	;

	for (XBSQLExprList *e3 = order ; e3 != 0 ; e3 = e3->next)
		if (e3->expr != 0)
			qrySet.setSortOrder (e3->index, e3->ascend) ;

	/* Scan the expressions to get their names, and the type that	*/
	/* each will return when evaluated.				*/
	if (!exprs->setTypeNames (qrySet))
		return	false	;

	return	true	;
}

/*  XBSQLSelect								*/
/*  processRow	: Process a row set from the database			*/
/*  recordNo	: xbLong	: Record number				*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLSelect::processRow
	(	xbLong	recordNo
	)
{
	int	rowno	= -1 ;

	/* If we have a group expression then evaluate it, and locate	*/
	/* it amongst the list of current group values (adding it if	*/
	/* not found). The offset is used as the row number.		*/
	if (group != 0)
	{
		xbString	concat	;
		if (!group->concatValues (concat, 0)) return false ;

		XBSQLValue 	value	= concat.getData() ;

		if ((rowno = grvals.find (value)) < 0)
		{
			rowno	= grvals.count() ;
			grvals.at(rowno) = value ;
		}
	}
	/* Next case is no group by but we do have aggregate functions,	*/
	/* in which case the row number is always zero.			*/
	else if	(hasAggr)
	{
		rowno	= 0 ;
	} 
	/* Last case is no group and no aggregate. We will increment	*/
	/* row number to be one greater than the current number of rows	*/
	else	rowno	= qrySet.getNumRows() ;

	/* Add a new row if necessary. Pass the table list so that in	*/
	/* slot mode, the query set can save the record numbers in	*/
	/* each of the tables.						*/
	if (rowno >= qrySet.getNumRows())
		qrySet.addNewRow (tables) ;

	/* ... and evaluate and fetch the expressions. The results will	*/
	/* be dropped into the appropriate row of the query set unless	*/
	/* we are in slow mode.						*/
	if (!goSlow)
		if (!exprs->fetchValues (qrySet, rowno)) return false ;

	if ((order  != 0) && !order ->fetchValues (qrySet, rowno)) return false ;
	if ((having != 0) && !having->fetchValues (qrySet, rowno)) return false ;

	return	true ;
}

/*  XBSQLSelect								*/
/*  getNumRows	: Get number of rows in the query set			*/
/*  (returns)	: int		: Number of rows			*/

int	XBSQLSelect::getNumRows ()
{
	return	qrySet.getNumRows () ;
}

/*  XBSQLSelect								*/
/*  getNumFields: Get number of fields in the query set			*/
/*  (returns)	: int		: Number of rows			*/

int	XBSQLSelect::getNumFields ()
{
	return	qrySet.getNumFields () ;
}

/*  XBSQLSelect								*/
/*  getField	: Get query set field					*/
/*  row		: int		 : Row number				*/
/*  fld		: int		 : Field number				*/
/*  (returns)	: const XBValue &: Result				*/

const XBSQLValue &XBSQLSelect::getField
	(	int	row,
		int	fld
	)
{
	static	XBSQLValue nullVal ;
	if ((row < 0) || (row >= qrySet.getNumRows  ())) return nullVal ;
	if ((fld < 0) || (fld >= qrySet.getNumFields())) return nullVal ;

	/* If in slow mode then we need to ensure that the correct	*/
	/* records have been loaded. This is needed each time that the	*/
	/* query row changes, whence we need to fetch each of the data	*/
	/* values. The fetch is into a local value buffer, which is	*/
	/* dynamically created the first time it is needed.		*/
	if (goSlow)
	{
//		fprintf
//		(	stderr,
//			"XBSQLSelect::getField: goSlow: row=%d fld=%d goRow=%d\n",
//			row, fld, goRow
//		)	;

		if (goValues == 0)
			goValues = new XBSQLValue [nAllExprs] ;

		if (goRow != row)
		{
			if (!tables->loadRecords (qrySet, row))
			{
				fprintf	(stderr, "XBSQLSelect::getField: GetRecord failed\n") ;
				return	nullVal	 ;
			}

			goRow	= row	;

			if (!exprs->fetchValues (goValues, row))
			{
				fprintf	(stderr, "XBSQLSelect::getField: fetchValues failed\n") ;
				return	nullVal	;
			}
		}

		return	goValues[fld + nQryExprs] ;
	}

	return	qrySet.getValue(row, fld + nQryExprs) ;
}

/*  XBSQLSelect								*/
/*  getFieldName: Get query set field name				*/
/*  fld		: int		: Field number				*/
/*  (returns)	: const char *	: File name				*/

const char *XBSQLSelect::getFieldName
	(	int	fld
	)
{
	if ((fld < 0) || (fld >= qrySet.getNumFields())) return 0 ;

	return	qrySet.getFieldName(fld + nQryExprs) ;
}

/*  XBSQLSelect								*/
/*  getFieldType: Get type of specified field				*/
/*  fld		: int		: Field number				*/
/*  (returns)	: xbShort	: Field type				*/

xbShort	XBSQLSelect::getFieldType
	(	int	fld
	)
{
	if ((fld < 0) || (fld >= qrySet.getNumFields())) return 0 ;

	return	VTypeToXType (qrySet.getFieldType (fld + nQryExprs)) ;
}

/*  XBSQLSelect								*/
/*  getFieldLength: Get length of specified field			*/
/*  fld		  : int		: Field number				*/
/*  (returns)	  : int		: Field length				*/

int	XBSQLSelect::getFieldLength
	(	int	fld
	)
{
	if ((fld < 0) || (fld >= qrySet.getNumFields())) return 0 ;

	return	qrySet.getFieldLength (fld + nQryExprs) ;
}

/*  XBSQLSelect								*/
/*  runQuery	: Execute the query					*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLSelect::runQuery ()
{
	/* Clear the query set, then drop down through the tables to	*/
	/* do the work.							*/
	qrySet.clear () ;
	if (!tables->scanRows (this)) return false ;

	/* If there is a having term then kill all rows for which this	*/
	/* is not true.							*/
	if (having != 0)
		for (int row = qrySet.getNumRows() - 1 ; row >= 0 ; row -= 1)
			if (!qrySet.getValue(row, having->index).isTRUE())
				qrySet.killrow (row) ;

	/* Potentially sort the query set. If there were any order by	*/
	/* terms then they will habe been added to the query set as	*/
	/* extra columns, and the sort ordering noted.			*/
	qrySet.sort  () ;
	return	true	;
}

/*  XBSQLSelect								*/
/*  killrow	: Drop specified row when no longer used		*/
/*  nrow	: int		: Row number				*/
/*  (returns)	: void		:					*/

void	XBSQLSelect::dumprow
	(	int	row
	)
{
	qrySet.dumprow (row) ;
}
