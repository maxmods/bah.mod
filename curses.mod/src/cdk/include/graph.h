/*
 * $Id: graph.h,v 1.21 2004/08/30 00:00:57 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKGRAPH_H
#define CDKGRAPH_H	1

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
 * Define the CDK graph structure.
 */
struct SGraph {
   CDKOBJS		obj;
   WINDOW *		parent;
   WINDOW *		win;
   WINDOW *		shadowWin;
   int			titleAdj;
   chtype *		graphChar;
   boolean		shadow;
   int			boxHeight;
   int			boxWidth;
   chtype *		xtitle;
   int			xtitlePos;
   int			xtitleLen;
   int *		values;
   int			count;
   int			minx;
   int			maxx;
   int			xscale;
   int			txpos;
   chtype *		ytitle;
   int			ytitlePos;
   int			ytitleLen;
   int			yscale;
   int			typos;
   EGraphDisplayType	displayType;
};
typedef struct SGraph CDKGRAPH;

/*
 * This creates a new CDK graph pointer.
 */
CDKGRAPH *newCDKGraph (
		CDKSCREEN *	/* cdkscreen */,
		int		/* xpos */,
		int		/* ypos */,
		int		/* height */,
		int		/* width */,
		char *		/* title */,
		char *		/* xtitle */,
		char *		/* ytitle */);

/*
 * This was added to make the builder simpler. All this will
 * do is call drawCDKGraph.
 */
void activateCDKGraph (
		CDKGRAPH *	/* graph */,
		chtype *	/* actions */);

/*
 * This sets multiple attributes of the widget.
 */
int setCDKGraph (
		CDKGRAPH *	/* graph */,
		int *		/* values */,
		int		/* count */,
		char *		/* graphChar */,
		boolean		/* startAtZero */,
		EGraphDisplayType  /* displayType */);

/*
 * This sets the values of the graph widget.
 */
int setCDKGraphValues (
		CDKGRAPH *	/* graph */,
		int *		/* values */,
		int		/* count */,
		boolean		/* startAtZero */);

int setCDKGraphValue (
		CDKGRAPH *	/* graph */,
		int		/* index */,
		int		/* value */,
		boolean		/* startAtZero */);

int *getCDKGraphValues (
		CDKGRAPH *	/* graph */,
		int *		/* size */);

int getCDKGraphValue (
		CDKGRAPH *	/* graph */,
		int		/* index */);

/*
 * This sets the characters of the graph widget.
 */
int setCDKGraphCharacters (
		CDKGRAPH *	/* graph */,
		char *		/* characters */);

int setCDKGraphCharacter (
		CDKGRAPH *	/* graph */,
		int		/* index */,
		char *		/* character */);

chtype *getCDKGraphCharacters (
		CDKGRAPH *	/* graph */);

chtype getCDKGraphCharacter (
		CDKGRAPH *	/* graph */,
		int		/* index */);

/*
 * This sets the display type of the graph.
 */
void setCDKGraphDisplayType (
		CDKGRAPH *	/* graph */,
		EGraphDisplayType  /* type */);

EGraphDisplayType getCDKGraphDisplayType (
		CDKGRAPH *	/* graph */);

/*
 * This sets the box attribute of the widget.
 */
void setCDKGraphBox (
		CDKGRAPH *	/* graph */,
		boolean		/* Box */);

boolean getCDKGraphBox (
		CDKGRAPH *	/* graph */);

/*
 * These set the drawing characters of the widget.
 */
#define setCDKGraphULChar(w,c)             setULCharOf(w,c)
#define setCDKGraphURChar(w,c)             setURCharOf(w,c)
#define setCDKGraphLLChar(w,c)             setLLCharOf(w,c)
#define setCDKGraphLRChar(w,c)             setLRCharOf(w,c)
#define setCDKGraphVerticalChar(w,c)       setVTCharOf(w,c)
#define setCDKGraphHorizontalChar(w,c)     setHZCharOf(w,c)
#define setCDKGraphBoxAttribute(w,c)       setBXAttrOf(w,c)

/*
 * This sets the background color of the widget.
 */
#define setCDKGraphBackgroundColor(w,c) setCDKObjectBackgroundColor(ObjOf(w),c)

/*
 * This sets the background attribute of the widget.
 */ 
#define setCDKGraphBackgroundAttrib(w,c) setBKAttrOf(w,c)

/*
 * This draws the graph on the screen.
 */
#define drawCDKGraph(obj,Box) drawCDKObject(obj,Box)

/*
 * This removes the graph from the screen.
 */
#define eraseCDKGraph(obj) eraseCDKObject(obj)

/*
 * This moves the widget to the given location.
 */
#define moveCDKGraph(obj,xpos,ypos,relative,refresh) moveCDKObject(obj,xpos,ypos,relative,refresh)

/*
 * This allows the user to interactively move the widget.
 */
#define positionCDKGraph(widget) positionCDKObject(ObjOf(widget),widget->win)

/*
 * This destroys the graph and all the associated memory.
 */
#define destroyCDKGraph(obj) destroyCDKObject(obj)

#ifdef __cplusplus
}
#endif

#endif /* CDKGRAPH_H */
#endif /* CDKINCLUDES */
