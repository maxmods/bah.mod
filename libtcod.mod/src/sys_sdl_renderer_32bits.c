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

// render with a 32bits fonts with alpha
void TCOD_sys_console_to_bitmap_32bits(void *vbitmap, TCOD_console_data_t *con, bool new_font) {
	int x,y;
	static SDL_Surface *charmap_backup=NULL;
	SDL_Surface *bitmap=(SDL_Surface *)vbitmap;
	SDL_Surface *charmap=(SDL_Surface *)TCOD_charmap;
	Uint32 sdl_back=0;
	TCOD_color_t fading_color = TCOD_console_get_fading_color();
	int fade = (int)TCOD_console_get_fade();
	bool track_changes=(TCOD_old_fade == fade && con->oldbuf);
	if ( charmap_backup == NULL || new_font ) {
		if ( charmap_backup ) SDL_FreeSurface(charmap_backup);
		charmap_backup=(SDL_Surface *)TCOD_sys_get_surface(charmap->w,charmap->h,true);
		SDL_BlitSurface(charmap,NULL,charmap_backup,NULL);
	}
	for (y=0;y<con->h;y++) {
		for (x=0; x<con->w; x++) {
			SDL_Rect srcRect,dstRect;
			bool changed=true;
			char_t *c=&con->buf[x+y*con->w];
			TCOD_bitmap_char_t *bitmap_char=&TCOD_font_chars[c->c];
			TCOD_color_t b=TCOD_image_get_pixel(con->backbuf,x,y);
			TCOD_color_t f=TCOD_image_get_pixel(con->forebuf,x,y);
			if ( c->cf == -1 ) c->cf = bitmap_char->ascii_to_tcod;
			if ( track_changes ) {
				char_t *oc=&con->oldbuf[x+y*con->w];
				TCOD_color_t oldb=TCOD_image_get_pixel(con->oldbackbuf,x,y);
				TCOD_color_t oldf=TCOD_image_get_pixel(con->oldforebuf,x,y);
				changed=false;
				if ( bitmap_char->updated || b.r != oldb.r || b.g != oldb.g
					|| b.b != oldb.b || f.r != oldf.r
					|| f.g != oldf.g || f.b != oldf.b
					|| c->c != oc->c || c->cf != oc->cf) {
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
				if ( c->c != ' ' ) {
					// draw foreground
					//int ascii=fontTcodLayout ? (int)(ascii_to_tcod[c->c]): c->c;
					int ascii=c->cf;
					TCOD_color_t *curtext = &bitmap_char->col;

					if ( fade != 255 ) {
						f.r = ((int)f.r) * fade / 255 + ((int)fading_color.r) * (255-fade)/255;
						f.g = ((int)f.g) * fade / 255 + ((int)fading_color.g) * (255-fade)/255;
						f.b = ((int)f.b) * fade / 255 + ((int)fading_color.b) * (255-fade)/255;
					}
					// only draw character if foreground color != background color
					if ( bitmap_char->updated || f.r != b.r || f.g != b.g || f.b != b.b ) {
						if (TCOD_font_is_in_row) {
							srcRect.x = (ascii%TCOD_font_nb_char_horiz)*TCOD_font_width;
							srcRect.y = (ascii/TCOD_font_nb_char_horiz)*TCOD_font_height;
						} else {
							srcRect.x = (ascii/TCOD_font_nb_char_vertic)*TCOD_font_width;
							srcRect.y = (ascii%TCOD_font_nb_char_vertic)*TCOD_font_height;
						}
						srcRect.w=TCOD_font_width;
						srcRect.h=TCOD_font_height;

						if ( bitmap_char->first || curtext->r != f.r || curtext->g != f.g || curtext->b!=f.b ) {
							// change the character color in the font
					    	bitmap_char->first=false;
							Uint32 sdl_fore=SDL_MapRGB(charmap->format,f.r,f.g,f.b) & TCOD_rgb_mask;
							*curtext=f;
							// 32 bits font : fill the whole character with color
							Uint32 *pix = (Uint32 *)(((Uint8 *)charmap->pixels)+srcRect.x*4 + srcRect.y*charmap->pitch);
							int hdelta=(charmap->pitch - TCOD_font_width*4)/4;
							int h=TCOD_font_height;
							if ( ! TCOD_font_chars[ascii].colored ) {
								while (h> 0) {
									int w=TCOD_font_width;
									while ( w > 0 ) {
										(*pix) &= TCOD_nrgb_mask; // erase the color
										(*pix) |= sdl_fore; // set the new color
										w--;
										pix++;
									}
									h--;
									pix += hdelta;
								}
							} else {
								Uint32 *pixorig = (Uint32 *)(((Uint8 *)charmap_backup->pixels)+srcRect.x*4 + srcRect.y*charmap_backup->pitch);
								int hdelta_backup=(charmap_backup->pitch - TCOD_font_width*4)/4;
								while (h> 0) {
									int w=TCOD_font_width;
									while ( w > 0 ) {
										(*pix) &= TCOD_nrgb_mask; // erase the color
										int r=(int)(*((Uint8 *)(pixorig)+charmap_backup->format->Rshift/8));
										int g=(int)(*((Uint8 *)(pixorig)+charmap_backup->format->Gshift/8));
										int b=(int)(*((Uint8 *)(pixorig)+charmap_backup->format->Bshift/8));
										r = r * f.r / 255;
										g = g * f.g / 255;
										b = b * f.b / 255;
										(*pix) |= (r<<charmap_backup->format->Rshift)|(g<<charmap_backup->format->Gshift)|(b<<charmap_backup->format->Bshift); // set the new color
										w--;
										pix++;
										pixorig++;
									}
									h--;
									pix += hdelta;
									pixorig += hdelta_backup;
								}
							}
						}
						SDL_BlitSurface(charmap,&srcRect,bitmap,&dstRect);
					}
				}
			}
		}
	}
	TCOD_old_fade=fade;
}

