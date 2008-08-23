/***************************************************************************
    file                 : xb_value.cpp
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

XBSQL::VType	XTypeToVType
	(	xbShort	xType
	)
{
	switch (xType)
	{
		case 'L' : return XBSQL::VBool	;
		case 'N' : return XBSQL::VNum	;
		case 'F' : return XBSQL::VDouble;
		case 'D' : return XBSQL::VDate	;
		case 'C' : return XBSQL::VText	;
		case 'M' : return XBSQL::VMemo	;
		default	 : break  ;
	}

	return	XBSQL::VNull	;
}

xbShort	VTypeToXType
	(	XBSQL::VType	vType
	)
{
	switch (vType)
	{
		case XBSQL::VBool   : return 'L' ;
		case XBSQL::VNum    : return 'N' ;
		case XBSQL::VDouble : return 'F' ;
		case XBSQL::VDate   : return 'D' ;
		case XBSQL::VText   : return 'C' ;
		case XBSQL::VMemo   : return 'M' ;
		default	 	    : break  ;
	}

	return	'X' ;
}

/*  XBSQLValue								*/
/*  XBSQLValue	: Default constructor for value class			*/
/*  (returns)	: void		:					*/

XBSQLValue::XBSQLValue ()
{
	tag	= XBSQL::VNull	;
	num	= 0		;
	len	= 0		;
}

/*  XBSQLValue								*/
/*  ~XBSQLValue	: Destructor for value class				*/
/*  (returns)	:		:					*/

XBSQLValue::~XBSQLValue ()
{
	clear	() ;
}

/*  XBSQLValue								*/
/*  clear	: Clear current value					*/
/*  (returns)	: void		:					*/

void	XBSQLValue::clear ()
{
	if ((tag == XBSQL::VText) || (tag == XBSQL::VDate) || (tag == XBSQL::VMemo))
		free	((void *)text) ;
}

/*  XBSQLValue								*/
/*  XBSQLValue	: Constructor for value					*/
/*  v		: const XBSQLValue & : Extant value			*/
/*  (returns)	: XBSQLValue	     :					*/

XBSQLValue::XBSQLValue
	(	const XBSQLValue &v
	)
{
	tag	= XBSQL::VNull	;
	*this	= v		;
}

/*  XBSQLValue								*/
/*  XBSQLValue	: Constructor for value					*/
/*  v		: const char *	: Extant value				*/
/*  (returns)	: XBSQLValue	:					*/

XBSQLValue::XBSQLValue
	(	const char *v
	)
{
	tag	= XBSQL::VNull	;
	*this	= v		;
}

/*  XBSQLValue								*/
/*  XBSQLValue	: Constructor for value					*/
/*  v		: char *	: Extant value				*/
/*  (returns)	: XBSQLValue	:					*/

XBSQLValue::XBSQLValue
	(	char	*v
	)
{
	tag	= XBSQL::VNull	;
	*this	= v		;
}

/*  XBSQLValue								*/
/*  XBSQLValue	: Constructor for value					*/
/*  v		: int		: Extant value				*/
/*  (returns)	: XBSQLValue	:					*/

XBSQLValue::XBSQLValue
	(	int	v
	)
{
	tag	= XBSQL::VNull	;
	*this	= v		;
}

/*  XBSQLValue								*/
/*  XBSQLValue	: Constructor for value					*/
/*  v		: double	: Extant value				*/
/*  (returns)	: XBSQLValue	:					*/

XBSQLValue::XBSQLValue
	(	double	v
	)
{
	tag	= XBSQL::VNull	;
	*this	= v		;
}

/*  XBSQLValue								*/
/*  operator =	: Assignment operator					*/
/*  v		: const XBSQLValue & : Value being assigned		*/
/*  (returns)	: XBSQLValue & 	     : Value				*/

const XBSQLValue &XBSQLValue::operator =
	(	 const XBSQLValue &v
	)
{
	clear	() ;

	switch (tag = v.tag)
	{
		case XBSQL::VNull	:
			return	*this	;

		case XBSQL::VNum	:
		case XBSQL::VBool	:
			num	= v.num	;
			return	*this	;

		case XBSQL::VDouble	:
			dbl	= v.dbl ;
			return	*this	;

		case XBSQL::VText	:
		case XBSQL::VDate	:
			/* Text and date are both stored as strings	*/
			/* so just duplicate.				*/
			text	= strdup (v.text) ;
			len	= v.len	;
			return	*this	;

		case XBSQL::VMemo	:
			/* Memo is binary data in a buffer to allocate	*/
			/* and copy.					*/
			len	= v.len		          ;
			text	= (char *)malloc(len + 1) ;
			memcpy	 ((void *)text, (void *)v.text, len + 1) ;
			return	*this	;

		default	:
			break	;
	}

	fprintf	(stderr, "Unknown XBSQLValue type %d\n", tag) ;
	tag	= XBSQL::VNull ;
	return	*this	;
}

/*  XBSQLValue								*/
/*  operator =	: Assignment operator					*/
/*  v		: const char *	: String to be assigned			*/
/*  (returns)	: XBSQLValue &	: Value					*/

const XBSQLValue &XBSQLValue::operator =
	(	 const char	*v
	)
{
	clear	() ;

	if (v == 0) v = ""	;

	text	= strdup (v)	;
	tag	= XBSQL::VText	;
	len	= strlen (v)	;
	return	*this		;
}

/*  XBSQLValue								*/
/*  operator =	: Assignment operator					*/
/*  v		: int		: Number to be assigned			*/
/*  (returns)	: XBSQLValue &	: Value					*/

const XBSQLValue &XBSQLValue::operator =
	(	 int	v
	)
{
	clear	() ;

	num	= v		;
	tag	= XBSQL::VNum	;
	len	= 0		;
	return	*this		;
}

/*  XBSQLValue								*/
/*  operator =	: Assignment operator					*/
/*  v		: double	: Double to be assigned			*/
/*  (returns)	: XBSQLValue &	: Value					*/

const XBSQLValue &XBSQLValue::operator =
	(	 double	v
	)
{
	clear	() ;

	dbl	= v		;
	tag	= XBSQL::VDouble;
	len	= 0		;
	return	*this		;
}

/*  XBSQLValue								*/
/*  operator =	: Assignment operator					*/
/*  v		: void *	: Used to set to null			*/
/*  (returns)	: XBSQLValue &	: Value					*/

const XBSQLValue &XBSQLValue::operator =
	(	 void	*
	)
{
	clear	() ;

	num	= 0		;
	tag	= XBSQL::VNull	;
	len	= 0		;
	return	*this		;
}

/*  XBSQLValue								*/
/*  setBinary	: Set to a binary (memo) value				*/
/*  _len	: int		: Data length				*/
/*  _data	: const char *	: Data 					*/
/*  (returns)	: void		:					*/

void	XBSQLValue::setBinary
	(	int		_len,
		const char	*_data
	)
{
	clear	() ;

	len	= _len				;
	text	= (char *)malloc(_len + 1)	;
	tag	= XBSQL::VMemo			;

	if (_data == 0)
		memset	(text, 0,     len) ;
	else	memcpy	(text, _data, len) ;

	text[len] = 0 ;
}

/*  XBSQLValue								*/
/*  setFromTable: Set value from table entry				*/
/*  table	: XBSQLTable *	: Table					*/
/*  fldnum	: int		: Field number in table			*/
/*  fldtype	: XBSQL::VType	: Type					*/
/*  fldlen	: int		: Field length				*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLValue::setFromTable
	(	XBSQLTable	*table,
		int		fldnum,
		XBSQL::VType	fldtype,
		int		fldlen
	)
{
	if ((tag == XBSQL::VText) || (tag == XBSQL::VDate) || (tag == XBSQL::VMemo))
	{	free	((void *)text) ;
		text	= 0 ;
	}

	if (fldnum == XBSQLField::FieldRowID)
	{
		tag	= XBSQL::VNum		;
		num	= table->GetCurRecNo () ;
		return	true	;
	}
	
	switch (tag = fldtype)
	{
		/* Text and date are actually strings, so just drop	*/
		/* through to the following code will allocate space	*/
		/* and get the field.					*/
		case XBSQL::VText	:
		case XBSQL::VDate	:
			break	;

		/* Numbers (integers), floats (actually doubles) and	*/
		/* logicals (treated as numbers) are easy.		*/
		case XBSQL::VNum	:
			num	= table->GetLongField    (fldnum) ;
			return	true	;

		case XBSQL::VDouble	:
			dbl	= table->GetDoubleField  (fldnum) ;
			return	true	;

		case XBSQL::VBool	:
			tag	= XBSQL::VNum	;
			num	= table->GetLogicalField (fldnum) ;
			return	true	;

		/* A memo is binary data so allocate the required space	*/
		/* and read in the memo data.				*/
		case XBSQL::VMemo	:
			len	= table->GetMemoFieldLen (fldnum) ;
			text	= (char *)malloc(len + 1) ;
			table->GetMemoField (fldnum, len, text, F_SETLK) ;
			text[len] = 0	;
			return	true	;

		default	:
			table->getXBase()->setError
			(	"Unrecognised field type \'%c\' (%d) in table \"%s\"",
				VTypeToXType (fldtype),
				fldtype,
				table->getTabName()
			)	;
			return	false	;
	}

	/* Allocate a text buffer and read the value from the table. We	*/
	/* then truncate all trailing spaces.				*/
	text	= (char *)malloc(fldlen + 1) ;
	table->GetField (fldnum, text)	     ;

	char	*cp = &text[fldlen - 1] ;
	while (cp >= text)
	{	if (*cp != ' ') break	;
		cp -= 1 ;
	}
	cp[1]	= 0  ;
	len	= strlen (text) ;
	return	true ;
}

/*  XBSQLValue								*/
/*  getText	: Get contents as text					*/
/*  (returns)	: const char *	: Text					*/

const char *XBSQLValue::getText () const
{
	/* Numbers are converted to strings in a static buffer, so the	*/
	/* caller must no assume that a value is retained over multiple	*/
	/* calls.							*/
	static	char	tmp[32] ;

	switch (tag)
	{
		case XBSQL::VNull	:
			return	"" ;

		case XBSQL::VNum	:
		case XBSQL::VBool	:
			sprintf	(tmp, "%d", num) ;
			return	tmp	;

		case XBSQL::VDouble	:
			sprintf	(tmp, "%f", dbl) ;
			return	tmp	;

		case XBSQL::VText	:
		case XBSQL::VDate	:
		case XBSQL::VMemo	:
			return	text	;

		default	:
			break	;
	}

	fprintf	(stderr, "Unknown XBSQLValue type %d\n", tag) ;
	return	"" ;
}

/*  XBSQLValue								*/
/*  isTRUE	: See if value is considered to be logically true	*/
/*  (returns)	: bool		: True if so				*/

bool	XBSQLValue::isTRUE () const
{
	switch (tag)
	{
		case XBSQL::VNull	:
			return	false		;

		case XBSQL::VNum	:
		case XBSQL::VBool	:
			return	num != 0	;

		case XBSQL::VDouble	:
			return	num != 0.0	;

		case XBSQL::VText	:
		case XBSQL::VDate	:
			return	text[0] != 0	;

		case XBSQL::VMemo	:
			return	len > 0	;

		default	:
			break	;
	}

	fprintf	(stderr, "Unknown XBSQLValue type %d\n", tag) ;
	return	false	;
}

/*  XBSQLValue								*/
/*  promote	: Promote value to a higher type			*/
/*  type	: XBSQL::VType	: Target type				*/
/*  (returns)	: void		:					*/

void	XBSQLValue::promote
	(	XBSQL::VType	type
	)
{
	/* This method is used during expression evaluation, so that,	*/
	/* for instance, "32 || 'bitsonline'" evaluates to the string	*/
	/* "32bitsonline". Basically, we allow:				*/
	/*	number --> double --> date --> text --> memo		*/
	/* Not all this may be sensible, but is covers most things. If	*/
	/* the value is already of "higher" type than the target then	*/
	/* it remains unchanged.					*/

	char	tmp[40]	;

	/* NOTE: We should never see the target as logical since we	*/
	/*	 convert logicals to numbers when reading from a table.	*/
	switch (type)
	{
		case XBSQL::VMemo	:
		case XBSQL::VDate	:
		case XBSQL::VText	:

			switch (tag)
			{
				case XBSQL::VNum	:
					sprintf	(tmp, "%d", num)	;
					text	= strdup (tmp)		;
					tag	= type			;
					return	;

				case XBSQL::VDouble	:
					sprintf	(tmp, "%f", dbl)	;
					text	= strdup (tmp)		;
					tag	= type			;
					return	;

				case XBSQL::VDate	:
				case XBSQL::VText	:
				case XBSQL::VMemo	:
					tag	= type	;
					return	;

				default	:
					break	;
			}

			break	;

		case XBSQL::VDouble	:

			switch (tag)
			{
				case XBSQL::VNum	:
					dbl	= num	;
					tag	= type	;
					return	;

				default	:
					break	;
			}

			break	;

		case XBSQL::VNum	:
			return	;

		default	:
			break	;

	}

	fprintf	(stderr, "Promote called with %d->%d\n", tag, type) ;
	if (type >= XBSQL::VDate) text	= strdup ("ERROR") ;
	tag	= type	;
}

/*  XBSQLValue								*/
/*  demote	: Demote a value to a lower type if possible		*/
/*  type	: XBSQL::VType	: Target type				*/
/*  (returns)	: void		:					*/

void	XBSQLValue::demote
	(	XBSQL::VType	type
	)
{
	int	tnum	;
	double	tdbl	;
	char	*r	;

	switch (type)
	{
		case XBSQL::VBool	:
			tag	= type		;
			len	= 0		;
			num	= isTRUE() 	;
			return	;

		case XBSQL::VNum	:

			switch (tag)
			{
				case XBSQL::VDouble :
					tag	= type		;
					num	= (int)dbl	;
					return	;

				case XBSQL::VMemo   :
				case XBSQL::VDate   :
				case XBSQL::VText   :
					tnum	= strtol (text, &r, 0) ;
					if ((r != 0) && (*r != 0)) break ;
					tag	= type	;
					num	= tnum	;
					return	;

				default	:
					break	;
			}

			break	;

		case XBSQL::VDouble	:

			switch (tag)
			{
				case XBSQL::VNum	:
					tag	= type	;
					dbl	= num	;
					return	;

				case XBSQL::VMemo   :
				case XBSQL::VDate   :
				case XBSQL::VText   :
					tdbl	= strtod (text, &r) ;
					if ((r != 0) && (*r != 0)) break ;
					tag	= type	;
					dbl	= tdbl	;
					return	;

				default	:
					break	;
			}

			break	;

		case XBSQL::VDate	:
		case XBSQL::VText	:
		case XBSQL::VMemo	:
			tag	= type	;
			break	;

		default	:
			break	;
	}
}

/*  XBSQLValue								*/
/*  order	: Order values for select ordering			*/
/*  other	: const XBSQLValue & : Value to compare against		*/
/*  (returns)	: int		     : Order value for quicksort	*/

int	XBSQLValue::order
	(	const XBSQLValue &other
	)
	const
{
	/* Check for nulls. Following MySQL, null values are considered	*/
	/* to be "before" any other value except another null.		*/
	if (tag       == XBSQL::VNull)
		return	other.tag == XBSQL::VNull ? 0 : -1 ;
	if (other.tag == XBSQL::VNull)
		return	tag 	  == XBSQL::VNull ? 0 : +1 ;

	/* We don't have to bother about type conversion, since we will	*/
	/* only be comparing values within a column ...			*/
	if (tag != other.tag)
	{	fprintf	(stderr, "XBSQLValue::order tags (%d,%d)\n", tag, other.tag) ;
		return	0 ;
	}

	switch (tag)
	{
		case XBSQL::VNum	:
		case XBSQL::VBool	:
//			fprintf	(stderr, "%d:%d->%d\n", num, other.num, num - other.num) ;
			return	num - other.num ;

		case XBSQL::VDouble	:
		{	double	d = dbl - other.dbl ;
			int	r = d == 0 ? 0 : d < 0 ? -1 : +1 ;
//			fprintf	(stderr, "%f:%f->%d\n", dbl, other.dbl, r) ;
			return	r	;
		}

		case XBSQL::VText	:
		case XBSQL::VDate	:
//			fprintf	(stderr, "%s:%s->%d\n", text, other.text, strcmp (text, other.text)) ;
			return	strcmp (text, other.text) ;

		case XBSQL::VMemo	:
			break	;

		default	:
			break	;
	}

	fprintf	(stderr, "XBSQLValue::order unknown tag=%d\n", tag) ;
	return	0 ;
}


/*  ------------------------------------------------------------------  */

/*  XBSQLValueList							*/
/*  XBSQLValueList							*/
/*		: Constructor for list of values object			*/
/*  (returns)	: XBSQLValueList :					*/

XBSQLValueList::XBSQLValueList ()
{
	values	= 0 ;
	nvals	= 0 ;
	cnt	= 0 ;
}

/*  XBSQLValueList							*/
/*  ~XBSQLValueList							*/
/*		: Destructor for list of values object			*/
/*  (returns)	:		:					*/

XBSQLValueList::~XBSQLValueList ()
{
	if (values != 0) delete [] values ;
}

/*  XBSQLValueList							*/
/*  at		: Get element at specified offset			*/
/*  idx		: int		: Required index			*/
/*  (returns)	: XBSQLValue &	: Value					*/

XBSQLValue
	&XBSQLValueList::at
	(	int	idx
	)
{
	/* First case is there are no values in the array, in which	*/
	/* allocate an initial array, with a bit of spare space.	*/
	if	(values == 0)
	{
		values	= new XBSQLValue [idx + 10] ;
		nvals	= idx + 10 ;
	}
	/* Second case is there is an array but it does not extend to	*/
	/* the value required. Extend and copy the existing array.	*/
	else if (idx >= nvals)
	{
		XBSQLValue *nv = new XBSQLValue [idx + 10] ;
		for (uint s = 0 ; s < nvals ; s += 1)
			nv[s] = values[s] ;
		delete	[] values ;
		values	=  nv	  ;
		nvals	= idx + 10;
	}

	/* Keep track of the maximum size that the array reaches, and	*/
	/* return the required element.					*/
	if (idx + 1 > cnt) cnt = idx + 1 ;
	return	values[idx] ;
}

/*  XBSQLValueList							*/
/*  count	: Get number of elements in array			*/
/*  (returns)	: int		: Number of elements			*/

int	XBSQLValueList::count ()
{
	return	cnt	;
}

/*  XBSQLValueList							*/
/*  find	: Find an element by value				*/
/*  value	: XBSQLValue &	: Required value			*/
/*  (returns)	: int		: Index or negtive if not found		*/

int	XBSQLValueList::find
	(	XBSQLValue	&value
	)
{
	for (int idx = 0 ; idx < cnt ; idx += 1)
		if (values[idx].order(value) == 0)
			return	idx	;

	return	-1 ;
}
