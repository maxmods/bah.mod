/*
 * $Id: mentry.h,v 1.22 2004/08/30 00:00:57 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKMENTRY_H
#define CDKMENTRY_H	1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CDK_H
#define CDKINCLUDES
#include <cdk.h>
#undef CDKINCLUDES
#include <binding.h>
#include <cdkscreen.h>
#include <cdk_objs.h>
#endif

/*
 * Changes 1999-2002,2003 copyright Thomas E. Dickey
 *
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
 *      This product includes software developed by Mike Glover
 *      and contributors.
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
 * Define the CDK multiple line entry field widget.
 */
typedef struct SMentry CDKMENTRY;
typedef void (*MENTRYCB) (CDKMENTRY *mentry, chtype character);

struct SMentry {
   CDKOBJS	obj;
   WINDOW *	parent;
   WINDOW *	win;
   WINDOW *	shadowWin;
   WINDOW *	labelWin;
   WINDOW *	fieldWin;
   int		titleAdj;
   chtype *	label;
   int		labelLen;
   int		labelPos;
   chtype	fieldAttr;
   int		fieldWidth;
   int		boxHeight;
   int		boxWidth;
   char *	info;
   int		totalWidth;
   int		rows;
   int		currentRow;
   int		currentCol;
   int		topRow;
   EDisplayType dispType;
   int		min;
   int		logicalRows;
   EExitType	exitType;
   boolean	shadow;
   chtype	filler;
   chtype	hidden;
   MENTRYCB	callbackfn;
};

/*
 * This creates a new CDK mentry field pointer.
 */
CDKMENTRY *newCDKMentry (
		CDKSCREEN *	/* cdkscreen */,
		int		/* xpos */,
		int		/* ypos */,
		char *		/* title */,
		char *		/* label */,
		chtype		/* fieldAttr */,
		chtype		/* filler */,
		EDisplayType	/* disptype */,
		int		/* fieldWidth */,
		int		/* fieldrows */,
		int		/* logicalRows */,
		int		/* min */,
		boolean		/* Box */,
		boolean		/* shadow */);

/*
 * This activates the widget.
 */
char *activateCDKMentry (
		CDKMENTRY *	/* mentry */,
		chtype *	/* input */);

/*
 * This injects a single character into the widget.
 */
#define injectCDKMentry(obj,input) injectCDKObject(obj,input,String)

/*
 * These set specific attributes of the widget.
 */
void setCDKMentry (
		CDKMENTRY *	/* mentry */,
		char *		/* value */,
		int		/* min */,
		boolean		/* Box */);

/*
 * This sets the value of the widget.
 */
void setCDKMentryValue (
		CDKMENTRY *	/* mentry */,
		char *		/* value */);

char *getCDKMentryValue (
		CDKMENTRY *	/* mentry */);

/*
 * This sets the minimum length of the value for the widget.
 */
void setCDKMentryMin (
		CDKMENTRY *	/* mentry */,
		int		/* min */);

int getCDKMentryMin (
		CDKMENTRY *	/* mentry */);

/*
 * This sets the filler character to use when drawing the widget.
 */
void setCDKMentryFillerChar (
		CDKMENTRY *	/* mentry */,
		chtype		/* filler */);

chtype getCDKMentryFillerChar (
		CDKMENTRY *	/* mentry */);

/*
 * This sets the character to use when a hidden character type is used.
 */
void setCDKMentryHiddenChar (
		CDKMENTRY *	/* mentry */,
		chtype		/* character */);

chtype getCDKMentryHiddenChar (
		CDKMENTRY *	/* mentry */);

/*
 * This sets the box attribute of the mentry widget.
 */
void setCDKMentryBox (
		CDKMENTRY *	/* mentry */,
		boolean		/* Box */);

boolean getCDKMentryBox (
		CDKMENTRY *	/* mentry */);

/*
 * These set the drawing characters of the widget.
 */
#define setCDKMentryULChar(w,c)            setULCharOf(w,c)
#define setCDKMentryURChar(w,c)            setURCharOf(w,c)
#define setCDKMentryLLChar(w,c)            setLLCharOf(w,c)
#define setCDKMentryLRChar(w,c)            setLRCharOf(w,c)
#define setCDKMentryVerticalChar(w,c)      setVTCharOf(w,c)
#define setCDKMentryHorizontalChar(w,c)    setHZCharOf(w,c)
#define setCDKMentryBoxAttribute(w,c)      setBXAttrOf(w,c)

/*
 * This sets the background color of the widget.
 */
#define setCDKMentryBackgroundColor(w,c) setCDKObjectBackgroundColor(ObjOf(w),c)

/*
 * This sets the background attribute of the widget.
 */ 
#define setCDKMentryBackgroundAttrib(w,c) setBKAttrOf(w,c)

/*
 * This draws the widget on the screen.
 */
#define drawCDKMentry(obj,Box) drawCDKObject(obj,Box)

/*
 * This erases the widget from the screen.
 */
#define eraseCDKMentry(obj) eraseCDKObject(obj)

/*
 * This cleans out the information in the widget.
 */
void cleanCDKMentry (
		CDKMENTRY *	/* mentry */);

/*
 * This moves the widget to the given location.
 */
#define moveCDKMentry(obj,xpos,ypos,relative,refresh) moveCDKObject(obj,xpos,ypos,relative,refresh)

/*
 * This interactively moves the widget on the screen.
 */
#define positionCDKMentry(widget) positionCDKObject(ObjOf(widget),widget->win)

/*
 * This destroys the widget pointer.
 */
#define destroyCDKMentry(obj) destroyCDKObject(obj)

/*
 * This draws the field of the widget.
 */
void drawCDKMentryField (
		CDKMENTRY *	/* mentry */);

/*
 * This sets the widgets main callback.
 */
void setCDKMentryCB (
		CDKMENTRY *	/* mentry */,
		MENTRYCB	/* callback */);

/*
 * These set the pre/post process callback functions.
 */
#define setCDKMentryPreProcess(w,f,d)  setCDKObjectPreProcess(ObjOf(w),f,d)
#define setCDKMentryPostProcess(w,f,d) setCDKObjectPostProcess(ObjOf(w),f,d)

#ifdef __cplusplus
}
#endif

#endif /* CDKMENTRY_H */
#endif /* CDKINCLUDES */
