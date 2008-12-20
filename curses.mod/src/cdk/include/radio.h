/*
 * $Id: radio.h,v 1.28 2005/12/28 21:44:56 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKRADIO_H
#define CDKRADIO_H	1

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
 * Changes 1999-2004,2005 copyright Thomas E. Dickey
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
 * Define the CDK radio list widget structure.
 */
struct SRadio {
   CDKOBJS	obj;
   WINDOW *	parent;
   WINDOW *	win;
   WINDOW *	scrollbarWin;
   WINDOW *	shadowWin;
   chtype **	item;
   int *	itemLen;
   int *	itemPos;
   int		titleAdj;	/* unused */
   chtype	choiceChar;
   chtype	leftBoxChar;
   chtype	rightBoxChar;
   int		maxLeftChar;
   int		widestItem;
   int		leftChar;
   int		selectedItem;
   int		currentTop;
   int		currentItem;
   int		currentHigh;
   int		scrollbarPlacement;
   boolean	scrollbar;

   int		toggleSize;
   int		togglePos;
   float	step;
   int		listSize;
   int		lastItem;
   int		maxTopItem;
   int		boxWidth;
   int		boxHeight;
   int		viewSize;

   int		defItem;
   EExitType	exitType;
   boolean	shadow;
   chtype	highlight;
};
typedef struct SRadio CDKRADIO;

/*
 * This creates a new radio widget pointer.
 */
CDKRADIO *newCDKRadio (
		CDKSCREEN *	/* cdkscreen */,
		int		/* xpos */,
		int		/* ypos */,
		int		/* spos */,
		int		/* height */,
		int		/* width */,
		char *		/* title */,
		char **		/* mesg */,
		int		/* items */,
		chtype		/* choiceChar */,
		int		/* defItem */,
		chtype		/* highlight */,
		boolean		/* Box */,
		boolean		/* shadow */);

/*
 * This activates the widget.
 */
int activateCDKRadio (
		CDKRADIO *	/* radio */,
		chtype *	/* actions */);

/*
 * This injects a single character into the widget.
 */
#define injectCDKRadio(obj,input) injectCDKObject(obj,input,Int)

/*
 * These set various attributes of the widget.
 */
void setCDKRadio (
		CDKRADIO *	/* radio */,
		chtype		/* highlight */,
		chtype		/* character */,
		boolean		/* Box */);

/*
 * This sets the contents of the radio list.
 */
void setCDKRadioItems (
		CDKRADIO *	/* radio */,
		char **		/* list */,
		int		/* listSize */);

int getCDKRadioItems (
		CDKRADIO *	/* radio */,
		char **		/* list */);

/*
 * This sets the highlight bar attribute.
 */
void setCDKRadioHighlight (
		CDKRADIO *	/* radio */,
		chtype		/* highlight */);

chtype getCDKRadioHighlight (
		CDKRADIO *	/* radio */);

/*
 * This sets the 'selected' character.
 */
void setCDKRadioChoiceCharacter (
		CDKRADIO *	/* radio */,
		chtype		/* character */);

chtype getCDKRadioChoiceCharacter (
		CDKRADIO *	/* radio */);

/*
 * This sets the character to draw on the left/right side of
 * the choice box.
 */
void setCDKRadioLeftBrace (
		CDKRADIO *	/* radio */,
		chtype		/* character */);

chtype getCDKRadioLeftBrace (
		CDKRADIO *	/* radio */);

void setCDKRadioRightBrace (
		CDKRADIO *	/* radio */,
		chtype		/* character */);

chtype getCDKRadioRightBrace (
		CDKRADIO *	/* radio */);

/*
 * This sets the box attribute of the widget.
 */
void setCDKRadioBox (
		CDKRADIO *	/* radio */,
		boolean		/* Box */);

boolean getCDKRadioBox (
		CDKRADIO *	/* radio */);

/*
 * This sets the current high lighted item of the widget
 */
void setCDKRadioCurrentItem (
		CDKRADIO *	/* radio */,
 		int 		/* current item */);

int getCDKRadioCurrentItem (
		CDKRADIO *	/* radio */);

/*
 * This sets the current selected item of the widget
 */
void setCDKRadioSelectedItem (
		CDKRADIO *	/* radio */,
 		int 		/* current item */);

int getCDKRadioSelectedItem (
		CDKRADIO *	/* radio */);


/*
 * These set the drawing characters of the widget.
 */
#define setCDKRadioULChar(w,c)             setULCharOf(w,c)
#define setCDKRadioURChar(w,c)             setURCharOf(w,c)
#define setCDKRadioLLChar(w,c)             setLLCharOf(w,c)
#define setCDKRadioLRChar(w,c)             setLRCharOf(w,c)
#define setCDKRadioVerticalChar(w,c)       setVTCharOf(w,c)
#define setCDKRadioHorizontalChar(w,c)     setHZCharOf(w,c)
#define setCDKRadioBoxAttribute(w,c)       setBXAttrOf(w,c)

/*
 * This sets the background color of the widget.
 */
#define setCDKRadioBackgroundColor(w,c) setCDKObjectBackgroundColor(ObjOf(w),c)

/*
 * This sets the background attribute of the widget.
 */
#define setCDKRadioBackgroundAttrib(w,c) setBKAttrOf(w,c)

/*
 * This draws the widget on the screen.
 */
#define drawCDKRadio(obj,Box) drawCDKObject(obj,Box)

/*
 * This erases the widget from the screen.
 */
#define eraseCDKRadio(obj) eraseCDKObject(obj)

/*
 * This moves the widget to the given screen location.
 */
#define moveCDKRadio(obj,xpos,ypos,relative,refresh) moveCDKObject(obj,xpos,ypos,relative,refresh)

/*
 * This interactively moves the widget to a new location on the screen.
 */
#define positionCDKRadio(widget) positionCDKObject(ObjOf(widget),widget->win)

/*
 * This destroys a widget pointer.
 */
#define destroyCDKRadio(obj) destroyCDKObject(obj)

/*
 * These set the pre/post process callback functions.
 */
#define setCDKRadioPreProcess(w,f,d)  setCDKObjectPreProcess(ObjOf(w),f,d)
#define setCDKRadioPostProcess(w,f,d) setCDKObjectPostProcess(ObjOf(w),f,d)

#ifdef __cplusplus
}
#endif

#endif /* CDKRADIO_H */
#endif /* CDKINCLUDES */
