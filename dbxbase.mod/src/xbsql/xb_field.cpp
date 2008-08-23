/***************************************************************************
    file                 : xb_field.cpp
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


/*  XBSQLField								*/
/*  getField	: Get field value from current record			*/
/*  value	: XBSQLValue &	: Resulting value			*/
/*  (returns)	: bool		: Success				*/

bool	XBSQLField::getField
	(	XBSQLValue	&value
	)
{
	return	value.setFromTable (table, fldnum, fldtype, fldlen) ;
}

/*  XBSQLField								*/
/*  setField	: Set field in current record				*/
/*  value	: XBSQLValue &	: Value					*/
/*  (returns)	: bool		: Sucess				*/

bool	XBSQLField::setField
	(	XBSQLValue	&value
	)
{
	xbShort		rc	= XB_NO_ERROR ;
	const char	*errm	= 0 ;

	if (fldnum == FieldRowID) return true ;

	if (fldtype > value.tag) value.promote (fldtype) ;
	if (fldtype < value.tag) value.demote  (fldtype) ;

	// fprintf (stderr, "%s/%d <- ", table->getTabName()) ;

	switch (fldtype)
	{
		case XBSQL::VBool   :
		//	fprintf	(stderr, "'%s'", value.isTRUE() ? "Y" : "N") ;
			rc	= table->PutField (fldnum, value.isTRUE() ? "Y" : "N") ;
			break	;

		case XBSQL::VNum    :
			if (value.tag != XBSQL::VNum)
			{	errm	= "Expected number" ;
				break	;
			}
		//	fprintf	(stderr, "%d", value.num) ;
			rc	= table->PutLongField (fldnum, value.num) ;
			break	;

		case XBSQL::VDouble :
			if (value.tag != XBSQL::VDouble)
			{	errm	= "Expected float" ;
				break	;
			}
		//	fprintf	(stderr, "%f", value.dbl) ;
			rc	= table->PutFloatField (fldnum, value.dbl) ;
			break	;

		case XBSQL::VText   :
			if (value.tag != XBSQL::VText)
			{	errm	= "Expected text" ;
				break	;
			}
		//	fprintf	(stderr, "'%s'", value.text) ;
			rc	= table->PutField (fldnum, value.text) ;
			break	;

		case XBSQL::VDate   :
			if (value.tag != XBSQL::VDate)
			{	errm	= "Expected date" ;
				break	;
			}
		//	fprintf	(stderr, "'%s'", value.text) ;
			rc	= table->PutField (fldnum, value.text) ;
			break	;

		case XBSQL::VMemo   :
			if (value.tag != XBSQL::VMemo)
			{	errm	= "Expected memo" ;
				break	;
			}
		//	fprintf	(stderr, "-> MEMO(%d): %.16s...", value.len, value.text) ;
			rc	= table->UpdateMemoData
				  (	fldnum,
					value.len,
					value.text,
					F_SETLK
				  )	; 
			break	;

		default	:
			errm	= "Type not handled" ;
			break	;
	}

	// fprintf (stderr, " -> %d\n", rc) ;

	if (errm != 0)
	{
		table->getXBase()->setError ("XBSQL field [%d][%d] update error: %s",
						fldtype,
						value.tag,
						errm
					    )	;
		return	false	;
	}

	if (rc != XB_NO_ERROR)
	{
		extern	xbShort	VTypeToXType (XBSQL::VType) ;

		table->getXBase()->setError
		(	rc,
			"Field type %c, data \"%.32s ...\"",
			VTypeToXType (fldtype),
			value.getText()
		)	;
		return	false	;
	}

	return	true	;
}

/*  XBSQLField								*/
/*  getFieldType: Get the field type					*/
/*  (returns)	: XBSQL::VType	: Field type				*/

XBSQL::VType XBSQLField::getFieldType ()
{
	return	fldtype	;
}

/*  XBSQLField								*/
/*  getFieldLength: Get the field length				*/
/*  (returns)	  : int		: Field length				*/

int	XBSQLField::getFieldLength ()
{
	return	fldlen	;
}

