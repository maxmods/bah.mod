/*
 Copyright (c) 2017 Bruce A Henderson
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of the project nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission. 

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "zint.h"
#include "brl.mod/blitz.mod/blitz.h"
#include "cairo.h"

void bmx_zint_setsymbology(struct zint_symbol *symbol, int symbology) {
	symbol->symbology = symbology;
}

void bmx_zint_bitmapdimensions(struct zint_symbol *symbol, int * w, int * h) {
	*w = symbol->bitmap_width;
	*h = symbol->bitmap_height;
}

char * bmx_zint_bitmap(struct zint_symbol *symbol) {
	return symbol->bitmap;
}

void bmx_zint_setscale(struct zint_symbol *symbol, float scale) {
	symbol->scale = scale;
}

void bmx_zint_setwhitespacewidth(struct zint_symbol *symbol, int width) {
	symbol->whitespace_width = width;
}

void bmx_zint_setborderwidth(struct zint_symbol *symbol, int width) {
	symbol->border_width = width;
}

void bmx_zint_setfgcolour(struct zint_symbol *symbol, int r, int g, int b) {
	char buf[7];
	sprintf(buf, "%02x%02x%02x", r % 0xff, g % 0xff, b % 0xff);
	strcpy(symbol->fgcolour, buf);
}

void bmx_zint_setbgcolour(struct zint_symbol *symbol, int r, int g, int b) {
	char buf[7];
	sprintf(buf, "%02x%02x%02x", r % 0xff, g % 0xff, b % 0xff);
	strcpy(symbol->bgcolour, buf);
}

int bmx_zint_cairoencode(struct zint_symbol *symbol, cairo_t * ctx, char * inp, int length, int rotateAngle) {
	symbol->context = ctx;
	strcpy(symbol->outfile, ".CGR");
	return ZBarcode_Encode_and_Print(symbol, inp, length, rotateAngle);
}

