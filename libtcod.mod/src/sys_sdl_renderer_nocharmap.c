/*
* libtcod 1.5.0
* Copyright (c) 2008,2009 J.C.Wilk
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of J.C.Wilk may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY J.C.WILK ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL J.C.WILK BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <SDL/SDL.h>
#include "libtcod.h"
#include "libtcod_int.h"

// render when no font is loaded (happens when using some TCODImage stuff without root console)
void TCOD_sys_console_to_bitmap_nocharmap(void *vbitmap, TCOD_console_data_t *con, bool new_font) {
	int x,y;
	SDL_Surface *bitmap=(SDL_Surface *)vbitmap;
	Uint32 sdl_back=0;
	TCOD_color_t fading_color = TCOD_console_get_fading_color();
	int fade = (int)TCOD_console_get_fade();
	bool track_changes=(TCOD_old_fade == fade && con->oldbuf);
	for (y=0;y<con->h;y++) {
		for (x=0; x<con->w; x++) {
			SDL_Rect dstRect;
			bool changed=true;
			char_t *c=&con->buf[x+y*con->w];
			TCOD_bitmap_char_t *bitmap_char=&TCOD_font_chars[c->c];
			TCOD_color_t b=TCOD_image_get_pixel(con->backbuf,x,y);
			if ( c->cf == -1 ) c->cf = bitmap_char->ascii_to_tcod;
			if ( track_changes ) {
				TCOD_color_t oldb=TCOD_image_get_pixel(con->oldbackbuf,x,y);
				changed=false;
				if ( bitmap_char->updated || b.r != oldb.r || b.g != oldb.g
					|| b.b != oldb.b ) {
					changed=true;
				}
			}
			if ( changed ) {
				dstRect.x=x*TCOD_font_width;
				dstRect.y=y*TCOD_font_height;
				dstRect.w=TCOD_font_width;
				dstRect.h=TCOD_font_height;
				// draw background
				if ( fade != 255 ) {
					b.r = ((int)b.r) * fade / 255 + ((int)fading_color.r) * (255-fade)/255;
					b.g = ((int)b.g) * fade / 255  + ((int)fading_color.g) * (255-fade)/255;
					b.b = ((int)b.b) * fade / 255 + ((int)fading_color.b) * (255-fade)/255;
				}
				sdl_back=SDL_MapRGB(bitmap->format,b.r,b.g,b.b);
				if ( bitmap == TCOD_screen && TCOD_fullscreen_on ) {
					dstRect.x+=TCOD_fullscreen_offsetx;
					dstRect.y+=TCOD_fullscreen_offsety;
				}
				SDL_FillRect(bitmap,&dstRect,sdl_back);
			}
		}
	}
	TCOD_old_fade=fade;
}

void * TCOD_sys_get_screen_surface_SDL() {
	return TCOD_screen;
}

void TCOD_sys_flush_SDL() {
	SDL_Flip(TCOD_screen);
}



