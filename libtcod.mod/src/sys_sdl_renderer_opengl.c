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
#include <SDL/SDL_opengl.h>
#include "libtcod.h"
#include "libtcod_int.h"

// opengl state has been defined ?
bool has_opengl_state=false;
// the opengl font texture
static GLuint fonttexture=0;
// the opengl texture holding background colors (one pixel per console cell)
static GLuint backtexture=0;
static int texwidth,texheight;
static void TCOD_set_opengl_font_texture() {
	SDL_Surface *charmap=(SDL_Surface *)TCOD_charmap;
	Uint32 bpp = charmap->format->BytesPerPixel;
	GLenum texture_format=0;
        if (charmap->format->Rmask == 0x000000ff)
                texture_format = GL_RGBA;
        else
                texture_format = GL_BGRA;
	glBindTexture( GL_TEXTURE_2D, fonttexture );
	glTexImage2D( GL_TEXTURE_2D, 0, bpp, charmap->w, charmap->h, 0,
                      texture_format, GL_UNSIGNED_BYTE, charmap->pixels );
}

static void TCOD_set_opengl_state() {
	if (! has_opengl_state) {
		SDL_Surface *charmap=(SDL_Surface *)TCOD_charmap;
		int width=TCOD_root->w*TCOD_font_width;
		int height=TCOD_root->h*TCOD_font_height;
		// initialize all opengl stuff
		glEnable( GL_TEXTURE_2D );
		glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
		glViewport( 0, 0, width, height );
		glClear( GL_COLOR_BUFFER_BIT );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		// opengl insane magic
		glTranslatef (0.375, 0.375, 0.);
		glEnable (GL_BLEND); 
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		// charmap must have power of 2 dimensions
		texwidth=1;
		texheight=1;
		while (texwidth < charmap->w) texwidth <<=1; 
		while (texheight < charmap->h) texheight <<=1; 
		SDL_Surface *surf=TCOD_sys_get_surface(texwidth,texheight,true);
		SDL_SetAlpha(charmap, 0, 0);
		SDL_BlitSurface(charmap,NULL,surf,NULL);
		charmap=surf;
		SDL_FreeSurface( TCOD_charmap );
		TCOD_charmap=charmap;

		// prepare the 2 textures (font + background color)
		if ( fonttexture != 0 ) {
			glDeleteTextures( 1, &fonttexture );
			glDeleteTextures( 2, &backtexture );
		}
		glGenTextures( 1, &fonttexture );
		glGenTextures( 2, &backtexture );
		// sets the font texture
		glBindTexture( GL_TEXTURE_2D, fonttexture );
       	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		TCOD_set_opengl_font_texture();

		// sets the background textures properties
		glBindTexture( GL_TEXTURE_2D, backtexture );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
#ifdef TCOD_WINDOWS
		if ( ! TCOD_fullscreen_on ) {
			// turn vsync off in windowed mode
			typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
			PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

			wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");

			if (wglSwapIntervalEXT) wglSwapIntervalEXT(0);
		}
#endif

		has_opengl_state=true;
	}
}

// render with a 32bits fonts with opengl
void TCOD_sys_console_to_bitmap_opengl(void *vbitmap, TCOD_console_data_t *con, bool new_font) {
	if (! has_opengl_state ) TCOD_set_opengl_state();

	int x,y;
	glClear(GL_COLOR_BUFFER_BIT );
	TCOD_color_t fading_color = TCOD_console_get_fading_color();
	int fade = (int)TCOD_console_get_fade();
	// if some characters have been updated in the font, update the texture
	if ( TCOD_any_ascii_updated ) {
		TCOD_set_opengl_font_texture();
	}
	// blit all the cells background color at once
	glBindTexture( GL_TEXTURE_2D, backtexture );
	glTexImage2D( GL_TEXTURE_2D, 0, 3, con->w, con->h, 0,
	                      GL_RGB, GL_UNSIGNED_BYTE, TCOD_image_get_pixel_data(con->backbuf) );	    
	glBegin( GL_QUADS );
	glColor3f(1.0,1.0,1.0);
	glTexCoord2f( 0.0, 0.0 );
	glVertex2i( 0, 0);
	glTexCoord2f( 0.0, 1.0 );
	glVertex2i( 0, con->h*TCOD_font_height );
	glTexCoord2f( 1.0, 1.0 );
	glVertex2i( con->w*TCOD_font_width, con->h*TCOD_font_height );
	glTexCoord2f( 1.0, 0.0 );
	glVertex2i( con->w*TCOD_font_width, 0);
	glEnd();

	// draw the characters
	glBindTexture( GL_TEXTURE_2D, fonttexture );
	for (y=0;y<con->h;y++) {
		for (x=0; x<con->w; x++) {
			int offset=x+y*con->w;
			char_t *c=&con->buf[offset];
			if ( c->cf == -1 ) c->cf = TCOD_font_chars[c->c].ascii_to_tcod;
			if ( c->c != ' ' ) {
				SDL_Rect srcRect,dstRect;
				dstRect.x=x*TCOD_font_width;
				dstRect.y=y*TCOD_font_height;
				dstRect.w=TCOD_font_width;
				dstRect.h=TCOD_font_height;
				if ( TCOD_fullscreen_on ) {
					dstRect.x+=TCOD_fullscreen_offsetx;
					dstRect.y+=TCOD_fullscreen_offsety;
				}
				// draw foreground
				int ascii=c->cf;
				TCOD_color_t f=TCOD_image_get_pixel(con->forebuf,x,y);
				TCOD_color_t b=TCOD_image_get_pixel(con->backbuf,x,y);
				// only draw character if foreground color != background color
				if ( f.r != b.r || f.g != b.g || f.b != b.b ) {
					if (TCOD_font_is_in_row) {
						srcRect.x = (ascii%TCOD_font_nb_char_horiz)*TCOD_font_width;
						srcRect.y = (ascii/TCOD_font_nb_char_horiz)*TCOD_font_height;
					} else {
						srcRect.x = (ascii/TCOD_font_nb_char_vertic)*TCOD_font_width;
						srcRect.y = (ascii%TCOD_font_nb_char_vertic)*TCOD_font_height;
					}
					srcRect.w=TCOD_font_width;
					srcRect.h=TCOD_font_height;
					
					glBegin( GL_QUADS );
					glColor3f(f.r/255.0,f.g/255.0,f.b/255.0);
					glTexCoord2f( (float)(srcRect.x)/(texwidth-1), (float)(srcRect.y)/(texheight-1) );
					glVertex2i( dstRect.x, dstRect.y);
					glTexCoord2f( (float)(srcRect.x)/(texwidth-1), (float)(srcRect.y+TCOD_font_height)/(texheight-1) );
					glVertex2i( dstRect.x, dstRect.y+TCOD_font_height );
					glTexCoord2f( (float)(srcRect.x+TCOD_font_width)/(texwidth-1), (float)(srcRect.y+TCOD_font_height)/(texheight-1) );
					glVertex2i( dstRect.x+TCOD_font_width, dstRect.y+TCOD_font_height );
					glTexCoord2f( (float)(srcRect.x+TCOD_font_width)/(texwidth-1), (float)(srcRect.y)/(texheight-1) );
					glVertex2i( dstRect.x+TCOD_font_width, dstRect.y );
					glEnd();
				}
			}
		}
	}
	// fading overlay
	if ( fade != 255 ) {
		glBindTexture( GL_TEXTURE_2D, 0 );
		glBegin( GL_QUADS );
		glColor4f(fading_color.r/255.0,fading_color.g/255.0,fading_color.b/255.0,1.0-fade/255.0);
		glVertex2i( 0, 0);
		glVertex2i( 0, con->h*TCOD_font_height );
		glVertex2i( con->w*TCOD_font_width, con->h*TCOD_font_height );
		glVertex2i( con->w*TCOD_font_width, 0);
		glEnd();
	}
	TCOD_old_fade=fade;
}

void * TCOD_sys_get_screen_surface_OPENGL() {
	static SDL_Surface *surf=NULL;
	int x,y;
	Uint32 mask,nmask;
	if ( surf ) SDL_FreeSurface(surf);
	surf=TCOD_sys_get_surface(TCOD_root->w*TCOD_font_width,TCOD_root->h*TCOD_font_height,false);
	glReadPixels(TCOD_fullscreen_offsetx,TCOD_fullscreen_offsety,
		TCOD_root->w*TCOD_font_width,TCOD_root->h*TCOD_font_height, 
		GL_RGB,GL_UNSIGNED_BYTE,surf->pixels);
	mask=surf->format->Rmask|surf->format->Gmask|surf->format->Bmask;
	nmask=~mask;
	// vertical flip (opengl has lower-left origin, SDL upper left)
	for (x=0; x < surf->w; x++) {
		for (y=0; y < surf->h/2; y++) {
			int offsrc=x*3+y*surf->pitch;
			int offdst=x*3+(surf->h-1-y)*surf->pitch;
			Uint32 *pixsrc = (Uint32 *)(((Uint8 *)surf->pixels)+offsrc);
			Uint32 *pixdst = (Uint32 *)(((Uint8 *)surf->pixels)+offdst);
			Uint32 tmp = *pixsrc;
			*pixsrc = ((*pixsrc) & nmask) | ((*pixdst) & mask);
			*pixdst = ((*pixdst) & nmask) | (tmp & mask);
		}
	}

	return surf;
}

void TCOD_sys_flush_OPENGL() {
	SDL_GL_SwapBuffers();
}



