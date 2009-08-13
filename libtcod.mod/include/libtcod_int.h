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

#ifndef _TCODLIB_INT_H
#define _TCODLIB_INT_H
#include <stdarg.h>
/* tcodlib internal stuff */
#ifdef __cplusplus
extern "C" {
#endif

/* a cell in the console */
typedef struct {
	int c;		/* character ascii code */
	int cf;		/* character number in font */
	uint8 dirt;	/* cell modified since last flush ? */
} char_t;

typedef struct {
	/* foreground brush */
	TCOD_color_t fore;
	TCOD_colorop_t foreflag;
	/* background brush */
	TCOD_color_t back;
	TCOD_colorop_t backflag;
} TCOD_brush_t;

/* TCODConsole non public data */
typedef struct {
	char_t *buf; /* current console */
	char_t *oldbuf; /* console for last frame */
	TCOD_image_t forebuf,oldforebuf;	/* foreground colors */
	TCOD_image_t backbuf,oldbackbuf;	/* background colors */
	uint8 fade;
	bool windowed;
	bool fullscreen;
	bool haskey; /* a key color has been defined */
	/* key color */
	TCOD_color_t key;
	TCOD_brush_t brush;
	TCOD_brush_t *brushes;
	int nbBrushes, maxBrushes;
	/* console width and height (in characters,not pixels) */
	int w,h;
} TCOD_console_data_t;

typedef enum {
	LEFT,CENTER,RIGHT
} alignment_t;

/* fov internal stuff */
typedef struct {
	unsigned int transparent:1;
	unsigned int walkable:1;
	unsigned int fov:1;
} cell_t;
typedef struct {
	int width;
	int height;
	int nbcells;
	cell_t *cells;
} map_t;

/* bitmap character data */
typedef struct {
	/* current color of the character in the font bitmap (for non opengl renderers) */
	TCOD_color_t col;
	/* convert ascii code to tcod layout code */
	int ascii_to_tcod;
	/* wether the character has been updated in the font */
	bool updated;
	/* wether this is the first time this character is drawn */
	bool first;
	/* wether this character is a colored tile */
	bool colored;
} TCOD_bitmap_char_t;

/* internal configuration */
typedef struct {
	bool disableOpenGL;
} TCOD_config_t;

#if 1
#define TCOD_IF(x) if (x)
#define TCOD_IFNOT(x) if (!(x))
#define TCOD_ASSERT(x)
#else
#include <assert.h>
#define TCOD_IF(x) assert(x);
#define TCOD_IFNOT(x) assert(x); if (0)
#define TCOD_ASSERT(x) assert(x)
#endif

void TCOD_map_compute_fov_circular_raycasting(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls);
void TCOD_map_compute_fov_diamond_raycasting(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls);
void TCOD_map_compute_fov_recursive_shadowcasting(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls);
void TCOD_map_compute_fov_permissive(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls);
void TCOD_map_compute_fov_permissive2(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls, int fovType);
void TCOD_map_compute_fov_restrictive_shadowcasting(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls);
void TCOD_map_postproc(map_t *map,int x0,int y0, int x1, int y1, int dx, int dy);

/* TCODConsole non public methods*/
bool TCOD_console_init(TCOD_console_t con,const char *title, bool fullscreen);
int TCOD_console_print(TCOD_console_t con,int x,int y, int w, int h, alignment_t align, char *msg, bool can_split, bool count_only);
int TCOD_console_stringLength(const char *s);
char * TCOD_console_forward(char *s,int l);
void TCOD_console_set_window_closed();
char *TCOD_console_vsprint(const char *fmt, va_list ap);
char_t *TCOD_console_get_buf(TCOD_console_t con);
/* fatal errors */
void TCOD_fatal(const char *fmt, ...);
void TCOD_fatal_nopar(const char *msg);

/* TCODSystem non public methods */
void TCOD_sys_startup();
bool TCOD_sys_init(TCOD_console_data_t *con, bool fullscreen);
void TCOD_sys_set_custom_font(const char *font_name,int nb_ch, int nb_cv,int flags);
void TCOD_sys_map_ascii_to_font(int asciiCode, int fontCharX, int fontCharY); 
void *TCOD_sys_create_bitmap_for_console(TCOD_console_t console);
void TCOD_sys_save_bitmap(void *bitmap, const char *filename);
void *TCOD_sys_create_bitmap(int width, int height, TCOD_color_t *buf);
void TCOD_sys_delete_bitmap(void *bitmap);
void TCOD_sys_console_to_bitmap(void *bitmap, TCOD_console_data_t *con);
void TCOD_sys_get_char_size(int *w, int *h);
void TCOD_sys_set_keyboard_repeat(int initial_delay, int interval);
void *TCOD_sys_get_surface(int width, int height, bool alpha);
void TCOD_sys_save_fps();
void TCOD_sys_restore_fps();
void *TCOD_sys_get_image_pixel_data(TCOD_image_t);
// renderers internals
void TCOD_sys_console_to_bitmap_nocharmap(void *vbitmap, TCOD_console_data_t *con, bool new_font);
void TCOD_sys_console_to_bitmap_32bits(void *vbitmap, TCOD_console_data_t *con, bool new_font);
void TCOD_sys_console_to_bitmap_24bits(void *vbitmap, TCOD_console_data_t *con, bool new_font);
void TCOD_sys_console_to_bitmap_opengl(void *vbitmap, TCOD_console_data_t *con, bool new_font);
void TCOD_sys_flush_SDL();
void TCOD_sys_flush_OPENGL();
void * TCOD_sys_get_screen_surface_OPENGL();
void * TCOD_sys_get_screen_surface_SDL();
// TCODSystem non public data
extern int TCOD_font_nb_char_horiz;
extern int TCOD_font_nb_char_vertic;
extern int TCOD_old_fade;
extern TCOD_bitmap_char_t * TCOD_font_chars;
extern void* TCOD_screen;
extern int TCOD_font_width;
extern int TCOD_font_height;
extern bool TCOD_fullscreen_on;
extern int TCOD_fullscreen_offsetx;
extern int TCOD_fullscreen_offsety;
extern int TCOD_max_font_chars;
extern bool TCOD_any_ascii_updated;
extern TCOD_color_t TCOD_font_key_col;
extern bool TCOD_font_is_in_row, TCOD_font_is_greyscale, TCOD_font_has_tcod_layout;
extern int32 TCOD_sdl_key, TCOD_rgb_mask, TCOD_nrgb_mask;
extern void* TCOD_charmap;
extern TCOD_console_data_t *TCOD_root;
extern TCOD_config_t TCOD_config;

/* switch fullscreen mode */
void TCOD_sys_set_fullscreen(bool fullscreen);
void TCOD_sys_flush(bool render);
TCOD_key_t TCOD_sys_check_for_keypress(int flags);
TCOD_key_t TCOD_sys_wait_for_keypress(bool flush);
bool TCOD_sys_is_key_pressed(TCOD_keycode_t key);
void TCOD_sys_set_window_title(const char *title);
/* close the window */
void TCOD_sys_term();

/* image manipulation */
void *TCOD_sys_load_image(const char *filename);
void TCOD_sys_get_image_size(const void *image, int *w,int *h);
TCOD_color_t TCOD_sys_get_image_pixel(const void *image,int x, int y);
int TCOD_sys_get_image_alpha(const void *image,int x, int y);
bool TCOD_sys_check_magic_number(const char *filename, int size, uint8 *data);

/* TCOD_list nonpublic methods */
void TCOD_list_set_size(TCOD_list_t l, int size);

/* color values */
#define TCOD_BLACK 0,0,0
#define TCOD_DARKER_GREY 31,31,31
#define TCOD_DARK_GREY 63,63,63
#define TCOD_GREY 128,128,128
#define TCOD_LIGHT_GREY 191,191,191
#define TCOD_WHITE 255,255,255

//standard colors
#define TCOD_RED 255,0,0
#define TCOD_ORANGE 255,127,0
#define TCOD_YELLOW 255,255,0
#define TCOD_CHARTREUSE 127,255,0
#define TCOD_GREEN 0,255,0
#define TCOD_SEA 0,255,127
#define TCOD_CYAN 0,255,255
#define TCOD_SKY 0,127,255
#define TCOD_BLUE 0,0,255
#define TCOD_VIOLET 127,0,255
#define TCOD_MAGENTA 255,0,255
#define TCOD_PINK 255,0,127

// dark colors
#define TCOD_DARK_RED 127,0,0
#define TCOD_DARK_ORANGE 127,63,0
#define TCOD_DARK_YELLOW 127,127,0
#define TCOD_DARK_CHARTREUSE 63,127,0
#define TCOD_DARK_GREEN 0,127,0
#define TCOD_DARK_SEA 0,127,63
#define TCOD_DARK_CYAN 0,127,127
#define TCOD_DARK_SKY 0,63,127
#define TCOD_DARK_BLUE 0,0,127
#define TCOD_DARK_VIOLET 63,0,127
#define TCOD_DARK_MAGENTA 127,0,127
#define TCOD_DARK_PINK 127,0,63

// darker colors
#define TCOD_DARKER_RED 63,0,0
#define TCOD_DARKER_ORANGE 63,31,0
#define TCOD_DARKER_YELLOW 63,63,0
#define TCOD_DARKER_CHARTREUSE 31,63,0
#define TCOD_DARKER_GREEN 0,63,0
#define TCOD_DARKER_SEA 0,63,31
#define TCOD_DARKER_CYAN 0,63,63
#define TCOD_DARKER_SKY 0,31,63
#define TCOD_DARKER_BLUE 0,0,63
#define TCOD_DARKER_VIOLET 31,0,63
#define TCOD_DARKER_MAGENTA 63,0,63
#define TCOD_DARKER_PINK 63,0,31

// light colors
#define TCOD_LIGHT_RED 255,127,127
#define TCOD_LIGHT_ORANGE 255,191,127
#define TCOD_LIGHT_YELLOW 255,255,127
#define TCOD_LIGHT_CHARTREUSE 191,255,127
#define TCOD_LIGHT_GREEN 127,255,127
#define TCOD_LIGHT_SEA 127,255,191
#define TCOD_LIGHT_CYAN 127,255,255
#define TCOD_LIGHT_SKY 127,191,255
#define TCOD_LIGHT_BLUE 127,127,255
#define TCOD_LIGHT_VIOLET 191,127,255
#define TCOD_LIGHT_MAGENTA 255,127,255
#define TCOD_LIGHT_PINK 255,127,191

// desaturated colors
#define TCOD_DESATURATED_RED 127,63,63
#define TCOD_DESATURATED_ORANGE 127,95,63
#define TCOD_DESATURATED_YELLOW 127,127,63
#define TCOD_DESATURATED_CHARTREUSE 95,127,63
#define TCOD_DESATURATED_GREEN 63,127,63
#define TCOD_DESATURATED_SEA 63,127,95
#define TCOD_DESATURATED_CYAN 63,127,127
#define TCOD_DESATURATED_SKY 63,95,127
#define TCOD_DESATURATED_BLUE 63,63,127
#define TCOD_DESATURATED_VIOLET 95,63,127
#define TCOD_DESATURATED_MAGENTA 127,63,127
#define TCOD_DESATURATED_PINK 127,63,95

// special
#define TCOD_SILVER 203,203,203
#define TCOD_GOLD 255,255,102


#ifdef __cplusplus
}
#endif
#endif

