/*
 * $Id: gen-slider.h,v 1.7 2004/08/30 00:00:57 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKSLIDER_H
#define CDKSLIDER_H	1

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
 * Define the CDK widget Slider structure.
 */
struct SSlider {
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
   int	low;
   int	high;
   int	inc;
   int	fastinc;
   int	current;
   EExitType	exitType;
   boolean	shadow;
};
typedef struct SSlider CDKSLIDER;

/*
 * This creates a new pointer to a CDK int-Slider widget.
 */
CDKSLIDER *newCDKSlider (
		CDKSCREEN *	/* cdkscreen */,
		int		/* xpos */,
		int		/* ypos */,
		char *		/* title */,
		char *		/* label */,
		chtype		/* fieldAttr */,
		int		/* fieldWidth */,
		int		/* start */,
		int		/* low */,
		int		/* high */,
		int		/* inc */,
		int		/* fastInc */,
		boolean		/* Box */,
		boolean		/* shadow */);

/*
 * This activates the widget.
 */
int activateCDKSlider (
		CDKSLIDER *	/* widget */,
		chtype *	/* actions */);

/*
 * This injects a single character into the widget.
 */
#define injectCDKSlider(obj,input) injectCDKObject(obj,input,Int)

/*
 * This sets various attributes of the widget.
 */
void setCDKSlider (
		CDKSLIDER *	/* widget */,
		int		/* low */,
		int		/* high */,
		int		/* value */,
		boolean		/* Box */);

/*
 * These set/get the low and high values.
 */
void setCDKSliderLowHigh (
		CDKSLIDER *	/* widget */,
		int		/* low */,
		int		/* high */);

int getCDKSliderLowValue (
		CDKSLIDER *	/* widget */);

int getCDKSliderHighValue (
		CDKSLIDER *	/* widget */);

/*
 * These set/get the digits.
 */

/*
 * These set/get the current value.
 */
void setCDKSliderValue (
		CDKSLIDER *	/* widget */,
		int		/* value */);

int getCDKSliderValue (
		CDKSLIDER *	/* widget */);

/*
 * This sets the box attribute of the widget.
 */
void setCDKSliderBox (
		CDKSLIDER *	/* widget */,
		boolean		/* Box */);

boolean getCDKSliderBox (
		CDKSLIDER *	/* widget */);

/*
 * These set the drawing characters of the widget.
 */
#define setCDKSliderULChar(w,c)            setULCharOf(w,c)
#define setCDKSliderURChar(w,c)            setURCharOf(w,c)
#define setCDKSliderLLChar(w,c)            setLLCharOf(w,c)
#define setCDKSliderLRChar(w,c)            setLRCharOf(w,c)
#define setCDKSliderVerticalChar(w,c)      setVTCharOf(w,c)
#define setCDKSliderHorizontalChar(w,c)    setHZCharOf(w,c)
#define setCDKSliderBoxAttribute(w,c)      setBXAttrOf(w,c)

/*
 * This sets the background color of the widget.
 */
#define setCDKSliderBackgroundColor(w,c) setCDKObjectBackgroundColor(ObjOf(w),c)

/*
 * This sets the background attribute of the widget.
 */ 
#define setCDKSliderBackgroundAttrib(w,c) setBKAttrOf(w,c)

/*
 * This draws the widget on the screen.
 */
#define drawCDKSlider(obj,Box) drawCDKObject(obj,Box)

/*
 * This erases the widget from the screen.
 */
#define eraseCDKSlider(obj) eraseCDKObject(obj)

/*
 * This moves the widget to the given location on the screen.
 */
#define moveCDKSlider(obj,xpos,ypos,relative,refresh) moveCDKObject(obj,xpos,ypos,relative,refresh)

/*
 * This allows the user to interactively position the widget on the screen.
 */
#define positionCDKSlider(widget) positionCDKObject(ObjOf(widget),widget->win)

/*
 * This destroys the widget and associated memory.
 */
#define destroyCDKSlider(obj) destroyCDKObject(obj)

/*
 * These set the pre/post process callback functions.
 */
#define setCDKSliderPreProcess(w,f,d)  setCDKObjectPreProcess(ObjOf(w),f,d)
#define setCDKSliderPostProcess(w,f,d) setCDKObjectPostProcess(ObjOf(w),f,d)

#ifdef __cplusplus
}
#endif

#endif /* CDKSLIDER_H */
#endif /* CDKINCLUDES */
