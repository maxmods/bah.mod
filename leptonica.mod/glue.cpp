/*
  Copyright (c) 2009-2011 Bruce A Henderson
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/ 

#include "glue.h"

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++


void bmx_pixFree(PIX *pix) {
	pixDestroy(&pix);
}

int bmx_pixRenderBoxArb(PIX *pix, BOX *box, int width, int rval, int gval, int bval) {
	return pixRenderBoxArb(pix, box, width, static_cast<l_uint8>(rval), static_cast<l_uint8>(gval), static_cast<l_uint8>(bval));
}

int bmx_pixRenderBoxBlend(PIX *pix, BOX *box, int width, int rval, int gval, int bval, float fract) {
	return pixRenderBoxBlend(pix, box, width, static_cast<l_uint8>(rval), static_cast<l_uint8>(gval), static_cast<l_uint8>(bval), fract);
}

int bmx_pixRenderHashBoxArb(PIX *pix, BOX *box, int spacing, int width, int orient, int outline, int rval, int gval, int bval) {
	return pixRenderHashBoxArb(pix, box, spacing, width, orient, outline, static_cast<l_uint8>(rval), static_cast<l_uint8>(gval), static_cast<l_uint8>(bval));
}

int bmx_pixRenderHashBoxBlend(PIX *pix, BOX *box, int spacing, int width, int orient, int outline, int rval, int gval, int bval, float fract) {
	return pixRenderHashBoxBlend(pix, box, spacing, width, orient, outline, static_cast<l_uint8>(rval), static_cast<l_uint8>(gval), static_cast<l_uint8>(bval), fract);
}

int bmx_pixRenderLineArb(PIX *pix, int x1, int y1, int x2, int y2, int width, int rval, int gval, int bval) {
	return pixRenderLineArb(pix, x1, y1, x2, y2, width, static_cast<l_uint8>(rval), static_cast<l_uint8>(gval), static_cast<l_uint8>(bval));
}

int bmx_pixRenderLineBlend(PIX *pix, int x1, int y1, int x2, int y2, int width, int rval, int gval, int bval, float fract) {
	return pixRenderLineBlend(pix, x1, y1, x2, y2, width, static_cast<l_uint8>(rval), static_cast<l_uint8>(gval), static_cast<l_uint8>(bval), fract);
}

int bmx_pixRenderPolylineArb(PIX *pix, PTA *ptas, int width, int rval, int gval, int bval, int closeFlag) {
	return pixRenderPolylineArb(pix, ptas, width, static_cast<l_uint8>(rval), static_cast<l_uint8>(gval), static_cast<l_uint8>(bval), closeFlag);
}

int bmx_pixRenderPolylineBlend(PIX *pix, PTA *ptas, int width, int rval, int gval, int bval, float fract, int closeFlag, int removeDups) {
	return pixRenderPolylineBlend(pix, ptas, width, static_cast<l_uint8>(rval), static_cast<l_uint8>(gval), static_cast<l_uint8>(bval), fract, closeFlag, removeDups);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void bmx_boxFree(BOX *box) {
	boxDestroy(&box);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void bmx_ptaFree(PTA *pta) {
	ptaDestroy(&pta);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void bmx_kernelFree(L_KERNEL *kernel) {
	kernelDestroy(&kernel);
}

