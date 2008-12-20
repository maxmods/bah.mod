/*
 * $Id: itemlist.h,v 1.23 2004/08/30 00:00:57 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKITEMLIST_H
#define CDKITEMLIST_H	1

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
 * Changes 1999-2003 copyright Thomas E. Dickey
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
 * Define the CDK itemlist widget structure.
 */
struct SItemList {
   CDKOBJS	obj;
   WINDOW *	parent;
   WINDOW *	win;
   WINDOW *	shadowWin;
   WINDOW *	labelWin;
   WINDOW *	fieldWin;
   int		titleAdj;
   chtype *	label;
   int		labelLen;
   int		fieldWidth;
   chtype **	item;
   int *	itemPos;
   int *	itemLen;
   int		listSize;
   int		currentItem;
   int		defaultItem;
   int		boxWidth;
   int		boxHeight;
   EExitType	exitType;
   boolean	shadow;
};
typedef struct SItemList CDKITEMLIST;

/*
 * This creates a pointer to a CDK itemlist widget.
 */
CDKITEMLIST *newCDKItemlist (
		CDKSCREEN *	/* cdkscreen */,
		int		/* xpos */,
		int		/* ypos */,
		char *		/* title */,
		char *		/* label */,
		char **		/* itemlist */,
		int		/* count */,
		int		/* defaultItem */,
		boolean		/* Box */,
		boolean		/* shadow */);

/*
 * This activates the itemlist widget.
 */
int activateCDKItemlist (
		CDKITEMLIST *	/* itemlist */,
		chtype *	/* actions */);

/*
 * This injects a single character into the itemlist widget.
 */
#define injectCDKItemlist(obj,input) injectCDKObject(obj,input,Int)

/*
 * These functions set specific elements of the itemlist widget.
 */
void setCDKItemlist (
		CDKITEMLIST *	/* itemlist */,
		char **		/* list */,
		int		/* count */,
		int		/* current */,
		boolean		/* Box */);

/*
 * This function sets the values of the item list widget.
 */
void setCDKItemlistValues (
		CDKITEMLIST *	/* itemlist */,
		char **		/* list */,
		int		/* count */,
		int		/* defaultItem */);

chtype **getCDKItemlistValues (
		CDKITEMLIST *	/* itemlist */,
		int *		/* size */);

/*
 * This sets the default item in the list.
 */
void setCDKItemlistDefaultItem (
		CDKITEMLIST *	/* itemlist */,
		int		/* defaultItem */);

int getCDKItemlistDefaultItem (
		CDKITEMLIST *	/* itemlist */);

/*
 * This returns an index to the current item in the list.
 */
void setCDKItemlistCurrentItem (
		CDKITEMLIST *	/* itemlist */,
		int		/* currentItem */);

int getCDKItemlistCurrentItem (
		CDKITEMLIST *	/* itemlist */);

/*
 * This sets the box attribute of the widget.
 */
void setCDKItemlistBox (
		CDKITEMLIST *	/* itemlist */,
		boolean		/* Box */);

boolean getCDKItemlistBox (
		CDKITEMLIST *	/* itemlist */);

/*
 * These set the drawing characters of the widget.
 */
#define setCDKItemlistULChar(w,c)          setULCharOf(w,c)
#define setCDKItemlistURChar(w,c)          setURCharOf(w,c)
#define setCDKItemlistLLChar(w,c)          setLLCharOf(w,c)
#define setCDKItemlistLRChar(w,c)          setLRCharOf(w,c)
#define setCDKItemlistVerticalChar(w,c)    setVTCharOf(w,c)
#define setCDKItemlistHorizontalChar(w,c)  setHZCharOf(w,c)
#define setCDKItemlistBoxAttribute(w,c)    setBXAttrOf(w,c)

/*
 * This sets the background color of the widget.
 */
#define setCDKItemlistBackgroundColor(w,c) setCDKObjectBackgroundColor(ObjOf(w),c)

/*
 * This sets the background attribute of the widget.
 */ 
#define setCDKItemlistBackgroundAttrib(w,c) setBKAttrOf(w,c)

/*
 * This draws the itemlist widget.
 */
#define drawCDKItemlist(obj,Box) drawCDKObject(obj,Box)

/*
 * This draws the itemlist field.
 */
void drawCDKItemlistField (
		CDKITEMLIST *	/* itemlist */,
		boolean         /* highlight */);

/*
 * This removes the widget from the screen.
 */
#define eraseCDKItemlist(obj) eraseCDKObject(obj)

/*
 * This moves the widget to the given position.
 */
#define moveCDKItemlist(obj,xpos,ypos,relative,refresh) moveCDKObject(obj,xpos,ypos,relative,refresh)

/*
 * This allows the user to interactively move the widget.
 */
#define positionCDKItemlist(widget) positionCDKObject(ObjOf(widget),widget->win)

/*
 * This destroys the widget and all the associated memory.
 */
#define destroyCDKItemlist(obj) destroyCDKObject(obj)

/*
 * These functions set the pre/post process functions.
 */
#define setCDKItemlistPreProcess(w,f,d)  setCDKObjectPreProcess(ObjOf(w),f,d)
#define setCDKItemlistPostProcess(w,f,d) setCDKObjectPostProcess(ObjOf(w),f,d)

#ifdef __cplusplus
}
#endif

#endif /* CDKITEMLIST_H */
#endif /* CDKINCLUDES */
