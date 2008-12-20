/*
 * $Id: selection.h,v 1.26 2005/12/28 21:45:16 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKSELECTION_H
#define CDKSELECTION_H	1

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
 * Changes 1999-2003,2005 copyright Thomas E. Dickey
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
 *	This product includes software developed by Mike Glover
 *	and contributors.
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
 * Declare selection list definitions.
 */
#define MAX_CHOICES	100	/* unused by widgets */

/*
 * Define the CDK selection widget structure.
 */
struct SSelection {
   CDKOBJS	obj;
   WINDOW *	parent;
   WINDOW *	win;
   WINDOW *	scrollbarWin;
   WINDOW *	shadowWin;
   int		titleAdj;	/* unused */
   chtype **	item;
   int *	itemLen;
   int *	itemPos;
   chtype **	choice;
   int *	choicelen;
   int		choiceCount;
   int		maxchoicelen;
   int *	selections;
   int *	mode;
   int		maxTopItem;
   int		maxLeftChar;
   int		leftChar;

   int		lastItem;
   int		currentTop;
   int		currentHigh;
   int		currentItem;
   int		listSize;
   int		scrollbarPlacement;
   boolean	scrollbar;
   int		toggleSize;
   int		togglePos;
   float	step;

   int		boxWidth;
   int		boxHeight;
   int		viewSize;
   EExitType	exitType;
   boolean	shadow;
   chtype	highlight;
};
typedef struct SSelection CDKSELECTION;

/*
 * This creates a new pointer to a selection widget.
 */
CDKSELECTION *newCDKSelection (
		CDKSCREEN *	/* cdkscreen */,
		int		/* xpos */,
		int		/* ypos */,
		int		/* spos */,
		int		/* height */,
		int		/* width */,
		char *		/* title */,
		char **		/* list */,
		int		/* listSize */,
		char **		/* choices */,
		int		/* choiceSize */,
		chtype		/* highlight */,
		boolean		/* Box */,
		boolean		/* shadow */);

/*
 * This activates the selection widget.
 */
int activateCDKSelection (
		CDKSELECTION *	/* selection */,
		chtype *	/* actions */);

/*
 * This injects a single character into the widget.
 */
#define injectCDKSelection(obj,input) injectCDKObject(obj,input,Int)

/*
 * This draws the selection widget.
 */
#define drawCDKSelection(obj,Box) drawCDKObject(obj,Box)

/*
 * This erases the selection widget from the screen.
 */
#define eraseCDKSelection(obj) eraseCDKObject(obj)

/*
 * This moves the widget to the given location.
 */
#define moveCDKSelection(obj,xpos,ypos,relative,refresh) moveCDKObject(obj,xpos,ypos,relative,refresh)

/*
 * This interactively moves the widget on the screen.
 */
#define positionCDKSelection(widget) positionCDKObject(ObjOf(widget),widget->win)

/*
 * This destroys the widget and all associated memory.
 */
#define destroyCDKSelection(obj) destroyCDKObject(obj)

/*
 * This sets various attributes of the selection widget.
 */
void setCDKSelection (
		CDKSELECTION *	/* selection */,
		chtype		/* highlight */,
		int *		/* defChoices */,
		boolean		/* Box */);

/*
 * This sets the contents of the selection list.
 */
void setCDKSelectionItems (
		CDKSELECTION *	/* selection */,
		char **		/* list */,
		int		/* listSize */);

int getCDKSelectionItems (
		CDKSELECTION *	/* selection */,
		char **		/* list */);

/*
 *
 */
void setCDKSelectionTitle (
		CDKSELECTION *	/* selection */,
		char *		/* title */);

char *getCDKSelectionTitle (
		CDKSELECTION *	/* selection */);

/*
 * This sets the selection list highlight bar.
 */
void setCDKSelectionHighlight (
		CDKSELECTION *	/* selection */,
		chtype		/* highlight */);

chtype getCDKSelectionHighlight (
		CDKSELECTION *	/* selection */);

/*
 * set/get the current item index
 */
void setCDKSelectionCurrent (
		CDKSELECTION *	/* selection */,
		int		/* index */);

int getCDKSelectionCurrent (
		CDKSELECTION *	/* selection */);

/*
 * This sets the choices of the selection list.
 */
void setCDKSelectionChoices (
		CDKSELECTION *	/* selection */,
		int *		/* choices */);

int *getCDKSelectionChoices (
		CDKSELECTION *	/* selection */);

void setCDKSelectionChoice (
		CDKSELECTION *	/* selection */,
		int		/* index */,
		int		/* choice */);

int getCDKSelectionChoice (
		CDKSELECTION *	/* selection */,
		int		/* index */);

/*
 * This sets the modes of the items in the selection list. Currently
 * there are only two: editable=0 and read-only=1
 */
void setCDKSelectionModes (
		CDKSELECTION *	/* selection */,
		int *		/* modes */);

int *getCDKSelectionModes (
		CDKSELECTION *	/* selection */);

void setCDKSelectionMode (
		CDKSELECTION *	/* selection */,
		int		/* index */,
		int		/* mode */);

int getCDKSelectionMode (
		CDKSELECTION *	/* selection */,
		int		/* index */);

/*
 * This sets the box attribute of the widget.
 */
void setCDKSelectionBox (
		CDKSELECTION *	/* selection */,
		boolean		/* Box */);

boolean getCDKSelectionBox (
		CDKSELECTION *	/* selection */);

/*
 * These set the drawing characters of the widget.
 */
#define setCDKSelectionULChar(w,c)         setULCharOf(w,c)
#define setCDKSelectionURChar(w,c)         setURCharOf(w,c)
#define setCDKSelectionLLChar(w,c)         setLLCharOf(w,c)
#define setCDKSelectionLRChar(w,c)         setLRCharOf(w,c)
#define setCDKSelectionVerticalChar(w,c)   setVTCharOf(w,c)
#define setCDKSelectionHorizontalChar(w,c) setHZCharOf(w,c)
#define setCDKSelectionBoxAttribute(w,c)   setBXAttrOf(w,c)

/*
 * This sets the background color of the widget.
 */
#define setCDKSelectionBackgroundColor(w,c) setCDKObjectBackgroundColor(ObjOf(w),c)

/*
 * This sets the background attribute of the widget.
 */
#define setCDKSelectionBackgroundAttrib(w,c) setBKAttrOf(w,c)

/*
 * These set the pre/post process callback functions.
 */
#define setCDKSelectionPreProcess(w,f,d)  setCDKObjectPreProcess(ObjOf(w),f,d)
#define setCDKSelectionPostProcess(w,f,d) setCDKObjectPostProcess(ObjOf(w),f,d)

#ifdef __cplusplus
}
#endif

#endif /* CDKSELECTION_H */
#endif /* CDKINCLUDES */
