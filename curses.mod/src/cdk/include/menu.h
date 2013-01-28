/*
 * $Id: menu.h,v 1.22 2012/03/21 21:15:30 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKMENU_H
#define CDKMENU_H	1

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
 * 	This product includes software developed by Mike Glover
 * 	and contributors.
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
 * Define menu specific values.
 */
#define MAX_MENU_ITEMS	30
#define MAX_SUB_ITEMS	98

/*
 * Define the CDK menu widget structure.
 */
struct SMenu {
   CDKOBJS	obj;
   WINDOW *	parent;
   WINDOW *	pullWin[MAX_MENU_ITEMS];
   WINDOW *	titleWin[MAX_MENU_ITEMS];
   chtype *	title[MAX_MENU_ITEMS];
   int		titleLen[MAX_MENU_ITEMS];
   chtype *	sublist[MAX_MENU_ITEMS][MAX_SUB_ITEMS];
   int		sublistLen[MAX_MENU_ITEMS][MAX_SUB_ITEMS];
   int		subsize[MAX_MENU_ITEMS];
   int		menuPos;
   int		menuItems;
   chtype	titleAttr;
   chtype	subtitleAttr;
   int		currentTitle;
   int		currentSubtitle;
   int		lastTitle;
   int		lastSubtitle;
   EExitType	exitType;
   int		lastSelection;
};
typedef struct SMenu CDKMENU;

/*
 * This creates a new CDK menu widget pointer.
 */
CDKMENU *newCDKMenu (
		CDKSCREEN *	/* cdkscreen */,
		const char *	/* menulist */ [MAX_MENU_ITEMS][MAX_SUB_ITEMS],
		int 		/* menuitems */,
		int *		/* subsize */,
		int *		/* menuloc */,
		int		/* menuPos */,
		chtype 		/* titleattr */,
		chtype 		/* subtitleattr */);

/*
 * This activates the menu.
 */
int activateCDKMenu (
		CDKMENU *	/* menu */,
		chtype *	/* actions */);

/*
 * This injects a single character into the menu widget.
 */
#define injectCDKMenu(obj,input) injectCDKObject(obj,input,Int)

/*
 * These set specific attributes of the menu.
 */
void setCDKMenu (
		CDKMENU *	/* menu */,
		int 		/* menuItem */,
		int 		/* subMenuItem */,
		chtype 		/* titleHighlight */,
		chtype 		/* subTitleHighlight */);

/*
 * This returns the current item the menu is on.
 */
void setCDKMenuCurrentItem (
		CDKMENU *	/* menu */,
		int 		/* menuItem */,
		int 		/* subMenuItem */);

void getCDKMenuCurrentItem (
		CDKMENU *	/* menu */,
		int *		/* menuItem */,
		int *		/* subMenuItem */);

/*
 * This sets the highlight of the title.
 */
void setCDKMenuTitleHighlight (
		CDKMENU *	/* menu */,
		chtype 		/* highlight */);

chtype getCDKMenuTitleHighlight (
		CDKMENU *	/* menu */);

/*
 * This sets the sub-menu title highlight.
 */
void setCDKMenuSubTitleHighlight (
		CDKMENU *	/* menu */,
		chtype 		/* highlight */);

chtype getCDKMenuSubTitleHighlight (
		CDKMENU *	/* menu */);

/*
 * This draws the menu on the screen.
 */
#define drawCDKMenu(obj,box) drawCDKObject(obj,box)

void drawCDKMenuSubwin (
		CDKMENU *	/* menu */);

/*
 * This erases the complere menu widget from the screen.
 */
#define eraseCDKMenu(obj) eraseCDKObject(obj)

void eraseCDKMenuSubwin (
		CDKMENU *	/* menu */);

/*
 * This sets the background color of the widget.
 */
#define setCDKMenuBackgroundColor(w,c) setCDKObjectBackgroundColor(ObjOf(w),c)

/*
 * This sets the background attribute of the widget.
 */ 
#define setCDKMenuBackgroundAttrib(w,c) setBKAttrOf(w,c)

/*
 * This destroys the menu widget.
 */
#define destroyCDKMenu(obj) destroyCDKObject(obj)

/*
 * These set the pre/post process callback functions.
 */
#define setCDKMenuPreProcess(w,f,d)  setCDKObjectPreProcess(ObjOf(w),f,d)
#define setCDKMenuPostProcess(w,f,d) setCDKObjectPostProcess(ObjOf(w),f,d)

#ifdef __cplusplus
}
#endif

#endif /* CDKMENU_H */
#endif /* CDKINCLUDES */
