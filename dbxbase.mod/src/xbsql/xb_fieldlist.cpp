/***************************************************************************
    file                 : xb_fieldlist.cpp
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

/*  XBSQLFieldList							*/
/*  XBSQLFieldList: Constructor for parsed table item			*/
/*  name	  : const char *     : Field name			*/
/*  next	  : XBSQLTabelItem * : Next item in list		*/
/*  (returns)	  : XBSQLFieldList   :					*/

XBSQLFieldList::XBSQLFieldList
	(	const char	*name,
		XBSQLFieldList	*next
	)
	:
	fldname	(name ),
	next	(next)
{
}

/*  XBSQLFieldList							*/
/*  ~XBSQLFieldList: Destructor for parsed field item			*/
/*  (returns)	   :		:					*/

XBSQLFieldList::~XBSQLFieldList ()
{
	if (next  != 0) delete next  ;
}

/*  XBSQLFieldList							*/
/*  linkDatabase: Link fields to database table				*/
/*  query	: XBSQLQuery *	: Parent query				*/
/*  (returns)	: void		:					*/

bool	XBSQLFieldList::linkDatabase
	(	XBSQLQuery	*query
	)
{
	int	maxtab	= -1 ;
	if (!query->findField (0, fldname, field, maxtab)) return false ;

	return	next == 0 ? true : next->linkDatabase (query) ;
}

/*  XBSQLFieldList							*/
/*  saveValue	: Save value into database				*/
/*  value	: XBSQLValue &	: The value				*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLFieldList::saveValue
	(	XBSQLValue	&value
	)
{
	return	field.setField (value) ;
}
