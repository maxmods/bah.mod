
#include "libtcod.hpp"
#include "libtcod_int.h"

class MaxTCODColor;
class MaxTCODConsole;
class MaxTCODKey;

extern "C" {

	#include "blitz.h"

	MaxTCODColor * bmx_tcodcolor_create(int r, int g, int b);
	void bmx_tcodcolor_delete(MaxTCODColor * color);
	bool bmx_tcodcolor_equals(MaxTCODColor * color, MaxTCODColor * other);
	void bmx_tcodcolor_updatecolors(MaxTCODColor * color, int * red, int * green, int * blue);
	void bmx_tcodcolor_setr(MaxTCODColor * color, int value);
	void bmx_tcodcolor_setg(MaxTCODColor * color, int value);
	void bmx_tcodcolor_setb(MaxTCODColor * color, int value);
	MaxTCODColor * bmx_tcodcolor_lerp(MaxTCODColor * a, MaxTCODColor * b, float coef);

	MaxTCODColor * bmx_tcodcolor_black();
	MaxTCODColor * bmx_tcodcolor_darkgrey();
	MaxTCODColor * bmx_tcodcolor_grey();
	MaxTCODColor * bmx_tcodcolor_lightgrey();
	MaxTCODColor * bmx_tcodcolor_white();
	MaxTCODColor * bmx_tcodcolor_red();
	MaxTCODColor * bmx_tcodcolor_orange();
	MaxTCODColor * bmx_tcodcolor_chartreuse();
	MaxTCODColor * bmx_tcodcolor_yellow();
	MaxTCODColor * bmx_tcodcolor_green();
	MaxTCODColor * bmx_tcodcolor_sea();
	MaxTCODColor * bmx_tcodcolor_cyan();
	MaxTCODColor * bmx_tcodcolor_sky();
	MaxTCODColor * bmx_tcodcolor_blue();
	MaxTCODColor * bmx_tcodcolor_violet();
	MaxTCODColor * bmx_tcodcolor_magenta();
	MaxTCODColor * bmx_tcodcolor_pink();
	MaxTCODColor * bmx_tcodcolor_darkred();
	MaxTCODColor * bmx_tcodcolor_darkorange();
	MaxTCODColor * bmx_tcodcolor_darkyellow();
	MaxTCODColor * bmx_tcodcolor_darkchartreuse();
	MaxTCODColor * bmx_tcodcolor_darkgreen();
	MaxTCODColor * bmx_tcodcolor_darksea();
	MaxTCODColor * bmx_tcodcolor_darkcyan();
	MaxTCODColor * bmx_tcodcolor_darksky();
	MaxTCODColor * bmx_tcodcolor_darkblue();
	MaxTCODColor * bmx_tcodcolor_darkviolet();
	MaxTCODColor * bmx_tcodcolor_darkmagenta();
	MaxTCODColor * bmx_tcodcolor_darkpink();
	MaxTCODColor * bmx_tcodcolor_darkerred();
	MaxTCODColor * bmx_tcodcolor_darkerorange();
	MaxTCODColor * bmx_tcodcolor_darkeryellow();
	MaxTCODColor * bmx_tcodcolor_darkerchartreuse();
	MaxTCODColor * bmx_tcodcolor_darkergreen();
	MaxTCODColor * bmx_tcodcolor_darkersea();
	MaxTCODColor * bmx_tcodcolor_darkercyan();
	MaxTCODColor * bmx_tcodcolor_darkersky();
	MaxTCODColor * bmx_tcodcolor_darkerblue();
	MaxTCODColor * bmx_tcodcolor_darkerviolet();
	MaxTCODColor * bmx_tcodcolor_darkermagenta();
	MaxTCODColor * bmx_tcodcolor_darkerpink();
	MaxTCODColor * bmx_tcodcolor_lightred();
	MaxTCODColor * bmx_tcodcolor_lightorange();
	MaxTCODColor * bmx_tcodcolor_lightyellow();
	MaxTCODColor * bmx_tcodcolor_lightchartreuse();
	MaxTCODColor * bmx_tcodcolor_lightgreen();
	MaxTCODColor * bmx_tcodcolor_lightsea();
	MaxTCODColor * bmx_tcodcolor_lightcyan();
	MaxTCODColor * bmx_tcodcolor_lightsky();
	MaxTCODColor * bmx_tcodcolor_lightblue();
	MaxTCODColor * bmx_tcodcolor_lightviolet();
	MaxTCODColor * bmx_tcodcolor_lightmagenta();
	MaxTCODColor * bmx_tcodcolor_lightpink();
	MaxTCODColor * bmx_tcodcolor_desaturatedred();
	MaxTCODColor * bmx_tcodcolor_desaturatedorange();
	MaxTCODColor * bmx_tcodcolor_desaturatedyellow();
	MaxTCODColor * bmx_tcodcolor_desaturatedchartreuse();
	MaxTCODColor * bmx_tcodcolor_desaturatedgreen();
	MaxTCODColor * bmx_tcodcolor_desaturatedsea();
	MaxTCODColor * bmx_tcodcolor_desaturatedcyan();
	MaxTCODColor * bmx_tcodcolor_desaturatedsky();
	MaxTCODColor * bmx_tcodcolor_desaturatedblue();
	MaxTCODColor * bmx_tcodcolor_desaturatedviolet();
	MaxTCODColor * bmx_tcodcolor_desaturatedmagenta();
	MaxTCODColor * bmx_tcodcolor_desaturatedpink();
	MaxTCODColor * bmx_tcodcolor_silver();
	MaxTCODColor * bmx_tcodcolor_gold();


	MaxTCODConsole * bmx_tcodconsole_initroot(BBObject * handle, int w, int h, BBString * title, bool fullscreen);
	void bmx_tcodconsole_free(MaxTCODConsole * console);
	void bmx_tcodconsole_setcustomfont(BBString *fontFile, int flags, int nbCharHoriz, int nbCharVertic);
	bool bmx_tcodconsole_iswindowclosed();
	void bmx_tcodconsole_flush();
	MaxTCODKey * bmx_tcodconsole_checkforkeypress();
	void bmx_tcodconsole_setforegroundcolor(MaxTCODConsole * console, MaxTCODColor * color);
	void bmx_tcodconsole_printright(MaxTCODConsole * console, int x, int y, TCOD_bkgnd_flag_t flag, BBString * text);
	void bmx_tcodconsole_printleft(MaxTCODConsole * console, int x, int y, TCOD_bkgnd_flag_t flag, BBString * text);
	MaxTCODConsole * bmx_tcodconsole_create(BBObject * handle, int width, int height);
	bool bmx_tcodconsole_isfullscreen();
	void bmx_tcodconsole_clear(MaxTCODConsole * console);
	MaxTCODColor * bmx_tcodconsole_getforegroundcolor(MaxTCODConsole * console);
	MaxTCODColor * bmx_tcodconsole_getbackgroundcolor(MaxTCODConsole * console);
	void bmx_tcodconsole_setbackgroundcolor(MaxTCODConsole * console, MaxTCODColor * color);
	void bmx_tcodconsole_setfore(MaxTCODConsole * console, int x, int y, MaxTCODColor * col);
	MaxTCODColor * bmx_tcodconsole_getfore(MaxTCODConsole * console, int x, int y);
	void bmx_tcodconsole_setback(MaxTCODConsole * console, int x, int y, MaxTCODColor * col, TCOD_bkgnd_flag_t flag);
	MaxTCODColor * bmx_tcodconsole_getback(MaxTCODConsole * console, int x, int y);
	void bmx_tcodconsole_printcenter(MaxTCODConsole * console, int x, int y, TCOD_bkgnd_flag_t flag, BBString * text);
	int bmx_tcodconsole_printrightrect(MaxTCODConsole * console, int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, BBString * text);
	int bmx_tcodconsole_printleftrect(MaxTCODConsole * console, int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, BBString * text);
	int bmx_tcodconsole_printcenterrect(MaxTCODConsole * console, int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, BBString * text);
	void bmx_tcodconsole_putchar(MaxTCODConsole * console, int x, int y, int c, TCOD_bkgnd_flag_t flag);
	void bmx_tcodconsole_blit(MaxTCODConsole * source, int x, int y, int width, int height, MaxTCODConsole * dest, int posX, int posY, int fade);
	void bmx_tcodconsole_setchar(MaxTCODConsole * console, int x, int y, int c);
	void bmx_tcodconsole_rect(MaxTCODConsole * console, int x, int y, int w, int h, bool clear, TCOD_bkgnd_flag_t flag);
	void bmx_tcodconsole_hline(MaxTCODConsole * console, int x, int y, int length, TCOD_bkgnd_flag_t flag);
	void bmx_tcodconsole_vline(MaxTCODConsole * console, int x, int y, int length, TCOD_bkgnd_flag_t flag);
	void bmx_tcodconsole_printframe(MaxTCODConsole * console, int x, int y, int w, int h, bool clear, BBString * fmt);
	void bmx_tcodconsole_credits();
	bool bmx_tcodconsole_rendercredits(int x, int y, bool alpha);
	void bmx_tcodconsole_setfullscreen(bool fullscreen);
	int bmx_tcodconsole_getchar(MaxTCODConsole * console, int x, int y);
	int bmx_tcodconsole_getheightleftrect(MaxTCODConsole * console, int x, int y, int w, int h, BBString * text);
	int bmx_tcodconsole_getheightcenterrect(MaxTCODConsole * console, int x, int y, int w, int h, BBString * text);
	int bmx_tcodconsole_getheightrightrect(MaxTCODConsole * console, int x, int y, int w, int h, BBString * text);

	uint32 bmx_tcodsystem_getelapsedmilli();
	float bmx_tcodsystem_getelapsedseconds();
	void bmx_tcodsystem_sleepmilli(uint32 value);
	void bmx_tcodsystem_setfps(int value);
	int bmx_tcodsystem_getfps();
	float bmx_tcodsystem_getlastframelength();
	void bmx_tcodsystem_getcurrentresolution(int * w, int * h);
	void bmx_tcodsystem_getcolsandascii(char_t * buf, int lookup, int * index, int * br, int * bg, int * bb, int * fr, int * fg, int * fb, int flags);

	TCODRandom * bmx_tcodrandom_getinstance();
	int bmx_tcodrandom_getint(TCODRandom * rand, int _min, int _max);
	float bmx_tcodrandom_getfloat(TCODRandom * rand, float _min, float _max);
	TCODRandom * bmx_tcodrandom_create(uint32 seed, bool useSeed);
	void bmx_tcodrandom_delete(TCODRandom * rand);

	void bmx_tcodline_init(int xFrom, int yFrom, int xTo, int yTo);
	bool bmx_tcodline_dostep(int * xCur, int * yCur);

	TCODMap * bmx_tcodmap_create(int width, int height);
	void bmx_tcodmap_setproperties(TCODMap * map, int x, int y, bool isTransparent, bool isWalkable);
	void bmx_tcodmap_clear(TCODMap * map);
	void bmx_tcodmap_copy(TCODMap * map, TCODMap * source);
	void bmx_tcodmap_computefov(TCODMap * map, int playerX, int playerY, int maxRadius, bool lightWalls, TCOD_fov_algorithm_t algo);
	bool bmx_tcodmap_isinfov(TCODMap * map, int x, int y);
	bool bmx_tcodmap_istransparent(TCODMap * map, int x, int y);
	bool bmx_tcodmap_iswalkable(TCODMap * map, int x, int y);
	void bmx_tcodmap_delete(TCODMap * map);

	TCODPath * bmx_tcodpath_create(TCODMap * map, float diagonalCost);
	bool bmx_tcodpath_compute(TCODPath * path, int ox, int oy, int dx, int dy);
	void bmx_tcodpath_getorigin(TCODPath * path, int * x, int * y);
	void bmx_tcodpath_getdestination(TCODPath * path, int * x, int * y);
	int bmx_tcodpath_size(TCODPath * path);
	void bmx_tcodpath_get(TCODPath * path, int index, int * x, int * y);
	bool bmx_tcodpath_isempty(TCODPath * path);
	bool bmx_tcodpath_walk(TCODPath * path, int * x, int * y, bool recalculateWhenNeeded);
	void bmx_tcodpath_delete(TCODPath * path);

	TCODNoise * bmx_tcodnoise_create(int dimensions, float hurst, float lacunarity, TCODRandom * generator);
	void bmx_tcodnoise_delete(TCODNoise * noise);
	float bmx_tcodnoise_getperlin(TCODNoise * noise, BBArray * f);
	float bmx_tcodnoise_getfbmperlin(TCODNoise * noise, BBArray * f, float octaves);
	float bmx_tcodnoise_getturbulenceperlin(TCODNoise * noise, BBArray * f, float octaves);
	float bmx_tcodnoise_getsimplex(TCODNoise * noise, BBArray * f);
	float bmx_tcodnoise_getfbmsimplex(TCODNoise * noise, BBArray * f, float octaves);
	float bmx_tcodnoise_getturbulencesimplex(TCODNoise * noise, BBArray * f, float octaves);
	float bmx_tcodnoise_getwavelet(TCODNoise * noise, BBArray * f);
	float bmx_tcodnoise_getfbmwavelet(TCODNoise * noise, BBArray * f, float octaves);
	float bmx_tcodnoise_getturbulencewavelet(TCODNoise * noise, BBArray * f, float octaves);

	TCODImage * bmx_tcodimage_createfromfile(BBString * filename);
	void bmx_tcodimage_setkeycolor(TCODImage * image, MaxTCODColor * keyColor);
	void bmx_tcodimage_blit(TCODImage * image, MaxTCODConsole * console, float x, float y, TCOD_bkgnd_flag_t backgroundFlag, float scalex, float scaley, float angle);
	void bmx_tcodimage_blitrect(TCODImage * image, MaxTCODConsole * console, int x, int y, int w, int h, TCOD_bkgnd_flag_t backgroundFlag);
	void bmx_tcodimage_clear(TCODImage * image, MaxTCODColor * col);
	void bmx_tcodimage_getsize(TCODImage * image, int * w, int * h);
	MaxTCODColor * bmx_tcodimage_getpixel(TCODImage * image, int x, int y);
	int bmx_tcodimage_getalpha(TCODImage * image, int x, int y);
	bool bmx_tcodimage_ispixeltransparent(TCODImage * image, int x, int y);
	MaxTCODColor * bmx_tcodimage_getmipmappixel(TCODImage * image, float x0, float y0, float x1, float y1);
	void bmx_tcodimage_putpixel(TCODImage * image, int x, int y, MaxTCODColor * col);
	void bmx_tcodimage_scale(TCODImage * image, int newW, int newH);
	void bmx_tcodimage_hflip(TCODImage * image);
	void bmx_tcodimage_vflip(TCODImage * image);
	void bmx_tcodimage_invert(TCODImage * image);
	void bmx_tcodimage_free(TCODImage * image);
	TCODImage * bmx_tcodimage_create(int width, int height);

}




class MaxTCODColor
{
public:
	MaxTCODColor(const TCODColor &c);
	MaxTCODColor(int r, int g, int b);
	TCODColor & Color();

private:
	TCODColor color;
};

class MaxTCODConsole
{
public:
	MaxTCODConsole(BBObject * handle, TCODConsole * c);
	~MaxTCODConsole();
	TCODConsole * Console();
	
private:
	BBObject * maxHandle;
	TCODConsole * console;
};

class MaxTCODKey
{
public:

private:
	TCOD_key_t key;
};




