/***************************************************************************
    file                 : xb_table.cpp
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


/*  XBSQLTable								*/
/*  XBSQLTable	: Constructor for table access class			*/
/*  xBase	: XBaseSQL *	: Database				*/
/*  tabname	: const char *	: Table name				*/
/*  dbf		: xbDbf *	: Underlying table			*/
/*  (returns)	: XBSQLTable	:					*/

XBSQLTable::XBSQLTable
	(	XBaseSQL	*xBase,
		const char	*tabname,
		xbDbf		*dbf
	)
	:
	dbf	(dbf),
	xBase	(xBase),
	tabname	(strdup(tabname))
{
	curRecNo = -1	;
	idxlist	 = 0	;
}


/*  XBSQLTable								*/
/*  XBSQLTable	: Destructor for table access class			*/
/*  (returns)	:		:					*/

XBSQLTable::~XBSQLTable ()
{
	xBase->closeTable   (dbf) ;
	free	((void *)tabname) ;
}


/*  XBSQLTable								*/
/*  findField	: Find named field in table				*/
/*  fdlname	: const char *	: Field name				*/
/*  field	: XBSQLField &	: Field information structure		*/
/*  (returns)	: bool		: Success, field found			*/

bool	XBSQLTable::findField
	(	const char	*fldname,
		XBSQLField	&field
	)
{
	int	fldno	;

	if (strcmp (fldname, "_rowid") == 0)
	{
		field.setup (this, XBSQLField::FieldRowID, XBSQL::VNum, 0) ;
		return	true ;
	}

	/* XBase field names are limited to ten characters, so silently	*/
	/* chop anything over.						*/
	char	_fldname[12] ;
	strncpy	(_fldname, fldname, 10) ;
	_fldname[10] = 0 ;

	if ((fldno = GetFieldNo (_fldname)) < 0)
	{
		xBase->setError
		(	"No column \"%s\" in table \"%s\"",
			fldname,
			tabname
		)	;
		return	false	;
	}

	field.setup (this, fldno,
			   XTypeToVType (dbf->GetFieldType (fldno)),
			   dbf->GetFieldLen  (fldno)) ;
	return	true ;
}

/*  XBSQLTable								*/
/*  rewind	: Rewind table prior to reading records			*/
/*  (returns)	: bool		:					*/

void	XBSQLTable::rewind ()
{
	curRecNo = -1 ;
}

/*  XBSQLTable								*/
/*  nextRecord	: Get next record from table				*/
/*  ok		: bool &	: Operation OK flag (ie., no errors)	*/
/*  (returns)	: bool		: Success (got a record)		*/

bool	XBSQLTable::nextRecord
	(	bool	&ok
	)
{
	xbShort	rc	;

	/* The starting (current record number is negative) then next	*/
	/* record fetch should return the first record. If set then	*/
	/* get the first record (except check in case the table is	*/
	/* empty), otherwise get the next record.			*/
	if (curRecNo < 0)
	{
		if (dbf->NoOfRecords () == 0)
		{	ok	= true  ;
			return	false	;
		}

		rc	= dbf->GetFirstRecord () ;
	}
	else
	{
		if (curRecNo != dbf->GetCurRecNo())
		{
			rc = dbf->GetRecord (curRecNo) ;
			if (rc != XB_NO_ERROR)
			{	ok	= false ;
				return	false	;
			}
		}

		rc	= dbf->GetNextRecord  () ;
	}

	/* Now scan forward for the first record that is not marked as	*/
	/* deleted, checking as we go for the end of the table and for	*/
	/* possible errors.						*/
	while (rc == XB_NO_ERROR)
	{
		if (dbf->RecordDeleted ())
		{	rc = dbf->GetNextRecord () ;
			continue ;
		}

		/* Note the record number in case the underlying table	*/
		/* is open more than once in the same select query.	*/
		curRecNo = dbf->GetCurRecNo() ;

		ok	 = true	 ;	/* No real error ...		*/
		return	 true	 ;	/* ... and got a record		*/
	}

	/* At the end of the table, in which case there is no error but	*/
	/* no record either.						*/
	if (rc == XB_EOF)
	{
		ok	 = true	;
		return	 false	;
	}
	

	/* Otherwise, we have got an error. Return false indicating no	*/
	/* record, with "ok" false to signify the error.		*/
	xBase->setError (rc)	;
	ok	= false	;
	return	false	;
}

/*  XBSQLTable								*/
/*  indexForField: Look for index for named field			*/
/*  _field	 : const char *	: Field in question			*/
/*  (returns)	 : XBSQLIndex *	: Index or NULL if none			*/

XBSQLIndex *XBSQLTable::indexForField
	(	const char *field
	)
{
	return	idxlist == 0 ? 0 : idxlist->indexForField (field) ;
}


/*  ------------------------------------------------------------------  */

/*  XBSQLIndex								*/
/*  XBSQLIndex	: Constructor for index					*/
/*  dbf		: xbDbf *	: Underlying table			*/
/*  path	: const char *	: Path to index file			*/
/*  field	: const char *	: Field name				*/
/*  next	: XBSQLIndex *	: Next on list				*/

XBSQLIndex::XBSQLIndex
	(	xbDbf		*dbf,
		const char	*path,
		const char	*_field,
		XBSQLIndex	*next
	)
	:
	xbNdx	(dbf),
	field	(strdup (_field)),
	next	(next)
{
	xbShort	rc	;

	if ((rc = OpenIndex (path)) != XB_NO_ERROR)
	{	fprintf	(stderr, "xbsql: open index error %d\n", rc) ;
		free	((void *)field) ;
		field	= strdup ("[nofield]") ;
	}
}

/*  XBSQLIndex								*/
/*  ~XBSQLIndex	: Destructor for index					*/
/*  (returns)	:		:					*/

XBSQLIndex::~XBSQLIndex ()
{
	if (next  != 0) delete next ;
	if (field != 0) free ((void *)field) ;

	CloseIndex ()  ;
}

/*  XBSQLIndex								*/
/*  indexForField: Look for index for named field			*/
/*  _field	 : const char *	: Field in question			*/
/*  (returns)	 : XBSQLIndex *	: Index or NULL if none			*/

XBSQLIndex *XBSQLIndex::indexForField
	(	const char	*_field
	)
{
	return	strcmp (field, _field) == 0 ? this :
		next		       == 0 ? 0	   :
					      next->indexForField (_field) ;
}


/*  ------------------------------------------------------------------  */

/*  Next block of methods are simply wrappers that map directly to the	*/
/*  underlying XBase file.						*/

/*  XBSQLTable								*/
/*  GetFieldNo	: Get field number for named field			*/
/*  name	: const char *	: Field name				*/
/*  (returns)	: xbShort	: Field number				*/

xbShort	XBSQLTable::GetFieldNo
	(	const char 	*name
	)
{
	return	dbf->GetFieldNo (name) ;
}

/*  XBSQLTable								*/
/*  GetFieldType: Get field type for specified field			*/
/*  fieldNo	: xbShort	: Field number				*/
/*  (returns)	: xbShort	: Field type				*/

xbShort	XBSQLTable::GetFieldType
	(	xbShort		fieldNo
	)
{
	return	dbf->GetFieldType (fieldNo) ;
}

/*  XBSQLTable								*/
/*  GetFieldName: Get field name for specified field			*/
/*  fieldNo	: xbShort	: Field number				*/
/*  (returns)	: char *	: Field name				*/

char	*XBSQLTable::GetFieldName
	(	xbShort		fieldNo
	)
{
	return	dbf->GetFieldName (fieldNo) ;
}

/*  XBSQLTable								*/
/*  GetFieldLen	: Get field length for specified field			*/
/*  fieldNo	: xbShort	: Field number				*/
/*  (returns)	: xbShort	: Field length				*/

xbShort	XBSQLTable::GetFieldLen
	(	xbShort		fieldNo
	)
{
	return	dbf->GetFieldLen (fieldNo) ;
}

/*  XBSQLTable								*/
/*  GetFieldDecimal							*/
/*		: Get field precision for specified field		*/
/*  fieldNo	: xbShort	: Field number				*/
/*  (returns)	: xbShort	: Field precision			*/

xbShort	XBSQLTable::GetFieldDecimal
	(	xbShort		fieldNo
	)
{
	return	dbf->GetFieldDecimal (fieldNo) ;
}

/*  XBSQLTable								*/
/*  FieldCount	: Get number of fields in table				*/
/*  (returns)	: xbShort	: Field count				*/

xbLong	XBSQLTable::FieldCount ()
{
	return	dbf->FieldCount () ;
}

/*  XBSQLTable								*/
/*  NoOfRecords	: Get number of records in table			*/
/*  (returns)	: xbLong	: Number of records			*/

xbLong	XBSQLTable::NoOfRecords	()
{
	return	dbf->NoOfRecords () ;
}

/*  XBSQLTable								*/
/*  GetRecord	: Load specified record					*/
/*  recNo	: xbLong	: Record number				*/
/*  (returns)	: xbShort	: Success or otherwise			*/

xbShort	XBSQLTable::GetRecord
	(	xbLong		recNo
	)
{
	return	dbf->GetRecord (curRecNo = recNo) ;
}

/*  ------------------------------------------------------------------  */

/*  Following methods are used to update values. These are wrappers as	*/
/*  above; since insert and update only allow a single table, the	*/
/*  underlying XBase file must already be positioned at the right	*/
/*  record number							*/


xbShort	XBSQLTable::PutField
	(	xbShort		fieldNo,
		const char 	*value
	)
{
	return	dbf->PutField (fieldNo, value) ;
}

xbShort	XBSQLTable::PutFloatField
	(	xbShort		fieldNo,
		xbFloat		value
	)
{
	return	dbf->PutFloatField (fieldNo, value) ;
}

xbShort	XBSQLTable::PutLongField
	(	xbShort		fieldNo,
		xbLong		value
	)
{
	return	dbf->PutLongField (fieldNo, value) ;
}

/*  XBSQLTable								*/
/*  UpdateMemoField							*/
/*		: Update data in memo field				*/
/*  fieldNo	: xbShort	: Field number				*/
/*  buffLen	: xbLong	: Data buffer length			*/
/*  buffer	: char *	: Data buffer				*/
/*  lock	: xbShort	: Lock option				*/
/*  (returns)	: xbShort	: Success or otherwise			*/

xbShort	XBSQLTable::UpdateMemoData
	(	xbShort		fieldNo,
		xbLong		buffLen,
		const char 	*buffer,
		xbShort		lock
	)
{
	return	dbf->UpdateMemoData (fieldNo, buffLen, buffer, lock) ;
}

xbShort	XBSQLTable::DeleteRecord ()
{
	return	dbf->DeleteRecord () ;
}

xbShort	XBSQLTable::AppendRecord ()
{
	return	dbf->AppendRecord () ;
}

void	XBSQLTable::BlankRecord ()
{
	dbf->BlankRecord () ;
}

xbShort	XBSQLTable::PutRecord ()
{
	return	dbf->PutRecord	() ;
}

xbShort	XBSQLTable::DeleteAllRecords ()
{
	return	dbf->DeleteAllRecords () ;
}


/*  ------------------------------------------------------------------  */

/*  Now come the methods used to retrieve records. Since a select query	*/
/*  may specify the same table more than once, we need to check that	*/
/*  we are at the correct record before retrieving the value. We will	*/
/*  assume that seeking always works, since we must have visited the	*/
/*  record prior to these calls.					*/

/*  XBSQLTable								*/
/*  getDoubleField							*/
/*		: Get value from double field				*/
/*  fieldNo	: xbShort	: Field number				*/
/*  (returns)	: xbDouble	: Value					*/

xbDouble XBSQLTable::GetDoubleField
	 (	xbShort		fieldNo
	 )
{
	if (curRecNo != dbf->GetCurRecNo())
		dbf->GetRecord (curRecNo) ;

	return dbf->GetDoubleField (fieldNo) ;
}

/*  XBSQLTable								*/
/*  getLogicalField							*/
/*		: Get value from logical field				*/
/*  fieldNo	: xbShort	: Field number				*/
/*  (returns)	: xbShort	: Value					*/

xbShort XBSQLTable::GetLogicalField
	(	xbShort		fieldNo
	)
{
	if (curRecNo != dbf->GetCurRecNo())
		dbf->GetRecord (curRecNo) ;

	return dbf->GetLogicalField (fieldNo) ;
}

/*  XBSQLTable								*/
/*  getLongField: Get value from long field				*/
/*  fieldNo	: xbShort	: Field number				*/
/*  (returns)	: xbLong	: Value					*/

xbLong	XBSQLTable::GetLongField
	(	xbShort		fieldNo
	)
{
	if (curRecNo != dbf->GetCurRecNo())
		dbf->GetRecord (curRecNo) ;

	return dbf->GetLongField (fieldNo) ;
}

/*  XBSQLTable								*/
/*  getField	: Get value from field					*/
/*  fieldNo	: xbShort	: Field number				*/
/*  buffer	: char *	: Data buffer				*/
/*  (returns)	: xbShort	: Success or otherwise			*/

xbShort	XBSQLTable::GetField
	(	xbShort		fieldNo,
		char		*buffer
	)
{
	if (curRecNo != dbf->GetCurRecNo())
		dbf->GetRecord (curRecNo) ;

	return	dbf->GetField (fieldNo, buffer) ;
}

/*  XBSQLTable								*/
/*  GetMemoFieldLen							*/
/*		: Get length of data in memo field			*/
/*  fieldNo	: xbShort	: Field number				*/
/*  (returns0	: xbLong	: Data length				*/

xbLong	XBSQLTable::GetMemoFieldLen
	(	xbShort		fieldNo
	)
{
	if (curRecNo != dbf->GetCurRecNo())
		dbf->GetRecord (curRecNo) ;

	return	dbf->GetMemoFieldLen (fieldNo) ;
}

/*  XBSQLTable								*/
/*  GetMemoField: Get data from memo field				*/
/*  fieldNo	: xbShort	: Field number				*/
/*  buffLen	: xbLong	: Data buffer length			*/
/*  buffer	: char *	: Data buffer				*/
/*  lock	: xbShort	: Lock option				*/
/*  (returns)	: xbShort	: Success or otherwise			*/

xbShort	XBSQLTable::GetMemoField
	(	xbShort		fieldNo,
		xbLong		buffLen,
		char 		*buffer,
		xbShort		lock
	)
{
	if (curRecNo != dbf->GetCurRecNo())
		dbf->GetRecord (curRecNo) ;

	return	dbf->GetMemoField   (fieldNo, buffLen, buffer, lock) ;
}

/*  XBSQLTable								*/
/*  GetCurRecNo	: Get current record number				*/
/*  (returns)	: xbLong	: Record number				*/

xbLong	XBSQLTable::GetCurRecNo ()
{
	/* Note that we return the value saved after the last call to	*/
	/* ::nextRecord, etc., which may *not* be the record that the	*/
	/* underlying XBase table is at.				*/
	return	curRecNo ;
}

void	XBSQLTable::NoteCurRecNo ()
{
	curRecNo = dbf->GetCurRecNo () ;
}
