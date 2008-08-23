/***************************************************************************
    file                 : xbsql.h
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

#ifndef	_XBASESQL_H
#define	_XBASESQL_H


#if 		defined( _WIN32 )

#include	<windows.h>

typedef		unsigned int	uint ;
#define		F_SETLK		(4)

// BaH
#include "fcntl.h"

// BaH
#define 	XBSQL_API

// BaH
//#ifdef 		XBSQL_EXPORTS
//#define 	XBSQL_API __declspec(dllexport)
//
//#else 		// !XBSQL_EXPORTS
//
//#define 	XBSQL_API __declspec(dllimport)
//
//#endif		// XBSQL_EXPORTS

//XBSQL_API int	strcasecmp  (const char *s1, const char *s2) ;
//XBSQL_API int	strncasecmp (const char *s1, const char *s2, size_t len) ;

#else

#define 	XBSQL_API

#endif

#if defined(__APPLE__) // BaH

typedef unsigned int uint;

#endif

#include	<xbase64/xbase64.h>
/*#include	<xbase64/xbexcept.h>*/

class	XBaseSQL	;
class	XBSQLTable	;
class	XBSQLTableList	;
class	XBSQLField	;
class	XBSQLIndex	;

class	XBSQLExprNode	;
class	XBSQLExprList	;

class	XBSQLQuerySet	;
class	XBSQLQuery	;
class	XBSQLMulti	;
class	XBSQLSelect	;
class	XBSQLInsert	;
class	XBSQLUpdate	;
class	XBSQLDelete	;
class	XBSQLCreate	;
class	XBSQLDrop	;
class	XBSQLValue	;

extern	void	initParser   (XBaseSQL *, const char *) ;
extern	char	*xbStoreText (const char *) ;

#define		MAX_OPEN_TABLES	256
#define		_LIKELIKE	1

struct	RecordInfo
{
	XBSQLValue	*data	;
	xbLong		recNo[1];
}	;
#define	RISIZE(n)	(sizeof(RecordInfo)+sizeof(xbLong)*(n-1))


class	XBSQL_API 	XBSQL
{

public	:

	/* Value types. Note that the ordering is such that conversion	*/
	/* works in ascending numerical value. The bit settings are	*/
	/* incorporated into operator codes to allow easy testing of	*/
	/* whether an operator is applicable.				*/
	enum	VType
	{
		VNull	= 0x0000,
		VBool	= 0x0001,
		VNum	= 0x0002,
		VDouble	= 0x0004,
		VDate	= 0x0008,
		VText	= 0x0010,
		VMemo	= 0x0020
	}	;

	/* Note: logicals are changed to numbers on reading from the	*/
	/* database tables, so we don't need to include them below.	*/
	enum	EType
	{	EField		= 0x00000000,
		ENumber		= 0x00010000,
		EDouble		= 0x00020000,
		EString		= 0x00030000,
		EPlace		= 0x00040000,
		EPlus		= 0x00050000 | XBSQL::VNum  | XBSQL::VDouble,
		EMinus		= 0x00060000 | XBSQL::VNum  | XBSQL::VDouble,
		EMultiply	= 0x00070000 | XBSQL::VNum  | XBSQL::VDouble,
		EDivide		= 0x00080000 | XBSQL::VNum  | XBSQL::VDouble,
		ERem		= 0x00080000 | XBSQL::VNum  ,
		ELike		= 0x0009ffff | XBSQL::VText | XBSQL::VMemo,
		EEquals		= 0x000affff,
		ENotEqual	= 0x000bffff,
		ELT		= 0x000cffff,
		ELTEqual	= 0x000dffff,
		EGT		= 0x000effff,
		EGTEqual	= 0x000fffff,
		EConcat		= 0x00100000 | XBSQL::VDate | XBSQL::VText   | XBSQL::VMemo,
		EFNMin		= 0x00110000 | XBSQL::VNum  | XBSQL::VDouble | XBSQL::VText,
		EFNMax		= 0x00120000 | XBSQL::VNum  | XBSQL::VDouble | XBSQL::VText,
		EFNSum		= 0x00130000 | XBSQL::VNum  | XBSQL::VDouble,
		EFNCount	= 0x00140000,
		EFNUpper	= 0x00150000 | XBSQL::VText,
		EFNLower	= 0x00160000 | XBSQL::VText,
		EFNToChar	= 0x0018ffff,
		EFNNullIF	= 0x00190000,
		EAnd		= 0x001affff,
		EOr		= 0x001bffff
	}	;

	enum	Index
	{
		IndexNone	= 0,
		IndexNotUnique	= 1,
		IndexUnique	= 2
	}	;
}	;


/*  XBSQLValue								*/
/*  ----------								*/
/*  This class is used to pass values about, typically when evaluating	*/
/*  expressions in a select query.					*/

class	XBSQL_API 	XBSQLValue
{
public	:

	XBSQL::VType	tag	;
	int		len	;

	union
	{	char	*text	;
		int	num	;
		double	dbl	;
	}	;

	XBSQLValue () ;
	XBSQLValue (const XBSQLValue &) ;
	XBSQLValue (const char       *) ;
	XBSQLValue (char	     *) ;
	XBSQLValue (int   ) ;
	XBSQLValue (double) ;

       ~XBSQLValue () ;

	const XBSQLValue &operator = (const XBSQLValue &) ;
	const XBSQLValue &operator = (const char       *) ;
	const XBSQLValue &operator = (int   ) ;
	const XBSQLValue &operator = (double) ;
	const XBSQLValue &operator = (void *) ;

	void		promote		(XBSQL::VType) ;
	void		demote		(XBSQL::VType) ;
	void		setBinary	(int, const char *) ;
	bool		setFromTable	(XBSQLTable *, int, XBSQL::VType, int) ;
	void		clear	 	() ;
	bool		isTRUE		() const ;
	const char	*getText	() const ;
	int		order		(const XBSQLValue &) const ;

	inline int	getLength	() const { return len ; } ;
}	;

/*  XBSQLValueList							*/
/*  --------------							*/
/*  Class used to implement a dynamically expandable list of values	*/

class	XBSQL_API 	XBSQLValueList
{
	XBSQLValue	*values	;
	int		nvals	;
	int		cnt	;

public	:

	XBSQLValueList	() ;
       ~XBSQLValueList	() ;

	XBSQLValue	&at	(int) 	;
	int		count	() 	;
	int		find	(XBSQLValue &) ;
}	;

/*  XBSQLQuerySet							*/
/*  -------------							*/
/*  This class is used to collect values retrieved from the database,	*/
/*  typically as the result of a select. Essentially, is is initialised	*/
/*  with the number of fields, after which new rows can be added and	*/
/*  values stored.							*/

class	XBSQL_API 	XBSQLQuerySet
{
	int		nQryFields	;
	int		nGetFields	;
	int		nAllFields	;
	int		nTables		;
	bool		goSlow		;

	int		nRows		;
	int		nAlloc		;

	XBSQLValue	**values	;
	RecordInfo	**info		;

	XBSQL::VType	*types		;
	const char	**names		;
	bool		*order		;
	int		*lengths	;
	void		cleanUp		() ;

public	:
	
	XBSQLQuerySet	 (bool = false) ;
       ~XBSQLQuerySet	 () ;

	void		setNumFields	(int, int, int, int = 0) ;
	void		setSortOrder	(int, bool)	;
	void		setFieldInfo	(int, XBSQL::VType, int, const char *) ;
	void		addNewRow	(XBSQLTableList  *) ;
	void		setValue	(XBSQLValue &, int) ;
	XBSQLValue	&getValue	(int)	;
	XBSQLValue	&getValue	(int, int) ;
	const char	*getFieldName	(int)	;
	XBSQL::VType	getFieldType	(int)	;
	int		getFieldLength	(int)	;
	int		getFieldWidth	(int)	;
	xbLong		getRecordNo	(int, int) ;
	void		clear		()	;	
	void		sort		()	;
	void		killrow		(int)	;
	void		dumprow		(int)	;

	inline	int	getNumRows	()
	{
		return	nRows 		;
	}
	inline	int	getNumFields	()
	{
		return	nGetFields	;
	}
}	;


/*  XBSQLField								*/
/*  ----------								*/
/*  The XBSQLField class is used to contain information about a		*/
/*  specific field (identifier by name) in specific table (identified	*/
/*  by an XBSQLTable pointer). It contains the field number, type and	*/
/*  length, and possibly a buffer into which field data is read.	*/

class	XBSQL_API 	XBSQLField
{
	XBSQLTable	*table	;
	int		fldnum	;
	XBSQL::VType	fldtype	;
	int		fldlen	;

public	:

	inline	void	setup
		(	XBSQLTable	*_table,
			int		_fldnum,
			XBSQL::VType	_fldtype,
			int		_fldlen
		)
	{
		table	= _table 	;
		fldnum	= _fldnum	;
		fldtype	= _fldtype	;
		fldlen	= _fldlen	;
	}
	bool		getField	(XBSQLValue &)  ;
	bool		setField	(XBSQLValue &)  ;
	XBSQL::VType	getFieldType	() ;
	int		getFieldLength	() ;

	inline	XBSQLTable *getTable	() { return table ; }

	enum
	{	FieldRowID	= -1
	}	;
}	;


/*  XBSQLExprList								*/
/*  ----------								*/
/*  This class is used for each item in a list of expressions, such as	*/
/*  the list of values to be retrieved by a select query.		*/

class	XBSQL_API 	XBSQLExprList
{
	XBaseSQL	*xbase	;
	int		index	;
	XBSQLExprNode	*expr	;
	const char	*asname	;
	bool		ascend	;
	XBSQLExprList	*next	;
	int		maxtab	;

public	:
	XBSQLExprList	(XBSQLExprNode *, const char *, XBSQLExprList *) ;
	XBSQLExprList	(XBSQLExprNode *, bool,		XBSQLExprList *) ;
       ~XBSQLExprList	() ;

	bool		linkDatabase (XBSQLQuery     *, bool &) ;
	bool		linkDatabase (XBSQLQuery     *, bool &, int  &) ;
	bool		moveToTables (XBSQLTableList *) ;
	bool		setTypeNames (XBSQLQuerySet  &) ;
	bool		fetchValues  (XBSQLQuerySet  &, int) ;
	bool		fetchValues  (XBSQLValue     *, int) ;
	bool		acceptable   (bool 	     &) ;
	bool		evaluate     (XBSQLValue     &, int) ;
	bool		getExprType  (XBSQL::VType   &) ;
	const char	*getFieldName(xbString &) ;
	const char	*getExprName (xbString &) ;
	void		getFuncName  (xbString &, char *) ;
	bool		concatValues (xbString &, int   ) ;

	void		print	     (FILE *, int) ;

	friend	class	XBSQLSelect	;
	friend	class	XBSQLInsert	;
	friend	class	XBSQLTableList	;
	friend	class	XBSQLExprNode	;
}	;


/*  XBSQLFieldList							*/
/*  --------------							*/
/*  Class for list of fields used in an insert statement. Will be built	*/
/*  by hand (rather than parsed) for "insert into t values (v1, ...)"	*/
/*  rather than "insert int t (f1, ...) values (v1, ...)"		*/

class	XBSQL_API 	XBSQLFieldList
{
	const char	*fldname	;
	XBSQLFieldList	*next		;
	XBSQLField	field		;

public	:

	XBSQLFieldList	(const char *, XBSQLFieldList *) ;
       ~XBSQLFieldList	() ;

	bool		linkDatabase	(XBSQLQuery   *) ;
	bool		saveValue	(XBSQLValue   &) ;
	friend	class	XBSQLInsert	;
}	;


/*  XBSQLAssignList							*/
/*  ---------------							*/
/*  Class used for the asignment expressions in an update query, ie.,	*/
/*  for v1, ... in "update t set v1 = e1, ..."				*/

class	XBSQL_API 	XBSQLAssignList
{
	const char	*fldname	;
	XBSQLExprNode	*expr		;
	XBSQLAssignList	*next		;
	XBSQLField	field		;

public	:

	XBSQLAssignList (const char *, XBSQLExprNode *, XBSQLAssignList *) ;
       ~XBSQLAssignList () ;

	bool	linkDatabase (XBSQLQuery   *) ;
	bool	assignValues () ;
}	;

/*  XBSQLExprNode							*/
/*  ----------								*/
/*  This class is used for nodes in expression trees, and also to	*/
/*  provide lists in, for instance, "select a, b+c, d from t".		*/

class	XBSQL_API 	XBSQLExprNode
{
	XBSQL::EType	oper	;	/* Node operator		*/
	const char	*text	;	/* Field name, string, etc.	*/
	const char	*tabname;	/* Table for tab.field		*/
	XBSQLField	field	;	/* Field for field leaf nodes	*/
	XBSQLExprNode	*left	;	/* Sub expressions ...		*/
	XBSQLExprNode	*right	;
	XBSQLExprList	*alist	;	/* Argument list		*/
	int		num	;	/* Number constant/place holder	*/
	double		dbl	;	/* Double value			*/
	XBSQLQuery	*query	;	/* Parent query			*/
	int		maxtab	;	/* Maximum table index		*/
	XBSQLValueList	agvals	;	/* Aggregate values		*/

public	:

	XBSQLExprNode	(const char *, bool) ;
	XBSQLExprNode	(const char *, const char *,  bool) ;
	XBSQLExprNode	(XBSQLExprNode *, XBSQLExprNode *, XBSQL::EType) ;
	XBSQLExprNode	(XBSQLExprList *,	XBSQL::EType) ;
	XBSQLExprNode	(int) ;
	XBSQLExprNode	(int,    XBSQL::EType) ;
	XBSQLExprNode	(double, XBSQL::EType) ;
       ~XBSQLExprNode	() ;

	bool		linkDatabase	(XBSQLQuery   *, bool &, int &) ;
	bool		evaluate	(XBSQLValue   &, int) ;
	bool		getExprType	(XBSQL::VType &) ;
	bool		getExprLength	(int	      &) ;
	const char	*getExprName	(xbString     &) ;

	bool		functionSum	(XBSQLValue   &, XBSQLValue   &, XBSQLValue &) ;
	bool		functionMM	(XBSQLValue   &, XBSQLValue   &, XBSQLValue &, XBSQL::EType) ;
	bool		functionArg1	(XBSQLValue   &, XBSQLValue   &, XBSQL::EType) ;
	XBSQLIndex	*indexable	(XBSQLTable   *, int, XBSQLExprNode *&, int &) ;

	void		print		(FILE *, int) ;
}	;

/*  XBSQLTableList							*/
/*  --------------							*/
/*  This class is used for each table in the list of tables in a query,	*/
/*  for instance "select .... from t1, t2, ...".			*/

class	XBSQL_API 	XBSQLTableList
{
	const char	*name		;
	const char	*alias		;
	XBSQLTableList	*next		;
	XBSQLTable	*table		;
	int		tabidx		;
	XBSQLExprList	*where		;
	XBaseSQL	*xbase		;

	XBSQLIndex	*useindex	;
	XBSQLExprList	*idxexprlist	;
	XBSQLExprNode	*idxexprnode	;
	int		ftype		;

	bool		scanRowsSimple	(XBSQLMulti *)	;
	bool		scanRowsIndexed	(XBSQLMulti *)	;

public	:

	XBSQLTableList	(const char *, const char *, XBSQLTableList *) ;
       ~XBSQLTableList	() ;

	bool		findField	(const char *, const char *, XBSQLField &, int &) ;
	bool		linkDatabase	(XBaseSQL   *,  int &) ;
	bool		attachExpr	(XBSQLExprList *, int) ;
	bool		scanRows	(XBSQLMulti *)	;
	void		setRecordNos	(xbLong     *)  ;
	bool		loadRecords	(XBSQLQuerySet &, int) ;
	XBSQLTable	*getTable	()		;

	XBSQLExprList	*getAllColumns	(XBaseSQL   *, XBSQLExprList *) ;

	friend	class	XBSQLDelete	;
}	;


/*  XBSQLTable								*/
/*  ----------								*/
/*  This class provides basic access to a table in terms of the fields	*/
/*  that are needed.							*/

class	XBSQL_API 	XBSQLTable 
{
	xbDbf		*dbf	;
	XBaseSQL	*xBase	;
	const char	*tabname;
	XBSQLIndex	*idxlist;
	xbLong		curRecNo;

public	:

	XBSQLTable (XBaseSQL *, const char *, xbDbf *) ;
       ~XBSQLTable () ;

	inline	const char *getTabName ()
	{
		return	tabname	;
	}
	inline	XBaseSQL   *getXBase   ()
	{
		return	xBase	;
	}

	inline	void	setIndexes
		(	XBSQLIndex *_idxlist
		)
	{
		idxlist	= _idxlist ;
	}

	bool		findField 	(const char *, XBSQLField &) ;
	void		rewind		() ;
	bool		nextRecord	(bool &) ;

	xbLong		FieldCount	()		;
	void		NoteCurRecNo	()		;
	xbLong		GetCurRecNo	() 		;
	xbShort		GetRecord	(xbLong )	;
	xbShort		AppendRecord	()		;
	void		BlankRecord	()		;

	xbLong		GetLongField	(xbShort)	;
	xbDouble	GetDoubleField	(xbShort)	;
	xbShort		GetLogicalField	(xbShort)	;
	xbLong		GetMemoFieldLen	(xbShort)	;
	xbShort		GetMemoField	(xbShort, xbLong, char *, xbShort) ;
	xbShort		GetField	(xbShort, 	  char *) ;

	xbShort		GetFieldType	(xbShort)	;
	char		*GetFieldName	(xbShort)	;
	xbShort		GetFieldLen	(xbShort)	;
	xbShort		GetFieldDecimal	(xbShort)	;
	xbShort		GetFieldNo	(const char *)	;
	xbShort		PutField	(xbShort, const char *	) ;
	xbShort		PutFloatField	(xbShort, xbFloat	) ;
	xbShort		PutLongField	(xbShort, xbLong	) ;
	xbShort		UpdateMemoData	(xbShort, xbLong, const char *, xbShort) ;
	xbShort		DeleteRecord	()		;
	xbShort		PutRecord	()		;
	xbShort		DeleteAllRecords()		;
	xbLong		NoOfRecords	()		;

	XBSQLIndex *indexForField (const char *) ;
}	;


/*  XBSQLIndex								*/
/*  ----------								*/
/*  Wrapper class for simple one-column indexes				*/

class	XBSQL_API 	XBSQLIndex : public xbNdx
{
	xbDbf		*dbf	;
	const char	*field	;
	XBSQLIndex 	*next	;

public	:

	XBSQLIndex (xbDbf *, const char *, const char *, XBSQLIndex *) ;
       ~XBSQLIndex ()	 ;

	XBSQLIndex	*indexForField (const char *) ;
}	;

/*  XBSQLCommand							*/
/*  ------------							*/
/*  This is a base class for all database commands, ie., queries	*/
/*  (select, insert, ....) and control functions (create, drop, ...)	*/

class	XBSQL_API 	XBSQLCommand
{
public	:

	virtual	XBSQLQuery	*isQuery  () { return 0 ; }
	virtual	XBSQLSelect	*isSelect () { return 0 ; }
	virtual	XBSQLInsert	*isInsert () { return 0 ; }
	virtual	XBSQLUpdate	*isUpdate () { return 0 ; }
	virtual	XBSQLDelete	*isDelete () { return 0 ; }
	virtual	XBSQLCreate	*isCreate () { return 0 ; }
	virtual	XBSQLDrop	*isDrop   () { return 0 ; }
}	;

/*  XBSQLQuery								*/
/*  ----------								*/
/*  Base class for all queries ...					*/

class	XBSQL_API 	XBSQLQuery : public XBSQLCommand
{
	char		*textb	;
	int		nvals	;
	XBSQLValue	*values	;

protected :

	XBaseSQL	*xbase	;
	XBSQLTableList	*tables	;
	int		nTables	;

public	:

	XBSQLQuery (XBaseSQL *, XBSQLTableList *) ;
virtual~XBSQLQuery ()		;


	bool		findField	(const char *, const char *, XBSQLField &, int &) ;

virtual	bool		runQuery	() = 0   ;
	bool		execute		(int, XBSQLValue *) ;
virtual	bool		linkDatabase	() ;
virtual	int		getNumRows	() ;
virtual	int		getNumFields	() ;

virtual	XBSQLQuery	*isQuery 	() { return this ; }

	XBSQLExprList	*getAllColumns	(XBSQLExprList   *) ;

	bool		getPlaceValue	(int, XBSQLValue &) ;
	XBSQL::VType	getPlaceType	(int) ;

	inline	XBaseSQL   *getXBase   ()
	{
		return	xbase	;
	}
}	;

/*  XBSQLMulti								*/
/*  ----------								*/
/*  Base class for queries which affect multiple rows; select, update	*/
/*  and delete.								*/

class	XBSQL_API 	XBSQLMulti : public XBSQLQuery
{
protected :

	XBSQLExprList	*where	;

public	:

	XBSQLMulti (XBaseSQL *, XBSQLTableList *, XBSQLExprList *) ;
virtual~XBSQLMulti ()		;

virtual	bool		linkDatabase () ;

virtual	bool		processRow   (xbLong) = 0 	;
}	;

/*  XBSQLSelect								*/
/*  -----------								*/
/*  This class represents a parsed SQL "select" query, and is also	*/
/*  used to fetch values from the database.				*/

class	XBSQL_API 	XBSQLSelect : public XBSQLMulti
{
	XBSQLExprList	*exprs		;
	XBSQLExprList	*group		;
	XBSQLExprList	*having		;
	XBSQLExprList	*order		;
	XBSQLQuerySet	qrySet		;

	XBSQLValueList	grvals		;

	int		nGetExprs	;
	int		nQryExprs	;
	int		nAllExprs	;
	bool		hasAggr		;

	bool		goSlow		;
	int		goRow		;
	XBSQLValue	*goValues	;

public	:

	XBSQLSelect	(XBaseSQL *, XBSQLExprList *, XBSQLTableList *,
				     XBSQLExprList *, XBSQLExprList  *,
				     XBSQLExprList *, XBSQLExprList  *) ;
virtual~XBSQLSelect	()	;

virtual	bool		 linkDatabase	() ;

virtual	int		 getNumRows	() ;
virtual	int		 getNumFields	() ;

	const XBSQLValue &getField	(int, int) ;
	xbShort		 getFieldType	(int) ;
	int		 getFieldLength	(int) ;
	int		 getFieldWidth	(int) ;
	const char	 *getFieldName	(int) ;
	void		 dumprow	(int) ;

virtual	bool		 runQuery	() ;
virtual	bool		 processRow	(xbLong)   ;

virtual	XBSQLSelect	 *isSelect 	() { return this ; }

	int		 getNumExprs	() { return nGetExprs ; }
}	;


/*  XBSQLInsert								*/
/*  -----------								*/
/*  Class used to handle SQL insert statements.				*/

class	XBSQL_API 	XBSQLInsert : public XBSQLQuery
{
	XBSQLFieldList	*fields	 ;
	XBSQLExprList	*exprs	 ;
	XBSQLSelect	*select	 ;
	int		numRows	 ;

	bool		copySelect () ;

public	:

	XBSQLInsert (XBaseSQL *, const char *, XBSQLFieldList *, XBSQLExprList *) ;
	XBSQLInsert (XBaseSQL *, const char *, XBSQLFieldList *, XBSQLSelect   *) ;
virtual~XBSQLInsert ()		 ;

virtual	bool		linkDatabase	() ;
virtual	int		getNumRows	() ;
virtual	bool		runQuery	() ;

virtual	XBSQLInsert	*isInsert () { return this ; }
}	;


/*  XBSQLDelete								*/
/*  -----------								*/
/*  Class used for delete queries.					*/

class	XBSQL_API 	XBSQLDelete : public XBSQLMulti
{
	int	numRows		;
	bool	delAll		;
	bool	realDelete	;

public	:

	XBSQLDelete (XBaseSQL *, const char *, XBSQLExprList *) ;
virtual~XBSQLDelete () ;

virtual	bool		runQuery	() ;

virtual	bool		linkDatabase	() ;
virtual	int		getNumRows	() ;

virtual	bool		processRow	(xbLong) ;

virtual	XBSQLDelete	*isDelete 	() { return this ; }

	XBSQLTable	*getTable	() ;

	void		setRealDelete	(bool) ;
}	;


/*  XBSQLUpdate								*/
/*  -----------								*/
/*  Class used for update queries.					*/

class	XBSQL_API 	XBSQLUpdate : public XBSQLMulti
{
	XBSQLAssignList	*assigns ;
	int		numRows	 ;

public	:

	XBSQLUpdate (XBaseSQL *, const char *, XBSQLAssignList *, XBSQLExprList *) ;
virtual~XBSQLUpdate () ;

virtual	bool		linkDatabase	() ;
virtual	int		getNumRows	() ;

virtual	bool		runQuery	() ;

virtual	bool		processRow	(xbLong) ;

virtual	XBSQLUpdate	*isUpdate 	() { return this ; }
}	;


/*  XBSQLColumnList							*/
/*  ---------------							*/
/*  Class used for information about a column in, for instance, a	*/
/*  create table command.						*/

class	XBSQL_API 	XBSQLColumnList
{
	XBSQLColumnList	*next	;
	xbSchema	schema	;
	XBSQL::Index	indexed	;

public	:

	XBSQLColumnList	(const char *, int, int, int, XBSQL::Index, XBSQLColumnList *) ;
       ~XBSQLColumnList ()	;

	friend	class	XBSQLCreate	;
}	;

/*  XBSQLCreate								*/
/*  -----------								*/
/*  Class used for create queries.					*/

class	XBSQL_API 	XBSQLCreate : public XBSQLCommand
{
	XBaseSQL	*xbase	 ;
	const char	*tabname ;
	XBSQLColumnList	*columns ;

public	:

	XBSQLCreate (XBaseSQL *, const char *, XBSQLColumnList *) ;
virtual~XBSQLCreate () ;

	bool		execute 	() ;

virtual	XBSQLCreate	*isCreate 	() { return this ; }
}	;


/*  XBSQLDrop								*/
/*  -----------								*/
/*  Class used for drop queries.					*/

class	XBSQL_API 	XBSQLDrop : public XBSQLCommand
{
	XBaseSQL	*xbase	 ;
	const char	*tabname ;

public	:

	XBSQLDrop   (XBaseSQL *, const char *) ;
virtual~XBSQLDrop   () ;

	bool		execute 	() ;

virtual	XBSQLDrop	*isDrop 	() { return this ; }
}	;


/*  XBSQLTableSet							*/
/*  -------------							*/
/*  Class used to return a list of all tables in the database		*/

class	XBSQL_API 	XBSQLTableSet
{
	XBaseSQL	*xbase	 ;
	XBSQLQuerySet	tableSet ;

public	:

	XBSQLTableSet	(XBaseSQL *) ;

	inline	int	getNumTables ()
	{
		return	tableSet.getNumRows () ;
	}
	inline	const char *getTableName (int tabno) 
	{
		return	tableSet.getValue(tabno, 0).getText() ;
	}
}	;


/*  XBSQLFieldSet							*/
/*  -------------							*/
/*  Class used to return a list of all the fields in a particular table	*/
/*  in the database.							*/

class	XBSQL_API 	XBSQLFieldSet
{
	XBaseSQL	*xbase	 ;
	XBSQLQuerySet	fieldSet ;
	const char	*tabname ;

public	:

	XBSQLFieldSet	(XBaseSQL *, XBSQLTable *) ;
       ~XBSQLFieldSet	() ;

	inline	int	getNumFields ()
	{
		return	fieldSet.getNumRows () ;
	}
	inline	const char *getFieldName (int fldno)
	{
		return	fieldSet.getValue(fldno, 0).getText() ;
	}
	inline	int	getFieldType	 (int fldno)
	{
		return	fieldSet.getValue(fldno, 1).num ;
	}
	inline	int	getFieldLength	 (int fldno)
	{
		return	fieldSet.getValue(fldno, 2).num ;
	}
	inline	int	getFieldPrec	 (int fldno)
	{
		return	fieldSet.getValue(fldno, 3).num ;
	}
	XBSQL::Index	fieldIndexed (int) ;
}	;


/*  XBaseSQL								*/
/*  --------								*/
/*  This is the main class for the XBase/SQL interface ....		*/

struct	XBSQLOpenTable
{
	xbDbf		*dbf	;
	const char	*name	;
	XBSQLIndex	*index	;
	int		usecount;
}	;

struct	XBSQLPackTable
{
	const char	*table	;
	XBSQLPackTable	*next	;
}	;

class	XBSQL_API 	XBaseSQL : public xbXBase
{
	enum	QryType
	{
		Select,
		Insert,
		Update,
		Delete
	}	;

	const char	*dbDir		;
	const char	*dbError	;

	bool		realDelete	;
	bool		closePack	;
	bool		caseSensitive	;
	bool		useWildcard	;
	bool		goSlow		;

	XBSQLPackTable	*packList	;

	XBSQLOpenTable	openTables[MAX_OPEN_TABLES]	;

	void		addClosePack	(const char *)	;

public	:


	XBaseSQL (const char *) ;
       ~XBaseSQL ()		;

	void		setError	(xbShort)	    ;
	void		setError	(xbShort, const char *, ...) ;
	void		setError	(const char *, ...) ;
	const char	*getPath	(const char *, const char *) ;

	bool		createTable	(const char *, xbSchema   *, XBSQL::Index *) ;
	XBSQLTable	*openTable	(const char *) ;
	void		closeTable	(xbDbf	    *) ;

	bool		findField	(const char *, const char *, XBSQLField   &) ;

	inline	const char	*lastError ()
	{
		return	dbError	;
	}
	inline	const char	*getDBDir  ()
	{
		return	dbDir	;
	}
	inline	void	setRealDelete
		(	bool	on
		)
	{
		// Disabled - does not seem to work correctly
		// realDelete = on ;
	}
	inline	void	setClosePack
		(	bool	on
		)
	{
		closePack = on ;
	}
	inline	void	setCaseSensitive
		(	bool	on
		)
	{
		caseSensitive = on ;
	}
	inline	void	setUseWildcard
		(	bool	on
		)
	{
		useWildcard = on ;
	}
	inline	void	setGoSlow
		(	bool	on
		)
	{
		goSlow	= on ;
	}

	XBSQLSelect	*openSelect	(const char *)	;
	XBSQLInsert	*openInsert	(const char *)	;
	XBSQLUpdate	*openUpdate	(const char *)	;
	XBSQLDelete	*openDelete	(const char *)	;
	XBSQLQuery	*openQuery	(const char *, bool &) ;

	XBSQLTableSet	*getTableSet	()		;
	XBSQLFieldSet	*getFieldSet	(const char *)	;

	bool		execCommand	(const char *)	;

	bool		dropTable	(const char *)	;
	bool		renameTable	(const char *, const char *) ;

	friend	class	XBSQLQuery	;
	friend	class	XBSQLSelect	;
	friend	class	XBSQLDelete	;
}	;

extern	XBSQL::VType	XTypeToVType	(xbShort     )  ;
extern	xbShort		VTypeToXType	(XBSQL::VType)  ;


#endif	// _XBSQL_H
