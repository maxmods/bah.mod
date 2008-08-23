/***************************************************************************
    file                 : xb_query.cpp
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

#include	<xbsql.h>

/*  XBSQLQuery								*/
/*  XBSQLQuery	: Constructor for query base class			*/
/*  xbase	: XBaseSQL *	   : Database object			*/
/*  tables	: XBSQLTableList * : List of associated tables		*/
/*  (returns)	: void		   :					*/

XBSQLQuery::XBSQLQuery
	(	XBaseSQL	*xbase,
		XBSQLTableList	*tables
	)
	:
	xbase	(xbase),
	tables	(tables)
{
	extern	char	*xbTextb ;
	textb	= xbTextb	 ;
	xbTextb	= 0		 ;
}

/*  XBSQLQuery								*/
/*  ~XBSQLQuery	: Destructor for query base class			*/
/*  (returns)	: void		:					*/

XBSQLQuery::~XBSQLQuery ()
{
	if (textb  != 0) free ((void *)textb) ;
	if (tables != 0) delete tables ;
}


/*  XBSQLQuery								*/
/*  linkDatabase: Link into database					*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLQuery::linkDatabase ()
{
	/* Link the tables into the database. If this succeeds then	*/
	/* the required tables will have been opened, and "nTables"	*/
	/* will be the count of tables.					*/
	nTables	= 0 ;
	return	tables->linkDatabase (xbase, nTables) ;
}

/*  XBSQLQuery								*/
/*  findField	: Locate field in associated tables			*/
/*  tabname	: const char *	: Table name				*/
/*  fldname	: const char *	: Field name				*/
/*  field	: XBSQLField &	: Field object				*/
/*  maxtab	: int &		: Maximum table index			*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLQuery::findField
	(	const char	*tabname,
		const char	*fldname,
		XBSQLField	&field,
		int		&maxtab
	)
{
	if (!tables->findField (tabname, fldname, field, maxtab))
	{
		if (tabname == 0)
			xbase->setError ("Cannot find field \"%s\"",		 fldname) ;
		else	xbase->setError ("Cannot find field \"%s.%s\"", tabname, fldname) ;
		return	false	;
	}

	return	true	;
}


/*  XBSQLQuery								*/
/*  execute	: Execute the query					*/
/*  _nvals	: int		: Number of substitution values		*/
/*  _values	: XBSQLValue *	: Vector of substitution values		*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLQuery::execute
	(	int		_nvals,
		XBSQLValue	*_values
	)
{
	extern	bool xbCaseSensitive ;
	extern	bool xbUseWildcard   ;

	xbCaseSensitive  = xbase->caseSensitive ;
	xbUseWildcard    = xbase->useWildcard   ;

	nvals	= _nvals   ;
	values	= _values  ;

	return	runQuery ();
}

/*  XBSQLQuery								*/
/*  getPlaceValue: Get placeholder value				*/
/*  num		 : int		: Placeholder number			*/
/*  value	 : XBSQLValue & : Result				*/
/*  (returns)	 : bool		: Success				*/

bool	XBSQLQuery::getPlaceValue
	(	int		num,
		XBSQLValue	&value
	)
{
	if ((num < 0) || (num >= nvals))
	{
		xbase->setError ("Placeholder %d out of range 0...%d", num, nvals - 1) ;
		return	false	;
	}

	value	= values[num]	;
	return	true		;
}

/*  XBSQLQuery								*/
/*  getPlaceType : Get placeholder type					*/
/*  num		 : int		: Placeholder number			*/
/*  (returns)	 : XBSQL::VType : Type					*/

XBSQL::VType XBSQLQuery::getPlaceType
	(	int	num
	)
{
	return	((num < 0) || (num >= nvals)) ? XBSQL::VNull : values[num].tag ;
}

/*  XBSQLQuery								*/
/*  getAllColumns: Get expression list for all columns			*/
/*  tail	 : XBSQLExprList * : List tail				*/
/*  (returns)	 : XBSQLExprList * : List with tail appended		*/

XBSQLExprList *XBSQLQuery::getAllColumns
	(	XBSQLExprList	*tail
	)
{
	return	tables->getAllColumns (xbase, tail) ;
}

/*  XBSQLQuery								*/
/*  getNumRows	: Get number of rows in the query set			*/
/*  (returns)	: int		: Number of rows			*/

int	XBSQLQuery::getNumRows ()
{
	return	0 ;
}

/*  XBSQLQuery								*/
/*  getNumFields: Get number of fields in the query set			*/
/*  (returns)	: int		: Number of rows			*/

int	XBSQLQuery::getNumFields ()
{
	return	0 ;
}




/*  ------------------------------------------------------------------  */

/*  XBSQLMulti								*/
/*  XBSQLMulti	: Constructor for multi-row query class			*/
/*  xbase	: XBaseSQL *	   : Database object			*/
/*  tables	: XBSQLTableList * : List of affected tables		*/
/*  where	: XBSQLExprList *  : Where expression list		*/
/*  (returns)	: XBSQLMulti	   :					*/

XBSQLMulti::XBSQLMulti
	(	XBaseSQL	*xbase,
		XBSQLTableList	*tables,
		XBSQLExprList	*where
	)
	:
	XBSQLQuery (xbase, tables),
	where	   (where) 
{
	where	= 0 	;
}

/*  XBSQLMulti								*/
/*  ~XBSQLMulti	: Destructor for multi-row query class			*/
/*  (returns)	: 		:					*/

XBSQLMulti::~XBSQLMulti ()
{
	if (where  != 0) delete where	;
}

/*  XBSQLMulti								*/
/*  linkDatabase: Link into database					*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLMulti::linkDatabase ()
{
	/* Process the common stuff, specifically linkin the tables.	*/
	if (!XBSQLQuery::linkDatabase ()) return false ;

	/* If there are any where conditions then link these, to attach	*/
	/* fields to the opened tables, then transfer the expressions	*/
	/* to the earliest table at which they can be evaluated.	*/
	if (where != 0)
	{
		bool	dummy	;
		if (!where->linkDatabase (this, dummy)) return false ;
		if (!where->moveToTables (tables)) return false ;
		where	= 0 ;
	}

	return	true	; 
}
