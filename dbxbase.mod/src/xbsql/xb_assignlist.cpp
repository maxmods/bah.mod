/***************************************************************************
    file                 : xb_assignlist.cpp
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

/*  XBSQLAssignList							*/
/*  XBSQLAssignList: Constructor for assignment item			*/
/*  fldname	   : const char *     : Field name			*/
/*  expr	   : XBSQLExprNode *  : Expression			*/
/*  next	   : XBSQLAssignList *: Next item in list		*/
/*  (returns)	   : XBSQLAssignList  :					*/

XBSQLAssignList::XBSQLAssignList
	(	const char	*fldname,
		XBSQLExprNode	*expr,
		XBSQLAssignList	*next
	)
	:
	fldname	(fldname),
	expr	(expr),
	next	(next)
{
}

/*  XBSQLAssignList							*/
/*  ~XBSQLAssignList: Destructor for parsed field item			*/
/*  (returns)	    :		:					*/

XBSQLAssignList::~XBSQLAssignList ()
{
	if (next != 0) delete next  ;
	if (expr != 0) delete expr  ;
}

/*  XBSQLAssignList							*/
/*  linkDatabase: Link fields to database table				*/
/*  query	: XBSQLQuery *	: Parent query				*/
/*  (returns)	: void		:					*/

bool	XBSQLAssignList::linkDatabase
	(	XBSQLQuery	*query
	)
{
	int	maxtab	= -1 ;
	bool	dummy	;
	if (!query->findField (0, fldname, field, maxtab)) return false  ;
	if (!expr ->linkDatabase   (query, dummy, maxtab)) return false  ;

	return	next == 0 ? true : next->linkDatabase (query) ;
}

/*  XBSQLAssignList							*/
/*  assignValues: Assign values from expressions into database		*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLAssignList::assignValues ()
{
	XBSQLValue	value	;

	if (!expr->evaluate (value, 0)) return false ;
	if (!field.setField (value   )) return false ;

	return	next == 0 ? true : next->assignValues () ;
}
