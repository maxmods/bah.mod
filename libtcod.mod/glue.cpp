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



MaxTCODColor::MaxTCODColor(const TCODColor &c)
	: color(c)
{
}

MaxTCODColor::MaxTCODColor(int r, int g, int b)
	: 	color(r, g, b)
{
}

TCODColor & MaxTCODColor::Color() {
	return color;
}

// ******************************************************************


MaxTCODColor * bmx_tcodcolor_create(int r, int g, int b) {
	return new MaxTCODColor(r, g, b);
}

void bmx_tcodcolor_delete(MaxTCODColor * color) {
	delete color;
}

bool bmx_tcodcolor_equals(MaxTCODColor * color, MaxTCODColor * other) {
	return color->Color() == other->Color();
}

void bmx_tcodcolor_updatecolors(MaxTCODColor * color, int * red, int * green, int * blue) {
	*red = color->Color().r;
	*green = color->Color().g;
	*blue = color->Color().b;
}

void bmx_tcodcolor_setr(MaxTCODColor * color, int value) {
	color->Color().r = value;
}

void bmx_tcodcolor_setg(MaxTCODColor * color, int value) {
	color->Color().g = value;
}

void bmx_tcodcolor_setb(MaxTCODColor * color, int value) {
	color->Color().b = value;
}

MaxTCODColor * bmx_tcodcolor_lerp(MaxTCODColor * a, MaxTCODColor * b, float coef) {
	return new MaxTCODColor(TCODColor::lerp(a->Color(), b->Color(), coef));
}


MaxTCODColor * bmx_tcodcolor_black() {
	return new MaxTCODColor(TCODColor::black);
}

MaxTCODColor * bmx_tcodcolor_darkgrey() {
	return new MaxTCODColor(TCODColor::darkGrey);
}

MaxTCODColor * bmx_tcodcolor_grey() {
	return new MaxTCODColor(TCODColor::grey);
}

MaxTCODColor * bmx_tcodcolor_lightgrey() {
	return new MaxTCODColor(TCODColor::lightGrey);
}

MaxTCODColor * bmx_tcodcolor_white() {
	return new MaxTCODColor(TCODColor::white);
}

MaxTCODColor * bmx_tcodcolor_red() {
	return new MaxTCODColor(TCODColor::red);
}

MaxTCODColor * bmx_tcodcolor_orange() {
	return new MaxTCODColor(TCODColor::orange);
}

MaxTCODColor * bmx_tcodcolor_chartreuse() {
	return new MaxTCODColor(TCODColor::chartreuse);
}

MaxTCODColor * bmx_tcodcolor_yellow() {
	return new MaxTCODColor(TCODColor::yellow);
}

MaxTCODColor * bmx_tcodcolor_green() {
	return new MaxTCODColor(TCODColor::green);
}

MaxTCODColor * bmx_tcodcolor_sea() {
	return new MaxTCODColor(TCODColor::sea);
}

MaxTCODColor * bmx_tcodcolor_cyan() {
	return new MaxTCODColor(TCODColor::cyan);
}

MaxTCODColor * bmx_tcodcolor_sky() {
	return new MaxTCODColor(TCODColor::sky);
}

MaxTCODColor * bmx_tcodcolor_blue() {
	return new MaxTCODColor(TCODColor::blue);
}

MaxTCODColor * bmx_tcodcolor_violet() {
	return new MaxTCODColor(TCODColor::violet);
}

MaxTCODColor * bmx_tcodcolor_magenta() {
	return new MaxTCODColor(TCODColor::magenta);
}

MaxTCODColor * bmx_tcodcolor_pink() {
	return new MaxTCODColor(TCODColor::pink);
}

MaxTCODColor * bmx_tcodcolor_darkred() {
	return new MaxTCODColor(TCODColor::darkRed);
}

MaxTCODColor * bmx_tcodcolor_darkorange() {
	return new MaxTCODColor(TCODColor::darkOrange);
}

MaxTCODColor * bmx_tcodcolor_darkyellow() {
	return new MaxTCODColor(TCODColor::darkYellow);
}

MaxTCODColor * bmx_tcodcolor_darkchartreuse() {
	return new MaxTCODColor(TCODColor::darkChartreuse);
}

MaxTCODColor * bmx_tcodcolor_darkgreen() {
	return new MaxTCODColor(TCODColor::darkGreen);
}

MaxTCODColor * bmx_tcodcolor_darksea() {
	return new MaxTCODColor(TCODColor::darkSea);
}

MaxTCODColor * bmx_tcodcolor_darkcyan() {
	return new MaxTCODColor(TCODColor::darkCyan);
}

MaxTCODColor * bmx_tcodcolor_darksky() {
	return new MaxTCODColor(TCODColor::darkSky);
}

MaxTCODColor * bmx_tcodcolor_darkblue() {
	return new MaxTCODColor(TCODColor::darkBlue);
}

MaxTCODColor * bmx_tcodcolor_darkviolet() {
	return new MaxTCODColor(TCODColor::darkViolet);
}

MaxTCODColor * bmx_tcodcolor_darkmagenta() {
	return new MaxTCODColor(TCODColor::darkMagenta);
}

MaxTCODColor * bmx_tcodcolor_darkpink() {
	return new MaxTCODColor(TCODColor::darkPink);
}

MaxTCODColor * bmx_tcodcolor_darkerred() {
	return new MaxTCODColor(TCODColor::darkerRed);
}

MaxTCODColor * bmx_tcodcolor_darkerorange() {
	return new MaxTCODColor(TCODColor::darkerOrange);
}

MaxTCODColor * bmx_tcodcolor_darkeryellow() {
	return new MaxTCODColor(TCODColor::darkerYellow);
}

MaxTCODColor * bmx_tcodcolor_darkerchartreuse() {
	return new MaxTCODColor(TCODColor::darkerChartreuse);
}

MaxTCODColor * bmx_tcodcolor_darkergreen() {
	return new MaxTCODColor(TCODColor::darkerGreen);
}

MaxTCODColor * bmx_tcodcolor_darkersea() {
	return new MaxTCODColor(TCODColor::darkerSea);
}

MaxTCODColor * bmx_tcodcolor_darkercyan() {
	return new MaxTCODColor(TCODColor::darkerCyan);
}

MaxTCODColor * bmx_tcodcolor_darkersky() {
	return new MaxTCODColor(TCODColor::darkerSky);
}

MaxTCODColor * bmx_tcodcolor_darkerblue() {
	return new MaxTCODColor(TCODColor::darkerBlue);
}

MaxTCODColor * bmx_tcodcolor_darkerviolet() {
	return new MaxTCODColor(TCODColor::darkerViolet);
}

MaxTCODColor * bmx_tcodcolor_darkermagenta() {
	return new MaxTCODColor(TCODColor::darkerMagenta);
}

MaxTCODColor * bmx_tcodcolor_darkerpink() {
	return new MaxTCODColor(TCODColor::darkerPink);
}

MaxTCODColor * bmx_tcodcolor_lightred() {
	return new MaxTCODColor(TCODColor::lightRed);
}

MaxTCODColor * bmx_tcodcolor_lightorange() {
	return new MaxTCODColor(TCODColor::lightOrange);
}

MaxTCODColor * bmx_tcodcolor_lightyellow() {
	return new MaxTCODColor(TCODColor::lightYellow);
}

MaxTCODColor * bmx_tcodcolor_lightchartreuse() {
	return new MaxTCODColor(TCODColor::lightChartreuse);
}

MaxTCODColor * bmx_tcodcolor_lightgreen() {
	return new MaxTCODColor(TCODColor::lightGreen);
}

MaxTCODColor * bmx_tcodcolor_lightsea() {
	return new MaxTCODColor(TCODColor::lightSea);
}

MaxTCODColor * bmx_tcodcolor_lightcyan() {
	return new MaxTCODColor(TCODColor::lightCyan);
}

MaxTCODColor * bmx_tcodcolor_lightsky() {
	return new MaxTCODColor(TCODColor::lightSky);
}

MaxTCODColor * bmx_tcodcolor_lightblue() {
	return new MaxTCODColor(TCODColor::lightBlue);
}

MaxTCODColor * bmx_tcodcolor_lightviolet() {
	return new MaxTCODColor(TCODColor::lightViolet);
}

MaxTCODColor * bmx_tcodcolor_lightmagenta() {
	return new MaxTCODColor(TCODColor::lightMagenta);
}

MaxTCODColor * bmx_tcodcolor_lightpink() {
	return new MaxTCODColor(TCODColor::lightPink);
}

MaxTCODColor * bmx_tcodcolor_desaturatedred() {
	return new MaxTCODColor(TCODColor::desaturatedRed);
}

MaxTCODColor * bmx_tcodcolor_desaturatedorange() {
	return new MaxTCODColor(TCODColor::desaturatedOrange);
}

MaxTCODColor * bmx_tcodcolor_desaturatedyellow() {
	return new MaxTCODColor(TCODColor::desaturatedYellow);
}

MaxTCODColor * bmx_tcodcolor_desaturatedchartreuse() {
	return new MaxTCODColor(TCODColor::desaturatedChartreuse);
}

MaxTCODColor * bmx_tcodcolor_desaturatedgreen() {
	return new MaxTCODColor(TCODColor::desaturatedGreen);
}

MaxTCODColor * bmx_tcodcolor_desaturatedsea() {
	return new MaxTCODColor(TCODColor::desaturatedSea);
}

MaxTCODColor * bmx_tcodcolor_desaturatedcyan() {
	return new MaxTCODColor(TCODColor::desaturatedCyan);
}

MaxTCODColor * bmx_tcodcolor_desaturatedsky() {
	return new MaxTCODColor(TCODColor::desaturatedSky);
}

MaxTCODColor * bmx_tcodcolor_desaturatedblue() {
	return new MaxTCODColor(TCODColor::desaturatedBlue);
}

MaxTCODColor * bmx_tcodcolor_desaturatedviolet() {
	return new MaxTCODColor(TCODColor::desaturatedViolet);
}

MaxTCODColor * bmx_tcodcolor_desaturatedmagenta() {
	return new MaxTCODColor(TCODColor::desaturatedMagenta);
}

MaxTCODColor * bmx_tcodcolor_desaturatedpink() {
	return new MaxTCODColor(TCODColor::desaturatedPink);
}

MaxTCODColor * bmx_tcodcolor_silver() {
	return new MaxTCODColor(TCODColor::silver);
}

MaxTCODColor * bmx_tcodcolor_gold() {
	return new MaxTCODColor(TCODColor::gold);
}


// ******************************************************************

MaxTCODConsole::MaxTCODConsole(BBObject * handle, TCODConsole * c)
	: maxHandle(handle), console(c)
{
	BBRETAIN(maxHandle);
}

MaxTCODConsole::~MaxTCODConsole() {
	BBRELEASE(maxHandle);
	delete console;
}

TCODConsole * MaxTCODConsole::Console() {
	return console;
}


MaxTCODConsole * bmx_tcodconsole_initroot(BBObject * handle, int w, int h, BBString * title, bool fullscreen) {
	char * t = bbStringToCString(title);
	TCODConsole::initRoot(w, h, t, fullscreen);
	bbMemFree(t);
	return new MaxTCODConsole(handle, TCODConsole::root);
}

void bmx_tcodconsole_free(MaxTCODConsole * console) {
	delete console;
}

void bmx_tcodconsole_setcustomfont(BBString *fontFile, int flags, int nbCharHoriz, int nbCharVertic) {
	char * t = bbStringToCString(fontFile);
	TCODConsole::setCustomFont(t, flags, nbCharHoriz, nbCharVertic);
	bbMemFree(t);
}

bool bmx_tcodconsole_iswindowclosed() {
	return TCODConsole::isWindowClosed();
}

void bmx_tcodconsole_flush() {
	TCODConsole::flush();
}

MaxTCODKey * bmx_tcodconsole_checkforkeypress() {
// TCODConsole::checkForKeypress
return 0;
}

void bmx_tcodconsole_setforegroundcolor(MaxTCODConsole * console, MaxTCODColor * color) {
	console->Console()->setForegroundColor(color->Color());
}

void bmx_tcodconsole_printright(MaxTCODConsole * console, int x, int y, TCOD_bkgnd_flag_t flag, BBString * text) {
	char * t = bbStringToCString(text);
	console->Console()->printRight(x, y, flag, t);
	bbMemFree(t);
}

void bmx_tcodconsole_printleft(MaxTCODConsole * console, int x, int y, TCOD_bkgnd_flag_t flag, BBString * text) {
	char * t = bbStringToCString(text);
	console->Console()->printLeft(x, y, flag, t);
	bbMemFree(t);
}

MaxTCODConsole * bmx_tcodconsole_create(BBObject * handle, int width, int height) {
	TCODConsole * console = new TCODConsole(width, height);
	return new MaxTCODConsole(handle, console);
}

bool bmx_tcodconsole_isfullscreen() {
	return TCODConsole::isFullscreen();
}

void bmx_tcodconsole_clear(MaxTCODConsole * console) {
	console->Console()->clear();
}

MaxTCODColor * bmx_tcodconsole_getforegroundcolor(MaxTCODConsole * console) {
	return new MaxTCODColor(console->Console()->getForegroundColor());
}

MaxTCODColor * bmx_tcodconsole_getbackgroundcolor(MaxTCODConsole * console) {
	return new MaxTCODColor(console->Console()->getBackgroundColor());
}

void bmx_tcodconsole_setbackgroundcolor(MaxTCODConsole * console, MaxTCODColor * color) {
	console->Console()->setBackgroundColor(color->Color());
}

void bmx_tcodconsole_setfore(MaxTCODConsole * console, int x, int y, MaxTCODColor * col) {
	console->Console()->setFore(x, y, col->Color());
}

MaxTCODColor * bmx_tcodconsole_getfore(MaxTCODConsole * console, int x, int y) {
	return new MaxTCODColor(console->Console()->getFore(x, y));
}

void bmx_tcodconsole_setback(MaxTCODConsole * console, int x, int y, MaxTCODColor * col, TCOD_bkgnd_flag_t flag) {
	console->Console()->setBack(x, y, col->Color(), flag);
}

MaxTCODColor * bmx_tcodconsole_getback(MaxTCODConsole * console, int x, int y) {
	return new MaxTCODColor(console->Console()->getBack(x, y));
}

void bmx_tcodconsole_printcenter(MaxTCODConsole * console, int x, int y, TCOD_bkgnd_flag_t flag, BBString * text) {
	char * t = bbStringToCString(text);
	console->Console()->printCenter(x, y, flag, t);
	bbMemFree(t);
}

int bmx_tcodconsole_printrightrect(MaxTCODConsole * console, int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, BBString * text) {
	char * t = bbStringToCString(text);
	int res = console->Console()->printRightRect(x, y, w, h, flag, t);
	bbMemFree(t);
	return res;
}

int bmx_tcodconsole_printleftrect(MaxTCODConsole * console, int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, BBString * text) {
	char * t = bbStringToCString(text);
	int res = console->Console()->printLeftRect(x, y, w, h, flag, t);
	bbMemFree(t);
	return res;
}

int bmx_tcodconsole_printcenterrect(MaxTCODConsole * console, int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, BBString * text) {
	char * t = bbStringToCString(text);
	int res = console->Console()->printCenterRect(x, y, w, h, flag, t);
	bbMemFree(t);
	return res;
}

void bmx_tcodconsole_putchar(MaxTCODConsole * console, int x, int y, int c, TCOD_bkgnd_flag_t flag) {
	console->Console()->putChar(x, y, c, flag);
}

void bmx_tcodconsole_blit(MaxTCODConsole * source, int x, int y, int width, int height, MaxTCODConsole * dest, int posX, int posY, int fade) {
	TCODConsole::blit(source->Console(), x, y, width, height, dest->Console(), posX, posY, fade);
}

void bmx_tcodconsole_setchar(MaxTCODConsole * console, int x, int y, int c) {
	console->Console()->setChar(x, y, c);
}

void bmx_tcodconsole_rect(MaxTCODConsole * console, int x, int y, int w, int h, bool clear, TCOD_bkgnd_flag_t flag) {
	console->Console()->rect(x, y, w, h, clear, flag);
}

void bmx_tcodconsole_hline(MaxTCODConsole * console, int x, int y, int length, TCOD_bkgnd_flag_t flag) {
	console->Console()->hline(x, y, length, flag);
}

void bmx_tcodconsole_vline(MaxTCODConsole * console, int x, int y, int length, TCOD_bkgnd_flag_t flag) {
	console->Console()->vline(x, y, length, flag);
}

void bmx_tcodconsole_printframe(MaxTCODConsole * console, int x, int y, int w, int h, bool clear, BBString * fmt) {
	char * t = 0;
	if (fmt != &bbEmptyString) {
		t = bbStringToCString(fmt);
	}
	console->Console()->printFrame(x, y, w, h, clear, t);
	
	if (t) bbMemFree(t);
}

void bmx_tcodconsole_credits() {
	TCODConsole::credits();
}

bool bmx_tcodconsole_rendercredits(int x, int y, bool alpha) {
	return TCODConsole::renderCredits(x, y, alpha);
}

void bmx_tcodconsole_setfullscreen(bool fullscreen) {
	TCODConsole::setFullscreen(fullscreen);
}

int bmx_tcodconsole_getchar(MaxTCODConsole * console, int x, int y) {
	return console->Console()->getChar(x, y);
}

int bmx_tcodconsole_getheightleftrect(MaxTCODConsole * console, int x, int y, int w, int h, BBString * text) {
	char * t = bbStringToCString(text);
	int res = console->Console()->getHeightLeftRect(x, y, w, h, t);
	bbMemFree(t);
	return res;
}

int bmx_tcodconsole_getheightcenterrect(MaxTCODConsole * console, int x, int y, int w, int h, BBString * text) {
	char * t = bbStringToCString(text);
	int res = console->Console()->getHeightCenterRect(x, y, w, h, t);
	bbMemFree(t);
	return res;
}

int bmx_tcodconsole_getheightrightrect(MaxTCODConsole * console, int x, int y, int w, int h, BBString * text) {
	char * t = bbStringToCString(text);
	int res = console->Console()->getHeightRightRect(x, y, w, h, t);
	bbMemFree(t);
	return res;
}

// ******************************************************************

uint32 bmx_tcodsystem_getelapsedmilli() {
	return TCODSystem::getElapsedMilli();
}

float bmx_tcodsystem_getelapsedseconds() {
	return TCODSystem::getElapsedSeconds();
}

void bmx_tcodsystem_sleepmilli(uint32 value) {
	TCODSystem::sleepMilli(value);
}

void bmx_tcodsystem_setfps(int value) {
	TCODSystem::setFps(value);
}

int bmx_tcodsystem_getfps() {
	return TCODSystem::getFps();
}

float bmx_tcodsystem_getlastframelength() {
	return TCODSystem::getLastFrameLength();
}

void bmx_tcodsystem_getcurrentresolution(int * w, int * h) {
	TCODSystem::getCurrentResolution(w, h);
}

extern int *ascii_to_tcod;

void bmx_tcodsystem_getcolsandascii(char_t * buf, int lookup, int * index, int * br, int * bg, int * bb, int * fr, int * fg, int * fb, int flags) {
	char_t *c = &buf[lookup];
	*index = ((flags & TCOD_FONT_LAYOUT_TCOD) != 0) ? ascii_to_tcod[(int)c->c] : (int)c->c;
//printf("old : %d ... new : %d \n", (int)c->c, *index);fflush(stdout);
	TCOD_color_t b=c->back;
	TCOD_color_t f=c->fore;
	*br = b.r;
	*bg = b.g;
	*bb = b.b;
	*fr = f.r;
	*fg = f.g;
	*fb = f.b;
}


// ******************************************************************

TCODRandom * bmx_tcodrandom_getinstance() {
	return TCODRandom::getInstance();
}

int bmx_tcodrandom_getint(TCODRandom * rand, int _min, int _max) {
	return rand->getInt(_min, _max);
}

float bmx_tcodrandom_getfloat(TCODRandom * rand, float _min, float _max) {
	return rand->getFloat(_min, _max);
}

TCODRandom * bmx_tcodrandom_create(uint32 seed, bool useSeed) {
	if (useSeed) {
		return new TCODRandom(seed);
	} else {
		return new TCODRandom();
	}
}

void bmx_tcodrandom_delete(TCODRandom * rand) {
	delete rand;
}

// ******************************************************************

void bmx_tcodline_init(int xFrom, int yFrom, int xTo, int yTo) {
	TCODLine::init(xFrom, yFrom, xTo, yTo);
}

bool bmx_tcodline_dostep(int * xCur, int * yCur) {
	return TCODLine::step(xCur, yCur);
}


// ******************************************************************

TCODMap * bmx_tcodmap_create(int width, int height) {
	return new TCODMap(width, height);
}

void bmx_tcodmap_setproperties(TCODMap * map, int x, int y, bool isTransparent, bool isWalkable) {
	map->setProperties(x, y, isTransparent, isWalkable);
}

void bmx_tcodmap_clear(TCODMap * map) {
	map->clear();
}

void bmx_tcodmap_copy(TCODMap * map, TCODMap * source) {
	map->copy(source);
}

void bmx_tcodmap_computefov(TCODMap * map, int playerX, int playerY, int maxRadius, bool lightWalls, TCOD_fov_algorithm_t algo) {
	map->computeFov(playerX, playerY, maxRadius, lightWalls, algo);
}

bool bmx_tcodmap_isinfov(TCODMap * map, int x, int y) {
	return map->isInFov(x, y);
}

bool bmx_tcodmap_istransparent(TCODMap * map, int x, int y) {
	return map->isTransparent(x, y);
}

bool bmx_tcodmap_iswalkable(TCODMap * map, int x, int y) {
	return map->isWalkable(x, y);
}

void bmx_tcodmap_delete(TCODMap * map) {
	delete map;
}


// ******************************************************************

TCODPath * bmx_tcodpath_create(TCODMap * map, float diagonalCost) {
	return new TCODPath(map, diagonalCost);
}

bool bmx_tcodpath_compute(TCODPath * path, int ox, int oy, int dx, int dy) {
	return path->compute(ox, oy, dx, dy);
}

void bmx_tcodpath_getorigin(TCODPath * path, int * x, int * y) {
	path->getOrigin(x, y);
}

void bmx_tcodpath_getdestination(TCODPath * path, int * x, int * y) {
	path->getDestination(x, y);
}

int bmx_tcodpath_size(TCODPath * path) {
	return path->size();
}

void bmx_tcodpath_get(TCODPath * path, int index, int * x, int * y) {
	path->get(index, x, y);
}

bool bmx_tcodpath_isempty(TCODPath * path) {
	return path->isEmpty();
}

bool bmx_tcodpath_walk(TCODPath * path, int * x, int * y, bool recalculateWhenNeeded) {
	return path->walk(x, y, recalculateWhenNeeded);
}

void bmx_tcodpath_delete(TCODPath * path) {
	delete path;
}

// ******************************************************************

TCODNoise * bmx_tcodnoise_create(int dimensions, float hurst, float lacunarity, TCODRandom * generator) {
	if (generator) {
		return new TCODNoise(dimensions, hurst, lacunarity, generator);
	} else {
		return new TCODNoise(dimensions, hurst, lacunarity);
	}
}

void bmx_tcodnoise_delete(TCODNoise * noise) {
	delete noise;
}

float bmx_tcodnoise_getperlin(TCODNoise * noise, BBArray * f) {
	return noise->getPerlin((float*)BBARRAYDATA( f,f->dims ));
}

float bmx_tcodnoise_getfbmperlin(TCODNoise * noise, BBArray * f, float octaves) {
	return noise->getFbmPerlin((float*)BBARRAYDATA( f,f->dims ), octaves);
}

float bmx_tcodnoise_getturbulenceperlin(TCODNoise * noise, BBArray * f, float octaves) {
	return noise->getTurbulencePerlin((float*)BBARRAYDATA( f,f->dims ), octaves);
}

float bmx_tcodnoise_getsimplex(TCODNoise * noise, BBArray * f) {
	return noise->getSimplex((float*)BBARRAYDATA( f,f->dims ));
}

float bmx_tcodnoise_getfbmsimplex(TCODNoise * noise, BBArray * f, float octaves) {
	return noise->getFbmSimplex((float*)BBARRAYDATA( f,f->dims ) , octaves);
}

float bmx_tcodnoise_getturbulencesimplex(TCODNoise * noise, BBArray * f, float octaves) {
	return noise->getTurbulenceSimplex((float*)BBARRAYDATA( f,f->dims ) , octaves);
}

float bmx_tcodnoise_getwavelet(TCODNoise * noise, BBArray * f) {
	return noise->getWavelet((float*)BBARRAYDATA( f,f->dims ));
}

float bmx_tcodnoise_getfbmwavelet(TCODNoise * noise, BBArray * f, float octaves) {
	return noise->getFbmWavelet((float*)BBARRAYDATA( f,f->dims ) , octaves);
}

float bmx_tcodnoise_getturbulencewavelet(TCODNoise * noise, BBArray * f, float octaves) {
	return noise->getTurbulenceWavelet((float*)BBARRAYDATA( f,f->dims ) , octaves);
}

// ******************************************************************

TCODImage * bmx_tcodimage_createfromfile(BBString * filename) {
	char * f = bbStringToCString(filename);
	TCODImage * image = new TCODImage(f);
	bbMemFree(f);
	return image;
}

void bmx_tcodimage_setkeycolor(TCODImage * image, MaxTCODColor * keyColor) {
	image->setKeyColor(keyColor->Color());
}

void bmx_tcodimage_blit(TCODImage * image, MaxTCODConsole * console, float x, float y, TCOD_bkgnd_flag_t backgroundFlag, float scalex, float scaley, float angle) {
	image->blit(console->Console(), x, y, backgroundFlag, scalex, scaley, angle * 0.0174532925);
}

void bmx_tcodimage_blitrect(TCODImage * image, MaxTCODConsole * console, int x, int y, int w, int h, TCOD_bkgnd_flag_t backgroundFlag) {
	image->blitRect(console->Console(), x, y, w, h, backgroundFlag);
}

void bmx_tcodimage_clear(TCODImage * image, MaxTCODColor * col) {
	image->clear(col->Color());
}

void bmx_tcodimage_getsize(TCODImage * image, int * w, int * h) {
	image->getSize(w, h);
}

MaxTCODColor * bmx_tcodimage_getpixel(TCODImage * image, int x, int y) {
	return new MaxTCODColor(image->getPixel(x, y));
}

int bmx_tcodimage_getalpha(TCODImage * image, int x, int y) {
	return image->getAlpha(x, y);
}

bool bmx_tcodimage_ispixeltransparent(TCODImage * image, int x, int y) {
	return image->isPixelTransparent(x, y);
}

MaxTCODColor * bmx_tcodimage_getmipmappixel(TCODImage * image, float x0, float y0, float x1, float y1) {
	return new MaxTCODColor(image->getMipmapPixel(x0, y0, x1, y1));
}

void bmx_tcodimage_putpixel(TCODImage * image, int x, int y, MaxTCODColor * col) {
	image->putPixel(x, y, col->Color());
}

void bmx_tcodimage_scale(TCODImage * image, int newW, int newH) {
	image->scale(newW, newH);
}

void bmx_tcodimage_hflip(TCODImage * image) {
	image->hflip();
}

void bmx_tcodimage_vflip(TCODImage * image) {
	image->vflip();
}

void bmx_tcodimage_invert(TCODImage * image) {
	image->invert();
}

void bmx_tcodimage_free(TCODImage * image) {
	delete image;
}

TCODImage * bmx_tcodimage_create(int width, int height) {
	return new TCODImage(width, height);
}

// ******************************************************************

BBObject * bmx_tcodmouse_getstatus() {
	TCOD_mouse_t ms = TCODMouse::getStatus();
	return _bah_libtcod_TCODMouse__create(ms.x, ms.y, ms.dx, ms.dy, ms.cx, ms.cy, ms.dcx, ms.dcy, ms.lbutton, ms.rbutton, ms.mbutton, 
			ms.lbutton_pressed, ms.rbutton_pressed, ms.mbutton_pressed, ms.wheel_up, ms.wheel_down);
}

void bmx_tcodmouse_move(int x, int y) {
	TCODMouse::move(x, y);
}

void bmx_tcodmouse_showcursor(bool visible) {
	TCODMouse::showCursor(visible);
}

// ******************************************************************
// ******************************************************************

void bmx_tcodwidget_callback(Widget *w, void *userData) {
	_bah_libtcod_TCODWidget__callback((BBObject*)userData);
}

void bmx_tcodwidget_setbackgroundcolor(MaxTCODColor  * col, MaxTCODColor * colFocus) {
	Widget::setBackgroundColor(col->Color(), colFocus->Color());
}

void bmx_tcodwidget_setforegroundcolor(MaxTCODColor  * col, MaxTCODColor * colFocus) {
	Widget::setForegroundColor(col->Color(), colFocus->Color());
}

void bmx_tcodwidget_setconsole(MaxTCODConsole * console) {
	Widget::setConsole(console->Console());
}

void bmx_tcodwidget_updatewidgets(TCOD_keycode_t vk, int c, int pressed, int lalt, int lctrl, int ralt, int rctrl, int shift) {
	TCOD_key_t key;
	key.vk = vk;
	key.c = static_cast<int>(c);
	key.pressed = pressed;
	key.lalt = lalt;
	key.lctrl = lctrl;
	key.ralt = ralt;
	key.rctrl = rctrl;
	key.shift = shift;
	Widget::updateWidgets(key);
}

void bmx_tcodwidget_renderwidgets() {
	Widget::renderWidgets();
}

void bmx_tcodwidget_move(Widget * widget, int x, int y) {
	widget->move(x, y);
}

void bmx_tcodwidget_settip(Widget * widget, BBString * tip) {
	char * t = bbStringToCString(tip);
	widget->setTip(t);
	bbMemFree(t);
}

void bmx_tcodwidget_setvisible(Widget * widget, bool visible) {

}

bool bmx_tcodwidget_isvisible(Widget * widget) {
	return widget->isVisible();
}

BBObject * bmx_tcodwidget_focus() {
	Widget * f = Widget::focus;
	if (f) {
		return (BBObject*)f->userData;
	} else {
		return &bbNullObject;
	}
}



// ******************************************************************

MaxTCODButton * bmx_tcodbutton_create(BBObject * handle, BBString * label, BBString * tip, int x, int y, int w, int h) {
	char * l = bbStringToCString(label);
	char * t = 0;
	if (tip != &bbEmptyString) {
		t = bbStringToCString(tip);
	}
	MaxTCODButton * button = new MaxTCODButton(x, y, w, h, l, t, bmx_tcodwidget_callback, handle);
	bbMemFree(l);
	if (t) bbMemFree(t);
	return button;
}

// ******************************************************************

MaxTCODStatusBar * bmx_tcodstatusbar_create(BBObject * handle, int x, int y, int w, int h) {
	return new MaxTCODStatusBar(x, y, w, h, handle);
}

// ******************************************************************

MaxTCODLabel * bmx_tcodlabel_create(BBObject * handle, int x, int y, BBString * label, BBString * tip) {
	char * l = bbStringToCString(label);
	char * t = 0;
	if (tip != &bbEmptyString) {
		t = bbStringToCString(tip);
	}
	MaxTCODLabel * lbl = new MaxTCODLabel(x, y, l, t, (void*)handle);
	bbMemFree(l);
	if (t) bbMemFree(t);
	return lbl;
}

void bmx_tcodlabel_setvalue(Label * widget, BBString * label) {
	char * l = bbStringToCString(label);
	widget->setValue(l);
	bbMemFree(l);
}

// ******************************************************************

MaxTCODVBox * bmx_tcodvbox_create(BBObject * handle, int x, int y, int padding) {
	return new MaxTCODVBox(x, y, padding, handle);
}


// ******************************************************************

void bmx_tcodcontainer_addwidget(Container * cont, Widget * wid) {
	cont->addWidget(wid);
}

void bmx_tcodcontainer_removewidget(Container * cont, Widget * wid) {
	cont->removeWidget(wid);
}

// ******************************************************************

MaxTCODToolBar * bmx_tcodtoolbar_create(BBObject * handle, int x, int y, BBString * name, BBString * tip) {
	char * l = bbStringToCString(name);
	char * t = 0;
	if (tip != &bbEmptyString) {
		t = bbStringToCString(tip);
	}
	MaxTCODToolBar * tb = new MaxTCODToolBar(x, y, l, t, (void*)handle);
	bbMemFree(l);
	if (t) bbMemFree(t);
	return tb;
}

void bmx_tcodtoolbar_setname(ToolBar * tb, BBString * name) {
	char * n = bbStringToCString(name);
	tb->setName(n);
	bbMemFree(n);
}

void bmx_tcodtoolbar_addseparator(ToolBar * tb, BBString * txt, BBString * tip) {
	char * l = bbStringToCString(txt);
	char * t = 0;
	if (tip != &bbEmptyString) {
		t = bbStringToCString(tip);
	}
	tb->addSeparator(l, t);
	bbMemFree(l);
	if (t) bbMemFree(t);
}

// ******************************************************************

TCODBsp * bmx_tcodbsp_create(int x, int y, int w, int h) {
	return new TCODBsp(x, y, w, h);
}

void bmx_tcodbsp_splitonce(TCODBsp * bsp, bool horizontal, int position) {
	bsp->splitOnce(horizontal, position);
}

void bmx_tcodbsp_splitrecursive(TCODBsp * bsp, TCODRandom * randomizer, int nb, int minHSize, int maxHRatio, int minVSize, int maxVRatio) {
	bsp->splitRecursive(randomizer, nb, minHSize, maxHRatio, minVSize, maxVRatio);
}

void bmx_tcodbsp_resize(TCODBsp * bsp, int x, int y, int w, int h) {
	bsp->resize(x, y, w, h);
}

void bmx_tcodbsp_removechildren(TCODBsp * bsp) {
	bsp->removeSons();
}

void bmx_tcodbsp_free(TCODBsp * bsp) {
	delete bsp;
}

void bmx_tcodbsp_update(TCODBsp * bsp, int * x, int * y, int * w, int * h, int * position, bool * horizontal, int * level) {
	*x = bsp->x;
	*y = bsp->y;
	*w = bsp->w;
	*h = bsp->h;
	*position = bsp->position;
	*x = bsp->x;
}

BBObject * bmx_tcodbsp_getleft(TCODBsp * bsp) {
	TCODBsp * b = bsp->getLeft();
	if (b) {
		return _bah_libtcod_TCODBsp__create(b, b->x, b->y, b->w, b->h, b->position, b->horizontal, b->level);
	} else {
		return &bbNullObject;
	}
}

BBObject * bmx_tcodbsp_getright(TCODBsp * bsp) {
	TCODBsp * b = bsp->getRight();
	if (b) {
		return _bah_libtcod_TCODBsp__create(b, b->x, b->y, b->w, b->h, b->position, b->horizontal, b->level);
	} else {
		return &bbNullObject;
	}
}

BBObject * bmx_tcodbsp_getparent(TCODBsp * bsp) {
	TCODBsp * b = bsp->getFather();
	if (b) {
		return _bah_libtcod_TCODBsp__create(b, b->x, b->y, b->w, b->h, b->position, b->horizontal, b->level);
	} else {
		return &bbNullObject;
	}
}

bool bmx_tcodbsp_isleaf(TCODBsp * bsp) {
	return bsp->isLeaf();
}

bool bmx_tcodbsp_contains(TCODBsp * bsp, int cx, int cy) {
	return bsp->contains(cx, cy);
}

BBObject * bmx_tcodbsp_findnode(TCODBsp * bsp, int cx, int cy) {
	TCODBsp * b = bsp->findNode(cx, cy);
	if (b) {
		return _bah_libtcod_TCODBsp__create(b, b->x, b->y, b->w, b->h, b->position, b->horizontal, b->level);
	} else {
		return &bbNullObject;
	}
}

class MaxBspCallback : public ITCODBspCallback
{
public :
	bool visitNode(TCODBsp *b, void *userData) {
		return _bah_libtcod_TCODBsp__Callback(_bah_libtcod_TCODBsp__create(b, b->x, b->y, b->w, b->h, b->position, b->horizontal, b->level));
	}
};

bool bmx_tcodbsp_traversepreorder(TCODBsp * bsp) {
	MaxBspCallback *cb = new MaxBspCallback();
	bool res = bsp->traversePreOrder(cb, NULL);
	delete cb;
	return res;
}

bool bmx_tcodbsp_traverseinorder(TCODBsp * bsp) {
	MaxBspCallback *cb = new MaxBspCallback();
	bool res = bsp->traverseInOrder(cb, NULL);
	delete cb;
	return res;
}

bool bmx_tcodbsp_traversepostorder(TCODBsp * bsp) {
	MaxBspCallback *cb = new MaxBspCallback();
	bool res = bsp->traversePostOrder(cb, NULL);
	delete cb;
	return res;
}

bool bmx_tcodbsp_traverselevelorder(TCODBsp * bsp) {
	MaxBspCallback *cb = new MaxBspCallback();
	bool res = bsp->traverseLevelOrder(cb, NULL);
	delete cb;
	return res;
}

bool bmx_tcodbsp_traverseinvertedlevelorder(TCODBsp * bsp) {
	MaxBspCallback *cb = new MaxBspCallback();
	bool res = bsp->traverseInvertedLevelOrder(cb, NULL);
	delete cb;
	return res;
}




