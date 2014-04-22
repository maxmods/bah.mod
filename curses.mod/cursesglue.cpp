/*
 Copyright (c) 2007-2013 Bruce A Henderson
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of Bruce A Henderson nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _WIN32
#include <sys/types.h>
#include <pwd.h>
#endif

extern "C" {

#include "blitz.h"
#ifdef __WIN32
#include <curses.h>
#endif
#include <cdk.h>

#ifdef BMX_NG
	BBObject * bah_curses_core_TMouseEvent__create(int id, int x, int y, int z, int bstate);
#else
	BBObject * _bah_curses_TMouseEvent__create(int id, int x, int y, int z, int bstate);
#endif

	EExitType bmx_entry_exitType(CDKENTRY * entry);
	EExitType bmx_slider_exitType(CDKSLIDER * slider);

	void bmx_curses_drawCDKEntry(CDKENTRY * entry);
	void bmx_curses_drawCDKLabel(CDKLABEL * label);
	void bmx_curses_drawCDKCalendar(CDKCALENDAR * calendar);

	void bmx_curses_positionCDKEntry(CDKENTRY * entry);
	void bmx_curses_positionCDKLabel(CDKLABEL * label);
	void bmx_curses_positionCDKCalendar(CDKCALENDAR * calendar);

	void bmx_curses_eraseCDKEntry(CDKENTRY * entry);
	void bmx_curses_eraseCDKLabel(CDKLABEL * label);
	void bmx_curses_eraseCDKCalendar(CDKCALENDAR * calendar);
	
	int bmx_dialog_currentButton(CDKDIALOG * dialog);
	
	void bmx_curses_injectCDKButtonBox(CDKBUTTONBOX * bbox, int input);
	void bmx_curses_injectCDKEntry(CDKENTRY * entry, int input);
	void bmx_curses_injectCDKCalendar(CDKCALENDAR * calendar, int input);
	
	int bmx_curses_getbegx(WINDOW * window);
	int bmx_curses_getbegy(WINDOW * window);
	int bmx_curses_waddstr(WINDOW * window, BBString * text, int maxChars);
	int bmx_curses_mvwaddstr(WINDOW * window, int x, int y, BBString * text, int maxChars);

	WINDOW * bmx_curses_CDKButtonbox_window(CDKBUTTONBOX * bbox);
	WINDOW * bmx_curses_CDKSlider_window(CDKSLIDER * slider);
	WINDOW * bmx_curses_CDKDialog_window(CDKDIALOG * dialog);
	WINDOW * bmx_curses_CDKEntry_window(CDKENTRY * entry);
	WINDOW * bmx_curses_CDKLabel_window(CDKLABEL * label);
	WINDOW * bmx_curses_CDKScroll_window(CDKLABEL * scroll);
	WINDOW * bmx_curses_CDKFSelect_window(CDKLABEL * fselect);
	WINDOW * bmx_curses_CDKCalendar_window(CDKCALENDAR * calendar);
	WINDOW * bmx_curses_CDKMentry_window(CDKMENTRY * entry);
	
	int bmx_curses_boxHeight(void * widget, EObjectType type);
	int bmx_curses_boxWidth(void * widget, EObjectType type);
	
	int bmx_buttonbox_currentButton(CDKBUTTONBOX * box);
	void bmx_curses_drawCDKButtonBox(CDKBUTTONBOX * box);

	EExitType bmx_mentry_exitType(CDKMENTRY * entry);
	void bmx_curses_drawCDKMentry(CDKMENTRY * entry);
	void bmx_curses_positionCDKMentry(CDKMENTRY * entry);
	void bmx_curses_eraseCDKMentry(CDKMENTRY * entry);
	void bmx_curses_injectCDKMentry(CDKMENTRY * entry, int input);

	void bmx_curses_setULCharOf(void * widget, int c);
	void bmx_curses_setLLCharOf(void * widget, int c);
	void bmx_curses_setURCharOf(void * widget, int c);
	void bmx_curses_setLRCharOf(void * widget, int c);
	
	int ACS_ULCORNER_();
	int ACS_LLCORNER_();
	int ACS_URCORNER_();
	int ACS_LRCORNER_();
	int ACS_LTEE_();
	int ACS_RTEE_();
	int ACS_BTEE_();
	int ACS_TTEE_();
	int ACS_HLINE_();
	int ACS_VLINE_();
	int ACS_PLUS_();
	int ACS_S1_();
	int ACS_S9_();
	int ACS_DIAMOND_();
	int ACS_CKBOARD_();
	int ACS_DEGREE_();
	int ACS_PLMINUS_();
	int ACS_BULLET_();
	int ACS_LARROW_();
	int ACS_RARROW_();
	int ACS_DARROW_();
	int ACS_UARROW_();
	int ACS_BOARD_();
	int ACS_LANTERN_();
	int ACS_BLOCK_();
	
	int BUTTON1_RELEASED_();
	int BUTTON1_PRESSED_();
	int BUTTON1_CLICKED_();
	int BUTTON1_DOUBLE_CLICKED_();
	int BUTTON1_TRIPLE_CLICKED_();
	int BUTTON2_RELEASED_();
	int BUTTON2_PRESSED_();
	int BUTTON2_CLICKED_();
	int BUTTON2_DOUBLE_CLICKED_();
	int BUTTON2_TRIPLE_CLICKED_();
	int BUTTON3_RELEASED_();
	int BUTTON3_PRESSED_();
	int BUTTON3_CLICKED_();
	int BUTTON3_DOUBLE_CLICKED_();
	int BUTTON3_TRIPLE_CLICKED_();
	int BUTTON4_RELEASED_();
	int BUTTON4_PRESSED_();
	int BUTTON4_CLICKED_();
	int BUTTON4_DOUBLE_CLICKED_();
	int BUTTON4_TRIPLE_CLICKED_();
	int BUTTON_CTRL_();
	int BUTTON_SHIFT_();
	int BUTTON_ALT_();
	int REPORT_MOUSE_POSITION_();
	int ALL_MOUSE_EVENTS_();
	
	int LINES_();
	int COLS_();

	
	int bmx_curses_getchCDKObject(void * widget, EObjectType type); 

	void bmx_curses_GetUserName(char * buffer);
	
	int bmx_mousemask(int mask, int * oldMask);
	BBObject * bmx_getmouse();

}


void bmx_curses_GetUserName(char * buffer) {
#ifdef _WIN32
	long unsigned int len = 256;
	GetUserNameA(buffer, &len);
#else
	struct passwd * p = getpwuid(getuid());
	strcpy(buffer, p->pw_name);	
#endif
}

// --------------------------------------------------------

EExitType bmx_entry_exitType(CDKENTRY * entry) {
	return entry->exitType;
}

EExitType bmx_slider_exitType(CDKSLIDER * slider) {
	return slider->exitType;
}


void bmx_curses_drawCDKEntry(CDKENTRY * entry) {
	drawCDKEntry(entry, ObjOf(entry)->box);
}

void bmx_curses_drawCDKLabel(CDKLABEL * label) {
	drawCDKLabel(label, ObjOf(label)->box);
}

void bmx_curses_drawCDKCalendar(CDKCALENDAR * calendar) {
	drawCDKCalendar(calendar, ObjOf(calendar)->box);
}

void bmx_curses_positionCDKEntry(CDKENTRY * entry) {
	positionCDKEntry(entry);
}

void bmx_curses_positionCDKLabel(CDKLABEL * label) {
	positionCDKLabel(label);
}

void bmx_curses_positionCDKCalendar(CDKCALENDAR * calendar) {
	positionCDKCalendar(calendar);
}

void bmx_curses_eraseCDKEntry(CDKENTRY * entry) {
	eraseCDKEntry(entry);
}

void bmx_curses_eraseCDKLabel(CDKLABEL * label) {
	eraseCDKLabel(label);
}

void bmx_curses_eraseCDKCalendar(CDKCALENDAR * calendar) {
	eraseCDKCalendar(calendar);
}

void bmx_curses_drawCDKButtonBox(CDKBUTTONBOX * box) {
	drawCDKButtonbox(box, ObjOf(box)->box);
}

int bmx_dialog_currentButton(CDKDIALOG * dialog) {
	return dialog->currentButton;
}

void bmx_curses_injectCDKButtonBox(CDKBUTTONBOX * bbox, int input) {
	injectCDKButtonbox(bbox, input);
}

void bmx_curses_injectCDKEntry(CDKENTRY * entry, int input) {
	injectCDKEntry(entry, input);
}

void bmx_curses_injectCDKCalendar(CDKCALENDAR * calendar, int input) {
	injectCDKCalendar(calendar, input);
}

int bmx_curses_getbegx(WINDOW * window) {
	return getbegx(window);
}

int bmx_curses_getbegy(WINDOW * window) {
	return getbegy(window);
}

	
WINDOW * bmx_curses_CDKButtonbox_window(CDKBUTTONBOX * bbox) {
	return bbox->win;
}

WINDOW * bmx_curses_CDKSlider_window(CDKSLIDER * slider) {
	return slider->win;
}

WINDOW * bmx_curses_CDKDialog_window(CDKDIALOG * dialog) {
	return dialog->win;
}

WINDOW * bmx_curses_CDKEntry_window(CDKENTRY * entry) {
	return entry->win;
}

WINDOW * bmx_curses_CDKLabel_window(CDKLABEL * label) {
	return label->win;
}

WINDOW * bmx_curses_CDKScroll_window(CDKLABEL * scroll) {
	return scroll->win;
}

WINDOW * bmx_curses_CDKFSelect_window(CDKLABEL * fselect) {
	return fselect->win;
}

WINDOW * bmx_curses_CDKCalendar_window(CDKCALENDAR * calendar) {
	return calendar->win;
}

WINDOW * bmx_curses_CDKMentry_window(CDKMENTRY * entry) {
	return entry->win;
}

int bmx_curses_boxHeight(void * widget, EObjectType type) {
	switch(type) {
		case vBUTTON:
			return ((CDKBUTTON *)widget)->boxHeight;
		case vBUTTONBOX:
			return ((CDKBUTTONBOX *)widget)->boxHeight;
		case vDIALOG:
			return ((CDKDIALOG *)widget)->boxHeight;
		case vDSCALE:
			return ((CDKDSCALE *)widget)->boxHeight;
		case vENTRY:
			return ((CDKENTRY *)widget)->boxHeight;
		case vFSCALE:
			return ((CDKFSCALE *)widget)->boxHeight;
		case vFSELECT:
			return ((CDKFSELECT *)widget)->boxHeight;
		case vFSLIDER:
			return ((CDKFSLIDER *)widget)->boxHeight;
		case vGRAPH:
			return ((CDKGRAPH *)widget)->boxHeight;
		case vHISTOGRAM:
			return ((CDKHISTOGRAM *)widget)->boxHeight;
		case vITEMLIST:
			return ((CDKITEMLIST *)widget)->boxHeight;
		case vLABEL:
			return ((CDKLABEL *)widget)->boxHeight;
		case vMARQUEE:
			return ((CDKMARQUEE *)widget)->boxHeight;
		case vMATRIX:
			return ((CDKMATRIX *)widget)->boxHeight;
		case vMENTRY:
			return ((CDKMENTRY *)widget)->boxHeight;
		case vRADIO:
			return ((CDKRADIO *)widget)->boxHeight;
		case vSCALE:
			return ((CDKSCALE *)widget)->boxHeight;
		case vSCROLL:
			return ((CDKSCROLL *)widget)->boxHeight;
		case vSELECTION:
			return ((CDKSELECTION *)widget)->boxHeight;
		case vSLIDER:
			return ((CDKSLIDER *)widget)->boxHeight;
		case vSWINDOW:
			return ((CDKSWINDOW *)widget)->boxHeight;
		case vTEMPLATE:
			return ((CDKTEMPLATE *)widget)->boxHeight;
		case vUSCALE:
			return ((CDKUSCALE *)widget)->boxHeight;
		case vUSLIDER:
			return ((CDKUSLIDER *)widget)->boxHeight;
		case vVIEWER:
			return ((CDKVIEWER *)widget)->boxHeight;
	}
	return 0;
}

int bmx_curses_boxWidth(void * widget, EObjectType type) {
	switch(type) {
		case vBUTTON:
			return ((CDKBUTTON *)widget)->boxHeight;
		case vBUTTONBOX:
			return ((CDKBUTTONBOX *)widget)->boxWidth;
		case vDIALOG:
			return ((CDKDIALOG *)widget)->boxWidth;
		case vDSCALE:
			return ((CDKDSCALE *)widget)->boxWidth;
		case vENTRY:
			return ((CDKENTRY *)widget)->boxWidth;
		case vFSCALE:
			return ((CDKFSCALE *)widget)->boxWidth;
		case vFSELECT:
			return ((CDKFSELECT *)widget)->boxWidth;
		case vFSLIDER:
			return ((CDKFSLIDER *)widget)->boxWidth;
		case vGRAPH:
			return ((CDKGRAPH *)widget)->boxWidth;
		case vHISTOGRAM:
			return ((CDKHISTOGRAM *)widget)->boxWidth;
		case vITEMLIST:
			return ((CDKITEMLIST *)widget)->boxWidth;
		case vLABEL:
			return ((CDKLABEL *)widget)->boxWidth;
		case vMARQUEE:
			return ((CDKMARQUEE *)widget)->boxWidth;
		case vMATRIX:
			return ((CDKMATRIX *)widget)->boxWidth;
		case vMENTRY:
			return ((CDKMENTRY *)widget)->boxWidth;
		case vRADIO:
			return ((CDKRADIO *)widget)->boxWidth;
		case vSCALE:
			return ((CDKSCALE *)widget)->boxWidth;
		case vSCROLL:
			return ((CDKSCROLL *)widget)->boxWidth;
		case vSELECTION:
			return ((CDKSELECTION *)widget)->boxWidth;
		case vSLIDER:
			return ((CDKSLIDER *)widget)->boxWidth;
		case vSWINDOW:
			return ((CDKSWINDOW *)widget)->boxWidth;
		case vTEMPLATE:
			return ((CDKTEMPLATE *)widget)->boxWidth;
		case vUSCALE:
			return ((CDKUSCALE *)widget)->boxWidth;
		case vUSLIDER:
			return ((CDKUSLIDER *)widget)->boxWidth;
		case vVIEWER:
			return ((CDKVIEWER *)widget)->boxWidth;
	}
	return 0;
}

int bmx_buttonbox_currentButton(CDKBUTTONBOX * box) {
	return box->currentButton;
}

void bmx_curses_setULCharOf(void * widget, int c) {
	setULCharOf((CDKENTRY*)widget, c);
}

void bmx_curses_setLLCharOf(void * widget, int c) {
	setLLCharOf((CDKENTRY*)widget, c);
}

void bmx_curses_setURCharOf(void * widget, int c) {
	setURCharOf((CDKENTRY*)widget, c);
}

void bmx_curses_setLRCharOf(void * widget, int c) {
	setLRCharOf((CDKENTRY*)widget, c);
}

// --------------------------------------------------------

EExitType bmx_mentry_exitType(CDKMENTRY * entry) {
	return entry->exitType;
}

void bmx_curses_drawCDKMentry(CDKMENTRY * entry) {
	drawCDKMentry(entry, ObjOf(entry)->box);
}

void bmx_curses_positionCDKMentry(CDKMENTRY * entry) {
	positionCDKMentry(entry);
}

void bmx_curses_eraseCDKMentry(CDKMENTRY * entry) {
	eraseCDKMentry(entry);
}

void bmx_curses_injectCDKMentry(CDKMENTRY * entry, int input) {
	injectCDKMentry(entry, input);
}

// --------------------------------------------------------

int ACS_ULCORNER_() {
	return ACS_ULCORNER;
}

int ACS_LLCORNER_() {
	return ACS_LLCORNER;
}

int ACS_URCORNER_() {
	return ACS_URCORNER;
}

int ACS_LRCORNER_() {
	return ACS_LRCORNER;
}

int ACS_LTEE_() {
	return ACS_LTEE;
}

int ACS_RTEE_() {
	return ACS_RTEE;
}

int ACS_BTEE_() {
	return ACS_BTEE;
}

int ACS_TTEE_() {
	return ACS_TTEE;
}

int ACS_HLINE_() {
	return ACS_HLINE;
}

int ACS_VLINE_() {
	return ACS_VLINE;
}

int ACS_PLUS_() {
	return ACS_PLUS;
}

int ACS_S1_() {
	return ACS_S1;
}

int ACS_S9_() {
	return ACS_S9;
}

int ACS_DIAMOND_() {
	return ACS_DIAMOND;
}

int ACS_CKBOARD_() {
	return ACS_CKBOARD;
}

int ACS_DEGREE_() {
	return ACS_DEGREE;
}

int ACS_PLMINUS_() {
	return ACS_PLMINUS;
}

int ACS_BULLET_() {
	return ACS_BULLET;
}

int ACS_LARROW_() {
	return ACS_HLINE;
}

int ACS_RARROW_() {
	return ACS_RARROW;
}

int ACS_DARROW_() {
	return ACS_DARROW;
}

int ACS_UARROW_() {
	return ACS_UARROW;
}

int ACS_BOARD_() {
	return ACS_BOARD;
}

int ACS_LANTERN_() {
	return ACS_LANTERN;
}

int ACS_BLOCK_() {
	return ACS_BLOCK;
}

// --------------------------------------------------------

int BUTTON1_RELEASED_() {
	return BUTTON1_RELEASED;
}
int BUTTON1_PRESSED_() {
	return BUTTON1_PRESSED;
}
int BUTTON1_CLICKED_() {
	return BUTTON1_CLICKED;
}
int BUTTON1_DOUBLE_CLICKED_() {
	return BUTTON1_DOUBLE_CLICKED;
}
int BUTTON1_TRIPLE_CLICKED_() {
	return BUTTON1_TRIPLE_CLICKED;
}
int BUTTON2_RELEASED_() {
	return BUTTON2_RELEASED;
}
int BUTTON2_PRESSED_() {
	return BUTTON2_PRESSED;
}
int BUTTON2_CLICKED_() {
	return BUTTON2_CLICKED;
}
int BUTTON2_DOUBLE_CLICKED_() {
	return BUTTON2_DOUBLE_CLICKED;
}
int BUTTON2_TRIPLE_CLICKED_() {
	return BUTTON2_TRIPLE_CLICKED;
}
int BUTTON3_RELEASED_() {
	return BUTTON3_RELEASED;
}
int BUTTON3_PRESSED_() {
	return BUTTON3_PRESSED;
}
int BUTTON3_CLICKED_() {
	return BUTTON3_CLICKED;
}
int BUTTON3_DOUBLE_CLICKED_() {
	return BUTTON3_DOUBLE_CLICKED;
}
int BUTTON3_TRIPLE_CLICKED_() {
	return BUTTON3_TRIPLE_CLICKED;
}
int BUTTON4_RELEASED_() {
	return BUTTON4_RELEASED;
}
int BUTTON4_PRESSED_() {
	return BUTTON4_PRESSED;
}
int BUTTON4_CLICKED_() {
	return BUTTON4_CLICKED;
}
int BUTTON4_DOUBLE_CLICKED_() {
	return BUTTON4_DOUBLE_CLICKED;
}
int BUTTON4_TRIPLE_CLICKED_() {
	return BUTTON4_TRIPLE_CLICKED;
}
int BUTTON_CTRL_() {
	return BUTTON_CTRL;
}
int BUTTON_SHIFT_() {
	return BUTTON_SHIFT;
}
int BUTTON_ALT_() {
	return BUTTON_ALT;
}
int REPORT_MOUSE_POSITION_() {
	return REPORT_MOUSE_POSITION;
}

int ALL_MOUSE_EVENTS_() {
	return ALL_MOUSE_EVENTS;
}


// --------------------------------------------------------

int LINES_() {
	return LINES;
}

int COLS_() {
	return COLS;
}


// --------------------------------------------------------

int bmx_curses_getchCDKObject(void * widget, EObjectType type) {

	boolean functionKey;
	
	switch(type) {
		case vENTRY:
			nodelay(InputWindowOf(ObjOf (((CDKENTRY *)widget))), true);
			return getchCDKObject(ObjOf (((CDKENTRY *)widget)), &functionKey);
	}
	
}

// --------------------------------------------------------

int bmx_mousemask(int mask, int * oldMask) {
	mmask_t om;
	int res = mousemask(mask, &om);
	*oldMask = om;
	return res;
}

BBObject * bmx_getmouse() {
	MEVENT event;
	BBObject * mouseEvent = &bbNullObject;

	if (getmouse(&event) == OK) {
printf("x = %d\n", event.x);fflush(stdout);
#ifdef BMX_NG
		mouseEvent = bah_curses_core_TMouseEvent__create(event.id, event.x, event.y, event.z, event.bstate);
#else
		mouseEvent = _bah_curses_TMouseEvent__create(event.id, event.x, event.y, event.z, event.bstate);
#endif
	}
	
	return mouseEvent;
}

int bmx_curses_waddstr(WINDOW * window, BBString * text, int maxChars) {
	char * t = bbStringToUTF8String(text);
	int ret = 0;

	if (maxChars >= 0) {
		ret = waddnstr(window, t, maxChars);
	} else {
		ret = waddstr(window, t);
	}
	bbMemFree(t);
	return ret;
}

int bmx_curses_mvwaddstr(WINDOW * window, int x, int y, BBString * text, int maxChars) {
	char * t = bbStringToUTF8String(text);
	int ret = 0;

	if (maxChars >= 0) {
		ret = mvwaddnstr(window, y, x, t, maxChars);
	} else {
		ret = mvwaddstr(window, y, x, t);
	}
	bbMemFree(t);
	return ret;
}

// --------------------------------------------------------


	