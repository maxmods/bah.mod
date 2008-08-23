/***************************************************************************
    file                 : xb_tableset.cpp
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
#include	<sys/types.h>

//#ifndef		_WIN32 // BaH
#include	<dirent.h>
//#else
//#include	<qdir.h>
//#endif

#include	"xbsql.h"


/*  XBSQLTableSet							*/
/*  XBSQLTableSet: Constructor for list-of-tables class			*/
/*  (returns)	 : XBSQLTableSet :					*/

XBSQLTableSet::XBSQLTableSet
	(	XBaseSQL	*xbase
	)
	:
	xbase	(xbase)
{
// #ifndef	x_WIN32 // BaH
	DIR		*dir	;
	struct dirent	*dent	;
	char		*cp	;
	XBSQLValue	val	;

	tableSet.setNumFields (1, 0, 1) ;

	if ((dir = opendir (xbase->getDBDir())) == 0)
		return	;

	while ((dent = readdir (dir)) != 0)
	{
		if (dent->d_name[0] == '.')
			continue ;

		if ((cp = strchr (dent->d_name, '.')) == 0)
			continue ;

		if ((strcmp (cp, ".dbf") != 0) && (strcmp (cp, ".DBF") != 0))
			continue ;

		*cp	= 0 		;
		val	= dent->d_name	;

		tableSet.addNewRow (0)  ;
		tableSet.setValue  (val, 0) ;
	}

	closedir (dir) ;
// #else
// 	QDir dir(xbase->getDBDir(), "*.dbf", QDir::Files) ;
// 	tableSet.setNumFields(1, 0, 1) ;
// 	XBSQLValue val ;
// 
// 	for (int i = 0 ; i < dir.count() ; i++)
// 	{
// 		val = dir[i].remove(dir[i].length() - 4, 4) ;
// 		tableSet.addNewRow () ;
// 		tableSet.setValue  (val, 0) ;
// 	}
// #endif
}
