/*
 Copyright (c) 2014 Bruce A Henderson
 
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
#include <gtk/gtk.h>

PangoFontDescription * bmx_gtk_style_get_fontdesc(GtkStyle *style) {
	return style->font_desc;
}

GValue * bmx_gtk_gvalue_new(int type) {
	GValue * val = (GValue*)calloc(1, sizeof(GValue));
	return g_value_init(val, type);
}

void bmx_gtk_gvalue_free(GValue * val) {
	free(val);
}

GtkTextIter * bmx_gtk_gtktextiter_new() {
	return (GtkTextIter*)calloc(1, sizeof(GtkTextIter));
}

void bmx_gtk_gtktextiter_free(GtkTextIter * iter) {
	free(iter);
}

GdkColor * bmx_gtk_gdkcolor_new(int r, int g, int b) {
	GdkColor * col = (GdkColor*)calloc(1, sizeof(GdkColor));
	col->red = r;
	col->green = g;
	col->blue = b;
	return col;
}

void bmx_gtk_gdkcolor_free(GdkColor * col) {
	free(col);
}

void bmx_gtk_gdkcolor_color(GdkColor * col, int * r, int * g, int * b) {
	*r = col->red;
	*g = col->green;
	*b = col->blue;
}

GtkAllocation * bmx_gtk_gtkallocation_new() {
	return (GtkAllocation*)calloc(1, sizeof(GtkAllocation));
}

void bmx_gtk_gtkallocation_dim(GtkAllocation * al, int * w, int * h) {
	*w = al->width;
	*h = al->height;
}

void bmx_gtk_gtkallocation_free(GtkAllocation * al) {
	free(al);
}

GtkRequisition * bmx_gtk_gtkrequisition_new() {
	return (GtkRequisition*)calloc(1, sizeof(GtkRequisition));
}

void bmx_gtk_gtkrequisition_dim(GtkRequisition * req, int * w, int * h) {
	*w = req->width;
	*h = req->height;
}

void bmx_gtk_gtkrequisition_free(GtkRequisition * req) {
	free(req);
}

GdkGeometry * bmx_gtk_gdkgeometry_new() {
	return (GdkGeometry*)calloc(1, sizeof(GdkGeometry));
}

void bmx_gtk_gdkgeometry_setmin(GdkGeometry * geom, int w, int h) {
	geom->min_width = w;
	geom->min_height = h;
}

void bmx_gtk_gdkgeometry_setmax(GdkGeometry * geom, int w, int h) {
	geom->max_width = w;
	geom->max_height = h;
}

void bmx_gtk_gdkgeometry_setbase(GdkGeometry * geom, int w, int h) {
	geom->base_width = w;
	geom->base_height = h;
}

void bmx_gtk_gdkgeometry_setinc(GdkGeometry * geom, int w, int h) {
	geom->width_inc = w;
	geom->height_inc = h;
}

void bmx_gtk_gdkgeometry_free(GdkGeometry * geom) {
	free(geom);
}

GtkTreeIter * bmx_gtk_gtktreeiter_new() {
	return (GtkTreeIter*)calloc(1, sizeof(GtkTreeIter));
}

void bmx_gtk_gtktreeiter_free(GtkTreeIter * iter) {
	free(iter);
}
