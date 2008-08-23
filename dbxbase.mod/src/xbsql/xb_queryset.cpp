/***************************************************************************
    file                 : xb_queryset.cpp
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

#define		ROWINCR		(32)


static	int	gQryFields	;
static	bool	*gOrder		;

static	int	QSCompare
	(	const void	*_a,
		const void	*_b
	)
{
	const XBSQLValue *a = *(const XBSQLValue **)_a ;
	const XBSQLValue *b = *(const XBSQLValue **)_b ;
	int		 rc ;

	for (int idx = 0 ; idx < gQryFields; idx += 1)
		if ((rc = a[idx].order(b[idx])) != 0)
			return	gOrder[idx] ? rc : -rc ;	

	return	0 ;
}

static	int	QSCompareSlow
	(	const void	*_a,
		const void	*_b
	)
{
	const XBSQLValue *a = (*(const RecordInfo **)_a)->data ;
	const XBSQLValue *b = (*(const RecordInfo **)_b)->data ;
	int		 rc ;

	for (int idx = 0 ; idx < gQryFields; idx += 1)
		if ((rc = a[idx].order(b[idx])) != 0)
			return	gOrder[idx] ? rc : -rc ;	

	return	0 ;
}


/*  XBSQLQuerySet							*/
/*  XBSQLQuerySet: Constructor for query set class			*/
/*  goSlow	 : bool		: Slow mode				*/
/*  (returns)	 : XBSQLQuerySet :					*/

XBSQLQuerySet::XBSQLQuerySet
	(	bool	goSlow
	)
	:
	goSlow	(goSlow)
{
	/* The set is initially empty with no space allocated.		*/
	nRows		= 0 ;
	nAlloc		= 0 ;
	values		= 0 ;
	info		= 0 ;
	names		= 0 ;
	types		= 0 ;
	lengths 	= 0 ;

	nQryFields	= 0 ;
	nGetFields	= 0 ;
	nAllFields	= 0 ;
}

/*  XBSQLQuerySet							*/
/*  XBSQLQuerySet: Constructor for query set class			*/
/*  (returns)	 :		:					*/

XBSQLQuerySet::~XBSQLQuerySet ()
{
	clear	() ;
	cleanUp	() ;
}

/*  XBSQLQuerySet							*/
/*  cleanUp	 : Clean up						*/
/*  (returns)	 : void		:					*/

void	XBSQLQuerySet::cleanUp ()
{
	if (types   != 0)
		delete [] types   ;

	if (lengths != 0)
		delete [] lengths ;

	if (names   != 0)
	{	for (uint idx = 0 ; idx < nAllFields ; idx += 1)
			if (names[idx] != 0)
				free	((void *)names[idx]) ;
		delete	[] names ;
	}
}

/*  XBSQLQuerySet							*/
/*  clear	 : Clear current contents				*/
/*  (returns)	 : void		:					*/

void	XBSQLQuerySet::clear ()
{
	if (values != 0)
	{
		for (int r = 0 ; r < nRows ; r += 1)
		{
			if (values[r] != 0) delete [] values[r] ;
			if (info != 0) free (info[r]) ;
		}

		delete	[] values ;
		if (info != 0) delete [] info ;

		values	= 0 ;
		info	= 0 ;
	}

	nAlloc		= ROWINCR  ;
	values		= new XBSQLValue *[ROWINCR] ;

	if (goSlow)
		info	= new RecordInfo *[ROWINCR] ;

	nRows		= 0	   ;
}

/*  XBSQLQuerySet							*/
/*  setNumFields: Set number of fields					*/
/*  _nGetFields	: int		: Number of data fields			*/
/*  _nQryFields	: int		: Number of query fields		*/
/*  _nAllFields	: int		: Number of fields to allocate		*/
/*  _nTables	: int		: Number of tables			*/
/*  (returns)	: void		:					*/

void	XBSQLQuerySet::setNumFields
	(	int	_nGetFields,
		int	_nQryFields,
		int	_nAllFields,
		int	_nTables
	)
{
	cleanUp	() ;

	nGetFields = _nGetFields ;
	nQryFields = _nQryFields ;
	nAllFields = _nAllFields ;
	nTables	   = _nTables	 ;

	types	= new XBSQL::VType [nAllFields] ;
	lengths	= new int	   [nAllFields] ;
	names	= new const char  *[nAllFields] ;
	order	= new bool	   [nAllFields] ;

	for (uint idx = 0 ; idx < nAllFields ; idx += 1)
	{	names  [idx] = 0 	;
		lengths[idx] = 0 	;
		order  [idx] = true 	;
	}
}

/*  XBSQLQuerySet							*/
/*  setSortOrder: Set sorting order					*/
/*  fldNo	: int		: Field number				*/
/*  ascend	: bool		: Sort ascending			*/
/*  (returns)	: void		:					*/

void	XBSQLQuerySet::setSortOrder
	(	int	fldNo,
		bool	ascend
	)
{
	fprintf	(stderr, "XBSQLQuerySet::setSortOrder (%d, %d)\n", fldNo, ascend) ;
	order[fldNo] = ascend ;
}

/*  XBSQLQuerySet							*/
/*  addNewRow	 : Add a new row to the query set			*/
/*  tableList	 : XBSQLTableList * : Associated table list		*/
/*  (returns)	 : void		    :					*/

void	XBSQLQuerySet::addNewRow
	(	XBSQLTableList	*tableList
	)
{
	/* If the current row vector is full then allocate a new one	*/
	/* and copy over the current entries. "memcpy" is OK since	*/
	/* we are only dealing with pointers.				*/
	if (nRows >= nAlloc)
	{
		XBSQLValue **nvals = new XBSQLValue * [nAlloc + ROWINCR] ;
		memcpy	(nvals, values,    nRows * sizeof(XBSQLValue *)) ;
		delete	[] values ;
		values	= nvals   ;

		if (goSlow)
		{
		RecordInfo **ninfo = new RecordInfo * [nAlloc + ROWINCR] ;
		memcpy  (ninfo, info,      nRows * sizeof(RecordInfo *)) ;
		delete	[] info	 ;
		info	= ninfo	 ;
		}

		nAlloc   += ROWINCR 	;
	}

	/* Add a new set of XBSQLValue objects for the new row, and	*/
	/* increment the row count.					*/

	if (goSlow)
	{
		RecordInfo *i = (RecordInfo *)malloc (RISIZE(nTables)) ;
		XBSQLValue *v = new XBSQLValue[nQryFields] ;

		i->data	      = v ;
		tableList->setRecordNos (&i->recNo[0]) ;

		info  [nRows] = i ;
		values[nRows] = v ;
	}
	else
		values[nRows] = new XBSQLValue[nAllFields] ;

	nRows += 1 ;
}

/*  XBSQLQuerySet							*/
/*  setValue	 : Set value in current row				*/
/*  value	 : XBSQLValue &	: Value to set				*/
/*  fldno	 : int		: Field number				*/
/*  (returns)	 : void		:					*/

void	XBSQLQuerySet::setValue
	(	XBSQLValue	&value,
		int		fldno
	)
{
	values[nRows - 1][fldno] = value ;
}

/*  XBSQLQuerySet							*/
/*  getValue	 : Get value in current row				*/
/*  fldno	 : int		: Field number				*/
/*  (returns)	 : XBSQLValue &	: Value					*/

XBSQLValue &XBSQLQuerySet::getValue
	(	int	fldno
	)
{
	/* Note that this method does not return a const-ref. This is	*/
	/* in order that the result of a query expression (which is	*/
	/* most likely a database field) can be stored directly into	*/
	/* an XBSQLValue using the "setFromTable" method.		*/
	return	values[nRows - 1][fldno] ;
}

/*  XBSQLQuerySet							*/
/*  getValue	 : Get value in specified row				*/
/*  rowno	 : int		: Row number				*/
/*  fldno	 : int		: Field number				*/
/*  (returns)	 : XBSQLValue &	: Value					*/

XBSQLValue &XBSQLQuerySet::getValue
	(	int	rowno,
		int	fldno
	)
{
	return	values[rowno][fldno] ;
}

/*  XBSQLQuerySet							*/
/*  getFieldType: Get field type for specified field			*/
/*  fldno	: int		: Field number				*/
/*  (returns)	: XBSQL::VType	: Field type				*/

XBSQL::VType	XBSQLQuerySet::getFieldType
	(	int	fldno
	)
{
	if ((fldno < 0) || (fldno >= nAllFields))
		return	XBSQL::VNull ;

	return	types[fldno] ;
}

/*  XBSQLQuerySet							*/
/*  getFieldLength: Get field length for specified field		*/
/*  fldno	  : int		: Field number				*/
/*  (returns)	  : int		: Field length				*/

int	XBSQLQuerySet::getFieldLength
	(	int	fldno
	)
{
	/* This returns the field length in the database ....		*/
	if ((fldno < 0) || (fldno >= nAllFields))
		return	-1 ;

	return	lengths[fldno] ;
}

/*  XBSQLQuerySet							*/
/*  getFieldName: Get field name for specified field			*/
/*  fldno	: int		: Field number				*/
/*  (returns)	: const char *	: Field name				*/

const char *XBSQLQuerySet::getFieldName
	(	int	fldno
	)
{
	if ((fldno < 0) || (fldno >= nAllFields))
		return	0 ;

	return	names[fldno] ;
}

/*  XBSQLQuerySet							*/
/*  setFieldInfo: Set field information for specified field		*/
/*  fldno	: int		: Field number				*/
/*  type	: XBSQL::VType	: Type					*/
/*  len		: int		: Length				*/
/*  ename	: const char *	: Expression name			*/
/*  (returns)	: xbShort	: XBase field type			*/

void	XBSQLQuerySet::setFieldInfo
	(	int		fldno,
		XBSQL::VType	type,
		int		length,
		const char	*ename
	)
{
	if ((fldno >= 0) && (fldno < nAllFields))
	{
		types  [fldno] = type   ;
		lengths[fldno] = length ;
		names  [fldno] = strdup (ename) ;
	}
}

/*  XBSQLQuerySet							*/
/*  sort	: Sort rows						*/
/*  (returns)	: void		:					*/

void	XBSQLQuerySet::sort ()
{
	if (nQryFields > 0)
	{
		gQryFields = nQryFields	;
		gOrder	   = order	;

		if (goSlow)
			qsort	(info,   nRows, sizeof(RecordInfo *), QSCompareSlow) ;
		else	qsort	(values, nRows, sizeof(XBSQLValue *), QSCompare	   ) ;
	}
}

/*  XBSQLQuerySet							*/
/*  killrow	: Drop specified row and move following rows down	*/
/*  nrow	: int		: Row number				*/
/*  (returns)	: void		:					*/

void	XBSQLQuerySet::killrow
	(	int	nrow
	)
{
	/* This method is used when a select query has a having term	*/
	/* and is called to drop rows that do not pass the test/	*/
	if ((nrow >= 0) && (nrow < nRows))
	{
		delete	[] values[nrow] ;

		while (nrow < nRows - 1)
		{	values[nrow] = values[nrow + 1] ;
			nrow += 1 ;
		}

		nRows	-= 1 ;
	}
}

/*  XBSQLQuerySet							*/
/*  killrow	: Drop specified row when no longer used		*/
/*  nrow	: int		: Row number				*/
/*  (returns)	: void		:					*/

void	XBSQLQuerySet::dumprow
	(	int	nrow
	)
{
	/* This method can be called (via XBSQLSelect) to release a	*/
	/* row which is no longer needed. Useful in order to conserve	*/
	/* space when the caller copies values to their own buffers.	*/

	/* NOTE: Code does not check for dumped rows. Calling getField	*/
	/*	 on a dumper row will crash the program :)		*/
	if ((nrow >= 0) && (nrow < nRows))
	{
		delete	[] values[nrow] ;
		values[nrow] = 0	;
	}

}


xbLong	XBSQLQuerySet::getRecordNo
	(	int	rowno,
		int	tabIdx
	)
{
	return	info == 0 ? 0 : info[rowno]->recNo[tabIdx] ;
}

