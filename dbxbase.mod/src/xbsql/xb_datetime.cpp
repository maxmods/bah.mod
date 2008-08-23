/***************************************************************************
    file	         : xb_datetime.cpp
    copyright            : (C) 2002 by Mike Richardson
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
#include	<ctype.h>
#include	<string.h>

#include	"xbsql.h"

/*  The following table lists the weekdays, the months, plus some other	*/
/*  things than can appear in dates. Note that we should do something	*/
/*  about making this language dependant.				*/

static	const char	*names[] =
{	"sunday",
	"monday",
	"tuesday",
	"wednesday",
	"thursday",
	"friday",
	"saturday",
	"january",
	"february",
	"march",
	"april",
	"may",
	"june",
	"july",
	"august",
	"september",
	"october",
	"november",
	"december",
	"am",
	"pm",
	"gmt",
	"pst",
	"bst",
	"edt",
	"st",
	"nd",
	"rd",
	"th",
	0
}	;
#define	NUMDAYS	7
#define	NUMMONS	12
#define	NUMISAM	(NUMDAYS+NUMMONS+0)
#define	NUMISPM	(NUMDAYS+NUMMONS+1)

static	int	mDays [] =
{	31,    28,    31,    30,    31,    30,
	31,    31,    30,    31,    30,    31
}	;


class	DTToken
{
public	:
	int		sep	;
	const char	*token	;
	bool		digit	;
	int		nidx	;
}	;

static	const char	Invalid[] = "Invalid date"		;
static	const char	HasTime[] = "Date has time component"	;
static	const char	NoDate [] = "No date component"		;


/*  getName	: Locate name entry for specified string		*/
/*  str		: const char *	: String				*/
/*  (returns)	: int		: Index in name list or negative	*/

static	int	getName
	(	const char	*str
	)
{
	const char	**nptr	;
	int		slen	= strlen (str) ;

	for (nptr = &names[0] ; *nptr != 0 ; nptr += 1)
	{
		const char	*nam	= *nptr	      ;
		int		nlen	= strlen(nam) ;

		/* If the name is less than three charactes then do a	*/
		/* length-dependant match. This distinguishes between,	*/
		/* say, "thursday" and "th".				*/
		if (nlen < 3)
		{	if (strcasecmp (str, nam) == 0) break ;
			continue ;
		}

		if ((slen >= 3) && (strncasecmp (str, nam, slen) == 0))
			break	;
	}

	return	(*nptr == 0) ? -1 : nptr - &names[0] ;
}

/*  fixYear	: Decode and fix year string				*/
/*  tokp	: DTToken	: Token					*/
/*  (returns)	: int		: Year as a number			*/

static	int	fixYear
	(	DTToken	*tokp
	)
{
	int	yno	= atoi(tokp->token)	;

	if (strlen(tokp->token) == 2)
		if	(yno >= 70) yno += 1900 ;
		else if (yno <  70) yno += 2000 ;

	return	yno	;
}


/*  XBSQLConvDate: Convert general date/time to date			*/
/*  srce	 : const char * : Date/Time string			*/
/*  dest	 : char *	: Result buffer				*/
/*  (returns)	 : const char *	: Error or NULL if OK			*/

const char
	*XBSQLConvDate
	(	const char	*srce,
		char		*dest
	)
{
#define	SF(f,v)	\
	{	if (f != -1) { free (copy) ; return Invalid ; }	\
		f = v ;						\
	}

	int		year	= -1	;
	int		mon	= -1	;
	int		day	= -1	;
	int		hour	= -1	;
	int		min	= -1	;
	int		sec	= -1	;
	int		ampm	= -1	;
	int		lch	= ' '	;
	int		ntoks	= 0	;
	int		scan	= 0	;
	DTToken		toks	[16]	;
	DTToken		*tokp	;
	int		left	;

	/* Duplicate the date/time string so that we can chop it up in	*/
	/* place.							*/
	/* unchanged.							*/
	char 		*copy  = strdup (srce) ;
	char		*dtstr = copy   ;

	memset	(toks, 0, sizeof(toks)) ;

	while (*dtstr)
	{
		if (!isalnum(*dtstr))
		{	lch	= *dtstr;
			dtstr    += 1	;
			continue	;
		}

		tokp		= &toks[ntoks] ;
		ntoks	       += 1	;
		
		tokp->token	= dtstr	;
		tokp->sep	= lch	;
		tokp->nidx	= 0	;
		tokp->digit	= isdigit(*dtstr)  ;

		while (tokp->digit ? isdigit(*dtstr) : isalpha(*dtstr))
			dtstr += 1 ;

		lch		= *dtstr;
		if (lch) *dtstr++ = 0	;

		if (!tokp->digit)
		{
			if ((tokp->nidx = getName(tokp->token)) < 0)
			{	free	(copy)	;
				return	Invalid	;
			}
		}
		else	tokp->nidx = atol (tokp->token) ;
	}

//	for (unsigned int idx = 0 ; idx < ntoks ; idx += 1)
//	{	DTToken *tokp = &toks[idx] ;
//		fprintf (stderr, "%c %s %d\n", tokp->sep, tokp->token, tokp->nidx) ;
//	}

	/* Special case: If there is a single token then see if it is	*/
	/* in the format YYYYMMDD. We do a partial check; the XBase	*/
	/* code can check properly.					*/
	if (ntoks == 1)
	{
		const char *dstr = toks[0].token ;

		if (strlen(dstr) != 8)
		{
			free	(copy)	;
			return	Invalid	;
		}

		for (unsigned int idx = 0 ; idx < 8 ; idx += 1)
			if (!isdigit(dstr[idx]))
			{
				free	(copy)	;
				return	Invalid	;
			}

		strcpy	(dest, dstr)	;
		free	(copy)		;
		return	0		;	
	}

	while ((left = ntoks - scan) > 0)
	{
		DTToken	*tok0	= &toks[scan] ;
		DTToken	*tok1	= left > 1 ? &toks[scan + 1] : 0 ;
		DTToken	*tok2	= left > 2 ? &toks[scan + 2] : 0 ;
		uint	drop	= 0 ;

		// fprintf (stderr, "dt:%s with %d\n", (cchar *)tok0->token, left) ;

		if (tok2 != 0)
		{
			if	(tok0->digit &&  tok1->digit && tok2->digit
					&& (tok1->sep == ':') && (tok2->sep == ':'))
			{
				SF(hour, tok0->nidx) ;
				SF(min,  tok1->nidx) ;
				SF(sec,  tok2->nidx) ;
				drop	= 3	;
				goto	advance	;
			}
			else if (tok0->digit && !tok1->digit && tok2->digit
					&& (tok1->sep == '-') && (tok2->sep == '-'))
			{
				SF(day,  tok0->nidx       ) ;
				SF(mon,  tok1->nidx - NUMDAYS + 1) ;
				SF(year, fixYear(tok2)	  ) ;
				drop	= 3	;
				goto	advance	;
			}
			else if (tok0->digit &&  tok1->digit && tok2->digit
					&& (tok1->sep == '-') && (tok2->sep == '-'))
			{
				SF(year, fixYear(tok0)    ) ;
				SF(mon,  tok1->nidx	  ) ;
				SF(day,  tok2->nidx       ) ;
				drop	= 3	;
				goto	advance	;
			}
			else if (tok0->digit &&  tok1->digit && tok2->digit
					&& (tok1->sep == '/') && (tok2->sep == '/'))
			{
				SF(day,  tok0->nidx       ) ;
				SF(mon,  tok1->nidx	  ) ;
				SF(year, fixYear(tok2)	  ) ;
				drop	= 3	;
				goto	advance	;
			}
		}

		if (tok1 != 0)
			if	(tok0->digit && tok1->digit && (tok1->sep == ':'))
			{
				SF(hour, tok0->nidx) ;
				SF(min,  tok1->nidx) ;
				SF(sec,  0) ;
				drop	= 2	;
				goto	advance ;
			}

		if (!tok0->digit)
		{
			if	(tok0->nidx <  NUMDAYS)
				;
			else if	(tok0->nidx <  NUMDAYS+NUMMONS)
				SF(mon,  tok0->nidx - NUMDAYS + 1)
			else if (tok0->nidx == NUMISAM)
				SF(ampm, NUMISAM)
			else if (tok0->nidx == NUMISPM)
				SF(ampm, NUMISPM)

			drop	= 1	;
			goto	advance	;
		}

		if (strlen(tok0->token) <= 2)
		{
			if ((tok0->sep != '+') && (tok0->sep != '-'))
				SF(day,  tok0->nidx) ;
			drop	= 1	;
			goto	advance	;
		}

		if (strlen(tok0->token) <= 4)
		{
			SF(year, tok0->nidx) ;
			drop	= 1	;
			goto	advance	;
		}

		return	Invalid	;

		advance	:
			scan	+= drop	;
	}

	if (hour != -1)
	{	if (ampm != 	  -1) if (hour >= 12) return Invalid ;
		if (ampm ==  NUMISPM) hour += 12 ;
	}

	free	(copy)	;

	// fprintf (stderr, "decoded: %d-%d-%d %d:%d:%d\n", year, mon, day, hour, min, sec) ;
	if ((hour != -1) || (min != -1) || (sec != -1))
	{
		return	HasTime	;
	}

	if ((year == -1) || (mon == -1) || (day == -1))
	{
		return	NoDate	;
	}

	sprintf	(dest, "%04d%02d%02d", year, mon, day) ;
	fprintf	(stderr, "Convert date [%s]->[%s]\n", srce, dest) ;
	return	0 ;
}

