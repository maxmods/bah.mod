/*
* Copyright (c) 2008-2009, Bruce A Henderson
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Bruce A Henderson nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "glue.h"

bool has_startup=false;

static int fullscreen_width=0;
static int fullscreen_height=0;

// number of characters in the bitmap font
int fontNbCharHoriz=16;
int fontNbCharVertic=16;


// convert ASCII code to TCOD layout position
static int init_ascii_to_tcod[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0, 76, 77,  0,  0,  0,  0,  0, // ASCII 0 to 15
 71, 70, 72,  0,  0,  0,  0,  0, 64, 65, 67, 66,  0, 73, 68, 69, // ASCII 16 to 31
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, // ASCII 32 to 47
 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, // ASCII 48 to 63
 32, 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110, // ASCII 64 to 79
111,112,113,114,115,116,117,118,119,120,121, 33, 34, 35, 36, 37, // ASCII 80 to 95
 38,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142, // ASCII 96 to 111
143,144,145,146,147,148,149,150,151,152,153, 39, 40, 41, 42,  0, // ASCII 112 to 127
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // ASCII 128 to 143
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // ASCII 144 to 159
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // ASCII 160 to 175
 43, 44, 45, 46, 49,  0,  0,  0,  0, 81, 78, 87, 88,  0,  0, 55, // ASCII 176 to 191
 53, 50, 52, 51, 47, 48,  0,  0, 85, 86, 82, 84, 83, 79, 80,  0, // ASCII 192 to 207
  0,  0,  0,  0,  0,  0,  0,  0,  0, 56, 54,  0,  0,  0,  0,  0, // ASCII 208 to 223
 74, 75, 57, 58, 59, 60, 61, 62, 63,  0,  0,  0,  0,  0,  0,  0, // ASCII 224 to 239
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // ASCII 240 to 255
};


static TCOD_color_t *charcols=NULL;

static bool *first_draw=NULL;


int *ascii_to_tcod=NULL;
static int max_font_chars=0;

static bool *ascii_updated=NULL;
static bool any_ascii_updated=false;

// total number of characters that can fit in the font bitmap
int TCOD_max_font_chars=255;

// number of characters in the bitmap font
int TCOD_font_nb_char_horiz=16;
int TCOD_font_nb_char_vertic=16;


// console size (in cells)
TCOD_console_data_t *TCOD_root=NULL;
TCOD_bitmap_char_t * TCOD_font_chars = NULL;


extern "C" {

	void _bah_libtcod_TCODSystem__Flush(bool render);
	bool _bah_libtcod_TCODSystem__Init(TCOD_console_data_t *con, int w, int h, char_t *buf, char_t *oldbuf, bool fullscreen);
	void _bah_libtcod_TCODSystem__SleepMilli(uint32 milliseconds);
	uint32 _bah_libtcod_TCODSystem__ElapsedMilli();
	void _bah_libtcod_TCODSystem__SetCustomFont(BBString * fontFile, int flags, int nbcw, int nbch);
	void _bah_libtcod_TCODSystem__ConsoleToBitmap(void *vbitmap, TCOD_console_data_t * con, int console_width, int console_height, char_t *console_buffer, char_t *prev_console_buffer);
	void _bah_libtcod_TCODSystem__SetFps(int val);
	int _bah_libtcod_TCODSystem__GetFps();
	float _bah_libtcod_TCODSystem__LastFrameLength();
	void _bah_libtcod_TCODSystem__GetCharSize(int * w, int * h);
	BBObject * _bah_libtcod_TCODSystem__LoadImage(BBString * filename);
	int _bah_libtcod_TCODSystem__ImageAlpha(BBObject * image, int x, int y);
	void _bah_libtcod_TCODSystem__ImageSize(BBObject * image, int * w, int * h);
	void _bah_libtcod_TCODSystem__ImagePixel(BBObject * image, int x, int y, uint8 * r, uint8 * g, uint8 * b);
	void _bah_libtcod_TCODSystem__SetFullscreen(bool fullscreen);
	void _bah_libtcod_TCODSystem__ShowCursor(bool visible);
	bool _bah_libtcod_TCODSystem__CursorVisible();
	void _bah_libtcod_TCODSystem__MoveMouse(int x, int y);
	void _bah_libtcod_TCODMouse__Status(int * x, int * y, int * _dx, int * _dy, int * mb1, int * mb2, int * mb3, int * wheel, int * fsx, int * fsy);


static void alloc_ascii_tables() {
	if ( ascii_to_tcod ) free(ascii_to_tcod);
	if ( ascii_updated ) free(ascii_updated);
	if ( charcols ) {
		free(charcols);
		free(first_draw);
	}

	TCOD_font_chars=(TCOD_bitmap_char_t *)calloc(sizeof(TCOD_bitmap_char_t),TCOD_max_font_chars);
	for (int i=0; i < 256; i++ ) {
		TCOD_font_chars[i].ascii_to_tcod=init_ascii_to_tcod[i];
	}
	for (int i=256; i < TCOD_max_font_chars; i++ ) {
		TCOD_font_chars[i].ascii_to_tcod=i;
	}

	
	ascii_to_tcod = (int *)calloc(sizeof(int),max_font_chars);
	ascii_updated = (bool *)calloc(sizeof(bool),max_font_chars);
	charcols = (TCOD_color_t *)calloc(sizeof(TCOD_color_t),max_font_chars);
	first_draw =(bool *)calloc(sizeof(bool),max_font_chars);
	memcpy(ascii_to_tcod,init_ascii_to_tcod,sizeof(int)*256);
}


void TCOD_sys_startup() {
	if (has_startup) return;
	max_font_chars=256;
	alloc_ascii_tables();
	has_startup=true;
}


void TCOD_sys_set_custom_font(const char *fontFile, int nbcw, int nbch, int flags) {
	_bah_libtcod_TCODSystem__SetCustomFont(bbStringFromCString(fontFile), flags, nbcw, nbch);
}

void *TCOD_sys_create_bitmap_for_console(TCOD_console_t console) {
printf("TCOD_sys_create_bitmap_for_console\n");fflush(stdout);
}

void TCOD_sys_console_to_bitmap(void *bitmap, TCOD_console_data_t *con) {
	_bah_libtcod_TCODSystem__ConsoleToBitmap(bitmap, con, con->w, con->h, con->buf, con->oldbuf);
}


void TCOD_sys_set_keyboard_repeat(int initial_delay, int interval) {
printf("TCOD_sys_set_keyboard_repeat\n");fflush(stdout);
}

bool TCOD_sys_init(TCOD_console_data_t *con, bool fullscreen) {
	if ( ! has_startup ) TCOD_sys_startup();
	
	return _bah_libtcod_TCODSystem__Init(con, con->w, con->h, con->buf, con->oldbuf, fullscreen);

}

void TCOD_sys_save_bitmap(void *bitmap, const char *filename) {
printf("TCOD_sys_save_bitmap\n");fflush(stdout);
}

void TCOD_sys_save_screenshot(const char *filename) {
printf("TCOD_sys_save_screenshot\n");fflush(stdout);
}

void TCOD_sys_set_fullscreen(bool fullscreen) {
	_bah_libtcod_TCODSystem__SetFullscreen(fullscreen);
}

void TCOD_sys_set_window_title(const char *title) {
	bbAppTitle = bbStringFromCString(title);
}

void TCOD_sys_flush(bool render) {

	_bah_libtcod_TCODSystem__Flush(render);

}

bool TCOD_sys_is_key_pressed(TCOD_keycode_t key) {
printf("TCOD_sys_is_key_pressed\n");fflush(stdout);
}

TCOD_key_t TCOD_sys_check_for_keypress(int flags) {
printf("TCOD_sys_check_for_keypress\n");fflush(stdout);
}

TCOD_key_t TCOD_sys_wait_for_keypress(bool flush) {
printf("TCOD_sys_wait_for_keypress\n");fflush(stdout);
}

void TCOD_sys_sleep_milli(uint32 milliseconds) {
	_bah_libtcod_TCODSystem__SleepMilli(milliseconds);
}

void TCOD_sys_term() {
}

void *TCOD_sys_load_image(const char *filename) {
	BBObject * obj = _bah_libtcod_TCODSystem__LoadImage(bbStringFromCString(filename));
	BBRETAIN(obj);
	return obj;
}

void TCOD_sys_get_image_size(const void *image, int *w, int *h) {
	_bah_libtcod_TCODSystem__ImageSize((BBObject*)image, w, h);
}

TCOD_color_t TCOD_sys_get_image_pixel(const void *image,int x, int y) {
	TCOD_color_t ret;
	uint8 r, g, b;
	_bah_libtcod_TCODSystem__ImagePixel((BBObject*)image, x, y, &r, &g, &b);
	ret.r = r;
	ret.g = g;
	ret.b = b;
	return ret;
}

int TCOD_sys_get_image_alpha(const void *image,int x, int y) {
	return _bah_libtcod_TCODSystem__ImageAlpha((BBObject*)image, x, y);
}

void TCOD_sys_delete_bitmap(void *bitmap) {
	BBRELEASE((BBObject*)bitmap);
}

uint32 TCOD_sys_elapsed_milli() {
	return _bah_libtcod_TCODSystem__ElapsedMilli();
}

float TCOD_sys_elapsed_seconds() {
	static float div=1.0f/1000.0f;
	return _bah_libtcod_TCODSystem__ElapsedMilli() * div;
}

void TCOD_sys_force_fullscreen_resolution(int width, int height) {
	fullscreen_width=width;
	fullscreen_height=height;
}

void * TCOD_sys_create_bitmap(int width, int height, TCOD_color_t *buf) {
printf("TCOD_sys_create_bitmap\n");fflush(stdout);
}

void TCOD_sys_set_fps(int val) {
	_bah_libtcod_TCODSystem__SetFps(val);
}

int TCOD_sys_get_fps() {
	return _bah_libtcod_TCODSystem__GetFps();
}

float TCOD_sys_get_last_frame_length() {
	return _bah_libtcod_TCODSystem__LastFrameLength();
}

void TCOD_sys_get_char_size(int *w, int *h) {
	_bah_libtcod_TCODSystem__GetCharSize(w, h);
}

void TCOD_sys_get_current_resolution(int *w, int *h) {
printf("TCOD_sys_get_current_resolution\n");fflush(stdout);
}

void TCOD_sys_save_fps() {
printf("TCOD_sys_save_fps\n");fflush(stdout);
}

void TCOD_sys_restore_fps() {
printf("TCOD_sys_restore_fps\n");fflush(stdout);
}

void TCOD_sys_map_ascii_to_font(int asciiCode, int fontCharX, int fontCharY) {
printf("TCOD_sys_map_ascii_to_font\n");fflush(stdout);
}


void TCOD_sys_update_char(int asciiCode, int fontx, int fonty, TCOD_image_t img, int x, int y) {

printf("TCOD_sys_update_char\n");fflush(stdout);
}

TCOD_mouse_t TCOD_mouse_get_status() {
	TCOD_mouse_t ms;
	int charWidth, charHeight;
	int wheel, mb1, mb2, mb3, fsx, fsy;
	
	_bah_libtcod_TCODMouse__Status(&ms.x, &ms.y, &ms.dx, &ms.dy, &mb1, &mb2, &mb3, &wheel, &fsx, &fsy);

	ms.lbutton = (mb1) ? true : false;
	ms.rbutton = (mb2) ? true : false;
	ms.mbutton = (mb3) ? true : false;
	ms.wheel_up = (wheel > 0) ?  1 : 0;
	ms.wheel_down = (wheel < 0) ? 1 : 0;
	
	// FIXME : implement this properly....
	ms.lbutton_pressed = false;
	ms.rbutton_pressed = false;
	ms.mbutton_pressed = false;

	TCOD_sys_get_char_size(&charWidth,&charHeight);
	ms.cx = (ms.x - fsx) / charWidth;
	ms.cy = (ms.y - fsy) / charHeight;
	ms.dcx = ms.dx / charWidth;
	ms.dcy = ms.dy / charHeight;
	return ms;
}

void TCOD_mouse_show_cursor(uint8 visible) {
	_bah_libtcod_TCODSystem__ShowCursor(visible);
}

uint8 TCOD_mouse_is_cursor_visible() {
	return _bah_libtcod_TCODSystem__CursorVisible();
}

void TCOD_mouse_move(int x, int y) {
	_bah_libtcod_TCODSystem__MoveMouse(x, y);
}

void *TCOD_sys_get_image_pixel_data(TCOD_image_t img) {
	return NULL;
}



}







