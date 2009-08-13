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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libtcod.h"
#include "libtcod_int.h"

#if defined( TCOD_VISUAL_STUDIO )
static const char *version_string ="libtcod "TCOD_STRVERSION;
#else
static const char *version_string __attribute__((unused)) ="libtcod "TCOD_STRVERSION;
#endif

static bool windowClosed=false;
static TCOD_console_t root = NULL;

static TCOD_color_t color_control_fore[TCOD_COLCTRL_NUMBER];
static TCOD_color_t color_control_back[TCOD_COLCTRL_NUMBER];

static TCOD_color_t fading_color={0,0,0};
static uint8 fade=255;

void TCOD_console_set_color_control(TCOD_colctrl_t con, TCOD_color_t fore, TCOD_color_t back) {
	TCOD_IFNOT(con >= TCOD_COLCTRL_1 && con <= TCOD_COLCTRL_NUMBER ) return;
	color_control_fore[con-1]=fore;
	color_control_back[con-1]=back;
}

void TCOD_fatal(const char *fmt, ...) {
	va_list ap;
	TCOD_sys_term();
	printf("%s\n",version_string);
	va_start(ap,fmt);
	if ( fmt != NULL ) vprintf(fmt,ap);
	va_end(ap);
	printf ("\n");
	exit (1);
}

void TCOD_fatal_nopar(const char *msg) {
	TCOD_sys_term();
	if ( msg != NULL ) printf("%s\n%s\n",version_string,msg);
	exit (1);
}

TCOD_console_t TCOD_console_new(int w, int h)  {
	TCOD_IFNOT(w > 0 && h > 0 ) {
		return NULL;
	} else {
		TCOD_console_data_t *con=(TCOD_console_data_t *)calloc(sizeof(TCOD_console_data_t),1);
		con->w=w;
		con->h=h;
		TCOD_console_init(con,NULL,false);
		return (TCOD_console_t)con;
	}
}

TCOD_key_t TCOD_console_check_for_keypress(int flags) {
	return TCOD_sys_check_for_keypress(flags);
}

TCOD_key_t TCOD_console_wait_for_keypress(bool flush) {
	return TCOD_sys_wait_for_keypress(flush);
}

bool TCOD_console_is_window_closed() {
	return windowClosed;
}

void TCOD_console_set_window_closed() {
	windowClosed=true;
}

void TCOD_console_set_window_title(const char *title) {
	if ( title == NULL ) title="";
	TCOD_sys_set_window_title(title);
}

void TCOD_console_set_fullscreen(bool fullscreen) {
	TCOD_console_data_t *dat=(TCOD_console_data_t *)(root);
	TCOD_IFNOT(dat != NULL) return;
	TCOD_sys_set_fullscreen(fullscreen);
	dat->fullscreen=fullscreen;
}

bool TCOD_console_is_fullscreen() {
	TCOD_console_data_t *dat=(TCOD_console_data_t *)(root);
	TCOD_IFNOT(dat != NULL) return false;
	return dat->fullscreen;
}

void TCOD_console_delete(TCOD_console_t con) {
	TCOD_console_data_t *dat;
	if (! con ) {
		con=root;
		TCOD_sys_term();
		root=NULL;
	}
	dat=(TCOD_console_data_t *)(con);
	free(dat->buf);
	if ( dat->oldbuf ) free(dat->oldbuf);
	TCOD_image_delete(dat->forebuf);
	TCOD_image_delete(dat->backbuf);
	if ( dat->oldforebuf ) TCOD_image_delete(dat->oldforebuf);
	if ( dat->oldbackbuf ) TCOD_image_delete(dat->oldbackbuf);
	free(dat);
}

void TCOD_console_blit(TCOD_console_t srcCon,int xSrc, int ySrc, int wSrc, int hSrc,
	TCOD_console_t dstCon, int xDst, int yDst, int fade_val) {
	TCOD_console_data_t *src;
	TCOD_console_data_t *dst;
	int cx,cy;
	if (! srcCon ) srcCon=root;
	if (! dstCon ) dstCon=root;
	src=(TCOD_console_data_t *)(srcCon);
	dst=(TCOD_console_data_t *)(dstCon);
	TCOD_ASSERT(xSrc >= 0 && ySrc >= 0 && xSrc+wSrc <= src->w && ySrc+hSrc <= src->h);
	xSrc = MAX(0,xSrc);
	ySrc = MAX(0,ySrc);
	wSrc = MIN(wSrc, src->w-xSrc);
	hSrc = MIN(hSrc, src->h-ySrc);
	TCOD_IFNOT(wSrc > 0 && hSrc > 0 ) return;
	TCOD_IFNOT(xDst+wSrc >= 0 && yDst+hSrc >= 0 && xDst < dst->w && yDst < dst->h) return;
	for (cx = xSrc; cx < xSrc+wSrc; cx++) {
		for (cy = ySrc; cy < ySrc+hSrc; cy++) {
			char_t srcChar=src->buf[cy * src->w+cx];
			TCOD_color_t srcfore=TCOD_image_get_pixel(src->forebuf,cx,cy);
			TCOD_color_t srcback=TCOD_image_get_pixel(src->backbuf,cx,cy);
			if (! src->haskey || srcback.r != src->key.r
				|| srcback.g != src->key.g || srcback.b != src->key.b ) {
				int dx=cx-xSrc+xDst;
				int dy=cy-ySrc+yDst;
				int dstoffset=dy * dst->w + dx;
				char_t dstChar;
				TCOD_color_t dstfore;
				TCOD_color_t dstback;
				if ( fade_val == 255 ) {
					dstChar=srcChar;
					dstfore=srcfore;
					dstback=srcback;
				} else {
					float ffade=(float)fade_val/255.0f;
					dstChar=dst->buf[ dstoffset ];
					dstfore=TCOD_image_get_pixel(dst->forebuf,dx,dy);
					dstback=TCOD_image_get_pixel(dst->backbuf,dx,dy);

					dstback = TCOD_color_lerp(dstback,srcback,ffade);
					if ( srcChar.c == ' ' ) {
						dstfore = TCOD_color_lerp(dstfore,srcback,ffade);
					} else if (dstChar.c == ' ') {
						dstChar.c=srcChar.c;
						dstChar.cf=srcChar.cf;
						dstfore = TCOD_color_lerp(dstback,srcfore,ffade);
					} else if (dstChar.c == srcChar.c) {
						dstfore = TCOD_color_lerp(dstfore,srcfore,ffade);
					} else {
						if ( fade_val < 128 ) {
							dstfore=TCOD_color_lerp(dstfore,dstback,
								(float)(fade_val)/127);
						} else {
							dstChar.c=srcChar.c;
							dstChar.cf=srcChar.cf;
							dstfore=TCOD_color_lerp(dstback,srcfore,
								(float)(fade_val-128)/127);
						}
					}
				}
				dst->buf[ dstoffset ] = dstChar;
				TCOD_image_put_pixel(dst->forebuf, dx, dy, dstfore,TCOD_COLOROP_SET);
				TCOD_image_put_pixel(dst->backbuf, dx, dy, dstback,TCOD_COLOROP_SET);
			}
		}
	}
}


void TCOD_console_flush() {
	TCOD_console_data_t *dat=(TCOD_console_data_t *)root;
	TCOD_IFNOT(root != NULL) return;
	TCOD_sys_flush(true);
	memcpy(dat->oldbuf,dat->buf,sizeof(char_t)*
		dat->w*dat->h);
	TCOD_image_blit_rect(dat->forebuf,dat->oldforebuf,0,0,-1,-1,TCOD_COLOROP_SET);
	TCOD_image_blit_rect(dat->backbuf,dat->oldbackbuf,0,0,-1,-1,TCOD_COLOROP_SET);
}

void TCOD_console_set_fade(uint8 val, TCOD_color_t fadecol) {
	fade=val;
	fading_color=fadecol;
}

uint8 TCOD_console_get_fade() {
	return fade;
}

TCOD_color_t TCOD_console_get_fading_color() {
	return fading_color;
}


void TCOD_console_put_char(TCOD_console_t con,int x, int y, int c, TCOD_color_t fore, TCOD_color_t back) {
	TCOD_console_data_t *dat;
	int offset;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT ( dat != NULL && (unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h ) return;
	TCOD_IFNOT (c >= 0 && c < TCOD_max_font_chars ) return;
	offset = y * dat->w + x;
	dat->buf[ offset ].c = c;
	dat->buf[ offset ].cf = TCOD_font_chars[c].ascii_to_tcod;
	TCOD_image_put_pixel(dat->forebuf,x,y,fore,TCOD_COLOROP_SET);
	TCOD_image_put_pixel(dat->backbuf,x,y,back,TCOD_COLOROP_SET);
}

void TCOD_console_clear(TCOD_console_t con) {
	TCOD_console_data_t *dat;
	int off;
	int spacecode;
	char_t *ch;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT(dat != NULL) return;
	ch=dat->buf;
	off=dat->w*dat->h-1;
	spacecode=TCOD_font_chars[' '].ascii_to_tcod;
	while (off >= 0) {
		ch->dirt=0;
		ch->c=' ';
		ch->cf=spacecode;
		off--;
		ch++;
	}
	TCOD_image_clear(dat->forebuf,dat->brush.fore);
	TCOD_image_clear(dat->backbuf,dat->brush.back);
}

TCOD_color_t TCOD_console_get_back(TCOD_console_t con,int x, int y) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	return TCOD_image_get_pixel(dat->backbuf,x,y);
}

void TCOD_console_set_fore(TCOD_console_t con,int x,int y, TCOD_color_t col) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_image_put_pixel(dat->forebuf,x,y,col,TCOD_COLOROP_SET);
}

TCOD_color_t TCOD_console_get_fore(TCOD_console_t con,int x, int y) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	return TCOD_image_get_pixel(dat->forebuf,x,y);
}

int TCOD_console_get_char(TCOD_console_t con,int x, int y) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT ( dat != NULL 
		&& (unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h ) 
		return 0;
	return dat->buf[ y * dat->w + x ].c;
}

void TCOD_console_set_back(TCOD_console_t con,int x, int y, TCOD_color_t col) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_image_put_pixel(dat->backbuf,x,y,col,TCOD_COLOROP_SET);
}

void TCOD_console_set_char(TCOD_console_t con,int x, int y, int c) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT ( dat != NULL 
		&& (unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h ) 
		return;
	dat->buf[ y * dat->w + x ].c=c;
	dat->buf[ y * dat->w + x ].cf = TCOD_font_chars[c].ascii_to_tcod;
}

void TCOD_console_brush_char(TCOD_console_t con,int x, int y, int c) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT ( dat != NULL 
		&& (unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h ) 
		return;
	TCOD_IFNOT(c>=0 && c < TCOD_max_font_chars) return;
	dat->buf[ y * dat->w + x ].c=c;
	dat->buf[ y * dat->w + x ].cf = TCOD_font_chars[c].ascii_to_tcod;
	TCOD_image_put_pixel(dat->forebuf,x,y,dat->brush.fore,dat->brush.foreflag);
	TCOD_image_put_pixel(dat->backbuf,x,y,dat->brush.back,dat->brush.backflag);
}

static void TCOD_console_clamp(int cx, int cy, int cw, int ch, int *x, int *y, int *w, int *h) {
	if ( *x + *w > cw ) *w = cw - *x;
	if ( *y + *h > ch ) *h = ch - *y;
	if (*x < cx ) {
		*w -= cx - *x;
		*x = cx;
	}
	if (*y < cy ) {
		*h -= cy - *y;
		*y = cy;
	}
}

void TCOD_console_rect(TCOD_console_t con,int x,int y, int rw, int rh, int c) {
	TCOD_console_data_t *dat;
	int cx,cy;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)(con);
	TCOD_IFNOT ( dat != NULL ) return;
	TCOD_IFNOT(c>=0 && c < TCOD_max_font_chars) return;
	TCOD_ASSERT((unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h );
	TCOD_ASSERT(x+rw <= dat->w && y+rh <= dat->h );
	
	TCOD_console_clamp(0,0,dat->w,dat->h,&x,&y,&rw,&rh);
	TCOD_IFNOT(rw > 0 && rh > 0) return;
	for (cx=x;cx < x+rw; cx++) {
		for (cy=y;cy<y+rh;cy++) {
			int curc = c;
			if ( c == 0 ) curc = TCOD_console_get_char(con,cx,cy);
			TCOD_console_brush_char(con,cx,cy,curc);
		}
	}
}

char *TCOD_console_vsprint(const char *fmt, va_list ap) {
	#define NB_BUFFERS 10
	#define INITIAL_SIZE 512
	// several static buffers in case the function is used more than once in a single function call
	static char *msg[NB_BUFFERS]={NULL};
	static int buflen[NB_BUFFERS]={0};
	static int curbuf=0;
	char *ret;
	bool ok=false;
	if (!msg[0]) {
		int i;
		for (i=0; i < NB_BUFFERS; i++) {
			buflen[i]=INITIAL_SIZE;
			msg[i]=(char *)calloc(sizeof(char),INITIAL_SIZE);
		}
	}
	do {
		/* warning ! depending on the compiler, vsnprintf return -1 or
		 the expected string length if the buffer is not big enough */
		int len = vsnprintf(msg[curbuf],buflen[curbuf],fmt,ap);
		ok=true;
		if (len < 0 || len >= buflen[curbuf]) {
			// buffer too small.
			if ( len > 0 ) {
				while ( buflen[curbuf] < len+1 ) buflen[curbuf]*=2;
			} else {
				buflen[curbuf]*=2;
			}
			free( msg[curbuf] );
			msg[curbuf]=(char *)calloc(sizeof(char),buflen[curbuf]);
			ok=false;
		}
	} while (! ok);
	ret=msg[curbuf];
	curbuf = (curbuf+1)%NB_BUFFERS;
	return ret;
}

void TCOD_console_print_left(TCOD_console_t con,int x, int y, const char *fmt, ...) {
	TCOD_console_data_t *dat;
	va_list ap;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	va_start(ap,fmt);
	TCOD_console_print(con,x,y,dat->w-x,dat->h-y,LEFT,TCOD_console_vsprint(fmt,ap), false, false);
	va_end(ap);
}

void TCOD_console_print_right(TCOD_console_t con,int x, int y, const char *fmt, ...) {
	TCOD_console_data_t *dat;
	va_list ap;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	va_start(ap,fmt);
	TCOD_console_print(con,x,y,x+1,dat->h-y,RIGHT,TCOD_console_vsprint(fmt,ap), false, false);
	va_end(ap);
}

void TCOD_console_print_center(TCOD_console_t con,int x, int y, const char *fmt, ...) {
	TCOD_console_data_t *dat;
	va_list ap;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	va_start(ap,fmt);
	TCOD_console_print(con,x,y,dat->w,dat->h-y,CENTER,TCOD_console_vsprint(fmt,ap), false, false);
	va_end(ap);
}

int TCOD_console_print_left_rect(TCOD_console_t con,int x, int y, int w, int h, const char *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret = TCOD_console_print(con,x,y,w,h,LEFT,TCOD_console_vsprint(fmt,ap), true, false);
	va_end(ap);
	return ret;
}

int TCOD_console_print_right_rect(TCOD_console_t con,int x, int y, int w, int h, const char *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret=TCOD_console_print(con,x,y,w,h,RIGHT,TCOD_console_vsprint(fmt,ap), true, false);
	va_end(ap);
	return ret;
}

int TCOD_console_print_center_rect(TCOD_console_t con,int x, int y, int w, int h, const char *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret=TCOD_console_print(con,x,y,w,h,CENTER,TCOD_console_vsprint(fmt,ap), true, false);
	va_end(ap);
	return ret;
}

int TCOD_console_height_left_rect(TCOD_console_t con,int x, int y, int w, int h, const char *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret = TCOD_console_print(con,x,y,w,h,LEFT,TCOD_console_vsprint(fmt,ap), true, true);
	va_end(ap);
	return ret;
}

int TCOD_console_height_right_rect(TCOD_console_t con,int x, int y, int w, int h, const char *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret=TCOD_console_print(con,x,y,w,h,RIGHT,TCOD_console_vsprint(fmt,ap), true, true);
	va_end(ap);
	return ret;
}

int TCOD_console_height_center_rect(TCOD_console_t con,int x, int y, int w, int h, const char *fmt, ...) {
	int ret;
	va_list ap;
	va_start(ap,fmt);
	ret=TCOD_console_print(con,x,y,w,h,CENTER,TCOD_console_vsprint(fmt,ap), true, true);
	va_end(ap);
	return ret;
}

// non public methods
int TCOD_console_stringLength(const char *s) {
	int l=0;
	while (*s) {
		if ( *s == (int)TCOD_COLCTRL_FORE_RGB || *s == (int)TCOD_COLCTRL_BACK_RGB ) s+=3;
		else if ( *s > (int)TCOD_COLCTRL_STOP ) l++;
		s++;
	}
	return l;
}

char * TCOD_console_forward(char *s,int l) {
	while ( *s && l > 0 ) {
		if ( *s == (int)TCOD_COLCTRL_FORE_RGB || *s == (int)TCOD_COLCTRL_BACK_RGB ) s+=3;
		else if ( *s > (int)TCOD_COLCTRL_STOP ) l--;
		s++;
	}
	return s;
}

int TCOD_console_print(TCOD_console_t con,int x,int y, int rw, int rh, 
	alignment_t align, char *msg, bool can_split, bool count_only) {
	TCOD_console_data_t *dat;
	char *c=msg;
	int cx=0,cy=y;
	int minx,maxx,miny,maxy;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)(con);
	TCOD_IFNOT ( dat != NULL 
		&& (unsigned)(x) < (unsigned)dat->w && (unsigned)(y) < (unsigned)dat->h ) 
		return 0;
	TCOD_IFNOT(msg != NULL) return 0;
	miny=y;
	maxy=dat->h-1;
	if (rh > 0) maxy=MIN(maxy,y+rh-1);
	switch (align) {
		case LEFT : minx=MAX(0,x); maxx=MIN(dat->w-1,x+rw-1); break;
		case RIGHT : minx=MAX(0,x-rw+1); maxx=MIN(dat->w-1,x); break;
		case CENTER : default : minx=MAX(0,x-rw/2); maxx=MIN(dat->w-1,x+rw/2); break;
	}

	do {
		// get \n delimited sub-message
		char *end=strchr(c,'\n');
		char bak=0;
		int cl;
		char *split=NULL;
		
		TCOD_color_t oldFore=dat->brush.fore;
		TCOD_color_t oldBack=dat->brush.back;
		if ( end ) *end=0;
		cl= TCOD_console_stringLength(c);
		// find starting x
		switch (align) {
			case LEFT : cx=x; break;
			case RIGHT : cx=x-cl+1; break;
			case CENTER : cx= x-cl/2;break;
		}
		// check if the string is completely out of the minx,miny,maxx,maxy frame
		if ( cy >= miny && cy <= maxy && cx <= maxx && cx+cl -1 >= minx ) {
			if ( can_split && cy < maxy ) {
				// if partially out of screen, try to split the sub-message
				if ( cx < minx ) split = TCOD_console_forward(c, align == CENTER ? cl-2*(minx-cx) : cl-(minx-cx));
				else if ( align==CENTER ) {
					if ( cx + cl/2 > maxx+1 ) split = TCOD_console_forward(c, maxx+1 - cx);
				} else {
					if ( cx + cl > maxx+1 ) split = TCOD_console_forward(c, maxx+1 - cx);
				}
			}
			if ( split ) {
				char *oldsplit=split;
				while ( ! isspace(*split) && split > c ) split --;
				if (end) *end='\n';
				if (!isspace(*split) ) {
					split=oldsplit;
				}
				end=split;
				bak=*split;
				*split=0;
				cl=TCOD_console_stringLength(c);
				switch (align) {
					case LEFT : cx=x; break;
					case RIGHT : cx=x-cl+1; break;
					case CENTER : cx= x-cl/2;break;
				}
			}
			if ( cx < minx ) {
				// truncate left part
				c += minx-cx;
				cl -= minx-cx;
				cx=minx;
			}
			if ( cx + cl > maxx+1 ) {
				// truncate right part
				split = TCOD_console_forward(c, maxx+1 - cx);
				*split=0;
			}
			// render the sub-message
			if ( cy >= 0 && cy < dat->h )
			while (*c) {
				if ( *c >= TCOD_COLCTRL_1 && *c <= TCOD_COLCTRL_NUMBER ) {
					dat->brush.fore=color_control_fore[(int)(*c)-1];
					dat->brush.back=color_control_back[(int)(*c)-1];
				} else if ( *c == TCOD_COLCTRL_FORE_RGB ) {
					c++;
					dat->brush.fore.r=*c++;
					dat->brush.fore.g=*c++;
					dat->brush.fore.b=*c;
				} else if ( *c == TCOD_COLCTRL_BACK_RGB ) {
					c++;
					dat->brush.back.r=*c++;
					dat->brush.back.g=*c++;
					dat->brush.back.b=*c;
				} else if ( *c == TCOD_COLCTRL_STOP ) {
					dat->brush.fore=oldFore;
					dat->brush.back=oldBack;
				} else {
					if (! count_only) TCOD_console_brush_char(con,cx,cy,(unsigned char)(*c));
					cx++;
				}
				c++;
			}
		}
		if ( end ) {
			// next line
			if ( split && ! isspace(bak) ) {
				*end=bak;
				c=end;
			} else {
				c=end+1;
			}
			cy++;
		} else c=NULL;
	} while ( c && cy < dat->h && (rh == 0 || cy < y+rh) );
	return cy-y+1;
}

void TCOD_console_init_root(int w, int h, const char*title, bool fullscreen) {
	TCOD_IF(w > 0 && h > 0) {
		TCOD_console_data_t *con=(TCOD_console_data_t *)calloc(sizeof(TCOD_console_data_t),1);
		int i;
		con->w=w;
		con->h=h;
		root=con;
		for (i=0; i < TCOD_COLCTRL_NUMBER; i++) {
			color_control_fore[i]=TCOD_white;
			color_control_back[i]=TCOD_black;
		}
		TCOD_console_init((TCOD_console_t)con,title,fullscreen);
	}
}

TCOD_image_t TCOD_console_get_background_image(TCOD_console_t con) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT(dat != NULL) return NULL;
	return dat->backbuf;
}

TCOD_image_t TCOD_console_get_foreground_image(TCOD_console_t con) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT(dat != NULL) return NULL;
	return dat->forebuf;
}

void TCOD_console_push_brush(TCOD_console_t con) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT(dat != NULL) return;
	if ( dat->nbBrushes == dat->maxBrushes ) {
		dat->maxBrushes *= 2;
		dat->brushes = (TCOD_brush_t *)realloc(dat->brushes,sizeof(TCOD_brush_t)*dat->maxBrushes);
		if ( dat->brushes == NULL ) TCOD_fatal("memory allocation failure");
	}
	dat->brushes[dat->nbBrushes]=dat->brush;
	dat->nbBrushes++;
}

void TCOD_console_pop_brush(TCOD_console_t con) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT(dat != NULL) return;
	if ( dat->nbBrushes > 0 ) {
		dat->nbBrushes--;
		dat->brush = dat->brushes[dat->nbBrushes];
	}
}

bool TCOD_console_init(TCOD_console_t con,const char *title, bool fullscreen) {
	TCOD_console_data_t *dat;
	int i;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT(dat != NULL) return false;
	dat->brush.fore=TCOD_white;
	dat->brush.back=TCOD_black;
	dat->brush.foreflag=TCOD_COLOROP_SET;
	dat->brush.backflag=TCOD_COLOROP_NONE;
	dat->fade=255;
	dat->fullscreen=fullscreen;
	dat->windowed=(title != NULL);
	dat->buf = (char_t *)calloc(sizeof(char_t),dat->w*dat->h);
	dat->forebuf=TCOD_image_new(dat->w,dat->h);
	dat->backbuf=TCOD_image_new(dat->w,dat->h);
	dat->brushes = (TCOD_brush_t *)malloc(sizeof(TCOD_brush_t)*16);
	dat->maxBrushes=16;
	dat->nbBrushes=0;
	windowClosed = false;
	for (i=0; i< dat->w*dat->h; i++) {
		dat->buf[i].c=' ';
		dat->buf[i].cf=-1;
	}
	if ( title ) {
		dat->oldbuf = (char_t *)calloc(sizeof(char_t),dat->w*dat->h);
		dat->oldforebuf=TCOD_image_new(dat->w,dat->h);
		dat->oldbackbuf=TCOD_image_new(dat->w,dat->h);
		if (! TCOD_sys_init(dat,fullscreen) ) return false;
		TCOD_sys_set_window_title(title);
	}
	return true;
}

void TCOD_console_set_foreground_color(TCOD_console_t con,TCOD_color_t col) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT(dat != NULL) return;
	dat->brush.fore=col;
}

void TCOD_console_set_background_color(TCOD_console_t con,TCOD_color_t col) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT(dat != NULL) return;
	dat->brush.back=col;
}

void TCOD_console_set_brush(TCOD_console_t con,TCOD_color_t forecol, TCOD_colorop_t foreflag, 
	TCOD_color_t backcol, TCOD_colorop_t backflag) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT(dat != NULL) return;
	dat->brush.back=backcol;
	dat->brush.backflag=backflag;
	dat->brush.fore=forecol;
	dat->brush.foreflag=foreflag;
}

void TCOD_console_set_background_brush(TCOD_console_t con,TCOD_color_t col, TCOD_colorop_t flag) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT(dat != NULL) return;
	dat->brush.back=col;
	dat->brush.backflag=flag;
}

void TCOD_console_set_foreground_brush(TCOD_console_t con,TCOD_color_t col, TCOD_colorop_t flag) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT(dat != NULL) return;
	dat->brush.fore=col;
	dat->brush.foreflag=flag;
}

void TCOD_console_set_background_flag(TCOD_console_t con,TCOD_colorop_t flag) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT(dat != NULL) return;
	dat->brush.backflag=flag;
}

void TCOD_console_set_foreground_flag(TCOD_console_t con,TCOD_colorop_t flag) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT(dat != NULL) return;
	dat->brush.foreflag=flag;
}


TCOD_color_t TCOD_console_get_foreground_color(TCOD_console_t con) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	if (! con ) return TCOD_white; /* return white if init_root was not called */
	dat=(TCOD_console_data_t *)con;
	return dat->brush.fore;
}

TCOD_color_t TCOD_console_get_background_color(TCOD_console_t con) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	if (! con ) return TCOD_black; /* return black if init_root was not called */
	dat=(TCOD_console_data_t *)con;
	return dat->brush.back;
}

int TCOD_console_get_width(TCOD_console_t con) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT(dat != NULL) return 0;
	return dat->w;
}

int TCOD_console_get_height(TCOD_console_t con) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT(dat != NULL) return 0;
	return dat->h;
}

char_t *TCOD_console_get_buf(TCOD_console_t con) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT(dat != NULL) return NULL;
	return dat->buf;
}

void TCOD_console_set_custom_font(const char *fontFile, int flags,int nb_char_horiz, int nb_char_vertic) {
	TCOD_sys_set_custom_font(fontFile, nb_char_horiz, nb_char_vertic, flags);
}

void TCOD_console_map_ascii_code_to_font(int asciiCode, int fontCharX, int fontCharY) {
	TCOD_sys_map_ascii_to_font(asciiCode, fontCharX, fontCharY);
}

void TCOD_console_map_ascii_codes_to_font(int asciiCode, int nbCodes, int fontCharX, int fontCharY) {
	int c;
	TCOD_IFNOT(asciiCode >= 0 && asciiCode+nbCodes <= TCOD_max_font_chars) return;
	for (c=asciiCode; c < asciiCode+nbCodes; c++ ) {
		TCOD_sys_map_ascii_to_font(c, fontCharX, fontCharY);
		fontCharX++;
		if ( fontCharX == TCOD_font_nb_char_horiz ) {
			fontCharX=0;
			fontCharY++;
		}
	}
}

void TCOD_console_map_string_to_font(const char *s, int fontCharX, int fontCharY) {
	TCOD_IFNOT(s != NULL) return;
	while (*s) {
		TCOD_sys_map_ascii_to_font(*s, fontCharX, fontCharY);
		fontCharX++;
		if ( fontCharX == TCOD_font_nb_char_horiz ) {
			fontCharX=0;
			fontCharY++;
		}
		s++;
	}
}

/*
void TCOD_console_set_keyboard_repeat(int initial_delay, int interval) {
	TCOD_sys_set_keyboard_repeat(initial_delay,interval);
}

void TCOD_console_disable_keyboard_repeat() {
	TCOD_sys_set_keyboard_repeat(0,0);
}
*/

bool TCOD_console_is_key_pressed(TCOD_keycode_t key) {
	return TCOD_sys_is_key_pressed(key);
}
void TCOD_console_set_key_color(TCOD_console_t con,TCOD_color_t col) {
	TCOD_console_data_t *dat;
	if (! con ) con=root;
	dat=(TCOD_console_data_t *)con;
	TCOD_IFNOT(dat != NULL) return;
	dat->key = col;
	dat->haskey=true;
}

void TCOD_console_credits() {
	bool end=false;
	int x=TCOD_console_get_width(NULL)/2-6;
	int y=TCOD_console_get_height(NULL)/2;
	int fade=260;
	TCOD_sys_save_fps();
	TCOD_sys_set_fps(25);
	while (!end ) {
		TCOD_key_t k;
		end=TCOD_console_credits_render(x,y,false);
		k=TCOD_console_check_for_keypress(TCOD_KEY_PRESSED);
		if ( fade == 260 && k.vk != TCODK_NONE ) {
			fade -= 10;
		}
		TCOD_console_flush();
		if ( fade < 260 ) {
			fade -= 10;
			TCOD_console_set_fade(fade,TCOD_black);
			if ( fade == 0 ) end=true;
		}
	}
	TCOD_console_set_fade(255,TCOD_black);
	TCOD_sys_restore_fps();
}

bool TCOD_console_credits_render(int x, int y, bool alpha) {
	static char poweredby[128];
	static float char_heat[128];
	static int char_x[128];
	static int char_y[128];
	static bool init1=false;
	static bool init2=false;
	static int len,len1,cw,ch;
	static float xstr;
	static TCOD_color_t colmap[64];
	static TCOD_color_t colmap_light[64];
	static TCOD_noise_t noise;
	static TCOD_color_t colkeys[4] = {
		{255,255,204},
		{255,204,0},
		{255,102,0},
		{102,153,255},
	};
	static TCOD_color_t colkeys_light[4] = {
		{255,255,204},
		{128,128,77},
		{51,51,31},
		{0,0,0},
	};
	static int colpos[4]={
		0,21,42,63
	};
	int i,xc,yc;
	float sparklex,sparkley,sparklerad,sparklerad2,noisex;

	if (!init1) {
		// initialize all static data, colormaps, ...
		TCOD_color_gen_map(colmap,4,colkeys,colpos);
		TCOD_color_gen_map(colmap_light,4,colkeys_light,colpos);
		cw=TCOD_console_get_width(NULL);
		ch=TCOD_console_get_height(NULL);
		sprintf(poweredby,"Powered by\n%s",version_string);
		noise=TCOD_noise_new(1,TCOD_NOISE_DEFAULT_HURST,TCOD_NOISE_DEFAULT_LACUNARITY,NULL);
		init1=true;
	}
	if (!init2) {
		// reset the credits vars ...
		int curx,cury;
		xstr=-4.0f;
		len=strlen(poweredby);
		len1=11; // sizeof "Powered by\n"
		curx=x;
		cury=y;
		for (i=0; i < len ;i++) {
			char_heat[i]=-1;
			char_x[i]=curx;
			char_y[i]=cury;
			curx++;
			if ( poweredby[i] == '\n' ) {
				curx=x;
				cury++;
			}
		}
		init2=true;
	}
	// draw the text
	for (i=0; i < len ;i++) {
		if ( char_heat[i] >= 0.0f && poweredby[i]!='\n') {
			int colidx=(int)(64*char_heat[i]);
			TCOD_color_t col;
			if ( colidx > 63 ) colidx=63;
			col=colmap[colidx];
			if ( xstr >= len  ) {
				float coef=(xstr-len)/len;
				if ( alpha ) {
					TCOD_color_t fore=TCOD_console_get_back(NULL,char_x[i],char_y[i]);
					int r=(int)(coef*fore.r + (1.0f-coef)*col.r);
					int g=(int)(coef*fore.g + (1.0f-coef)*col.g);
					int b=(int)(coef*fore.b + (1.0f-coef)*col.b);
					col.r = CLAMP(0,255,r);
					col.g = CLAMP(0,255,g);
					col.b = CLAMP(0,255,b);
					TCOD_console_set_fore(NULL,char_x[i],char_y[i],col);
				} else {
					col=TCOD_color_lerp(col,TCOD_black,coef);
				}
			}
			TCOD_console_set_char(NULL,char_x[i],char_y[i],poweredby[i]);
			TCOD_console_set_fore(NULL,char_x[i],char_y[i],col);
		}
	}
	if ( xstr < (float)len1 ) {
		sparklex=x+xstr;
		sparkley=(float)y;
	} else {
		sparklex=x-len1+xstr;
		sparkley=(float)y+1;
	}
	noisex=xstr*6;
	sparklerad=3.0f+2*TCOD_noise_simplex(noise,&noisex);
	if ( xstr >= len-1 ) sparklerad -= (xstr-len+1)*4.0f;
	else if ( xstr < 0.0f ) sparklerad += xstr*4.0f;
	else if ( poweredby[ (int)(xstr+0.5f) ] == ' ' || poweredby[ (int)(xstr+0.5f) ] == '\n' ) sparklerad/=2;
	sparklerad2=sparklerad*sparklerad;
	// draw the light
	for (xc=MAX(x-4,0); xc < MIN(x+len,cw-1); xc++) {
		for (yc=MAX(y-4,0); yc < MAX(y+6,ch-1); yc++) {
			float dist=((xc-sparklex)*(xc-sparklex)+(yc-sparkley)*(yc-sparkley));
			TCOD_color_t newback;
			if ( sparklerad >= 0.0f && dist < sparklerad2 ) {
				int colidx=63-(int)(63*(sparklerad2-dist)/sparklerad2);
				newback=colmap_light[colidx];
			} else {
				newback=TCOD_black;
			}
			if ( alpha ) {
				TCOD_color_t curback=TCOD_console_get_back(NULL,xc,yc);
				newback = TCOD_color_operation(curback,newback,TCOD_COLOROP_ADD);
			}
			TCOD_console_set_back(NULL,xc,yc,newback);
		}
	}
	// update letters heat
	xstr += TCOD_sys_get_last_frame_length() * 4;
	for (i=0; i < (int)(xstr+0.5f); i++) {
		char_heat[i]=(xstr-i)/(len/2);
	}
	if ( xstr <= 2*len ) return false;
	init2=false;
	return true;
}


