/*
 Copyright (c) 2014 Bruce A Henderson
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 
*/

#include "ScintillaView.h"
//#elif __APPLE__
//#include "Platform.h"
//#include "PlatCocoa.h"
//#else
//#include "PlatWin.h"
//#endif
#include "SciLexer.h"
#include "ScintillaWidget.h"

extern "C" {
#include "blitz.h"
	ScintillaView * bmx_mgta_scintilla_getsci(void * editor, int id);
	BBString * bmx_mgta_scintilla_gettext(ScintillaView * sci);
	void bmx_mgta_scintilla_settext(ScintillaView * sci, BBString * text);
	void bmx_mgta_scintilla_setfont(ScintillaView * sci, BBString * name, int size);
	void bmx_mgta_scintilla_setlinedigits(ScintillaView * sci, int * digits);
	int bmx_mgta_scintilla_textwidth(ScintillaView * sci, BBString * text);
	int bmx_mgta_scintilla_charfrombyte(ScintillaView * sci, int pos, int startPos);
	int bmx_mgta_scintilla_bytefromchar(ScintillaView * sci, int charLength, int startBytePos, int startCharPos);
	int bmx_mgta_scintilla_positionfromline(ScintillaView * sci, int line, int valueInBytes);
	void bmx_mgta_scintilla_setselectionstart(ScintillaView * sci, int pos);
	void bmx_mgta_scintilla_setselectionend(ScintillaView * sci, int pos);
	void bmx_mgta_scintilla_scrollcaret(ScintillaView * sci);
	void bmx_mgta_scintilla_setsel(ScintillaView * sci, int startPos, int endPos);
	void bmx_mgta_scintilla_replacesel(ScintillaView * sci, const char * text);
	void bmx_mgta_scintilla_stylesetback(ScintillaView * sci, int col);
	void bmx_mgta_scintilla_stylesetfore(ScintillaView * sci, int style, int color);
	void bmx_mgta_scintilla_stylesetitalic(ScintillaView * sci, int style, int value);
	void bmx_mgta_scintilla_stylesetbold(ScintillaView * sci, int style, int value);
	void bmx_mgta_scintilla_stylesetunderline(ScintillaView * sci, int style, int value);
	void bmx_mgta_scintilla_startstyling(ScintillaView * sci, int startPos);
	void bmx_mgta_scintilla_setstyling(ScintillaView * sci, int realLength, int style);
	BBString * bmx_mgta_scintilla_gettextrange(ScintillaView * sci, int startPos, int endPos);
	int bmx_mgta_scintilla_getlinecount(ScintillaView * sci);
	int bmx_mgta_scintilla_getlength(ScintillaView * sci);
	int bmx_mgta_scintilla_getcurrentpos(ScintillaView * sci);
	int bmx_mgta_scintilla_getcurrentline(ScintillaView * sci);
	void bmx_mgta_scintilla_settabwidth(ScintillaView * sci, int tabs);
	void bmx_mgta_scintilla_settargetstart(ScintillaView * sci, int pos);
	void bmx_mgta_scintilla_settargetend(ScintillaView * sci, int pos);
	void bmx_mgta_scintilla_replacetarget(ScintillaView * sci, const char * text);
	void bmx_mgta_scintilla_cut(ScintillaView * sci);
	void bmx_mgta_scintilla_copy(ScintillaView * sci);
	void bmx_mgta_scintilla_paste(ScintillaView * sci);
	int bmx_mgta_scintilla_linefromposition(ScintillaView * sci, int pos);
	void bmx_mgta_scintilla_appendtext(ScintillaView * sci, BBString * text);
	void bmx_mgta_scintilla_scrolltoend(ScintillaView * sci);
	int bmx_mgta_scintilla_getselectionlength(ScintillaView * sci, int units);
	void bmx_mgta_scintilla_addtext(ScintillaView * sci, char * text);

}

ScintillaView * bmx_mgta_scintilla_getsci(void * editor, int id) {
	ScintillaView * obj = SCINTILLA(editor);
	scintilla_set_id(obj, id);

	scintilla_send_message(obj, SCI_SETCODEPAGE, SC_CP_UTF8, 0);

	return obj;
}

BBString * bmx_mgta_scintilla_gettext(ScintillaView * sci) {
	int len = scintilla_send_message(sci, SCI_GETLENGTH, 0, 0);
	
	char * buffer = malloc(len + 1);
	
	scintilla_send_message(sci, SCI_GETTEXT, len, buffer);
	
	BBString * s = bbStringFromUTF8String(buffer);
	
	free(buffer);

	return s;
}

void bmx_mgta_scintilla_settext(ScintillaView * sci, BBString * text) {

	char * t = bbStringToUTF8String(text);

	scintilla_send_message(sci, SCI_SETTEXT, 0, t);
	
	bbMemFree(t);
}

void bmx_mgta_scintilla_setfont(ScintillaView * sci, BBString * name, int size) {
	int style;

	char * n = bbStringToUTF8String(name);

	/* make sure all the styles are changed */
	for (style = 0; style < STYLE_MAX; style++) {
		scintilla_send_message(sci, SCI_STYLESETFONT, style, n);
		scintilla_send_message(sci, SCI_STYLESETSIZE, style, size);
	}
	
	bbMemFree(n);
}

void bmx_mgta_scintilla_setlinedigits(ScintillaView * sci, int * digits) {

	int lines = scintilla_send_message(sci, SCI_GETLINECOUNT, 0, 0);
	int newDigits = (lines < 10 ? 1 : (lines < 100 ? 2 :   
		(lines < 1000 ? 3 : (lines < 10000 ? 4 :   
		(lines < 100000 ? 5 : (lines < 1000000 ? 6 :   
		(lines < 10000000 ? 7 : (lines < 100000000 ? 8 :  
		(lines < 1000000000 ? 9 : 10)))))))));
	
	if (*digits != newDigits) {
		*digits = newDigits;

		int i;
		int len = newDigits + 1;
		char * buf = malloc(len + 1);
		buf[0] = '_';
		buf[len] = 0;
		for (i = 1; i < len; i++) {
			buf[i] = '9';
		}
		
		/* set the linenumber margin width */
		int textWidth = scintilla_send_message(sci, SCI_TEXTWIDTH, STYLE_LINENUMBER, buf);
		scintilla_send_message(sci, SCI_SETMARGINWIDTHN, 0, textWidth);
		
		free(buf);
	}
}

int bmx_mgta_scintilla_textwidth(ScintillaView * sci, BBString * text) {
	char * t = bbStringToUTF8String(text);
	int textWidth = scintilla_send_message(sci, SCI_TEXTWIDTH, STYLE_LINENUMBER, t);
	bbMemFree(t);

	return textWidth;
}

int bmx_mgta_scintilla_charfrombyte(ScintillaView * sci, int pos, int startPos) {
	int i;
	int characterOffset = 0;
	int length = pos - startPos;

	if (length == 0) {
		return 0;
	}

	struct Sci_TextRange range;
	range.chrg.cpMin = startPos;
	range.chrg.cpMax = pos;
	
	range.lpstrText = malloc(length + 1);
	
	int len = scintilla_send_message(sci, SCI_GETTEXTRANGE, 0, &range);

	for (i = 0; i < length; i++) {
		char c = range.lpstrText[i];
		if ((c & 0xc0) != 0x80) {
				characterOffset++;
		}
	}
	free(range.lpstrText);

	return characterOffset;
}

/* startBytePos is a known byte offset for startCharPos */
int bmx_mgta_scintilla_bytefromchar(ScintillaView * sci, int charLength, int startBytePos, int startCharPos) {
	int i;
	int characterOffset = startBytePos;

	int characters = charLength;
	if (!characters) {
		return startBytePos;
	}

	int endBytePos = scintilla_send_message(sci, SCI_GETLENGTH, 0, 0);

	if (characters == -1) {
		return endBytePos;
	}

	int length = endBytePos - startBytePos + 1;

	struct Sci_TextRange range;
	range.chrg.cpMin = startBytePos;
	range.chrg.cpMax = endBytePos;
	
	range.lpstrText = malloc(length);
	
	int len = scintilla_send_message(sci, SCI_GETTEXTRANGE, 0, &range);

	for (i = 0; i < len; i++) {
		char c = range.lpstrText[i];
		if ((c & 0xc0) != 0x80) {
				if (! --characters) {
					break;
				}
		}
	}
	free(range.lpstrText);

	return startBytePos + i + 1;
}

int bmx_mgta_scintilla_positionfromline(ScintillaView * sci, int line, int valueInBytes) {
	int bytePos = scintilla_send_message(sci, SCI_POSITIONFROMLINE, line, 0);
	return (valueInBytes) ? bytePos : bmx_mgta_scintilla_charfrombyte(sci, bytePos, 0);
}

void bmx_mgta_scintilla_setselectionstart(ScintillaView * sci, int pos) {
	scintilla_send_message(sci, SCI_SETSELECTIONSTART, pos, 0);
}

void bmx_mgta_scintilla_setselectionend(ScintillaView * sci, int pos) {
	scintilla_send_message(sci, SCI_SETSELECTIONEND, pos, 0);
}

void bmx_mgta_scintilla_scrollcaret(ScintillaView * sci) {
	scintilla_send_message(sci, SCI_SCROLLCARET, 0, 0);
}

void bmx_mgta_scintilla_setsel(ScintillaView * sci, int startPos, int endPos) {
	scintilla_send_message(sci, SCI_SETSEL, startPos, endPos);
}

void bmx_mgta_scintilla_replacesel(ScintillaView * sci, const char * text) {
	scintilla_send_message(sci, SCI_REPLACESEL, 0, text);
}

void bmx_mgta_scintilla_stylesetback(ScintillaView * sci, int col) {
	int style;
	
	for (style = 0; style < 33; style++) {
		scintilla_send_message(sci, SCI_STYLESETBACK, style, col);
	}
}

void bmx_mgta_scintilla_stylesetfore(ScintillaView * sci, int style, int color) {
	scintilla_send_message(sci, SCI_STYLESETFORE, style, color);
}

void bmx_mgta_scintilla_stylesetitalic(ScintillaView * sci, int style, int value) {
	scintilla_send_message(sci, SCI_STYLESETITALIC, style, value);
}

void bmx_mgta_scintilla_stylesetbold(ScintillaView * sci, int style, int value) {
	scintilla_send_message(sci, SCI_STYLESETBOLD, style, value);
}

void bmx_mgta_scintilla_stylesetunderline(ScintillaView * sci, int style, int value) {
	scintilla_send_message(sci, SCI_STYLESETUNDERLINE, style, value);
}

void bmx_mgta_scintilla_startstyling(ScintillaView * sci, int startPos) {
	scintilla_send_message(sci, SCI_STARTSTYLING, startPos, 0x1f);
}

void bmx_mgta_scintilla_setstyling(ScintillaView * sci, int realLength, int style) {
	if (realLength == -1) {
		realLength = scintilla_send_message(sci, SCI_GETLENGTH, 0, 0);
	}
	scintilla_send_message(sci, SCI_SETSTYLING, realLength, style);
}

BBString * bmx_mgta_scintilla_gettextrange(ScintillaView * sci, int startPos, int endPos) {
	if (endPos == -1) {
		endPos = scintilla_send_message(sci, SCI_GETLENGTH, 0, 0);
	}

	struct Sci_TextRange range;
	range.chrg.cpMin = startPos;
	range.chrg.cpMax = endPos;
	
	range.lpstrText = malloc(endPos - startPos + 1);
	
	int len = scintilla_send_message(sci, SCI_GETTEXTRANGE, 0, &range);
	
	BBString * s = bbStringFromUTF8String(range.lpstrText);
	free(range.lpstrText);
	return s;
}

int bmx_mgta_scintilla_getlinecount(ScintillaView * sci) {
	return scintilla_send_message(sci, SCI_GETLINECOUNT, 0, 0);
}

int bmx_mgta_scintilla_getlength(ScintillaView * sci) {
	return scintilla_send_message(sci, SCI_GETLENGTH, 0, 0);
}

int bmx_mgta_scintilla_getcurrentpos(ScintillaView * sci) {
	int bytePos = scintilla_send_message(sci, SCI_GETSELECTIONSTART, 0, 0);
	return bmx_mgta_scintilla_charfrombyte(sci, bytePos, 0);
}

int bmx_mgta_scintilla_getcurrentline(ScintillaView * sci) {
	return scintilla_send_message(sci, SCI_LINEFROMPOSITION, scintilla_send_message(sci, SCI_GETSELECTIONSTART, 0, 0), 0);
}

void bmx_mgta_scintilla_settabwidth(ScintillaView * sci, int tabs) {
	scintilla_send_message(sci, SCI_SETTABWIDTH, tabs, 0);
}

void bmx_mgta_scintilla_settargetstart(ScintillaView * sci, int pos) {
	scintilla_send_message(sci, SCI_SETTARGETSTART, pos, 0);
}

void bmx_mgta_scintilla_settargetend(ScintillaView * sci, int pos) {
	scintilla_send_message(sci, SCI_SETTARGETEND, pos, 0);
}

void bmx_mgta_scintilla_replacetarget(ScintillaView * sci, const char * text) {
	scintilla_send_message(sci, SCI_REPLACETARGET, -1, text);
}

void bmx_mgta_scintilla_cut(ScintillaView * sci) {
	scintilla_send_message(sci, SCI_CUT, 0, 0);
}

void bmx_mgta_scintilla_copy(ScintillaView * sci) {
	scintilla_send_message(sci, SCI_COPY, 0, 0);
}

void bmx_mgta_scintilla_paste(ScintillaView * sci) {
	scintilla_send_message(sci, SCI_PASTE, 0, 0);
}

int bmx_mgta_scintilla_linefromposition(ScintillaView * sci, int pos) {
	return scintilla_send_message(sci, SCI_LINEFROMPOSITION, pos, 0);
}

void bmx_mgta_scintilla_appendtext(ScintillaView * sci, BBString * text) {
	char * s = bbStringToUTF8String(text);
	scintilla_send_message(sci, SCI_APPENDTEXT, strlen(s), s);
	bbMemFree(s);
}

void bmx_mgta_scintilla_scrolltoend(ScintillaView * sci) {
	scintilla_send_message(sci, SCI_GOTOPOS, scintilla_send_message(sci, SCI_GETLENGTH, 0, 0), 0);
}

int bmx_mgta_scintilla_getselectionlength(ScintillaView * sci, int units) {
	if (units == 2) {
		/* lines */
		int startPos = scintilla_send_message(sci, SCI_LINEFROMPOSITION, scintilla_send_message(sci, SCI_GETSELECTIONSTART, 0, 0), 0);
		int endPos = scintilla_send_message(sci, SCI_LINEFROMPOSITION, scintilla_send_message(sci, SCI_GETSELECTIONEND, 0, 0), 0);
		return endPos - startPos;
	} else {
		/* chars */
		int startPos = bmx_mgta_scintilla_charfrombyte(sci, scintilla_send_message(sci, SCI_GETSELECTIONSTART, 0, 0), 0);
		int length = bmx_mgta_scintilla_charfrombyte(sci, scintilla_send_message(sci, SCI_GETSELECTIONEND, 0, 0), startPos);
		return length;
	}
}

void bmx_mgta_scintilla_addtext(ScintillaView * sci, char * text) {
	int length = strlen(text);
	scintilla_send_message(sci, SCI_ADDTEXT, length, text);
}

