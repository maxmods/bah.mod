/*
 * $Id: dialog.h,v 1.26 2012/03/21 21:15:30 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKDIALOG_H
#define CDKDIALOG_H	1

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
 * Changes 1999-2003,2012 copyright Thomas E. Dickey
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
 * Declare definitions the dialog box may need.
 */
#define MAX_DIALOG_ROWS		50	/* unused by widgets */
#define MAX_DIALOG_BUTTONS	25	/* unused by widgets */
#define MIN_DIALOG_WIDTH	10

/*
 * Define the CDK dialog structure.
 */
struct SDialogBox {
   CDKOBJS	obj;
   WINDOW *	parent;
   WINDOW *	win;
   WINDOW *	shadowWin;
   chtype **	info;
   int *	infoLen;
   int *	infoPos;
   chtype **	buttonLabel;
   int *	buttonLen;
   int *	buttonPos;
   int		messageRows;
   int		buttonCount;
   int		currentButton;
   int		boxWidth;
   int		boxHeight;
   EExitType	exitType;
   boolean	separator;
   boolean	shadow;
   chtype	highlight;
};
typedef struct SDialogBox CDKDIALOG;

/*
 * This returns a CDK dialog widget pointer.
 */
CDKDIALOG *newCDKDialog (
		CDKSCREEN *	/* cdkscreen */,
		int		/* xPos */,
		int		/* yPos */,
		CDK_CSTRING2	/* message */,
		int		/* Rows */,
		CDK_CSTRING2	/* buttons */,
		int		/* buttonCount */,
		chtype		/* highlight */,
		boolean		/* separator */,
		boolean		/* Box */,
		boolean		/* shadow */);

/*
 * This activates the widget.
 */
int activateCDKDialog (
		CDKDIALOG *	/* dialog */,
		chtype *	/* actions */);

/*
 * This injects a single character into the widget.
 */
#define injectCDKDialog(obj,input) injectCDKObject(obj,input,Int)

/*
 * This sets multiple attributes of the widget.
 */
void setCDKDialog (
		CDKDIALOG *	/* dialog */,
		chtype		/* highlight */,
		boolean		/* separator */,
		boolean		/* Box */);

/*
 * This sets the highlight attribute for the buttons.
 */
void setCDKDialogHighlight (
		CDKDIALOG *	/* dialog */,
		chtype		/* highlight */);

chtype getCDKDialogHighlight (
		CDKDIALOG *	/* dialog */);

/*
 * This sets whether or not the dialog box will have a separator line.
 */
void setCDKDialogSeparator (
		CDKDIALOG *	/* dialog */,
		boolean		/* separator */);

boolean getCDKDialogSeparator (
		CDKDIALOG *	/* dialog */);

/*
 * This sets the box attribute of the widget.
 */
void setCDKDialogBox (
		CDKDIALOG *	/* dialog */,
		boolean		/* Box */);

boolean getCDKDialogBox (
		CDKDIALOG *	/* dialog */);

/*
 * These set the drawing characters of the widget.
 */
#define setCDKDialogULChar(w,c)            setULCharOf(w,c)
#define setCDKDialogURChar(w,c)            setURCharOf(w,c)
#define setCDKDialogLLChar(w,c)            setLLCharOf(w,c)
#define setCDKDialogLRChar(w,c)            setLRCharOf(w,c)
#define setCDKDialogVerticalChar(w,c)      setVTCharOf(w,c)
#define setCDKDialogHorizontalChar(w,c)    setHZCharOf(w,c)
#define setCDKDialogBoxAttribute(w,c)      setBXAttrOf(w,c)

/*
 * This sets the background color of the widget.
 */
#define setCDKDialogBackgroundColor(w,c) setCDKObjectBackgroundColor(ObjOf(w),c)

/*
 * This sets the background attribute of the widget.
 */ 
#define setCDKDialogBackgroundAttrib(w,c) setBKAttrOf(w,c)

/*
 * This draws the dialog box widget.
 */
#define drawCDKDialog(obj,box) drawCDKObject(obj,box)

/*
 * This erases the dialog box from the screen.
 */
#define eraseCDKDialog(obj) eraseCDKObject(obj)

/*
 * This moves the dialog box to a new screen location.
 */
#define moveCDKDialog(obj,xpos,ypos,relative,refresh) moveCDKObject(obj,xpos,ypos,relative,refresh)

/*
 * This allows the user to position the widget on the screen interactively.
 */
#define positionCDKDialog(widget) positionCDKObject(ObjOf(widget),widget->win)

/*
 * This destroys the widget and all the memory associated with it.
 */
#define destroyCDKDialog(obj) destroyCDKObject(obj)

/*
 * This draws the dialog box buttons.
 */
void drawCDKDialogButtons (
		CDKDIALOG *	/* dialog */);

/*
 * These set the pre/post process functions of the dialog widget.
 */
#define setCDKDialogPreProcess(w,f,d)  setCDKObjectPreProcess(ObjOf(w),f,d)
#define setCDKDialogPostProcess(w,f,d) setCDKObjectPostProcess(ObjOf(w),f,d)

#ifdef __cplusplus
}
#endif

#endif /* CDKDIALOG_H */
#endif /* CDKINCLUDES */
