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
#include "libtcod.hpp"
#include "libtcod_int.h"

TCODConsole * TCODConsole::root = NULL;

TCODConsole::TCODConsole() {}
TCODConsole::TCODConsole(int w, int h) {
	data = TCOD_console_new(w,h);
	fore = new TCODImage(((TCOD_console_data_t *)data)->forebuf);
	back = new TCODImage(((TCOD_console_data_t *)data)->backbuf);
}

TCODImage *TCODConsole::getForegroundImage() const {
	return fore;
}

TCODImage *TCODConsole::getBackgroundImage() const {
	return back;
}
	
void TCODConsole::setCustomFont(const char *fontFile, int flags,int nbCharHoriz, int nbCharVertic) {
	TCOD_console_set_custom_font(fontFile,flags,nbCharHoriz,nbCharVertic);
}

void TCODConsole::mapAsciiCodeToFont(int asciiCode, int fontCharX, int fontCharY) {
	TCOD_console_map_ascii_code_to_font(asciiCode,fontCharX,fontCharY);
}

void TCODConsole::mapAsciiCodesToFont(int firstAsciiCode, int nbCodes, int fontCharX, int fontCharY) {
	TCOD_console_map_ascii_codes_to_font(firstAsciiCode,nbCodes,fontCharX,fontCharY);
}

void TCODConsole::mapStringToFont(const char *s, int fontCharX, int fontCharY) {
	TCOD_console_map_string_to_font(s, fontCharX, fontCharY);
}

TCOD_key_t TCODConsole::checkForKeypress(int flags) {
	return TCOD_sys_check_for_keypress(flags);
}

TCOD_key_t TCODConsole::waitForKeypress(bool flush) {
	return TCOD_sys_wait_for_keypress(flush);
}

bool TCODConsole::isWindowClosed() {
	return TCOD_console_is_window_closed() != 0;
}

int TCODConsole::getWidth() const {
	return TCOD_console_get_width(data);
}

int TCODConsole::getHeight() const {
	return TCOD_console_get_height(data);
}

void TCODConsole::setColorControl(TCOD_colctrl_t con, const TCODColor &fore, const TCODColor &back) {
	TCOD_color_t b={back.r,back.g,back.b},f={fore.r,fore.g,fore.b};
	TCOD_console_set_color_control(con,f,b);
}

TCODColor TCODConsole::getBackgroundColor() const {
	TCOD_color_t c= TCOD_console_get_background_color(data);
	TCODColor ret;
	ret.r=c.r;
	ret.g=c.g;
	ret.b=c.b;
	return ret;
}
TCODColor TCODConsole::getForegroundColor() const {
	return TCOD_console_get_foreground_color(data);
}
void TCODConsole::setBackgroundColor(const TCODColor &back) {
	TCOD_color_t b={back.r,back.g,back.b};
	TCOD_console_set_background_color(data,b);
}
void TCODConsole::setForegroundColor(const TCODColor &fore) {
	TCOD_color_t f={fore.r,fore.g,fore.b};
	TCOD_console_set_foreground_color(data,f);
}

void TCODConsole::setBackgroundFlag(TCOD_colorop_t flag) {
	TCOD_console_set_background_flag(data,flag);
}
void TCODConsole::setForegroundFlag(TCOD_colorop_t flag) {
	TCOD_console_set_foreground_flag(data,flag);
}
void TCODConsole::setBackgroundBrush(const TCODColor &back,TCOD_colorop_t flag) {
	TCOD_color_t b={back.r,back.g,back.b};
	TCOD_console_set_background_brush(data,b,flag);
}
void TCODConsole::setForegroundBrush(const TCODColor &fore,TCOD_colorop_t flag) {
	TCOD_color_t f={fore.r,fore.g,fore.b};
	TCOD_console_set_foreground_brush(data,f,flag);
}

void TCODConsole::setBrush(const TCODColor &fore,TCOD_colorop_t foreflag,const TCODColor &back,TCOD_colorop_t backflag) {
	TCOD_color_t b={back.r,back.g,back.b};
	TCOD_color_t f={fore.r,fore.g,fore.b};
	TCOD_console_set_brush(data,f,foreflag,b,backflag);
}

void TCODConsole::pushBrush() {
	TCOD_console_push_brush(data);
}

void TCODConsole::popBrush() {
	TCOD_console_pop_brush(data);
}


void TCODConsole::setWindowTitle(const char *title) {
	TCOD_sys_set_window_title(title);
}

void TCODConsole::initRoot(int w, int h, const char *title, bool fullscreen) {
	TCODConsole *con=new TCODConsole();
	TCOD_console_init_root(w,h,title,fullscreen);
	con->data=NULL;
	con->fore = new TCODImage(TCOD_console_get_foreground_image(NULL));
	con->back = new TCODImage(TCOD_console_get_background_image(NULL));
	TCODConsole::root=con;
}

void TCODConsole::setFullscreen(bool fullscreen) {
	TCOD_console_set_fullscreen(fullscreen);
}

bool TCODConsole::isFullscreen() {
	return TCOD_console_is_fullscreen() != 0;
}

TCODConsole::~TCODConsole() {
	TCOD_console_delete(data);
	delete fore;
	delete back;
}

void TCODConsole::blit(const TCODConsole *srcCon,int xSrc, int ySrc, int wSrc, int hSrc,
	TCODConsole *dstCon, int xDst, int yDst, int fade) {
	TCOD_console_blit(srcCon->data,xSrc,ySrc,wSrc,hSrc,dstCon->data,xDst,yDst,fade);
}


void TCODConsole::flush() {
	TCOD_console_flush();
}

void TCODConsole::setFade(uint8 val, const TCODColor &fade) {
	TCOD_color_t f= {fade.r,fade.g,fade.b};
	TCOD_console_set_fade(val,f);
}

uint8 TCODConsole::getFade() {
	return TCOD_console_get_fade();
}

TCODColor TCODConsole::getFadingColor() {
	return TCOD_console_get_fading_color();
}

void TCODConsole::putChar(int x, int y, int c, const TCODColor &fore, const TCODColor &back) {
	TCOD_color_t f= {fore.r,fore.g,fore.b};
	TCOD_color_t b= {back.r,back.g,back.b};
	TCOD_console_put_char(data,x,y,c,f,b);
}

void TCODConsole::clear() {
	TCOD_console_clear(data);
}

TCODColor TCODConsole::getBack(int x, int y) const {
	return TCOD_console_get_back(data,x,y);
}
void TCODConsole::setFore(int x,int y, const TCODColor &col) {
	TCOD_color_t c={col.r,col.g,col.b};
	TCOD_console_set_fore(data,x,y,c);
}
TCODColor TCODConsole::getFore(int x, int y) const {
	return TCOD_console_get_fore(data,x,y);
}

int TCODConsole::getChar(int x, int y) const {
	return TCOD_console_get_char(data,x,y);
}

void TCODConsole::setBack(int x, int y, const TCODColor &col) {
	TCOD_color_t c={col.r,col.g,col.b};
	TCOD_console_set_back(data,x,y,c);
}

void TCODConsole::setChar(int x, int y, int c) {
	TCOD_console_set_char(data,x,y,c);
}

void TCODConsole::brushChar(int x, int y, int c) {
	TCOD_console_brush_char(data,x,y,c);
}

void TCODConsole::rect(int x,int y, int rw, int rh, int c) {
	TCOD_console_rect(data,x,y,rw,rh,c);
}

void TCODConsole::printLeft(int x, int y, const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	TCOD_console_print(data,x,y,getWidth()-x,getHeight()-y,LEFT,TCOD_console_vsprint(fmt,ap),false,false);
	va_end(ap);
}

void TCODConsole::printRight(int x, int y, const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	TCOD_console_print(data,x,y,x+1,getHeight()-y,RIGHT,TCOD_console_vsprint(fmt,ap),false,false);
	va_end(ap);
}

void TCODConsole::printCenter(int x, int y, const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	TCOD_console_print(data,x,y,getWidth(),getHeight()-y,CENTER,TCOD_console_vsprint(fmt,ap),false,false);
	va_end(ap);
}

int TCODConsole::printLeftRect(int x, int y, int w, int h, const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	int ret = TCOD_console_print(data,x,y,w,h,LEFT,TCOD_console_vsprint(fmt,ap),true,false);
	va_end(ap);
	return ret;
}

int TCODConsole::printRightRect(int x, int y, int w, int h, const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	int ret = TCOD_console_print(data,x,y,w,h,RIGHT,TCOD_console_vsprint(fmt,ap),true,false);
	va_end(ap);
	return ret;
}

int TCODConsole::printCenterRect(int x, int y, int w, int h, const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	int ret = TCOD_console_print(data,x,y,w,h,CENTER,TCOD_console_vsprint(fmt,ap),true,false);
	va_end(ap);
	return ret;
}

int TCODConsole::getHeightLeftRect(int x, int y, int w, int h, const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	int ret = TCOD_console_print(data,x,y,w,h,LEFT,TCOD_console_vsprint(fmt,ap),true,true);
	va_end(ap);
	return ret;
}

int TCODConsole::getHeightRightRect(int x, int y, int w, int h, const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	int ret = TCOD_console_print(data,x,y,w,h,RIGHT,TCOD_console_vsprint(fmt,ap),true,true);
	va_end(ap);
	return ret;
}

int TCODConsole::getHeightCenterRect(int x, int y, int w, int h,const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	int ret = TCOD_console_print(data,x,y,w,h,CENTER,TCOD_console_vsprint(fmt,ap),true,true);
	va_end(ap);
	return ret;
}

/*
void TCODConsole::setKeyboardRepeat(int initialDelay,int interval) {
	TCOD_console_set_keyboard_repeat(initialDelay,interval);
}

void TCODConsole::disableKeyboardRepeat() {
	TCOD_console_disable_keyboard_repeat();
}
*/

bool TCODConsole::isKeyPressed(TCOD_keycode_t key) {
	return TCOD_console_is_key_pressed(key) != 0;
}
void TCODConsole::setKeyColor(const TCODColor &col) {
	TCOD_color_t c={col.r,col.g,col.b};
	TCOD_console_set_key_color(data,c);
}

void TCODConsole::credits() {
	TCOD_console_credits();
}

bool TCODConsole::renderCredits(int x, int y, bool alpha) {
	return TCOD_console_credits_render(x,y,alpha) != 0;
}

