/*
 * $Id: buttonbox.h,v 1.27 2012/03/21 21:15:30 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKBUTTONBOX_H
#define CDKBUTTONBOX_H	1

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
 * Changes 1999-2005,2012 copyright Thomas E. Dickey
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
 * Define the CDK buttonbox structure.
 */
struct SButtonBox {
   CDKOBJS	obj;
   WINDOW *	parent;
   WINDOW *	win;
   WINDOW *	shadowWin;
   int		titleAdj;
   chtype **	button;
   int *	buttonLen;
   int *	buttonPos;
   int *	columnWidths;
   int		buttonCount;
   int		buttonWidth;
   int		currentButton;
   int		rows;
   int		cols;
   int		colAdjust;
   int		rowAdjust;
   int		boxWidth;
   int		boxHeight;
   chtype	ButtonAttrib;
   EExitType	exitType;
   boolean	shadow;
   chtype	highlight;
};
typedef struct SButtonBox CDKBUTTONBOX;

/*
 * This returns a CDK buttonbox widget pointer.
 */
CDKBUTTONBOX *newCDKButtonbox (
		CDKSCREEN *	/* cdkscreen */,
		int		/* xPos */,
		int		/* yPos */,
		int		/* height */,
		int		/* width */,
		const char *	/* title */,
		int		/* rows */,
		int		/* cols */,
		CDK_CSTRING2	/* buttons */,
		int		/* buttonCount */,
		chtype		/* highlight */,
		boolean		/* Box */,
		boolean		/* shadow */);

/*
 * This activates the widget.
 */
int activateCDKButtonbox (
		CDKBUTTONBOX *	/* buttonbox */,
		chtype *	/* actions */);

/*
 * This injects a single character into the widget.
 */
#define injectCDKButtonbox(obj,input) injectCDKObject(obj,input,Int)

/*
 * This sets multiple attributes of the widget.
 */
void setCDKButtonbox (
		CDKBUTTONBOX *	/* buttonbox */,
		chtype		/* highlight */,
		boolean		/* Box */);

void setCDKButtonboxCurrentButton (
		CDKBUTTONBOX *	/* buttonbox */,
		int		/* button */);

int getCDKButtonboxCurrentButton (
		CDKBUTTONBOX *	/* buttonbox */);

int getCDKButtonboxButtonCount (
		CDKBUTTONBOX *	/* buttonbox */);

/*
 * This sets the highlight attribute for the buttonbox.
 */
void setCDKButtonboxHighlight (
		CDKBUTTONBOX *	/* buttonbox */,
		chtype		/* highlight */);

chtype getCDKButtonboxHighlight (
		CDKBUTTONBOX *	/* buttonbox */);

/*
 * This sets the box attribute of the widget.
 */
void setCDKButtonboxBox (
		CDKBUTTONBOX *	/* buttonbox */,
		boolean		/* Box */);

boolean getCDKButtonboxBox (
		CDKBUTTONBOX *	/* buttonbox */);

/*
 * These set the drawing characters of the widget.
 */
#define setCDKButtonboxULChar(w,c)         setULCharOf(w,c)
#define setCDKButtonboxURChar(w,c)         setURCharOf(w,c)
#define setCDKButtonboxLLChar(w,c)         setLLCharOf(w,c)
#define setCDKButtonboxLRChar(w,c)         setLRCharOf(w,c)
#define setCDKButtonboxVerticalChar(w,c)   setVTCharOf(w,c)
#define setCDKButtonboxHorizontalChar(w,c) setHZCharOf(w,c)
#define setCDKButtonboxBoxAttribute(w,c)   setBXAttrOf(w,c)

/*
 * This sets the background color of the widget.
 */
#define setCDKButtonboxBackgroundColor(w,c) setCDKObjectBackgroundColor(ObjOf(w),c)

/*
 * This sets the background attribute of the widget.
 */ 
#define setCDKButtonboxBackgroundAttrib(w,c) setBKAttrOf(w,c)

/*
 * This draws the buttonbox box widget.
 */
#define drawCDKButtonbox(obj,box) drawCDKObject(obj,box)

void drawCDKButtonboxButtons (
		CDKBUTTONBOX *	/* buttonbox */);

/*
 * This erases the buttonbox box from the screen.
 */
#define eraseCDKButtonbox(obj) eraseCDKObject(obj)

/*
 * This moves the buttonbox box to a new screen location.
 */
#define moveCDKButtonbox(obj,xpos,ypos,relative,refresh) moveCDKObject(obj,xpos,ypos,relative,refresh)

/*
 * This allows the user to position the widget on the screen interactively.
 */
#define positionCDKButtonbox(widget) positionCDKObject(ObjOf(widget),widget->win)

/*
 * This destroys the widget and all the memory associated with it.
 */
#define destroyCDKButtonbox(obj) destroyCDKObject(obj)

/*
 * This redraws the buttonbox box buttonboxs.
 */
void redrawCDKButtonboxButtonboxs (
		CDKBUTTONBOX *	/* buttonbox */);

/*
 * These set the pre/post process functions of the buttonbox widget.
 */
#define setCDKButtonboxPreProcess(w,f,d)  setCDKObjectPreProcess(ObjOf(w),f,d)
#define setCDKButtonboxPostProcess(w,f,d) setCDKObjectPostProcess(ObjOf(w),f,d)

#ifdef __cplusplus
}
#endif

#endif /* CDKBUTTONBOX_H */
#endif /* CDKINCLUDES */
