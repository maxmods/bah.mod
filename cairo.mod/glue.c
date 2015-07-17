/*
 Copyright (c) 2006-2015 Bruce A Henderson

  The contents of this file are subject to the Mozilla Public License
  Version 1.1 (the "License"); you may not use this file except in
  compliance with the License. You may obtain a copy of the License at
  http://www.mozilla.org/MPL/
  
  Software distributed under the License is distributed on an "AS IS"
  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
  License for the specific language governing rights and limitations
  under the License.
  
  The Original Code is BaH.Cairo.
*/

typedef struct {
    int x, y;
    int width, height;
} cairo_rectangle_int_t;

cairo_rectangle_int_t * bmx_cairo_rectangleint_new() {
	cairo_rectangle_int_t * rect = (cairo_rectangle_int_t*)malloc(sizeof(cairo_rectangle_int_t));
	return rect;
}

void bmx_cairo_rectangleint_free(cairo_rectangle_int_t * rect) {
	free(rect);
}

void bmx_cairo_rectangleint_dimensions(cairo_rectangle_int_t * rect, int * x, int * y, int * w, int * h) {
	*x = rect->x;
	*y = rect->y;
	*w = rect->width;
	*h = rect->height;
}

void bmx_cairo_rectangleint_setdimensions(cairo_rectangle_int_t * rect, int x, int y, int w, int h) {
	rect->x = x;
	rect->y = y;
	rect->width = w;
	rect->height = h;
}

void bmx_cairo_rectangleint_setx(cairo_rectangle_int_t * rect, int x) {
	rect->x = x;
}

void bmx_cairo_rectangleint_sety(cairo_rectangle_int_t * rect, int y) {
	rect->y = y;
}

void bmx_cairo_rectangleint_setwidth(cairo_rectangle_int_t * rect, int w) {
	rect->width = w;
}

void bmx_cairo_rectangleint_setheight(cairo_rectangle_int_t * rect, int h) {
	rect->height = h;
}

