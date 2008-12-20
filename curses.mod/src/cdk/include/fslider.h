/*
 * $Id: gen-slider.h,v 1.7 2004/08/30 00:00:57 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKFSLIDER_H
#define CDKFSLIDER_H	1

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
 * Copyright 2004, Thomas E. Dickey
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
 *      This product includes software developed by Thomas Dickey
 *      and contributors.
 * 4. Neither the name of Thomas Dickey, nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THOMAS DICKEY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THOMAS DICKEY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Define the CDK widget FSlider structure.
 */
struct SFSlider {
   CDKOBJS	obj;
   WINDOW *	parent;
   WINDOW *	win;
   WINDOW *	shadowWin;
   WINDOW *	fieldWin;
   WINDOW *	labelWin;
   int		titleAdj;
   chtype *	label;
   int		labelLen;
   int		boxHeight;
   int		boxWidth;
   int		fieldWidth;
   int		fieldEdit;	/* offset from right-margin of field */
   chtype	filler;
   float	low;
   float	high;
   float	inc;
   float	fastinc;
   float	current;
   int		digits;
   EExitType	exitType;
   boolean	shadow;
};
typedef struct SFSlider CDKFSLIDER;

/*
 * This creates a new pointer to a CDK float-FSlider widget.
 */
CDKFSLIDER *newCDKFSlider (
		CDKSCREEN *	/* cdkscreen */,
		int		/* xpos */,
		int		/* ypos */,
		char *		/* title */,
		char *		/* label */,
		chtype		/* fieldAttr */,
		int		/* fieldWidth */,
		float		/* start */,
		float		/* low */,
		float		/* high */,
		float		/* inc */,
		float		/* fastInc */,
		int		/* digits */,
		boolean		/* Box */,
		boolean		/* shadow */);

/*
 * This activates the widget.
 */
float activateCDKFSlider (
		CDKFSLIDER *	/* widget */,
		chtype *	/* actions */);

/*
 * This injects a single character into the widget.
 */
#define injectCDKFSlider(obj,input) injectCDKObject(obj,input,Float)

/*
 * This sets various attributes of the widget.
 */
void setCDKFSlider (
		CDKFSLIDER *	/* widget */,
		float		/* low */,
		float		/* high */,
		float		/* value */,
		boolean		/* Box */);

/*
 * These set/get the low and high values.
 */
void setCDKFSliderLowHigh (
		CDKFSLIDER *	/* widget */,
		float		/* low */,
		float		/* high */);

float getCDKFSliderLowValue (
		CDKFSLIDER *	/* widget */);

float getCDKFSliderHighValue (
		CDKFSLIDER *	/* widget */);

/*
 * These set/get the digits.
 */
void setCDKFSliderDigits (
		CDKFSLIDER *	/* widget */,
		int		/* digits */);

int getCDKFSliderDigits (
		CDKFSLIDER *	/* widget */);

/*
 * These set/get the current value.
 */
void setCDKFSliderValue (
		CDKFSLIDER *	/* widget */,
		float		/* value */);

float getCDKFSliderValue (
		CDKFSLIDER *	/* widget */);

/*
 * This sets the box attribute of the widget.
 */
void setCDKFSliderBox (
		CDKFSLIDER *	/* widget */,
		boolean		/* Box */);

boolean getCDKFSliderBox (
		CDKFSLIDER *	/* widget */);

/*
 * These set the drawing characters of the widget.
 */
#define setCDKFSliderULChar(w,c)            setULCharOf(w,c)
#define setCDKFSliderURChar(w,c)            setURCharOf(w,c)
#define setCDKFSliderLLChar(w,c)            setLLCharOf(w,c)
#define setCDKFSliderLRChar(w,c)            setLRCharOf(w,c)
#define setCDKFSliderVerticalChar(w,c)      setVTCharOf(w,c)
#define setCDKFSliderHorizontalChar(w,c)    setHZCharOf(w,c)
#define setCDKFSliderBoxAttribute(w,c)      setBXAttrOf(w,c)

/*
 * This sets the background color of the widget.
 */
#define setCDKFSliderBackgroundColor(w,c) setCDKObjectBackgroundColor(ObjOf(w),c)

/*
 * This sets the background attribute of the widget.
 */ 
#define setCDKFSliderBackgroundAttrib(w,c) setBKAttrOf(w,c)

/*
 * This draws the widget on the screen.
 */
#define drawCDKFSlider(obj,Box) drawCDKObject(obj,Box)

/*
 * This erases the widget from the screen.
 */
#define eraseCDKFSlider(obj) eraseCDKObject(obj)

/*
 * This moves the widget to the given location on the screen.
 */
#define moveCDKFSlider(obj,xpos,ypos,relative,refresh) moveCDKObject(obj,xpos,ypos,relative,refresh)

/*
 * This allows the user to interactively position the widget on the screen.
 */
#define positionCDKFSlider(widget) positionCDKObject(ObjOf(widget),widget->win)

/*
 * This destroys the widget and associated memory.
 */
#define destroyCDKFSlider(obj) destroyCDKObject(obj)

/*
 * These set the pre/post process callback functions.
 */
#define setCDKFSliderPreProcess(w,f,d)  setCDKObjectPreProcess(ObjOf(w),f,d)
#define setCDKFSliderPostProcess(w,f,d) setCDKObjectPostProcess(ObjOf(w),f,d)

#ifdef __cplusplus
}
#endif

#endif /* CDKFSLIDER_H */
#endif /* CDKINCLUDES */
