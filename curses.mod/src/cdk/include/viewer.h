/*
 * $Id: viewer.h,v 1.22 2012/03/21 21:15:30 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKVIEWER_H
#define CDKVIEWER_H	1

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
 * Changes 1999-2004,2012 copyright Thomas E. Dickey
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
 * Define the CDK viewer widget structure.
 */
struct SViewer {
   CDKOBJS	obj;
   WINDOW *	parent;
   WINDOW *	win;
   WINDOW *	shadowWin;
   chtype **	button;
   int *	buttonLen;
   int *	buttonPos;
   int		buttonCount;
   chtype	buttonHighlight;
   chtype **	list;
   int *	listLen;
   int *	listPos;
   int		titleAdj;
   int		listSize;
   int		boxHeight;
   int		boxWidth;
   int		viewSize;
   int		currentTop;
   int		currentButton;
   int		leftChar;
   int		length;
   int		maxLeftChar;
   int		maxTopLine;
   int		widestLine;
   long		characters;
   boolean	inProgress;
   boolean	showLineInfo;
   boolean	interpret;
   EExitType	exitType;
   boolean	shadow;
};
typedef struct SViewer CDKVIEWER;

/*
 * This creates a pointer to a new CDK viewer widget.
 */
CDKVIEWER *newCDKViewer (
		CDKSCREEN *	/* cdkscreen */,
		int		/* xpos */,
		int		/* ypos */,
		int		/* height */,
		int		/* width */,
		CDK_CSTRING2	/* buttons */,
		int		/* buttonCount */,
		chtype		/* buttonHighlight */,
		boolean		/* Box */,
		boolean		/* shadow */);

/*
 * This activates the viewer widget.
 */
int activateCDKViewer (
		CDKVIEWER *	/* viewer */,
		chtype *	/* actions */);

/*
 * This sets various attributes of the viewer widget.
 */
int setCDKViewer (
		CDKVIEWER *	/* viewer */,
		const char *	/* title */,
		CDK_CSTRING2	/* info */,
		int		/* infoSize */,
		chtype		/* buttonHighlight */,
		boolean		/* interpret */,
		boolean		/* showLineInfo */,
		boolean		/* Box */);

/*
 * This sets the contents of the viewer widget.
 */
int setCDKViewerInfo (
		CDKVIEWER *	/* viewer */,
		CDK_CSTRING2	/* info */,
		int		/* infoSize */,
		boolean		/* interpret */);

chtype **getCDKViewerInfo (
		CDKVIEWER *	/* viewer */,
		int *		/* size */);

/*
 * This sets the title of the viewer widget.
 */
void setCDKViewerTitle (
		CDKVIEWER *	/* viewer */,
		const char *	/* title */);

chtype **getCDKViewerTitle (
		CDKVIEWER *	/* viewer */);

/*
 * This sets the highlight bar attribute.
 */
void setCDKViewerHighlight (
		CDKVIEWER *	/* viewer */,
		chtype		/* buttonHighlight */);

chtype getCDKViewerHighlight (
		CDKVIEWER *	/* viewer */);

/*
 * This sets the boolean flag dictating whether a information line
 * will be displayed in the top left corner of the viewer.
 */
void setCDKViewerInfoLine (
		CDKVIEWER *	/* viewer */,
		boolean		/* showLineInfo */);

boolean getCDKViewerInfoLine (
		CDKVIEWER *	/* viewer */);

/*
 * This sets the box attribute of the widget.
 */
void setCDKViewerBox (
		CDKVIEWER *	/* viewer */,
		boolean		/* Box */);

boolean getCDKViewerBox (
		CDKVIEWER *	/* viewer */);

/*
 * These set the drawing characters of the widget.
 */
#define setCDKViewerULChar(w,c)            setULCharOf(w,c)
#define setCDKViewerURChar(w,c)            setURCharOf(w,c)
#define setCDKViewerLLChar(w,c)            setLLCharOf(w,c)
#define setCDKViewerLRChar(w,c)            setLRCharOf(w,c)
#define setCDKViewerVerticalChar(w,c)      setVTCharOf(w,c)
#define setCDKViewerHorizontalChar(w,c)    setHZCharOf(w,c)
#define setCDKViewerBoxAttribute(w,c)      setBXAttrOf(w,c)

/*
 * This sets the background color of the widget.
 */
#define setCDKViewerBackgroundColor(w,c) setCDKObjectBackgroundColor(ObjOf(w),c)

/*
 * This sets the background attribute of the widget.
 */ 
#define setCDKViewerBackgroundAttrib(w,c) setBKAttrOf(w,c)

/*
 * This draws the viewer field on the screen.
 */
#define drawCDKViewer(obj,box) drawCDKObject(obj,box)

/*
 * This erases the widget from the screen.
 */
#define eraseCDKViewer(obj) eraseCDKObject(obj)

/*
 * This cleans out all of the information from the viewer.
 */
void cleanCDKViewer (
		CDKVIEWER *	/* viewer */);

/*
 * This moves the widget to the given location.
 */
#define moveCDKViewer(obj,xpos,ypos,relative,refresh) moveCDKObject(obj,xpos,ypos,relative,refresh)

/*
 * This is an interactive method of moving the widget.
 */
#define positionCDKViewer(widget) positionCDKObject(ObjOf(widget),widget->win)

/*
 * This destroys the widget and all the memory associated with the widget.
 */
#define destroyCDKViewer(obj) destroyCDKObject(obj)

#ifdef __cplusplus
}
#endif

#endif /* CDKVIEWER_H */
#endif /* CDKINCLUDES */
