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

#include <gtk/gtk.h>
#include "Scintilla.h"
#include "SciLexer.h"
#include "ScintillaWidget.h"

#include "blitz.h"

#ifdef BMX_NG
	void bah_maxguitextareascintilla_common_TSCNotification__update(BBObject *, int, int, int);
#else
	void _bah_maxguitextareascintilla_TSCNotification__update(BBObject *, int, int, int);
#endif

ScintillaObject * bmx_mgta_scintilla_getsci(void * editor, int id) {
	ScintillaObject * obj = SCINTILLA(editor);
	scintilla_set_id(obj, id);

	scintilla_send_message(obj, SCI_SETCODEPAGE, SC_CP_UTF8, 0);

	return obj;
}

BBString * bmx_mgta_scintilla_gettext(ScintillaObject * sci) {
	int len = scintilla_send_message(sci, SCI_GETLENGTH, 0, 0);
	
	char * buffer = malloc(len + 1);
	
	scintilla_send_message(sci, SCI_GETTEXT, len, buffer);
	
	BBString * s = bbStringFromUTF8String(buffer);
	
	free(buffer);

	return s;
}

void bmx_mgta_scintilla_settext(ScintillaObject * sci, BBString * text) {

	char * t = bbStringToUTF8String(text);

	scintilla_send_message(sci, SCI_SETTEXT, 0, t);
	
	bbMemFree(t);
}

void bmx_mgta_scintilla_setfont(ScintillaObject * sci, BBString * name, int size) {
	int style;

	char * n = bbStringToUTF8String(name);

	/* make sure all the styles are changed */
	for (style = 0; style < STYLE_MAX; style++) {
		scintilla_send_message(sci, SCI_STYLESETFONT, style, n);
		scintilla_send_message(sci, SCI_STYLESETSIZE, style, size);
	}
	
	bbMemFree(n);
}

void bmx_mgta_scintilla_setlinedigits(ScintillaObject * sci, int * digits) {

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
		int textWidth = scintilla_send_message(sci, SCI_TEXTWIDTH, STYLE_LINENUMBER, buf) + 4;
		scintilla_send_message(sci, SCI_SETMARGINWIDTHN, 0, textWidth);
		
		free(buf);
	}
}

int bmx_mgta_scintilla_textwidth(ScintillaObject * sci, BBString * text) {
	char * t = bbStringToUTF8String(text);
	int textWidth = scintilla_send_message(sci, SCI_TEXTWIDTH, STYLE_LINENUMBER, t);
	bbMemFree(t);

	return textWidth;
}

int bmx_mgta_scintilla_charfrombyte(ScintillaObject * sci, int pos, int startPos) {
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
int bmx_mgta_scintilla_bytefromchar(ScintillaObject * sci, int charLength, int startBytePos, int startCharPos) {
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

int bmx_mgta_scintilla_positionfromline(ScintillaObject * sci, int line, int valueInBytes) {
	int bytePos = scintilla_send_message(sci, SCI_POSITIONFROMLINE, line, 0);
	return (valueInBytes) ? bytePos : bmx_mgta_scintilla_charfrombyte(sci, bytePos, 0);
}

void bmx_mgta_scintilla_setselectionstart(ScintillaObject * sci, int pos) {
	scintilla_send_message(sci, SCI_SETSELECTIONSTART, pos, 0);
}

void bmx_mgta_scintilla_setselectionend(ScintillaObject * sci, int pos) {
	scintilla_send_message(sci, SCI_SETSELECTIONEND, pos, 0);
}

void bmx_mgta_scintilla_scrollcaret(ScintillaObject * sci) {
	scintilla_send_message(sci, SCI_SCROLLCARET, 0, 0);
}

void bmx_mgta_scintilla_setsel(ScintillaObject * sci, int startPos, int endPos) {
	scintilla_send_message(sci, SCI_SETSEL, startPos, endPos);
}

void bmx_mgta_scintilla_replacesel(ScintillaObject * sci, const char * text) {
	scintilla_send_message(sci, SCI_REPLACESEL, 0, text);
}

void bmx_mgta_scintilla_stylesetback(ScintillaObject * sci, int col) {
	int style;
	
	for (style = 0; style < 33; style++) {
		scintilla_send_message(sci, SCI_STYLESETBACK, style, col);
	}
}

void bmx_mgta_scintilla_stylesetfore(ScintillaObject * sci, int style, int color) {
	scintilla_send_message(sci, SCI_STYLESETFORE, style, color);
}

void bmx_mgta_scintilla_stylesetitalic(ScintillaObject * sci, int style, int value) {
	scintilla_send_message(sci, SCI_STYLESETITALIC, style, value);
}

void bmx_mgta_scintilla_stylesetbold(ScintillaObject * sci, int style, int value) {
	scintilla_send_message(sci, SCI_STYLESETBOLD, style, value);
}

void bmx_mgta_scintilla_stylesetunderline(ScintillaObject * sci, int style, int value) {
	scintilla_send_message(sci, SCI_STYLESETUNDERLINE, style, value);
}

void bmx_mgta_scintilla_startstyling(ScintillaObject * sci, int startPos) {
	scintilla_send_message(sci, SCI_STARTSTYLING, startPos, 0x1f);
}

void bmx_mgta_scintilla_setstyling(ScintillaObject * sci, int realLength, int style) {
	if (realLength == -1) {
		realLength = scintilla_send_message(sci, SCI_GETLENGTH, 0, 0);
	}
	scintilla_send_message(sci, SCI_SETSTYLING, realLength, style);
}

BBString * bmx_mgta_scintilla_gettextrange(ScintillaObject * sci, int startPos, int endPos) {
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

int bmx_mgta_scintilla_getlinecount(ScintillaObject * sci) {
	return scintilla_send_message(sci, SCI_GETLINECOUNT, 0, 0);
}

int bmx_mgta_scintilla_getlength(ScintillaObject * sci) {
	return scintilla_send_message(sci, SCI_GETLENGTH, 0, 0);
}

int bmx_mgta_scintilla_getcurrentpos(ScintillaObject * sci) {
	int bytePos = scintilla_send_message(sci, SCI_GETSELECTIONSTART, 0, 0);
	return bmx_mgta_scintilla_charfrombyte(sci, bytePos, 0);
}

int bmx_mgta_scintilla_getcurrentline(ScintillaObject * sci) {
	return scintilla_send_message(sci, SCI_LINEFROMPOSITION, scintilla_send_message(sci, SCI_GETSELECTIONSTART, 0, 0), 0);
}

void bmx_mgta_scintilla_settabwidth(ScintillaObject * sci, int tabs) {
	scintilla_send_message(sci, SCI_SETTABWIDTH, tabs, 0);
}

void bmx_mgta_scintilla_settargetstart(ScintillaObject * sci, int pos) {
	scintilla_send_message(sci, SCI_SETTARGETSTART, pos, 0);
}

void bmx_mgta_scintilla_settargetend(ScintillaObject * sci, int pos) {
	scintilla_send_message(sci, SCI_SETTARGETEND, pos, 0);
}

void bmx_mgta_scintilla_replacetarget(ScintillaObject * sci, const char * text) {
	scintilla_send_message(sci, SCI_REPLACETARGET, -1, text);
}

void bmx_mgta_scintilla_cut(ScintillaObject * sci) {
	scintilla_send_message(sci, SCI_CUT, 0, 0);
}

void bmx_mgta_scintilla_copy(ScintillaObject * sci) {
	scintilla_send_message(sci, SCI_COPY, 0, 0);
}

void bmx_mgta_scintilla_paste(ScintillaObject * sci) {
	scintilla_send_message(sci, SCI_PASTE, 0, 0);
}

int bmx_mgta_scintilla_linefromposition(ScintillaObject * sci, int pos) {
	return scintilla_send_message(sci, SCI_LINEFROMPOSITION, pos, 0);
}

void bmx_mgta_scintilla_appendtext(ScintillaObject * sci, BBString * text) {
	char * s = bbStringToUTF8String(text);
	scintilla_send_message(sci, SCI_APPENDTEXT, strlen(s), s);
	bbMemFree(s);
}

void bmx_mgta_scintilla_scrolltoend(ScintillaObject * sci) {
	scintilla_send_message(sci, SCI_GOTOPOS, scintilla_send_message(sci, SCI_GETLENGTH, 0, 0), 0);
}

int bmx_mgta_scintilla_getselectionlength(ScintillaObject * sci, int units) {
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

void bmx_mgta_scintilla_addtext(ScintillaObject * sci, gchar * text) {
	int length = strlen(text);
	scintilla_send_message(sci, SCI_ADDTEXT, length, text);
}

void bmx_mgta_scintilla_notifcation_update(BBObject * obj, struct SCNotification * notification) {
#ifdef BMX_NG
	bah_maxguitextareascintilla_common_TSCNotification__update(obj, notification->nmhdr.code, notification->modificationType, notification->updated);
#else
	_bah_maxguitextareascintilla_TSCNotification__update(obj, notification->nmhdr.code, notification->modificationType, notification->updated);
#endif
}

void bmx_mgta_scintilla_setmarginleft(ScintillaObject * sci, int leftmargin) {
	scintilla_send_message(sci, SCI_SETMARGINLEFT, 0, leftmargin);
}

