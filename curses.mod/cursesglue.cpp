

extern "C" {

#include <cdk.h>


	EExitType bmx_entry_exitType(CDKENTRY * entry);
	EExitType bmx_slider_exitType(CDKSLIDER * slider);

	void bmx_curses_drawCDKEntry(CDKENTRY * entry);
	void bmx_curses_positionCDKEntry(CDKENTRY * entry);
	void bmx_curses_positionCDKLabel(CDKLABEL * label);
	void bmx_curses_drawCDKLabel(CDKLABEL * label);

	void bmx_curses_eraseCDKEntry(CDKENTRY * entry);
	void bmx_curses_eraseCDKLabel(CDKLABEL * label);
	
	int bmx_dialog_currentButton(CDKDIALOG * dialog);
	
	void bmx_curses_injectCDKButtonBox(CDKBUTTONBOX * bbox, int input);
	void bmx_curses_injectCDKEntry(CDKENTRY * entry, int input);
	
	int bmx_curses_getbegx(WINDOW * window);
	int bmx_curses_getbegy(WINDOW * window);
	
	WINDOW * bmx_curses_CDKButtonbox_window(CDKBUTTONBOX * bbox);
	WINDOW * bmx_curses_CDKSlider_window(CDKSLIDER * slider);
	WINDOW * bmx_curses_CDKDialog_window(CDKDIALOG * dialog);
	WINDOW * bmx_curses_CDKEntry_window(CDKENTRY * entry);
	WINDOW * bmx_curses_CDKLabel_window(CDKLABEL * label);
	WINDOW * bmx_curses_CDKScroll_window(CDKLABEL * scroll);
	WINDOW * bmx_curses_CDKFSelect_window(CDKLABEL * fselect);
	
	int bmx_curses_boxHeight(void * widget, EObjectType type);
	int bmx_curses_boxWidth(void * widget, EObjectType type);
	
	int bmx_buttonbox_currentButton(CDKBUTTONBOX * box);
	void bmx_curses_drawCDKButtonBox(CDKBUTTONBOX * box);

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
	
	int bmx_curses_getchCDKObject(void * widget, EObjectType type); 

}

EExitType bmx_entry_exitType(CDKENTRY * entry) {
	return entry->exitType;
}

EExitType bmx_slider_exitType(CDKSLIDER * slider) {
	return slider->exitType;
}


void bmx_curses_drawCDKEntry(CDKENTRY * entry) {
	drawCDKEntry(entry, ObjOf(entry)->box);
}

void bmx_curses_positionCDKEntry(CDKENTRY * entry) {
	positionCDKEntry(entry);
}

void bmx_curses_positionCDKLabel(CDKLABEL * label) {
	positionCDKLabel(label);
}

void bmx_curses_eraseCDKEntry(CDKENTRY * entry) {
	eraseCDKEntry(entry);
}

void bmx_curses_eraseCDKLabel(CDKLABEL * label) {
	eraseCDKLabel(label);
}

void bmx_curses_drawCDKLabel(CDKLABEL * label) {
	drawCDKLabel(label, ObjOf(label)->box);
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
			return ((CDKENTRY *)widget)->boxHeight;
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
			return ((CDKENTRY *)widget)->boxWidth;
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

int bmx_curses_getchCDKObject(void * widget, EObjectType type) {

	boolean functionKey;
	
	switch(type) {
		case vENTRY:
			nodelay(InputWindowOf(ObjOf (((CDKENTRY *)widget))), true);
			return getchCDKObject(ObjOf (((CDKENTRY *)widget)), &functionKey);
	}
	
}

	
	