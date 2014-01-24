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

#include <gdk/gdk.h>

void bmx_gtkmaxgui_gdkeventbutton(GdkEventButton * event, double * x, double * y, int * button) {
	*x = event->x;
	*y = event->y;
	*button = event->button;
}

void bmx_gtkmaxgui_gdkeventmotion(GdkEventMotion * event, double * x, double * y, int * state) {
	*x = event->x;
	*y = event->y;
	*state = event->state;
}

void bmx_gtkmaxgui_gdkeventscroll(GdkEventScroll * event, double * x, double * y, int * direction) {
	*x = event->x;
	*y = event->y;
	*direction = event->direction;
}

void bmx_gtkmaxgui_gdkeventkey(GdkEventKey * event, int * keyval, int * state) {
	*keyval = event->keyval;
	*state = event->state;
}

void bmx_gtkmaxgui_gdkeventconfigure(GdkEventConfigure * event, int * x, int * y, int * w, int * h) {
	*x = event->x;
	*y = event->y;
	*w = event->width;
	*h = event->height;
}

void bmx_gtkmaxgui_gdkeventwindowstate(GdkEventWindowState * event, int * state) {
	*state = event->new_window_state;
}
