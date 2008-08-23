/***************************************************************************
    file                 : xb_fieldset.cpp
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
#include	<unistd.h>

#include	"xbsql.h"


/*  XBSQLFieldSet							*/
/*  XBSQLFieldSet: Constructor for list-of-fields class			*/
/*  xbase	 : XBaseSQL *	: Database				*/
/*  table	 : XBSQLTable *	: Open table				*/

XBSQLFieldSet::XBSQLFieldSet
	(	XBaseSQL	*xbase,
		XBSQLTable	*table
	)
	:
	xbase	(xbase),
	tabname	(strdup (table->getTabName()))
{
	fieldSet.setNumFields (4, 0, 4) ;

	for (int idx = 0 ; idx < table->FieldCount() ; idx += 1)
	{
		fieldSet.addNewRow(0) ;
		fieldSet.getValue (0) = table->GetFieldName    (idx) ;
		fieldSet.getValue (1) = table->GetFieldType    (idx) ;
		fieldSet.getValue (2) = table->GetFieldLen     (idx) ;
		fieldSet.getValue (3) = table->GetFieldDecimal (idx) ;
	}
}

XBSQLFieldSet::~XBSQLFieldSet ()
{
	free	((void *)tabname) ;
}

XBSQL::Index
	XBSQLFieldSet::fieldIndexed
	(	int	fldno
	)
{
	if ((fldno < 0) || (fldno >= fieldSet.getNumFields()))
		return	XBSQL::IndexNone	;

	char	buff[255]	;
	strncpy	(buff, tabname,		    sizeof(buff)) ;
	strncat	(buff, "_",		    sizeof(buff)) ;
	strncat	(buff, getFieldName(fldno), sizeof(buff)) ;

	const char *path = xbase->getPath (buff, "ndx")	;
	int	fd	 = open (path, O_RDONLY)	;
	free	((void *)path) ;

	if (fd == 0) return XBSQL::IndexNone ;
	if (read (fd, buff, 24) != 24)
	{	close	(fd) ;
		return XBSQL::IndexNone ;
	}

	close	(fd) ;
	return	buff[23] == 0 ? XBSQL::IndexNotUnique : XBSQL::IndexUnique ;
}
