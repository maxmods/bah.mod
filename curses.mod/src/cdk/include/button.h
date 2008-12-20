/*
 * $Id: button.h,v 1.10 2004/08/30 00:00:57 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKBUTTON_H
#define CDKBUTTON_H	1

#include "cdk.h"

#ifndef CDK_H
#define CDKINCLUDES
#include <cdk.h>
#undef CDKINCLUDES
#include <binding.h>
#include <cdkscreen.h>
#include <cdk_objs.h>
#endif

/*
 * Changes 2002,2003 copyright Thomas E. Dickey
 *
 * Copyright 1999, Grant Edwards
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
 *      This product includes software developed by Grant Edwards
 *      and contributors.
 * 4. Neither the name of Grant Edwards, nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GRANT EDWARDS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL GRANT EDWARDS OR CONTRIBUTORS BE LIABLE
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

/*
 * Declare the CDK label structure.
 */
struct SButton {
   CDKOBJS	obj;
   WINDOW *	parent;
   WINDOW *	win;
   WINDOW *	shadowWin;
   chtype *	info;
   void         (*callback)(struct SButton *button);
   int		infoLen;
   int		infoPos;
   int		boxWidth;
   int		boxHeight;
   int		xpos;
   int		ypos;
   int		rows;
   EExitType	exitType;
   boolean	shadow;
};

typedef struct SButton CDKBUTTON;

typedef void (*tButtonCallback)(struct SButton *button);

/*
 * This creates a new CDK button widget.
 */
CDKBUTTON *newCDKButton (
		CDKSCREEN *	/* screen */,
		int		/* xPos */,
		int		/* yPos */,
		char *		/* button text */,
		tButtonCallback /* callback function */,
		boolean		/* Box */,
		boolean		/* shadow */);

/*
 * This was added to make the builder simpler. All this will
 * do is call drawCDKLabel.
 */
int activateCDKButton (
		CDKBUTTON *	/* button */,
		chtype *	/* actions */);

/*
 * This injects a single character into the widget.
 */
#define injectCDKButton(obj,input) injectCDKObject(obj,input,Int)

/*
 * This sets multiple attributes of the widget.
 */
void setCDKButton (
		CDKBUTTON *	/* button */,
		char *		/* text */,
		boolean		/* Box */);

/*
 * This sets the contents of the label.
 */
void setCDKButtonMessage (
		CDKBUTTON *	/* button */,
		char *		/* mesg */);

chtype *getCDKButtonMessage (
		CDKBUTTON *);

/*
 * This sets the box attribute of the widget.
 */
void setCDKButtonBox (
		CDKBUTTON *	/* button */,
		boolean		/* Box */);

boolean getCDKButtonBox (
		CDKBUTTON *	/* button */);

/*
 * This draws the label.
 */
#define drawCDKButton(obj,Box) drawCDKObject(obj,Box)

/*
 * These set the drawing characters of the widget.
 */
#define setCDKButtonULChar(w,c)            setULCharOf(w,c)
#define setCDKButtonURChar(w,c)            setURCharOf(w,c)
#define setCDKButtonLLChar(w,c)            setLLCharOf(w,c)
#define setCDKButtonLRChar(w,c)            setLRCharOf(w,c)
#define setCDKButtonVerticalChar(w,c)      setVTCharOf(w,c)
#define setCDKButtonHorizontalChar(w,c)    setHZCharOf(w,c)
#define setCDKButtonBoxAttribute(w,c)      setBXAttrOf(w,c)

/*
 * This sets the background color of the widget.
 */
#define setCDKButtonBackgroundColor(w,c) setCDKObjectBackgroundColor(ObjOf(w),c)

/*
 * This sets the background attribute of the widget.
 */ 
#define setCDKButtonBackgroundAttrib(w,c) setBKAttrOf(w,c)

/*
 * This erases the button.
 */
#define eraseCDKButton(obj) eraseCDKObject(obj)

/*
 * This destroys the button and the memory used by it.
 */
#define destroyCDKButton(obj) destroyCDKObject(obj)

/*
 * This moves the button to a new screen location.
 */
#define moveCDKButton(obj,xpos,ypos,relative,refresh) moveCDKObject(obj,xpos,ypos,relative,refresh)

/*
 * This allows the user to interactively position the button.
 */
void positionCDKButton (CDKBUTTON *);

#endif /* CDKBUTTON_H */
#endif /* CDKINCLUDES */
