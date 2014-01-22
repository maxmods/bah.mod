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
#include <glib.h>
#include <stdio.h>

typedef struct {
	GSource source;
	GPollFD fd;
} MaxEventSource;

static gboolean bmx_gtk_event_source_prepare(GSource * base, int * timeout) {
	*timeout = -1;
	return FALSE;
}

static gboolean bmx_gtk_event_source_check(GSource * base) {
	MaxEventSource *source = (MaxEventSource*)base;
	return source->fd.revents;
}

static gboolean bmx_gtk_event_source_dispatch(GSource * base, GSourceFunc callback, void * data) {
	MaxEventSource * source = (MaxEventSource*)base;
	bbSystemFlushAsyncOps();
	return TRUE;
}

static GSourceFuncs bmx_gtk_event_source_funcs =
{
	bmx_gtk_event_source_prepare,
	bmx_gtk_event_source_check,
	bmx_gtk_event_source_dispatch,
	NULL
};

GSource * bmx_gtk_event_source_new(int fd) {
	MaxEventSource * source = (MaxEventSource *) g_source_new(&bmx_gtk_event_source_funcs, sizeof(MaxEventSource));
	source->fd.fd = fd;
	source->fd.events = G_IO_IN | G_IO_HUP | G_IO_ERR;
	g_source_add_poll(&source->source, &source->fd);
	
	g_source_attach(&source->source, g_main_context_default());
	
	return &source->source;
}

