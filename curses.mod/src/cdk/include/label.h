/*
 * $Id: label.h,v 1.18 2004/08/30 00:00:57 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKLABEL_H
#define CDKLABEL_H	1

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
 * Changes 2000-2002,2003 copyright Thomas E. Dickey
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
 * Declare any definitions you need to make.
 */
#define MAX_LABEL_ROWS	300	/* unused by widgets */

/*
 * Declare the CDK label structure.
 */
struct SLabel {
   CDKOBJS	obj;
   WINDOW *	parent;
   WINDOW *	win;
   WINDOW *	shadowWin;
   chtype **	info;
   int *	infoLen;
   int *	infoPos;
   int		boxWidth;
   int		boxHeight;
   int		xpos;
   int		ypos;
   int		rows;
   boolean	shadow;
};
typedef struct SLabel CDKLABEL;

/*
 * This creates a new CDK label widget.
 */
CDKLABEL *newCDKLabel (
		CDKSCREEN *	/* screen */,
		int		/* xPos */,
		int		/* yPos */,
		char **		/* mesg */,
		int		/* rows */,
		boolean		/* Box */,
		boolean		/* shadow */);

/*
 * This was added to make the builder simpler. All this will
 * do is call drawCDKLabel.
 */
void activateCDKLabel (
		CDKLABEL *	/* label */,
		chtype *	/* actions */);

/*
 * This sets multiple attributes of the widget.
 */
void setCDKLabel (
		CDKLABEL *	/* label */,
		char **		/* message */,
		int		/* lines */,
		boolean		/* Box */);

/*
 * This sets the contents of the label.
 */
void setCDKLabelMessage (
		CDKLABEL *	/* label */,
		char **		/* mesg */,
		int		/* lines */);
chtype **getCDKLabelMessage (
		CDKLABEL *	/* label */,
		int *		/* size */);

/*
 * This sets the box attribute of the widget.
 */
void setCDKLabelBox (
		CDKLABEL *	/* label */,
		boolean		/* Box */);
boolean getCDKLabelBox (
		CDKLABEL *	/* label */);

/*
 * This draws the label.
 */
#define drawCDKLabel(obj,Box) drawCDKObject(obj,Box)

/*
 * These set the drawing characters of the widget.
 */
#define setCDKLabelULChar(w,c)             setULCharOf(w,c)
#define setCDKLabelURChar(w,c)             setURCharOf(w,c)
#define setCDKLabelLLChar(w,c)             setLLCharOf(w,c)
#define setCDKLabelLRChar(w,c)             setLRCharOf(w,c)
#define setCDKLabelVerticalChar(w,c)       setVTCharOf(w,c)
#define setCDKLabelHorizontalChar(w,c)     setHZCharOf(w,c)
#define setCDKLabelBoxAttribute(w,c)       setBXAttrOf(w,c)

/*
 * This sets the background color of the widget.
 */
#define setCDKLabelBackgroundColor(w,c) setCDKObjectBackgroundColor(ObjOf(w),c)

/*
 * This sets the background attribute of the widget.
 */
#define setCDKLabelBackgroundAttrib(w,c) setBKAttrOf(w,c)

/*
 * This erases the label.
 */
#define eraseCDKLabel(obj) eraseCDKObject(obj)

/*
 * This destroys the label and the memory used by it.
 */
#define destroyCDKLabel(obj) destroyCDKObject(obj)

/*
 * This draws the label then waits for the user to press
 * the key defined by the 'key' parameter.
 */
char waitCDKLabel (
		CDKLABEL *	/* label */,
		char		/* key */);

/*
 * This moves the label.
 */
#define moveCDKLabel(obj,xpos,ypos,relative,refresh) moveCDKObject(obj,xpos,ypos,relative,refresh)

/*
 * This allows the user to interactively position the label.
 */
#define positionCDKLabel(widget) positionCDKObject(ObjOf(widget),widget->win)

#ifdef __cplusplus
}
#endif

#endif /* CDKLABEL_H */
#endif /* CDKINCLUDES */
