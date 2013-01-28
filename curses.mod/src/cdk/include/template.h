/*
 * $Id: template.h,v 1.24 2012/03/21 21:15:30 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKTEMPLATE_H
#define CDKTEMPLATE_H	1

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
 * Define the CDK cdktemplate widget structure.
 */
typedef struct STemplate CDKTEMPLATE;
typedef void (*TEMPLATECB) (CDKTEMPLATE *cdktemplate, chtype input);

struct STemplate {
   CDKOBJS	obj;
   WINDOW *	parent;
   WINDOW *	win;
   WINDOW *	shadowWin;
   WINDOW *	labelWin;
   WINDOW *	fieldWin;
   int		titleAdj;
   chtype *	label;
   chtype *	overlay;
   chtype	overlayAttr;
   char *	plate;
   char *	info;
   int		labelLen;
   int		overlayLen;
   int		labelPos;
   int		fieldWidth;
   int		boxWidth;
   int		boxHeight;
   int		platePos;
   int		plateLen;
   int		screenPos;
   int		infoPos;
   int		min;
   chtype	labelAttr;
   chtype	fieldAttr;
   EExitType	exitType;
   boolean	shadow;
   TEMPLATECB	callbackfn;
};

/*
 * This creates a pointer to a new CDK cdktemplate widget.
 */
CDKTEMPLATE *newCDKTemplate (
		CDKSCREEN *	/* cdkscreen */,
		int 		/* xpos */,
		int 		/* ypos */,
		const char *	/* title */,
		const char *	/* label */,
		const char *	/* plate */,
		const char *	/* overlay */,
		boolean 	/* Box */,
		boolean 	/* shadow */);

/*
 * This activates the cdktemplate widget.
 */
char *activateCDKTemplate (
		CDKTEMPLATE *	/* cdktemplate */,
		chtype *	/* actions */);

/*
 * This injects a single character into the widget.
 */
#define injectCDKTemplate(obj,input) injectCDKObject(obj,input,String)

/*
 * This sets various attributes of the widget.
 */
void setCDKTemplate (
		CDKTEMPLATE *	/* cdktemplate */,
		const char *	/* value */,
		boolean 	/* Box */);

/*
 * This sets the value in the cdktemplate widget.
 */
void setCDKTemplateValue (
		CDKTEMPLATE *	/* cdktemplate */,
		const char *	/* value */);

char *getCDKTemplateValue (
		CDKTEMPLATE *	/* cdktemplate */);

/*
 * This sets the minimum number of characters to enter.
 */
void setCDKTemplateMin (
		CDKTEMPLATE *	/* cdktemplate */,
		int 		/* min */);

int getCDKTemplateMin (
		CDKTEMPLATE *	/* cdktemplate */);

/*
 * This sets the box attribute of the widget.
 */
void setCDKTemplateBox (
		CDKTEMPLATE *	/* cdktemplate */,
		boolean 	/* Box */);

boolean getCDKTemplateBox (
		CDKTEMPLATE *	/* cdktemplate */);

/*
 * These set the drawing characters of the widget.
 */
#define setCDKTemplateULChar(w,c)          setULCharOf(w,c)
#define setCDKTemplateURChar(w,c)          setURCharOf(w,c)
#define setCDKTemplateLLChar(w,c)          setLLCharOf(w,c)
#define setCDKTemplateLRChar(w,c)          setLRCharOf(w,c)
#define setCDKTemplateVerticalChar(w,c)    setVTCharOf(w,c)
#define setCDKTemplateHorizontalChar(w,c)  setHZCharOf(w,c)
#define setCDKTemplateBoxAttribute(w,c)    setBXAttrOf(w,c)

/*
 * This sets the background color of the widget.
 */
#define setCDKTemplateBackgroundColor(w,c) setCDKObjectBackgroundColor(ObjOf(w),c)

/*
 * This sets the background attribute of the widget.
 */ 
#define setCDKTemplateBackgroundAttrib(w,c) setBKAttrOf(w,c)

/*
 * This draws the cdktemplate on the screen.
 */
#define drawCDKTemplate(obj,Box) drawCDKObject(obj,Box)

/*
 * This erases the widget from the screen.
 */
#define eraseCDKTemplate(obj) eraseCDKObject(obj)

/*
 * This erases the cdktemplates contents.
 */
void cleanCDKTemplate (
		CDKTEMPLATE *	/* cdktemplate */);

/*
 * This moves the widget to the given location on the screen.
 */
#define moveCDKTemplate(obj,xpos,ypos,relative,refresh) moveCDKObject(obj,xpos,ypos,relative,refresh)

/*
 * This interactively positions the widget on the screen.
 */
#define positionCDKTemplate(widget) positionCDKObject(ObjOf(widget),widget->win)

/*
 * This destroys the widget and all associated memory.
 */
#define destroyCDKTemplate(obj) destroyCDKObject(obj)

/*
 * This sets the main callback function.
 */
void setCDKTemplateCB (
		CDKTEMPLATE *	/* cdktemplate */,
		TEMPLATECB 	/* callback */);

/*
 * This returns a character pointer to the contents of the cdktemplate
 * mixed with the plate.
 */
char *mixCDKTemplate (
		CDKTEMPLATE *	/* cdktemplate */);

/*
 * This returns a character pointer to the cdktemplate with the plate
 * stripped out.
 */
char *unmixCDKTemplate (
		CDKTEMPLATE *	/* cdktemplate */,
		const char *	/* string */);

/*
 * These set the pre/post callback functions.
 */
#define setCDKTemplatePreProcess(w,f,d)  setCDKObjectPreProcess(ObjOf(w),f,d)
#define setCDKTemplatePostProcess(w,f,d) setCDKObjectPostProcess(ObjOf(w),f,d)

#ifdef __cplusplus
}
#endif

#endif /* CDKTEMPLATE_H */
#endif /* CDKINCLUDES */
