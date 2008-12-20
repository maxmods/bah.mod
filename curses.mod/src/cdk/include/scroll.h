/*
 * $Id: scroll.h,v 1.30 2006/05/04 11:24:28 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKSCROLL_H
#define CDKSCROLL_H	1

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
 * Changes 1999-2005,2006 copyright Thomas E. Dickey
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
 * Declare scrolling list definitions.
 */
#define	NUMBERS		TRUE
#define	NONUMBERS	FALSE

/*
 * Declare scrolling list definitions.
 */
struct SScroll {
   CDKOBJS	obj;
   WINDOW	*parent;
   WINDOW	*win;
   WINDOW	*scrollbarWin;
   WINDOW       *listWin;
   WINDOW	*shadowWin;
   int		titleAdj;	/* unused */
   chtype **	item;		/* */
   int *	itemPos;	/* */
   int *	itemLen;	/* */
   int		maxTopItem;	/* */
   int		maxLeftChar;	/* */
   int		leftChar;	/* */
   int		lastItem;	/* */
   int		currentTop;	/* */
   int		currentItem;	/* */
   int		currentHigh;	/* */
   int		listSize;	/* */
   int		boxWidth;	/* */
   int		boxHeight;	/* */
   int		viewSize;	/* */

   int		scrollbarPlacement; /* UNUSED */
   boolean	scrollbar;	/* UNUSED */
   int		toggleSize;	/* size of scrollbar thumb/toggle */
   int		togglePos;	/* position of scrollbar thumb/toggle */
   float	step;		/* increment for scrollbar */

   EExitType	exitType;	/* */
   boolean	shadow;		/* */
   boolean	numbers;	/* */
   chtype	titlehighlight;	/* */
   chtype	highlight;	/* */
};
typedef struct SScroll CDKSCROLL;

/*
 * This creates a new CDK scrolling list pointer.
 */
CDKSCROLL *newCDKScroll (
		CDKSCREEN *	/* cdkscreen */,
		int		/* xpos */,
		int		/* ypos */,
		int		/* spos */,
		int		/* height */,
		int		/* width */,
		char *		/* title */,
		char **		/* itemList */,
		int		/* items */,
		boolean		/* numbers */,
		chtype		/* highlight */,
		boolean		/* Box */,
		boolean		/* shadow */);

/*
 * This activates the scrolling list.
 */
int activateCDKScroll (
		CDKSCROLL *	/* scroll */,
		chtype *	/* actions */);

/*
 * This injects a single character into the scrolling list.
 */
#define injectCDKScroll(obj,input) injectCDKObject(obj,input,Int)

/*
 * This sets various attributes of the scrolling list.
 */
void setCDKScroll (
		CDKSCROLL *	/* scroll */,
		char **		/* itemList */,
		int		/* items */,
		boolean		/* numbers */,
		chtype		/* highlight */,
		boolean		/* Box */);

void setCDKScrollPosition (
		CDKSCROLL *	/* scroll */,
		int		/* item */);

int  getCDKScrollCurrent(
		CDKSCROLL *	/* scroll */);

void setCDKScrollCurrent(
		CDKSCROLL *	/* scroll */,
		int		/* i */);

int  getCDKScrollCurrentTop(
		CDKSCROLL *	/* scroll */);

void setCDKScrollCurrentTop(
		CDKSCROLL *	/* scroll */,
		int		/* i */);

int  getCDKScrollCurrentItem(
		CDKSCROLL *	/* scroll */);

void setCDKScrollCurrentItem(
		CDKSCROLL *	/* scroll */,
		int		/* i */);

/*
 * This sets the contents of the scrolling list.
 */
void setCDKScrollItems (
		CDKSCROLL *	/* scroll */,
		char **		/* itemList */,
		int		/* items */,
		boolean		/* numbers */);

int getCDKScrollItems (
		CDKSCROLL *	/* scroll */,
		char **		/* itemList */);

/*
 * This sets the highlight bar of the scrolling list.
 */
void setCDKScrollHighlight (
		CDKSCROLL *	/* scroll */,
		chtype		/* highlight */);

chtype getCDKScrollHighlight (
		CDKSCROLL *	/* scroll */,
		chtype		/* highlight */);

/*
 * This sets the box attribute of the widget.
 */
void setCDKScrollBox (
		CDKSCROLL *	/* scroll */,
		boolean		/* Box */);

boolean getCDKScrollBox (
		CDKSCROLL *	/* scroll */);

/*
 * These set the drawing characters of the widget.
 */
#define setCDKScrollULChar(w,c)            setULCharOf(w,c)
#define setCDKScrollURChar(w,c)            setURCharOf(w,c)
#define setCDKScrollLLChar(w,c)            setLLCharOf(w,c)
#define setCDKScrollLRChar(w,c)            setLRCharOf(w,c)
#define setCDKScrollVerticalChar(w,c)      setVTCharOf(w,c)
#define setCDKScrollHorizontalChar(w,c)    setHZCharOf(w,c)
#define setCDKScrollBoxAttribute(w,c)      setBXAttrOf(w,c)

/*
 * This sets the background color of the widget.
 */ 
#define setCDKScrollBackgroundColor(w,c) setCDKObjectBackgroundColor(ObjOf(w),c)

/*
 * This sets the background attribute of the widget.
 */ 
#define setCDKScrollBackgroundAttrib(w,c) setBKAttrOf(w,c)

/*
 * This adds a single item to the end of scrolling list.
 */
void addCDKScrollItem (
		CDKSCROLL *	/* scroll */,
		char *		/* item */);

/*
 * This adds a single item into the scrolling list.
 */
void insertCDKScrollItem (
		CDKSCROLL *	/* scroll */,
		char *		/* item */);

/*
 * This deletes a single item from the scrolling list.
 */
void deleteCDKScrollItem (
		CDKSCROLL *	/* scroll */,
		int		/* position */);

/*
 * This draws the scrolling list on the screen.
 */
#define drawCDKScroll(obj,Box) drawCDKObject(obj,Box)

/*
 * This removes the scrolling list from the screen.
 */
#define eraseCDKScroll(obj) eraseCDKObject(obj)

/*
 * This moves the widget to the given location.
 */
#define moveCDKScroll(obj,xpos,ypos,relative,refresh) moveCDKObject(obj,xpos,ypos,relative,refresh)

/*
 * This allows the user to interactively position the widget on the screen.
 */
#define positionCDKScroll(widget) positionCDKObject(ObjOf(widget),widget->win)

/*
 * This destroys the widget and all associated memory.
 */
#define destroyCDKScroll(obj) destroyCDKObject(obj)

/*
 * These set the scrolling list pre/post process functions.
 */
#define setCDKScrollPreProcess(w,f,d)  setCDKObjectPreProcess(ObjOf(w),f,d)
#define setCDKScrollPostProcess(w,f,d) setCDKObjectPostProcess(ObjOf(w),f,d)

#ifdef __cplusplus
}
#endif

#endif /* CDKSCROLL_H */
#endif /* CDKINCLUDES */
