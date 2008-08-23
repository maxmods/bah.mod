/***************************************************************************
    file                 : xb_create.cpp
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

/*  XBSQLColumnList							*/
/*  XBSQLColumnList							*/
/*		: Constructor for column list item			*/
/*  name	: const char *	    : Column name			*/
/*  type	: int		    : Column type			*/
/*  width	: int		    : Column field length		*/
/*  precision	: int		    : Column precision			*/
/*  indexed	: XBSQL::Indexed    : Column is indexed			*/
/*  next	: XBSQLColumnList * : Next column list item		*/

XBSQLColumnList::XBSQLColumnList
	(	const char	*name,
		int		type,
		int		width,
		int		precision,
		XBSQL::Index	indexed,
		XBSQLColumnList	*next
	)
	:
	next	(next),
	indexed	(indexed)
{
	strncpy	(schema.FieldName,  name, sizeof(schema.FieldName)) ;

	switch (type)
	{
		case 'N' :
			if (width     == 0) width     = 10 ;
			break	;

		case 'F' :
			if (width     == 0) width     = 10 ;
			if (precision == 0) precision = 2  ;
			break	;

		case 'C' :
			if (width     == 0) width     = 80 ;
			break	;

		case 'M' :
			width	= 10	;
			break	;

		default	:
			break	;
	}
	
	schema.Type	= type		;
	schema.FieldLen	= width		;
	schema.NoOfDecs	= precision	;
}

/*  XBSQLColumnList							*/
/*  ~XBSQLColumnList							*/
/*		: Destructor for column list item			*/
/*  (returns)	:		:					*/

XBSQLColumnList::~XBSQLColumnList ()
{
	if (next != 0) delete next ;
}


/*  XBSQLCreate								*/
/*  XBSQLCreate	: Constructor for create table command			*/
/*  xbase	: XBaseSQL *	    : Database access object		*/
/*  tabname	: const char *	    : Table name			*/
/*  columns	: XBSQLColumnList * : List of columns			*/
/*  (returns)	: XBSQLCreate	    :					*/

XBSQLCreate::XBSQLCreate
	(	XBaseSQL	*xbase,
		const char	*tabname,
		XBSQLColumnList	*columns
	)
	:
	xbase	(xbase),
	tabname	(tabname),
	columns	(columns)
{
}

/*  XBSQLCreate								*/
/*  ~XBSQLCreate: Destructor for create table command			*/
/*  (returns)	:		:					*/

XBSQLCreate::~XBSQLCreate ()
{
	if (columns != 0) delete columns ;
}

/*  XBSQLCreate								*/
/*  execute	: Execute table creation				*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLCreate::execute ()
{
	xbSchema 	*schema	 ;
	int	 	colno	 ;
	bool	 	rc	 ;
	XBSQL::Index	 *idxflag;
	
	colno	 = 0	 ;
	for (XBSQLColumnList *c0 = columns ; c0 != 0 ; c0 = c0->next)
		colno	+= 1 ;

	schema	= new xbSchema	  [colno+1] ;
	idxflag	= new XBSQL::Index[colno  ] ;

	colno	 = 0	 ;
	for (XBSQLColumnList *c1 = columns ; c1 != 0 ; c1 = c1->next)
	{	schema [colno] = c1->schema  ;
		idxflag[colno] = c1->indexed ;
		colno	+= 1 ;
	}

	memset	(&schema[colno], 0, sizeof(xbSchema)) ;

	rc	= xbase->createTable (tabname, schema, idxflag) ;
	delete	[] idxflag ;
	delete	[] schema  ;
	return	rc ;
}

/*  XBSQLDrop								*/
/*  XBSQLDrop	: Constructor for drop table command			*/
/*  xbase	: XBaseSQL *	    : Database access object		*/
/*  tabname	: const char *	    : Table name			*/
/*  (returns)	: XBSQLDrop	    :					*/

XBSQLDrop::XBSQLDrop
	(	XBaseSQL	*xbase,
		const char	*tabname
	)
	:
	xbase	(xbase),
	tabname	(tabname)
{
}

/*  XBSQLDrop								*/
/*  ~XBSQLDrop	: Destructor for drop table command			*/
/*  (returns)	:		:					*/

XBSQLDrop::~XBSQLDrop ()
{
}

/*  XBSQLDrop								*/
/*  execute	: Execute table destruction				*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLDrop::execute ()
{
	return	xbase->dropTable (tabname) ;
}
