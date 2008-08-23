/***************************************************************************
    file                 : xql.cpp
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
#include	<ctype.h>
#include	<string.h>
#include	<unistd.h>

#include	"xbsql.h"

#ifndef NO_READLINE
extern "C"
{
#include	<readline/readline.h>
#include	<readline/history.h>
}
#endif


#define	USAGE	"xql [-pwdsv] [dir]\n"


static	bool	dumprows	= false	;
static	int	verbose		= 0	;


char	*getline
	(	char	*prompt
	)
{
	static	char	line[10240] ;

	return	
#ifndef NO_READLINE
		isatty (fileno (stdin)) ?
			readline (prompt) :
#endif
			fgets    (line, sizeof(line), stdin) ;
}

void	dashes
	(	FILE	*f,
		int	w
	)
{
	while (w > 0)
	{	fprintf	(f, "-") ;
		w -= 1	;
	}
}

void	doCopy
	(	XBaseSQL	&xb,
		const char	*name
	)
{
	XBSQLFieldSet	*fs	= xb.getFieldSet (name) ;
	XBSQLInsert	*ins	= 0	;
	bool		skip	= false	;

	if (fs != 0)
	{
		char	query[2048] ;

		sprintf	(query, "insert into %s (", name) ;

		for (int i = 0 ; i < fs->getNumFields () ; i += 1)
		{
			if (i > 0) strcat (query, ", ") ;
			strcat (query, fs->getFieldName (i)) ;
		}

		strcat	(query, ") values (") ;
		for (int i = 0 ; i < fs->getNumFields () ; i += 1)
		{
			if (i > 0) strcat (query, ", ") ;
			strcat (query, "?") ;
		}

		strcat	(query, ")") ;

		fprintf	(stderr, "[%s]\n", query) ;
		ins = xb.openInsert (query) ;
	}

	if (ins == 0)
		fprintf	(stderr, "xbsql: %s\n", xb.lastError()) ;

	for (;;)
	{
		XBSQLValue values[128]	;
		char	   *cp		;
		char	   *nl		;

		if ((cp = getline ((char *)"")) == 0) exit (0) ;
#ifndef NO_READLINE
		add_history  (cp) ;
#endif
		if (skip)
			continue ;

		if ((nl = strchr (cp, '\n')) != 0)
			*nl = 0  ;

		if ((cp[0] == '\\') && (cp[1] == '.'))
			break	 ;

		int	   idx	= 0 ;
		const char *v	= strsep (&cp, "\t") ;

		while ((v != 0) && (idx < fs->getNumFields()))
		{
			switch (fs->getFieldType(idx))
			{
				case 'C' :
					values[idx] = v ;
					break	;

				case 'F' :
					values[idx] = atof(v) ;
					break	;

				case 'N' :
					values[idx] = atoi(v) ;
					break	;
			}

			v	= strsep (&cp, "\t") ;
			idx    += 1 ;
		}

		if (!ins->execute (idx, values))
		{
			fprintf	(stderr, "xbsql: %s\n", xb.lastError()) ;
			skip	= true	;
		}

		if (verbose == 2)
		{	fprintf	(stderr, ".")	;
			fflush	(stderr)	;
		}
	}

	if (verbose == 2)
		fprintf	(stderr, "\n") ;

	if (ins != 0) delete ins ;
	if (fs  != 0) delete fs  ;
}

void	doSelect
	(	XBaseSQL	&xb,
		XBSQLSelect	*q
	)
{
	int	    nr ;
	int	    nf ;

	if (!q->execute (0, 0))
	{	fprintf	(stderr, "xbsql: %s\n", xb.lastError()) ;
		delete	q ;
		return	;
	}

	nr = q->getNumRows  () ;
	nf = q->getNumFields() ;

	for (uint idx = 0 ; idx < nf ; idx += 1)
		fprintf
		(	stderr,
			"%3d: %c : %s\n",
			idx,
			q->getFieldType(idx),
			q->getFieldName(idx)
		)	;

	int	*w	= new int[nf] ;
	memset	(w, 0, nf * sizeof(int)) ;

	for (int r = 0 ; r < nr ; r += 1)
		for (int f = 0 ; f < nf ; f += 1)
		{	XBSQLValue v = q->getField (r, f) ;
			if (v.getLength() > w[f]) w[f] = v.getLength() ;
		}
	for (int f = 0 ; f < nf ; f += 1)
		if (strlen(q->getFieldName (f)) > w[f])
			w[f] = strlen(q->getFieldName (f)) ;

	for (int f = 0 ; f < nf ; f += 1)
		if (w[f] > 100)
		{	fprintf	(stderr, "Trunc col %3d from %d\n") ;
			w[f] = 100 ;
		}

	fprintf	(stdout, "+") ;
	for (int f = 0 ; f < nf ; f += 1)
	{	dashes	(stdout, w[f] + 2) ;
		fprintf	(stdout, "+") ;
	}
	fprintf	(stdout, "\n") ;

	fprintf	(stdout, "|") ;
	for (int f = 0 ; f < nf ; f += 1)
		fprintf	(stdout, " %-*s|", w[f] + 1, q->getFieldName (f)) ;
	fprintf	(stdout, "\n") ;

	fprintf	(stdout, "+") ;
	for (int f = 0 ; f < nf ; f += 1)
	{	dashes	(stdout, w[f] + 2) ;
		fprintf	(stdout, "+") ;
	}
	fprintf	(stdout, "\n") ;

	for (int r = 0 ; r < nr ; r += 1)
	{
		fprintf	(stdout, "|") ;
		for (int f = 0 ; f < nf ; f += 1)
		{
			XBSQLValue v = q->getField (r, f) ;
			fprintf	(stdout, " %-*s|", w[f] + 1, v.getText())  ;
		}

		fprintf	(stdout, "\n") ;

		if (dumprows) q->dumprow (r) ;
	}

	fprintf	(stdout, "+") ;
	for (int f = 0 ; f < nf ; f += 1)
	{	dashes	(stdout, w[f] + 2) ;
		fprintf	(stdout, "+") ;
	}
	fprintf	(stdout, "\n") ;

	fprintf	(stdout, "\nReturned %d rows with %d columns\n", nr, nf) ;
	delete	q	;
	delete	[]w	;
}

void	doInsert
	(	XBaseSQL	&xb,
		XBSQLInsert	*q
	)
{
	if (!q->execute (0, 0))
	{	fprintf	(stderr, "xbsql: %s\n", xb.lastError()) ;
		delete	q ;
		return	;
	}

	delete	q	;
}

void	doUpdate
	(	XBaseSQL	&xb,
		XBSQLUpdate	*q
	)
{
	if (!q->execute (0, 0))
	{	fprintf	(stderr, "xbsql: %s\n", xb.lastError()) ;
		delete	q ;
		return	;
	}

	delete	q	;
}

void	doDelete
	(	XBaseSQL	&xb,
		XBSQLDelete	*q
	)
{
	if (!q->execute (0, 0))
	{	fprintf	(stderr, "xbsql: %s\n", xb.lastError()) ;
		delete	q ;
		return	;
	}

	delete	q	;
}

int	main
	(	int	argc,
		char	*argv[]
	)
{
	char	sqlb[10240]	;
	char	name[  128]	;
	char	*cp		;

	bool	packClose	= false	;
	bool	caseSensitive	= false	;
	bool	useWildcard	= false	;
	bool	goSlow		= false	;
	int	verbose		= 0	;

	argc	-= 1	;
	argv	+= 1	;

	while ((argc > 0) && (argv[0][0] == '-'))
	{
		char	*opts	= &argv[0][1] ;

		argc	-= 1	;
		argv	+= 1	;

		while (*opts) 
			switch (*opts)
			{
				case 'c' :
					caseSensitive = true ;
					opts	  += 1	  ;
					break	  ;

				case 'd' :
					dumprows   = true ;
					opts	  += 1	  ;
					break	  ;

				case 'p' :
					packClose  = true ;
					opts	  += 1	  ;
					break	  ;

				case 's' :
					goSlow 	   = true ;
					opts	  += 1	  ;
					break	  ;

				case 'w' :
					useWildcard  = true ;
					opts	  += 1	  ;
					break	  ;

				case 'v' :
					verbose    = 1	  ;
					opts	  += 1	  ;
					break	  ;

				case 'V' :
					verbose    = 2	  ;
					opts	  += 1	  ;
					break	  ;

				default	 :
					fprintf	  (stderr, USAGE) ;
					exit	  (1) ;
			}
	}

	if (argc > 1)
	{	fprintf	  (stderr, USAGE) ;
		exit	  (1) ;
	}	

	XBaseSQL xb (argc > 0 ? argv[0] : ".") 	;

	xb.setClosePack	    (packClose    ) ;
	xb.setCaseSensitive (caseSensitive) ;
	xb.setUseWildcard   (useWildcard  ) ;
	xb.setGoSlow	    (goSlow	  ) ;

	for (;;)
	{
		sqlb[0] = 0 ;

		for (;;)
		{
			const char *pr = sqlb[0] == 0 ? "xql> " : "xql  " ;
			if ((cp = getline ((char *)pr)) == 0) return 0 ;
			strcat (sqlb, cp) ;
#ifndef NO_READLINE
			add_history  (cp) ;
#endif
			if ((cp = strchr (sqlb, '\n')) != 0) *cp = ' ' ;

			cp = &sqlb[strlen(sqlb) - 1] ;
			while ((cp >= &sqlb[0]) && isspace(*cp)) cp -= 1 ;
			if ((cp >= &sqlb[0]) && (*cp == ';')) break ;
		}

		*cp = 0 ;
		cp = sqlb ;
		while (isspace(*cp)) cp += 1 ;

		if (verbose == 2)
		{	fprintf	(stdout, "\n") ;
			fprintf	(stdout, "----------------\n") ;
			fprintf	(stdout, "%s\n", cp) ;
			fprintf	(stdout, "----------------\n") ;
			fprintf	(stdout, "\n") ;
		}

		if (strncmp (cp, "exit", 4) == 0)
			break	;

		if (sscanf (cp, "desc %127[^ \t]", name) == 1)
		{
			XBSQLFieldSet *fs = xb.getFieldSet (name) ;
			if (fs == 0)
			{	fprintf	(stderr, "xbsql: %s\n", xb.lastError()) ;
				continue ;
			}
			for (int i = 0 ; i < fs->getNumFields () ; i += 1)
			{
				int	     tchr = fs->getFieldType (i) ; 
				XBSQL::Index idx  = fs->fieldIndexed (i) ;

				const char *tstr = tchr == 'N' ? "Integer" : 
						   tchr == 'F' ? "Float  " :
						   tchr == 'C' ? "Char   " :
						   tchr == 'M' ? "Blob   " :
						   tchr == 'D' ? "Date   " :
						   tchr == 'L' ? "Bool   " :
								 "????   " ;

				char  w[10] ;
				if (tchr == 'F')
					sprintf	(w, ",%d", fs->getFieldPrec (i)) ;
				else	strcpy	(w, "") ;

				fprintf	(stdout, "%-12s %s (%3d%s) %s\n",
						 fs->getFieldName   (i),
						 tstr,
						 fs->getFieldLength (i),
						 w,
						 idx == XBSQL::IndexUnique    ? "unique index" :
						 idx == XBSQL::IndexNotUnique ? "index"	       :
										"") ;
			}

			delete	 fs ;
			continue ;
		}

		if (strncmp (cp, "show tables", 11) == 0)
		{
			XBSQLTableSet *ts = xb.getTableSet() ;

			for (int i = 0 ; i < ts->getNumTables() ; i += 1)
				fprintf	(stdout, "%s\n", ts->getTableName (i)) ;
			delete	 ts ;
			continue ;
		}

		if (sscanf (cp, "copy %127[^ \t]", name) == 1)
		{
			doCopy (xb, name) ;
			continue ;
		}

		bool	   parsed ;
		XBSQLQuery *q	  = xb.openQuery (cp, parsed) ;

		if (q != 0)
		{
			if ((verbose == 1) && (q->isSelect() != 0))
			{	fprintf	(stdout, "\n") ;
				fprintf	(stdout, "----------------\n") ;
				fprintf	(stdout, "%s\n", cp) ;
				fprintf	(stdout, "----------------\n") ;
				fprintf	(stdout, "\n") ;
			}

			if (q->isSelect() != 0)
			{	doSelect (xb, q->isSelect()) ;
				continue ;
			}
			if (q->isInsert() != 0)
			{	doInsert (xb, q->isInsert()) ;
				continue ;
			}
			if (q->isUpdate() != 0)
			{	doUpdate (xb, q->isUpdate()) ;
				continue ;
			}
			if (q->isDelete() != 0)
			{	doDelete (xb, q->isDelete()) ;
				continue ;
			}

			delete	q ;
		}

		if (parsed)
		{
			fprintf	(stderr, "xbsql: %s\n", xb.lastError()) ;
			continue ;
		}

		if (!xb.execCommand (cp))
		{
			fprintf	(stderr, "xbsql: %s\n", xb.lastError()) ;
			continue ;
		}
	}

	return	0 ;
}
