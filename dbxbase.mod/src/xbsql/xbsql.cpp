/***************************************************************************
    file                 : xbsql.cpp
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



#ifndef 	_WIN32
#include	<dirent.h>
#include	<unistd.h>
#else
#include 	<io.h>
#include 	<fcntl.h>
#endif

#include	<stdio.h>
#include	<stdarg.h>
#include	<sys/types.h>

#include	<errno.h>
#include	<ctype.h>


#include	<xbsql.h>

extern	XBSQLCommand	*xbQuery 	;
extern	int	   	xbsql_parse()	;

// #ifdef	_WIN32 // BaH
// 
// int	strcasecmp
// 	(	const char *s1,
// 		const char *s2
// 	)
// {
// 	while (*s1 && *s2)
// 	{
// 		char ch1 = *s1++ ;
// 		char ch2 = *s2++ ;
// 		if (isupper(ch1)) ch1 = tolower(ch1) ;
// 		if (isupper(ch2)) ch2 = tolower(ch2) ;
// 		if (ch1 != ch2) return ch1 - ch2 ;
// 	}
// 
// 	return *s1 - *s2 ;
// }
// 
// #endif

static	uint	openCount	;
static	uint	closeCount	;


static	bool	validName
	(	const char	*name
	)
{
	if (!isalpha(*name) && (*name != '_'))
		return	false	;

	for (const char *cp = &name[1] ; *cp != 0 ; cp += 1)
		if (!isalnum(*cp) && (*cp != '_') && (*cp != ' '))
			return	false	;

	return	true	;
}

/*  XBaseSQL								*/
/*  XBaseSQL	: Constructor for XBase SQL interface			*/
/*  dbDir	: const char *	: Directory in which database resides	*/
/*  (returns)	: XBaseSQL	:					*/

XBaseSQL::XBaseSQL
	(	const char	*dbDir
	)
	:
	dbDir	(strdup (dbDir))
{
	dbError	= strdup ("") ;
	memset	(openTables, 0, sizeof(openTables)) ;

	realDelete 	= false ;
	closePack  	= false ;
	caseSensitive	= false ;
	useWildcard	= false ;
	packList   	= 0	;
	goSlow		= false	;
}

/*  XBaseSQL								*/
/*  XBaseSQL	: Destructor for XBase SQL interface			*/
/*  (returns)	: void		:					*/

XBaseSQL::~XBaseSQL ()
{
	for (int idx = 0 ; idx < MAX_OPEN_TABLES ; idx += 1)
		if (openTables[idx].dbf != 0)
		{	if (openTables[idx].index != 0) delete openTables[idx].index ;
			delete	openTables[idx].dbf ;
		}
			

	while (packList != 0)
	{
		XBSQLPackTable	*pack  = packList ;
		packList = packList->next ;

		xbDbf		table	(this) ;
		const char	*path	= getPath (pack->table, "dbf") ;
		xbShort		rc ;

		//fprintf	(stderr, "XBSQL: Packing %s\n", pack->table) ;

		if	((rc = table.OpenDatabase (path   )) != XB_NO_ERROR)
		{
			setError (rc) ;

			/*fprintf
			(	stderr,
				"XBSQL: OpenDatabase(%s) failed: %s\n",
				path,
				dbError
			)	;*/
		}
		else if	((rc = table.PackDatabase (F_SETLK)) != XB_NO_ERROR)
		{
			setError (rc) ;

			/*fprintf
			(	stderr,
				"XBSQL: PackDatabase(%s) failed: %s\n",
				path,
				dbError
			)	;*/

			table.CloseDatabase() ;
		}
		else	table.CloseDatabase() ;

		free	((void *)path)		;
		free	((void *)pack->table)	;
		delete	pack ;
	}

	free	((void *)dbDir  ) ;
	free	((void *)dbError) ;

	/*fprintf	(stderr, "XBSQL: openCount=%u, closeCount=%u\n",
			 openCount,
			 closeCount) ;*/
}

/*  XBaseSQL								*/
/*  setError	: Set error to specified message			*/
/*  error	: const char *	: Error message				*/
/*  ...		: ...		: Arguments				*/
/*  (returns)	: void		:					*/

void	XBaseSQL::setError
	(	const char	*msg,
		...
	)
{
	char	t[1024]	;
	va_list	aptr	;

	va_start  (aptr, msg) ;
#ifndef _WIN32
	vsnprintf (t, sizeof(t), msg, aptr) ;
#else
	vsprintf(t, msg, aptr);
#endif
	va_end	  (aptr) ;

	free	((void *)dbError) ;
	dbError	= strdup (t)	  ;
}

/*  XBaseSQL								*/
/*  setError	: Set error to XBase error				*/
/*  rc		: xbShort	: XBase error code			*/
/*  (returns)	: void		:					*/

void	XBaseSQL::setError
	(	xbShort	rc
	)
{
	const char	*e	;

	free	((void *)dbError) ;

	switch (rc)
	{
		case XB_FILE_EXISTS :
			e	= "Table already exists" ;
			break	;

		default	:
			e	= strerror (rc) ;
			break	;
	}

	dbError	= strdup (e) ;
}

/*  XBaseSQL								*/
/*  setError	: Set error to code and specified message		*/
/*  rc		: xbShort	: XBase error code			*/
/*  error	: const char *	: Error message				*/
/*  ...		: ...		: Arguments				*/
/*  (returns)	: void		:					*/

void	XBaseSQL::setError
	(	xbShort		rc,
		const char	*msg,
		...
	)
{
	char	t[1024]	;
	va_list	aptr	;

	strcpy	  (t, strerror (rc)) ;
	strcat	  (t, ": ") ;
	int	  l = strlen(t) ;

	va_start  (aptr, msg) ;
#ifndef _WIN32
	vsnprintf (&t[l], sizeof(t) - l, msg, aptr) ;
#else
	vsprintf(&t[1], msg, aptr);
#endif
	va_end	  (aptr) ;

	free	((void *)dbError) ;
	dbError	= strdup (t)	  ;
}

/*  XBaseSQL								*/
/*  getPath	: Get path to database file				*/
/*  name	: const char *	: File name				*/
/*  suffix	: const char *	: Optional suffix			*/
/*  (returns)	: const char *	: Path					*/

const char *XBaseSQL::getPath
	(	const char	*name,
		const char	*suffix
	)
{
	/* Append the file name to the database directory. Note that it	*/
	/* is the callers responsbility to free the buffer.		*/
	int	plen	= strlen(dbDir) + strlen(name) + 2 ;

	if (suffix != 0) plen += strlen (suffix) + 1 ;

	char	*path	= (char *)malloc(plen) ;

	strcpy	(path, dbDir)	;
#ifndef _WIN32
	strcat	(path, "/"  )	;
#else
	strcat(path, "\\");
#endif
	strcat	(path, name )	;

	if (suffix != 0)
	{
		strcat	(path, "."   ) ;
		strcat	(path, suffix) ;
	}

	return	path	;
}

/*  XBaseSQL								*/
/*  createTable	: Create a new table					*/
/*  table	: const char *	: Table name				*/
/*  schema	: xbSchema *	: Table schema				*/
/*  index	: IndexType *	: Column index flags			*/
/*  (returns)	: bool		: Success				*/

bool	XBaseSQL::createTable
	(	const char	*table,
		xbSchema	*schema,
		XBSQL::Index	*index
	)
{
	extern	bool	xbIsKeyword (const char *) ;

	xbDbf		dbfFile (this)	;
	xbShort		rc		;
	const char	*path		= getPath (table, "dbf") ;

	/* Sanity checks. XBase will allow lots of things as valid	*/
	/* table and column names, most of which are not going to be	*/
	/* valid in an SQL query. Also, creating zero length columns	*/
	/* is not a good idea, and XBase does not automatically map	*/
	/* memo fields to the correct 10 characters in the table	*/
	/* itself.							*/
	if (xbIsKeyword (table))
	{
		setError("Table name %s is a keyword", table) ;
		return	false	;
	}

	if (!validName (table))
	{
		setError("Table name %s contains invalid characters", table) ;
		return	false	;
	}


	for (xbSchema *s = schema ; s->FieldName[0] != 0 ; s += 1)
	{
		if (xbIsKeyword (s->FieldName))
		{
			setError("Column name %s is a keyword", s->FieldName) ;
			return	false	;
		}

		if (!validName (s->FieldName))
		{
			setError("Column name %s contains invalid characters", s->FieldName) ;
			return	false	;
		}

		if (s->Type == 'M')
			s->FieldLen = 10 ;

		if (s->FieldLen == 0)
		{
			setError("Column %s cannot have zero length", s->FieldName) ;
			return	false	;
		}
	}

#ifndef _WIN32
	if ( access(path, F_OK) == 0)
#else
	if (_access(path,    6) == 0)
#endif
	{
		setError("Table already exists") ;
		return	false	;
	}

	dbfFile.SetVersion (4)	 ;

	if ((rc = dbfFile.CreateDatabase (path, schema, XB_DONTOVERLAY)) != XB_NO_ERROR)
	{
		setError (rc)	 	 ;
		dbfFile.CloseDatabase () ;
		unlink	 (path)		 ;
		free	 ((void *)path)	 ;
		return	 false		 ;
	}

	free	((void *)path)	;

	if (index != 0)
		for (int idx = 0 ; schema[idx].FieldName[0] != 0 ; idx += 1)
		{
			if (index[idx] == XBSQL::IndexNone) continue  ;

			int	idxflag		   ;
			char	name	[256]	   ;
			xbNdx 	ndxFile	(&dbfFile) ;

			strncpy	(name, table,			sizeof(name)) ;
			strncat	(name, "_",   			sizeof(name)) ;
			strncat	(name, schema[idx].FieldName,	sizeof(name)) ;

			path	= getPath (name, "ndx") ;
			idxflag	= index[idx] == XBSQL::IndexUnique ?
						XB_UNIQUE	:
						XB_NOT_UNIQUE	;

			if ((rc = ndxFile.CreateIndex (path, schema[idx].FieldName,
							     idxflag,
							     XB_DONTOVERLAY)) != XB_NO_ERROR)
			{
				setError (rc)	 	 ;
				ndxFile.CloseIndex    () ;
				dbfFile.CloseDatabase () ;
				unlink	 (path)		 ;
				free	 ((void *)path)	 ;
				return	 false		 ;
			}

			ndxFile.CloseIndex () ;
			free   ((void *)path) ;
		}

	dbfFile.CloseDatabase () ;
	return	true		 ;
}

/*  XBaseSQL								*/
/*  openTable	: Open an extant table					*/
/*  table	: const char *	: Table name				*/
/*  (returns)	: XBSQLTable *	: Table or null on error		*/

XBSQLTable *XBaseSQL::openTable
	(	const char	*table
	)
{
	int		slot	  ;
	XBSQLOpenTable	*openTbl  ;

	for (slot = 0 ; slot < MAX_OPEN_TABLES ; slot += 1)
	{
		openTbl = &openTables[slot] ;

		if (openTbl->dbf 		  == 0) continue ;
		if (strcmp (openTbl->name, table) != 0) continue ;

		openTbl->usecount += 1 ;

		XBSQLTable *xbTable = new XBSQLTable (this, table, openTbl->dbf) ;
		xbTable->setIndexes (openTbl->index) ;
		return	    xbTable ;
	}


	for (slot = 0 ; slot < MAX_OPEN_TABLES ; slot += 1)
		if (openTables[slot].dbf == 0)
			break	;

	if (slot >= MAX_OPEN_TABLES)
	{	setError ("Maximum number of open tables reached") ;
		return	 0 ;
	}

	const char	*path	= getPath   (table, "dbf") ;
	xbDbf		*dbf	= new xbDbf (this) ;
	xbShort		rc	;

	if ((rc = dbf->OpenDatabase (path)) != XB_NO_ERROR)
	{	delete	 dbf	;
		free	 ((void *)path) ;
		setError (rc)	;
		return	 0	;
	}

	XBSQLTable *xbTable	= new XBSQLTable (this, table, dbf) ;

	openTables[slot].name	  = strdup(table) ;
	openTables[slot].dbf	  = dbf	;
	openTables[slot].index	  = 0	;
	openTables[slot].usecount = 1	;

	free	((void *)path)	;

	XBSQLFieldSet fSet (this, xbTable) ;

	for (int idx = 0 ; idx < fSet.getNumFields() ; idx += 1)
	{
		char	name[256]	;

		strncpy	(name, table,			sizeof(name)) ;
		strncat	(name, "_",			sizeof(name)) ;
		strncat	(name, fSet.getFieldName (idx),	sizeof(name)) ;

		path	= getPath (name, "ndx") ;
#ifndef _WIN32
		if ( access(path, R_OK) == 0)
#else
		if (_access(path,    6) == 0)
#endif
			openTables[slot].index = new XBSQLIndex
						 (	dbf, 
							path,
							fSet.getFieldName(idx),
							openTables[slot].index
						 )	;

		free	((void *)path) ;
	}

	xbTable->setIndexes (openTables[slot].index) ;

//	fprintf	(stderr, "XBSQL: openTable(%s) -> %p\n", table, (void *)xbTable) ;
	openCount += 1	;
	return	xbTable	;
}

/*  XBaseSQL								*/
/*  closeTable	: Close an open table					*/
/*  dbf		: xbDbf *	: Underlying table			*/
/*  (returns)	: void		:					*/

void	XBaseSQL::closeTable
	(	xbDbf	*dbf
	)
{
	/* Scan the open tables list looking for the one that is being	*/
	/* closed. NOTE: Don't delete it since this routine is called	*/
	/* from the XBSQLTable destructor.				*/
	for (int idx = 0 ; idx < MAX_OPEN_TABLES ; idx += 1)
		if (openTables[idx].dbf == dbf)
		{
			if ((openTables[idx].usecount -= 1) <= 0)
			{	
				//fprintf	(stderr, "XBSQL: closeTable(%s) -> %p\n", openTables[idx].name, (void *)dbf) ;
				closeCount += 1	;

				if (openTables[idx].index != 0) delete openTables[idx].index ;

				dbf->CloseDatabase ()     ;
				delete	dbf ;

				free	((void *)openTables[idx].name) ;

				openTables[idx].dbf   = 0 ;
				openTables[idx].name  = 0 ;
				openTables[idx].index = 0 ;
			}

			return	;
		}
}

/*  XBaseSQL								*/
/*  openSelect	: Open a select query					*/
/*  query	: const char *	: The query				*/
/*  (returns)	: XBSQLSelect *	: New select object or null on error	*/

XBSQLSelect *XBaseSQL::openSelect
	(	const char	*query
	)
{
	XBSQLSelect	*xbSelect ;

//	fprintf	   (stderr, "parse: %s\n", query) ;
	initParser (this, query) ;
	xbsql_parse () ;

	if (xbQuery == 0)
	{	setError ("SQL parse error") ;
		return	 0 ;
	}

	if ((xbSelect = xbQuery->isSelect ()) == 0)
	{	setError ("SQL parse error or not a select query") ;
		return	 0 ;
	}

	if (!xbSelect->linkDatabase ())
	{	delete	xbSelect ;
		return	0 ;
	}

	return	xbSelect   ;
}

/*  XBaseSQL								*/
/*  openInsert	: Open an insert query					*/
/*  query	: const char *	: The query				*/
/*  (returns)	: XBSQLInsert *	: New insert object or null on error	*/

XBSQLInsert *XBaseSQL::openInsert
	(	const char	*query
	)
{
	XBSQLInsert	*xbInsert ;

	initParser (this, query) ;
	xbsql_parse	   () ;

	if (xbQuery == 0)
	{	setError ("SQL parse error") ;
		return	 0 ;
	}

	if ((xbInsert = xbQuery->isInsert ()) == 0)
	{	setError ("SQL parse error or not an insert query") ;
		return	 0 ;
	}

	if (!xbInsert->linkDatabase ())
	{	delete	xbInsert ;
		return	0 ;
	}

	return	xbInsert   ;
}

/*  XBaseSQL								*/
/*  openUpdate	: Open an update query					*/
/*  query	: const char *	: The query				*/
/*  (returns)	: XBSQLUpdate *	: New update object or null on error	*/

XBSQLUpdate *XBaseSQL::openUpdate
	(	const char	*query
	)
{
	XBSQLUpdate	*xbUpdate ;

	initParser (this, query) ;
	xbsql_parse () ;

	if (xbQuery == 0)
	{	setError ("SQL parse error") ;
		return	 0 ;
	}

	if ((xbUpdate = xbQuery->isUpdate ()) == 0)
	{	setError ("SQL parse error or not an insert query") ;
		return	 0 ;
	}

	if (!xbUpdate->linkDatabase ())
	{	delete	xbUpdate ;
		return	0 ;
	}

	return	xbUpdate   ;
}

/*  XBaseSQL								*/
/*  addClosePack: Add table to pack-on-close list			*/
/*  table	: const char *	: Table name				*/
/*  (returns)	: void		:					*/

void	XBaseSQL::addClosePack
	(	const char *	table
	)
{
	if (closePack)
	{
		XBSQLPackTable *pack ;

		for (pack = packList ; pack != 0 ; pack = pack->next)
			if (strcmp (table, pack->table) == 0)
				break	;

		if  (pack == 0)
		{
			/*fprintf
			(	stderr,
				"XBSQL: Adding [%s] to pack list\n",
				table
			) ;*/

			pack	    = new XBSQLPackTable ;
			pack->table = strdup (table)	 ;
			pack->next  = packList	;
			packList    = pack	;
		}
	}
}

/*  XBaseSQL								*/
/*  openDelete	: Open a delete query					*/
/*  query	: const char *	: The query				*/
/*  (returns)	: XBSQLUpdate *	: New delete object or null on error	*/

XBSQLDelete *XBaseSQL::openDelete
	(	const char	*query
	)
{
	XBSQLDelete	*xbDelete ;

	initParser (this, query) ;
	xbsql_parse () ;

	if (xbQuery == 0)
	{	setError ("SQL parse error") ;
		return	 0 ;
	}

	if ((xbDelete = xbQuery->isDelete ()) == 0)
	{	setError ("SQL parse error or not a delete query") ;
		return	 0 ;
	}

	if (!xbDelete->linkDatabase ())
	{	delete	xbDelete ;
		return	0 ;
	}

	xbDelete->setRealDelete (realDelete) ;

	return	xbDelete   ;
}

/*  XBaseSQL								*/
/*  openDelete	: Open an unspecified query				*/
/*  query	: const char *	: The query				*/
/*  parsed	: bool &	: Set true if query was parsed		*/
/*  (returns)	: XBSQLQuery *	: New query object or null on error	*/

XBSQLQuery *XBaseSQL::openQuery
	(	const char	*query,
		bool		&parsed
	)
{
	/* This method is used to open an unspecified query (ie., an	*/
	/* instance of select, insert, update or delete). We parse the	*/
	/* query and check that it is at least recognised.		*/
	parsed	= false ;
	initParser (this, query) ;
	xbsql_parse () ;

	if ((XBSQLQuery*)xbQuery == 0)
	{	setError ("SQL parse error") ;
		return	 0 ;
	}

	if ( (xbQuery->isSelect () == 0) &&
	     (xbQuery->isInsert () == 0) &&
	     (xbQuery->isUpdate () == 0) &&
	     (xbQuery->isDelete () == 0) )
	{
		setError ("Unrecognised SQL query") ;
		delete	 xbQuery ;
		return	 0 ;
	}

	if (xbQuery->isDelete() != 0)
		xbQuery->isDelete()->setRealDelete (realDelete) ;

	parsed	= true	;
	if (!xbQuery->isQuery()->linkDatabase ())
	{	delete	xbQuery	;
		return	0	;
	}

	return	xbQuery->isQuery() ;
}

/*  XBaseSQL								*/
/*  execCommand	: Execute a command					*/
/*  command	: const char *	: Command				*/
/*  (returns)	: bool		: Success				*/

bool	XBaseSQL::execCommand
	(	const char	*command
	)
{
	/* This method is intended as a showcut to execute a command	*/
	/* (such as "create table .....") where the command either	*/
	/* succeeds or not (and returns no other results), and does not	*/
	/* involve placeholders.					*/
	XBSQLCreate	*xbCreate  ;
	XBSQLDrop	*xbDrop  ;
	bool		rc	   = false ;

	initParser (this, command) ;
	xbsql_parse	() ;

	if (xbQuery == 0)
	{	setError ("SQL parse error") ;
		return	 0 ;
	}

	/* Check for each of the possible cases. Note that the parse	*/
	/* might succeed without the command being recognised, for	*/
	/* instance if we were actuall passed "select ..."		*/
	if	((xbCreate = xbQuery->isCreate ()) != 0)
	{
		rc	= xbCreate->execute () ;
	}
	else if ((xbDrop   = xbQuery->isDrop   ()) != 0)
	{
		rc	= xbDrop  ->execute () ;
	}
	else	setError ("Unrecognised XBSQL SQL command") ;

	delete	xbQuery	;
	return	rc	;
}

/*  XBaseSQL								*/
/*  getTableSet	: Get list of tables in database			*/
/*  (returns)	: XBSQLTableSet * : List of tables object		*/

XBSQLTableSet *XBaseSQL::getTableSet ()
{
	return	new XBSQLTableSet (this) ;
}

/*  XBaseSQL								*/
/*  getFieldSet	: Get list of fields in a table in the database		*/
/*  tabname	: const char *	  : Table name				*/
/*  (returns)	: XBSQLFieldSet * : List of tables object		*/

XBSQLFieldSet *XBaseSQL::getFieldSet
	(	const char	*tabname
	)
{
	XBSQLTable	*table	;
	XBSQLFieldSet	*fields	;

	if ((table = openTable (tabname)) == 0) return 0 ;

	fields	= new XBSQLFieldSet (this, table) ;

	delete	table	;
	return	fields	;
}

/*  XBaseSQL								*/
/*  renameTable	: Rename a table					*/
/*  oldName	: const char *	: Old table name			*/
/*  newName	: const char *	: New table name			*/
/*  (returns)	: bool		: Success				*/

bool	XBaseSQL::renameTable
	(	const char	*oldName,
		const char	*newName
	)
{
	const char	*oldPath = getPath (oldName, "dbf") ;
	const char	*newPath = getPath (newName, "dbf") ;
	const char	*oldAnon = getPath (oldName, "dbt") ;
	const char	*newAnon = getPath (newName, "dbt") ;
	XBSQLTable	*dbfFile = 0 ;
	bool		rc	 = false ;

	/* Start with the table file itself. Check that it exists and	*/
	/* that we have read access, and then rename it. If this is OK	*/
	/* then rename any additional memo file; the latter is in error	*/
	/* for anything other than non-existance.			*/
#ifndef _WIN32
	if (access (oldPath, R_OK) != 0)
#else
	if (_access(oldPath,    6) !=0)
#endif
	{	setError ("Table %s does not exist or is inaccessible", oldName) ;
		goto	 finish	;
	}
	if (rename (oldPath, newPath) != 0)
	{	setError ("Failed to rename %s: %s", oldName, strerror(errno)) ;
		goto	 finish	;
	}
	if (rename (oldAnon, newAnon) != 0)
		if (errno != ENOENT)
		{	setError ("Failed to rename memo %s: %s", oldName, strerror(errno)) ;
			goto	 finish	;
		}

	free	((void *)oldAnon) ; oldAnon = 0 ;
	free	((void *)newAnon) ; newAnon = 0 ;

	/* We now need to rename any addition index files. Start by	*/
	/* opening the table.						*/
	if ((dbfFile = openTable (newName)) == 0)
		goto	 finish	;


	/* Next bit is nested as the "goto finish" code above would	*/
	/* otherwise skip over the field set constructor.		*/
	{
		XBSQLFieldSet fSet (this, dbfFile) ;

		for (int idx = 0 ; idx < fSet.getNumFields() ; idx += 1)
		{
			/* Generate the paths for the existing and new	*/
			/* index files for the field, and then attempt	*/
			/* to rename the index ....			*/
			char		_oldName[256]	;
			char		_newName[256]	;
			const char	*fname	= fSet.getFieldName (idx) ;

			strncpy	(_oldName, oldName, sizeof(_oldName)) ;
			strncat	(_oldName, "_",     sizeof(_oldName)) ;
			strncat	(_oldName, fname,   sizeof(_oldName)) ;
			strncpy	(_newName, newName, sizeof(_newName)) ;
			strncat	(_newName, "_",	    sizeof(_newName)) ;
			strncat	(_newName, fname,   sizeof(_newName)) ;

			oldAnon	= getPath (_oldName, "ndx") ;
			newAnon	= getPath (_newName, "ndx") ;

			/* An error is OK if the index does not exist	*/
			/* (presumably there isn't one) but any other	*/
			/* error is. Of course, we've now lost an index	*/
			if ((rename (oldAnon, newAnon) != 0) && (errno != ENOENT))
			{
				setError
				(	"Failed to rename %s index %s: %s",
					oldName,
					fname,
					strerror(errno)
				) ;
				goto	 finish	;
			}

			free	((void *)oldAnon) ; oldAnon = 0 ;
			free	((void *)newAnon) ; newAnon = 0 ;
		}
	}

	/* Success. We can not set the return code to success and drop	*/
	/* through the cleanup code.					*/
	rc	= true ;

	finish	:
		free	((void *)oldPath) ;
		free	((void *)newPath) ;
		if (oldAnon != 0) free ((void *)oldAnon) ;
		if (newAnon != 0) free ((void *)newAnon) ;
		if (dbfFile != 0) delete dbfFile ;
		return	rc ;
}

/*  XBaseSQL								*/
/*  dropTable	: Delete a table					*/
/*  table	: const char *	: Table name				*/
/*  (returns)	: bool		: Success				*/

bool	XBaseSQL::dropTable
	(	const char	*table
	)
{
	const char	*tabPath = getPath (table, "dbf") ;
	const char	*memPath = getPath (table, "dbt") ;
	const char	*tabAnon = 0 ;
	XBSQLTable	*dbfFile = 0 ;
	bool		rc	 = false ;

	/* Make an initial check that the table exists and is		*/
	/* accessible ...						*/
#ifndef _WIN32
	if (access (tabPath, R_OK) != 0)
#else
	if (_access(tabPath,    6) != 0)
#endif
	{	setError ("Table %s does not exist or is inaccessible", table) ;
		goto	 finish	;
	}

	/* We first need to delete any additional index files. Start by	*/
	/* opening the table and deleting associated indices. If the	*/
	/* open fails then ignore it: it may just be a duff file.	*/
	if ((dbfFile = openTable (table)) != 0)
	{
		XBSQLFieldSet fSet (this, dbfFile) ;

		for (int idx = 0 ; idx < fSet.getNumFields() ; idx += 1)
		{
			/* Generate the paths for the existing and new	*/
			/* index files for the field, and then attempt	*/
			/* to rename the index ....			*/
			char		_idxName[256]	;
			const char	*fname	= fSet.getFieldName (idx) ;

			strncpy	(_idxName, table, sizeof(_idxName)) ;
			strncat	(_idxName, "_",   sizeof(_idxName)) ;
			strncat	(_idxName, fname, sizeof(_idxName)) ;

			tabAnon	= getPath (_idxName, "ndx") ;

			/* An error is OK if the index does not exist	*/
			/* (presumably there isn't one) but any other	*/
			/* error is. Of course, we've now lost an index	*/
#ifdef _WIN32
			chmod(tabAnon, 0666); // BaH
#endif
			if ((unlink (tabAnon) != 0) && (errno != ENOENT))
			{
				setError
				(	"Failed to delete %s index %s: %s",
					table,
					fname,
					strerror(errno)
				) ;
				goto	 finish	;
			}

			free	((void *)tabAnon) ; tabAnon = 0 ;
		}
	}


	delete	dbfFile	;
	dbfFile	= 0	;

	if (unlink (tabPath) != 0)
	{	setError ("Failed to delete %s: %s", table, strerror(errno)) ;
		goto	 finish	;
	}
	if (unlink (memPath) != 0)
		if (errno != ENOENT)
		{	setError ("Failed to delete %s memo: %s", table, strerror(errno)) ;
			goto	 finish	;
		}

	/* Success. We can not set the return code to success and drop	*/
	/* through the cleanup code.					*/
	rc	= true ;

	finish	:
		free	((void *)tabPath) ;
		free	((void *)memPath) ;
		if (tabAnon != 0) free ((void *)tabAnon) ;
		if (dbfFile != 0) delete dbfFile ;
		return	rc ;
}

