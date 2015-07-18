/*
 Copyright (c) 2015 Bruce A Henderson

 The contents of this file are subject to the Mozilla Public License
  Version 1.1 (the "License"); you may not use this file except in
  compliance with the License. You may obtain a copy of the License at
  http://www.mozilla.org/MPL/
  
  Software distributed under the License is distributed on an "AS IS"
  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
  License for the specific language governing rights and limitations
  under the License.
  
  The Original Code is BaH.CairoXcb.
*/  
#include "bah.mod/xcb.mod/glue.h"
#include <cairo/cairo.h>

cairo_surface_t * bmx_cairo_xcb_surface_create(struct MaxXcb * xcb, xcb_window_t winId) {

	xcb_screen_t * screen = xcb_setup_roots_iterator(xcb_get_setup(xcb->conn)).data;
	xcb_visualtype_t * visual_type = xcb_depth_visuals_iterator((xcb_screen_allowed_depths_iterator (screen)).data).data;

	xcb_get_geometry_cookie_t cookie = xcb_get_geometry(xcb->conn, winId);
    xcb_get_geometry_reply_t * geom = xcb_get_geometry_reply(xcb->conn, cookie, NULL);

	cairo_surface_t * surf = cairo_xcb_surface_create(xcb->conn, winId, visual_type, geom->width, geom->height);
	
	free(geom);

	return surf;
}

void bmx_cairo_xcb_surface_set_size(cairo_surface_t * surf, int width, int height) {
	cairo_xcb_surface_set_size(surf, width, height);
}

