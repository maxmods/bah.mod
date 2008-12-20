/*
 * $Id: matrix.h,v 1.28 2004/08/30 00:00:57 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKMATRIX_H
#define CDKMATRIX_H	1

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
 * Changes 1999-2003 copyright Thomas E. Dickey
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
 * Declare some matrix definitions.
 */
#define MAX_MATRIX_ROWS 1000
#define MAX_MATRIX_COLS 1000

/*
 * Define the CDK matrix widget structure.
 */
typedef struct SMatrix CDKMATRIX;
typedef void (*MATRIXCB) (CDKMATRIX *matrix, chtype input);

#define CELL_LIMIT                  MAX_MATRIX_ROWS][MAX_MATRIX_COLS

#define NEW_CDKMATRIX 1

#if NEW_CDKMATRIX
#define CELL_INDEX(matrix, row,col) (((row) * ((matrix)->cols + 1)) + (col))
#else
#define CELL_INDEX(matrix, row,col) (row)][(col)
#endif

#define MATRIX_CELL(matrix,row,col) ((matrix)->cell[CELL_INDEX(matrix, row, col)])
#define MATRIX_INFO(matrix,row,col) ((matrix)->info[CELL_INDEX(matrix, row, col)])

struct SMatrix {
   CDKOBJS	obj;
   WINDOW *	parent;
   WINDOW *	win;
   WINDOW *	shadowWin;
#if NEW_CDKMATRIX
   WINDOW **	cell;
   char **	info;
#else
   WINDOW *	cell[CELL_LIMIT];
   char *	info[CELL_LIMIT];
#endif
   int		titleAdj;
   int		rows;
   int		cols;
   int		vrows;
   int		vcols;
   int *	colwidths;
   int *	colvalues;
   chtype **	coltitle;
   int *	coltitleLen;
   int *	coltitlePos;
   int		maxct;
   chtype **	rowtitle;
   int *	rowtitleLen;
   int *	rowtitlePos;
   int		maxrt;
   int		boxHeight;
   int		boxWidth;
   int		rowSpace;
   int		colSpace;
   int		row;
   int		col;
   int		crow;		/* current row */
   int		ccol;		/* current column */
   int		trow;
   int		lcol;
   int		oldcrow;
   int		oldccol;
   int		oldvrow;
   int		oldvcol;
   EExitType	exitType;
   boolean	boxCell;
   boolean	shadow;
   chtype	highlight;
   int		dominant;
   chtype	filler;
   MATRIXCB	callbackfn;
};

/*
 * This creates a new pointer to a matrix widget.
 */
CDKMATRIX *newCDKMatrix (
		CDKSCREEN *	/* cdkscreen */,
		int		/* xpos */,
		int		/* ypos */,
		int		/* rows */,
		int		/* cols */,
		int		/* vrows */,
		int		/* vcols */,
		char *		/* title */,
		char **		/* rowtitles */,
		char **		/* coltitles */,
		int *		/* colwidths */,
		int *		/* coltypes */,
		int		/* rowspace */,
		int		/* colspace */,
		chtype		/* filler */,
		int		/* dominantAttrib */,
		boolean		/* boxMatrix */,
		boolean		/* boxCell */,
		boolean		/* shadow */);

/*
 * This activates the matrix.
 */
int activateCDKMatrix (
		CDKMATRIX *	/* matrix */,
		chtype *	/* actions */);

/*
 * This injects a single character into the matrix widget.
 */
#define injectCDKMatrix(obj,input) injectCDKObject(obj,input,Int)

/*
 * This sets the contents of the matrix widget from a fixed-size 2d array.
 * The predefined array limits are very large.
 * Use setCDKMatrixCells() instead.
 */
#define setCDKMatrix(matrix, info, rows, subSize) \
	setCDKMatrixCells(matrix, &info[0][0], rows, MAX_MATRIX_COLS, subSize)

/*
 * This sets the contents of the matrix widget from an array defined by the
 * caller.  It may be any size.  For compatibility with setCDKMatrix(), the
 * info[][] array's subscripts start at 1.
 */
void setCDKMatrixCells (
		CDKMATRIX *	/* matrix */,
		char **		/* info */,
		int		/* rows */,
		int		/* cols */,
		int *		/* subSize */);

/*
 * This sets the value of a given cell.
 */
int setCDKMatrixCell (
		CDKMATRIX *	/* matrix */,
		int		/* row */,
		int		/* col */,
		char *		/* value */);

char *getCDKMatrixCell (
		CDKMATRIX *	/* matrix */,
		int		/* row */,
		int		/* col */);

/*
 * This returns the row/col of the matrix.
 */
int getCDKMatrixCol (
		CDKMATRIX *	/* matrix */);

int getCDKMatrixRow (
		CDKMATRIX *	/* matrix */);

/*
 * These set the drawing characters of the widget.
 */
#define setCDKMatrixULChar(w,c)            setULCharOf(w,c)
#define setCDKMatrixURChar(w,c)            setURCharOf(w,c)
#define setCDKMatrixLLChar(w,c)            setLLCharOf(w,c)
#define setCDKMatrixLRChar(w,c)            setLRCharOf(w,c)
#define setCDKMatrixVerticalChar(w,c)      setVTCharOf(w,c)
#define setCDKMatrixHorizontalChar(w,c)    setHZCharOf(w,c)
#define setCDKMatrixBoxAttribute(w,c)      setBXAttrOf(w,c)

/*
 * This sets the background color of the widget.
 */
#define setCDKMatrixBackgroundColor(w,c) setCDKObjectBackgroundColor(ObjOf(w),c)

/*
 * This sets the background attribute of the widget.
 */
#define setCDKMatrixBackgroundAttrib(w,c) setBKAttrOf(w,c)

/*
 * This draws the matrix on the screen.
 */
#define drawCDKMatrix(obj,Box) drawCDKObject(obj,Box)

/*
 * This removes the matrix from the screen.
 */
#define eraseCDKMatrix(obj) eraseCDKObject(obj)

/*
 * This cleans out all the cells from the matrix.
 */
void cleanCDKMatrix (
		CDKMATRIX *	/* matrix */);

/*
 * This cleans one cell in the matrix.
 */
void cleanCDKMatrixCell (
		CDKMATRIX *	/* matrix */,
		int		/* row */,
		int		/* col */);

/*
 * This sets the main callback in the matrix.
 */
void setCDKMatrixCB (
		CDKMATRIX *	/* matrix */,
		MATRIXCB	/* callback */);

/*
 * This moves the matrix to the given cell.
 */
int moveToCDKMatrixCell (
		CDKMATRIX *	/* matrix */,
		int		/* newrow */,
		int		/* newcol */);

/*
 * This sets the box attribute of the matrix widget.
 */
void setCDKMatrixBox (
		CDKMATRIX *	/* matrix */,
		boolean		/* Box */);

boolean getCDKMatrixBox (
		CDKMATRIX *	/* matrix */);

/*
 * This moves the matrix on the screen to the given location.
 */
#define moveCDKMatrix(obj,xpos,ypos,relative,refresh) moveCDKObject(obj,xpos,ypos,relative,refresh)

/*
 * This allows the user to interactively position the matrix.
 */
#define positionCDKMatrix(widget) positionCDKObject(ObjOf(widget),widget->win)

/*
 * This destroys the matrix widget and associated memory.
 */
#define destroyCDKMatrix(obj) destroyCDKObject(obj)

/*
 * This jumps to the given matrix cell. You can pass in
 * -1 for both the row/col if you want to interactively
 * pick the cell.
 */
int jumpToCell (
		CDKMATRIX *	/* matrix */,
		int		/* row */,
		int		/* col */);

/*
 * These set the pre/post process callback functions.
 */
#define setCDKMatrixPreProcess(w,f,d)  setCDKObjectPreProcess(ObjOf(w),f,d)
#define setCDKMatrixPostProcess(w,f,d) setCDKObjectPostProcess(ObjOf(w),f,d)

#ifdef __cplusplus
}
#endif

#endif /* CDKMATRIX_H */
#endif /* CDKINCLUDES */
