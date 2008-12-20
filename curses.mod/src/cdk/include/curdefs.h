#ifndef CDKINCLUDES
#ifndef CURDEF_H
#define CURDEF_H

#ifndef CDK_H
#define CDKINCLUDES
#include <cdk.h>
#undef CDKINCLUDES
#endif

/*
 * $Id: curdefs.h,v 1.13 2004/08/30 23:57:07 tom Exp $
 *
 * Changes 1999-2002,2004 copyright Thomas E. Dickey
 * Copyright 1999, Mike Glover
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgment:
 * 	This product includes software developed by Mike Glover
 * 	and contributors.
 * 4. Neither the name of Mike Glover, nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MIKE GLOVER AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL MIKE GLOVER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * This header file adds some useful curses-style definitions.
 */

#undef CTRL
#define CTRL(c)		((c)&0x1f)

#undef  CDK_REFRESH
#define CDK_REFRESH	CTRL('L')
#undef  CDK_PASTE
#define CDK_PASTE	CTRL('V')
#undef  CDK_COPY
#define CDK_COPY	CTRL('Y')
#undef  CDK_ERASE
#define CDK_ERASE	CTRL('U')
#undef  CDK_CUT
#define CDK_CUT		CTRL('X')
#undef  CDK_BEGOFLINE
#define CDK_BEGOFLINE	CTRL('A')
#undef  CDK_ENDOFLINE
#define CDK_ENDOFLINE	CTRL('E')
#undef  CDK_BACKCHAR
#define CDK_BACKCHAR	CTRL('B')
#undef  CDK_FORCHAR
#define CDK_FORCHAR	CTRL('F')
#undef  CDK_TRANSPOSE
#define CDK_TRANSPOSE	CTRL('T')
#undef  CDK_NEXT
#define CDK_NEXT	CTRL('N')
#undef  CDK_PREV
#define CDK_PREV	CTRL('P')
#undef  SPACE
#define SPACE		' '
#undef  DELETE
#define DELETE		'\177'	/* Delete key				*/
#undef  TAB
#define TAB		'\t'	/* Tab key.				*/
#undef  KEY_ESC
#define KEY_ESC		'\033'	/* Escape Key.				*/
#undef  KEY_RETURN
#define KEY_RETURN	'\012'	/* Return key				*/
#undef  KEY_TAB
#define KEY_TAB		'\t'	/* Tab key				*/
#undef  KEY_F1
#define KEY_F1          KEY_F(1)
#undef  KEY_F2
#define KEY_F2          KEY_F(2)
#undef  KEY_F3
#define KEY_F3          KEY_F(3)
#undef  KEY_F4
#define KEY_F4          KEY_F(4)
#undef  KEY_F5
#define KEY_F5          KEY_F(5)
#undef  KEY_F6
#define KEY_F6          KEY_F(6)
#undef  KEY_F7
#define KEY_F7          KEY_F(7)
#undef  KEY_F8
#define KEY_F8          KEY_F(8)
#undef  KEY_F9
#define KEY_F9          KEY_F(9)
#undef  KEY_F10
#define KEY_F10		KEY_F(10)
#undef  KEY_F11
#define KEY_F11		KEY_F(11)
#undef  KEY_F12
#define KEY_F12		KEY_F(12)

/* these definitions may work for antique versions of curses */
#if	!defined(HAVE_GETBEGYX) && !defined(getbegyx)
#define getbegyx(win,y,x)	(y = (win)?(win)->_begy:ERR, x = (win)?(win)->_begx:ERR)
#endif

#if	!defined(HAVE_GETMAXYX) && !defined(getmaxyx)
#define getmaxyx(win,y,x)	(y = (win)?(win)->_maxy:ERR, x = (win)?(win)->_maxx:ERR)
#endif

/* these definitions may be needed for bleeding-edge curses implementations */
#if !(defined(HAVE_GETBEGX) && defined(HAVE_GETBEGY))
#undef getbegx
#undef getbegy
#define getbegx(win) cdk_getbegx(win)
#define getbegy(win) cdk_getbegy(win)
extern int cdk_getbegx(WINDOW *);
extern int cdk_getbegy(WINDOW *);
#endif

#if !(defined(HAVE_GETMAXX) && defined(HAVE_GETMAXY))
#undef getmaxx
#undef getmaxy
#define getmaxx(win) cdk_getmaxx(win)
#define getmaxy(win) cdk_getmaxy(win)
extern int cdk_getmaxx(WINDOW *);
extern int cdk_getmaxy(WINDOW *);
#endif

/*
 * Derived macros
 */
#define getendx(a)	(getbegx(a) + getmaxx(a))
#define getendy(a)	(getbegy(a) + getmaxy(a))

#endif /* CURDEF_H */
#endif /* CDKINCLUDES */
