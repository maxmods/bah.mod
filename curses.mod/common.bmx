' Copyright (c) 2007-2013 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Import BRL.Blitz

?linux
Import "-L/usr/lib"
Import "-lncurses"
Import "-lpanel"

?win32
Import "pdcurses_src.bmx"

Import "-ladvapi32"
Import "src/pdcurses/*.h"

?macos

Import "-L/usr/lib"
Import "-lncurses"
Import "-lpanel"

?

Import "cdk_src.bmx"


Import "src/cdk/include/*.h"
Import "cursesglue.cpp"

?win32
Import "win32glue.cpp"
?

Extern

	Function initscr:Byte Ptr()
	Function endCDK()
	Function initCDKScreen:Byte Ptr(win:Byte Ptr)
	Function destroyCDKScreen(cdk:Byte Ptr)
	
	Function refreshCDKScreen(screen:Byte Ptr)
	
	Function initCDKColor()
	
	Function newCDKDialog:Byte Ptr(screen:Byte Ptr, x:Int, y:Int, text:Byte Ptr, tcount:Int, buttons:Byte Ptr, bcount:Int, highlight:Int, seaparator:Int, box:Int, shadow:Int)
	Function destroyCDKObject(obj:Byte Ptr) = "_destroyCDKObject"
	Function activateCDKDialog:Int(dialog:Byte Ptr, actions:Int)
	Function bmx_curses_CDKDialog_window:Byte Ptr(dialog:Byte Ptr)
	
	Function newCDKEntry:Byte Ptr(screen:Byte Ptr, x:Int, y:Int, title:Byte Ptr, label:Byte Ptr, ..
		attribute:Int, filler:Int, displayType:Int, width:Int, _min:Int, _max:Int, box:Int, shadow:Int)
	Function activateCDKEntry:Byte Ptr(entry:Byte Ptr, actions:Int)
	Function setCDKEntry(entry:Byte Ptr, value:Byte Ptr, _min:Int, _max:Int, box:Int)
	Function setCDKEntryValue(entry:Byte Ptr, value:Byte Ptr)
	Function setCDKEntryMax(entry:Byte Ptr, value:Int)
	Function setCDKEntryMin(entry:Byte Ptr, value:Int)
	Function bmx_curses_CDKEntry_window:Byte Ptr(entry:Byte Ptr)
	Function bmx_curses_injectCDKEntry(entry:Byte Ptr, key:Int)
	Function getCDKEntryValue:Byte Ptr(entry:Byte Ptr)
	
	Function popupLabel_(screen:Byte Ptr, msg:Byte Ptr, length:Int) = "popupLabel"
	Function popupLabelAttrib(screen:Byte Ptr, msg:Byte Ptr, length:Int, attrib:Int)
	
	Function newCDKLabel:Byte Ptr(screen:Byte Ptr, x:Int, y:Int, mesg:Byte Ptr, mcount:Int, box:Int, shadow:Int)
	Function waitCDKLabel(widget:Byte Ptr, char:Int)
	Function setCDKLabelMessage(widget:Byte Ptr, mesg:Byte Ptr, mcount:Int)
	Function bmx_curses_CDKLabel_window:Byte Ptr(label:Byte Ptr)
	
	Function newCDKScroll:Byte Ptr(screen:Byte Ptr, x:Int, y:Int, spos:Int, width:Int, height:Int, ..
		titlePtr:Byte Ptr, listPtr:Byte Ptr, listCount:Int, numbers:Int, highlight:Int, box:Int, shadow:Int)
	Function activateCDKScroll:Int(scroll:Byte Ptr, actions:Int)
	Function bmx_curses_scroll_GetItem:Byte Ptr(scroll:Byte Ptr, index:Int)
	Function getCDKScrollCurrentItem:Int(scroll:Byte Ptr)
	Function setCDKScrollCurrentItem(scroll:Byte Ptr, index:Int)
	Function addCDKScrollItem(scroll:Byte Ptr, item:Byte Ptr)
	Function insertCDKScrollItem(scroll:Byte Ptr, item:Byte Ptr)
	Function deleteCDKScrollItem(scroll:Byte Ptr, index:Int)
	Function bmx_curses_CDKScroll_window:Byte Ptr(slider:Byte Ptr)

	Function newCDKSlider:Byte Ptr(screen:Byte Ptr, x:Int, y:Int, title:Byte Ptr, label:Byte Ptr, ..
		fieldAttribute:Int, fieldWidth:Int, start:Int, low:Int, high:Int, stepSize:Int, pageSize:Int, box:Int, shadow:Int)
	Function activateCDKSlider:Int(slider:Byte Ptr, actions:Int)
	Function setCDKSliderBox(slider:Byte Ptr, box:Int)
	Function getCDKSliderValue:Int(slider:Byte Ptr)
	Function getCDKSliderBox:Int(slider:Byte Ptr)
	Function setCDKSliderLowHigh(slider:Byte Ptr, low:Int, high:Int)
	Function getCDKSliderLowValue:Int(slider:Byte Ptr)
	Function getCDKSliderHighValue:Int(slider:Byte Ptr)
	Function setCDKSliderValue(slider:Byte Ptr, value:Int)
	Function bmx_curses_CDKSlider_window:Byte Ptr(slider:Byte Ptr)
	
	Function newCDKCalendar:Byte Ptr(screen:Byte Ptr, x:Int, y:Int, title:Byte Ptr, _day:Int, _month:Int, ..
		_year:Int, dayAttrib:Int, monthAttrib:Int, yearAttrib:Int, highlight:Int, box:Int, shadow:Int)
	Function setCDKCalendarDate(calendar:Byte Ptr, _day:Int, _month:Int, _year:Int)
	Function getCDKCalendarDate(calendar:Byte Ptr, _day:Int Ptr, _month:Int Ptr, _year:Int Ptr)
	Function setCDKCalendarMonthsNames(calendar:Byte Ptr, names:Byte Ptr)
	Function setCDKCalendarDaysNames(calendar:Byte Ptr, names:Byte Ptr)
	Function setCDKCalendarMarker(calendar:Byte Ptr, _day:Int, _month:Int, _year:Int, marker:Int)
	Function getCDKCalendarMarker:Int(calendar:Byte Ptr, _day:Int, _month:Int, _year:Int)
	Function removeCDKCalendarMarker(calendar:Byte Ptr, _day:Int, _month:Int, _year:Int)
	Function bmx_curses_positionCDKCalendar(calendar:Byte Ptr)
	Function bmx_curses_drawCDKCalendar(widgetPtr:Byte Ptr)
	Function bmx_curses_eraseCDKCalendar(calendar:Byte Ptr)
	Function bmx_curses_injectCDKCalendar(widgetPtr:Byte Ptr, char:Int)
	Function bmx_curses_CDKCalendar_window:Byte Ptr(calendar:Byte Ptr)

	Function bmx_entry_exitType:Int(entry:Byte Ptr)
	Function bmx_curses_drawCDKEntry(entry:Byte Ptr)
	Function bmx_curses_positionCDKEntry(entry:Byte Ptr)
	Function bmx_curses_eraseCDKEntry(entry:Byte Ptr)
	
	Function bmx_curses_positionCDKLabel(label:Byte Ptr)
	Function bmx_curses_drawCDKLabel(widgetPtr:Byte Ptr)
	Function bmx_curses_eraseCDKLabel(label:Byte Ptr)
	
	Function bmx_slider_exitType:Int(slider:Byte Ptr)
	
	Function cbreak:Int()
	Function nocbreak:Int()
	Function echo:Int()
	Function noecho:Int()
	Function raw:Int()
	Function noraw:Int()
	
	Function newwin:Byte Ptr(h:Int, w:Int, y:Int, x:Int)
	Function delwin:Int(window:Byte Ptr)
	Function mvwin:Int(window:Byte Ptr, y:Int, x:Int)
	
	Function bmx_curses_waddstr:Int(window:Byte Ptr, text:String, maxChars:Int)
	Function bmx_curses_mvwaddstr:Int(window:Byte Ptr, x:Int, y:Int, text:String, maxChars:Int)

	
	'Function bmx_cursespanel_new:Byte Ptr(handle:Object, x:Int, y:Int, w:Int, h:Int)
	
	Function new_panel:Byte Ptr(window:Byte Ptr)
	Function hide_panel:Int(panel:Byte Ptr)
	Function bottom_panel:Int(panel:Byte Ptr)
	Function move_panel:Int(panel:Byte Ptr, y:Int, x:Int)
	Function panel_hidden:Int(panel:Byte Ptr)
	Function show_panel:Int(panel:Byte Ptr)
	Function top_panel:Int(panel:Byte Ptr)

	Function bindCDKObject(cdktype:Int, widget:Byte Ptr, key:Int, ..
		callback:Int(cdktype:Int, obj:Byte Ptr, clientData:Object, inp:Int), data:Object)
	Function unbindCDKObject(cdktype:Int, widget:Byte Ptr, key:Int)
	Function cleanCDKObjectBindings(cdktype:Int, widget:Byte Ptr)
	Function isCDKObjectBind:Int(cdktype:Int, widget:Byte Ptr, key:Int)
	
	Function setCDKObjectPreProcess(widget:Byte Ptr, ..
		callback:Int(cdktype:Int, obj:Byte Ptr, clientData:Object, inp:Int), data:Object)
	
	Function bmx_dialog_currentButton:Int(widget:Byte Ptr)

	Function newCDKFselect:Byte Ptr(screen:Byte Ptr, x:Int, y:Int, height:Int, width:Int, title:Byte Ptr, label:Byte Ptr, ..
		fieldAttribute:Int, filler:Int, highlight:Int, dAttribute:Byte Ptr, fAttribute:Byte Ptr, lAttribute:Byte Ptr, ..
		sAttribute:Byte Ptr, box:Int, shadow:Int)
	Function activateCDKFselect:Byte Ptr(widgetPtr:Byte Ptr, actions:Int)
	Function bmx_curses_CDKFSelect_window:Byte Ptr(slider:Byte Ptr)
	
	Function newCDKButtonbox:Byte Ptr(screen:Byte Ptr, x:Int, y:Int, height:Int, width:Int, title:Byte Ptr, ..
		rows:Int, cols:Int, buttons:Byte Ptr, buttonCount:Int, highlight:Int, box:Int, shadow:Int)
	Function activateCDKButtonbox:Int(widgetPtr:Byte Ptr, actions:Int)
	Function bmx_curses_CDKButtonbox_window:Byte Ptr(widgetPtr:Byte Ptr)
	Function bmx_buttonbox_currentButton:Int(widgetPtr:Byte Ptr)
	Function bmx_curses_drawCDKButtonBox(widgetPtr:Byte Ptr)

	Function bmx_curses_injectCDKButtonBox(widgetPtr:Byte Ptr, char:Int)
	
	Function newCDKMentry:Byte Ptr(screen:Byte Ptr, x:Int, y:Int, title:Byte Ptr, label:Byte Ptr, ..
		attribute:Int, filler:Int, displayType:Int, width:Int, rows:Int, logicalRows:Int, _min:Int, box:Int, shadow:Int)
	Function activateCDKMentry:Byte Ptr(entry:Byte Ptr, actions:Int)
	Function setCDKMentry(entry:Byte Ptr, value:Byte Ptr, _min:Int, box:Int)
	Function setCDKMentryValue(entry:Byte Ptr, value:Byte Ptr)
	Function setCDKMentryMin(entry:Byte Ptr, value:Int)
	Function bmx_curses_CDKMentry_window:Byte Ptr(entry:Byte Ptr)
	Function bmx_curses_injectCDKMentry(entry:Byte Ptr, key:Int)
	Function getCDKMentryValue:Byte Ptr(entry:Byte Ptr)
	Function getCDKMentryMin:Int(entry:Byte Ptr)
	Function bmx_mentry_exitType:Int(slider:Byte Ptr)
	Function bmx_curses_drawCDKMentry(entry:Byte Ptr)
	Function bmx_curses_positionCDKMentry(entry:Byte Ptr)
	Function bmx_curses_eraseCDKMentry(entry:Byte Ptr)

	Function bmx_curses_getbegx:Int(widgetPtr:Byte Ptr)
	Function bmx_curses_getbegy:Int(widgetPtr:Byte Ptr)
	
	Function bmx_curses_boxHeight:Int(widgetPtr:Byte Ptr, wtype:Int)
	Function bmx_curses_boxWidth:Int(widgetPtr:Byte Ptr, wtype:Int)
	
	Function bmx_curses_setULCharOf(widgetPtr:Byte Ptr, char:Int)
	Function bmx_curses_setLLCharOf(widgetPtr:Byte Ptr, char:Int)
	Function bmx_curses_setURCharOf(widgetPtr:Byte Ptr, char:Int)
	Function bmx_curses_setLRCharOf(widgetPtr:Byte Ptr, char:Int)
	
	Function ACS_ULCORNER_:Int()
	Function ACS_LLCORNER_:Int()
	Function ACS_URCORNER_:Int()
	Function ACS_LRCORNER_:Int()
	Function ACS_LTEE_:Int()
	Function ACS_RTEE_:Int()
	Function ACS_BTEE_:Int()
	Function ACS_TTEE_:Int()
	Function ACS_HLINE_:Int()
	Function ACS_VLINE_:Int()
	Function ACS_PLUS_:Int()
	Function ACS_S1_:Int()
	Function ACS_S9_:Int()
	Function ACS_DIAMOND_:Int()
	Function ACS_CKBOARD_:Int()
	Function ACS_DEGREE_:Int()
	Function ACS_PLMINUS_:Int()
	Function ACS_BULLET_:Int()
	Function ACS_LARROW_:Int()
	Function ACS_RARROW_:Int()
	Function ACS_DARROW_:Int()
	Function ACS_UARROW_:Int()
	Function ACS_BOARD_:Int()
	Function ACS_LANTERN_:Int()
	Function ACS_BLOCK_:Int()

	Function BUTTON1_RELEASED_:Int()
	Function BUTTON1_PRESSED_:Int()
	Function BUTTON1_CLICKED_:Int()
	Function BUTTON1_DOUBLE_CLICKED_:Int()
	Function BUTTON1_TRIPLE_CLICKED_:Int()
	Function BUTTON2_RELEASED_:Int()
	Function BUTTON2_PRESSED_:Int()
	Function BUTTON2_CLICKED_:Int()
	Function BUTTON2_DOUBLE_CLICKED_:Int()
	Function BUTTON2_TRIPLE_CLICKED_:Int()
	Function BUTTON3_RELEASED_:Int()
	Function BUTTON3_PRESSED_:Int()
	Function BUTTON3_CLICKED_:Int()
	Function BUTTON3_DOUBLE_CLICKED_:Int()
	Function BUTTON3_TRIPLE_CLICKED_:Int()
	Function BUTTON4_RELEASED_:Int()
	Function BUTTON4_PRESSED_:Int()
	Function BUTTON4_CLICKED_:Int()
	Function BUTTON4_DOUBLE_CLICKED_:Int()
	Function BUTTON4_TRIPLE_CLICKED_:Int()
	Function BUTTON_CTRL_:Int()
	Function BUTTON_SHIFT_:Int()
	Function BUTTON_ALT_:Int()
	Function REPORT_MOUSE_POSITION_:Int()
	Function ALL_MOUSE_EVENTS_:Int()
	
	Function LINES_:Int()
	Function COLS_:Int()
	
	Function drawCDKScreen(screen:Byte Ptr)
	Function eraseCDKScreen(screen:Byte Ptr)
	
	Function nodelay(window:Byte Ptr, b:Int)
	Function getch:Int()
'	Function cbreak:Int()
	Function def_prog_mode:Int()
	Function endwin:Int()
	Function refresh:Int()

	Rem
	bbdoc: Will sound the audible bell on the terminal if possible, and if not, will flash the screen (visible bell).
	End Rem
	Function beep:Int()
	Rem
	bbdoc: Flashes the screen.
	End Rem
	Function flash:Int()
	
	Function bmx_curses_getchCDKObject:Int(widget:Byte Ptr, t:Int)
	
	Function bmx_mousemask:Int(mask:Int, oldMask:Int Ptr)
	Function bmx_getmouse:Object()
	
End Extern

Function COLOR_PAIR:Int(n:Int)

	Return (n Shl 24) & A_COLOR
	
End Function

' custom function to convert a String array to a "C" string array.
Function arrayToCStrings:Byte Ptr(stringArray:String[])

	Local cStrings:Byte Ptr = MemAlloc(4 * stringArray.length)
		
	For Local i:Int = 0 Until stringArray.length
		
		Local p:Int Ptr = Int Ptr(cStrings)
		p[i] = Int(stringArray[i].toCString())
		
	Next

	Return cStrings

End Function

' custom function to free a previously created "C" array of strings.
Function freeCStringArray(cStrings:Byte Ptr, length:Int)

	For Local i:Int = 0 Until length
	
		Local p:Int Ptr = Int Ptr(cStrings)
		MemFree(Byte Ptr(p[i]))
	
	Next

	MemFree(cStrings)
	
End Function


Const A_NORMAL:Int = 0
?win32
Const A_ALTCHARSET:Int = $00010000
Const A_RIGHTLINE:Int = $00020000
Const A_LEFTLINE:Int = $00040000
Const A_INVIS:Int = $00080000
Const A_UNDERLINE:Int = $00100000
Const A_REVERSE:Int = $00200000
Const A_BLINK:Int = $00400000
Const A_BOLD:Int = $00800000

Const A_ATTRIBUTES:Int = $ffff0000
Const A_CHARTEXT:Int = $0000ffff
Const A_COLOR:Int = $ff000000

Const A_ITALIC:Int = A_INVIS
Const A_PROTECT:Int = (A_UNDERLINE | A_LEFTLINE | A_RIGHTLINE)

?linux
Const A_BOLD:Int = $0100
Const A_REVERSE:Int = $0200
Const A_BLINK:Int = $0400

Const A_ATTRIBUTES:Int = $ff00
Const A_CHARTEXT:Int = $00ff
Const A_COLOR:Int = $f800

Const A_ALTCHARSET:Int = A_NORMAL
Const A_PROTECT:Int = A_NORMAL
Const A_UNDERLINE:Int = A_NORMAL
Const A_INVIS:Int = A_NORMAL
Const A_LEFTLINE:Int = A_NORMAL
Const A_RIGHTLINE:Int = A_NORMAL
Const A_ITALIC:Int = A_NORMAL
?macos
Const A_BOLD:Int = $0100
Const A_REVERSE:Int = $0200
Const A_BLINK:Int = $0400

Const A_ATTRIBUTES:Int = $ff00
Const A_CHARTEXT:Int = $00ff
Const A_COLOR:Int = $f800

Const A_ALTCHARSET:Int = A_NORMAL
Const A_PROTECT:Int = A_NORMAL
Const A_UNDERLINE:Int = A_NORMAL
Const A_INVIS:Int = A_NORMAL
Const A_LEFTLINE:Int = A_NORMAL
Const A_RIGHTLINE:Int = A_NORMAL
Const A_ITALIC:Int = A_NORMAL
?



Const POS_LEFT:Int = 9000
Const POS_RIGHT:Int = 9001
Const POS_CENTER:Int = 9002
Const POS_TOP:Int = 9003
Const POS_BOTTOM:Int = 9004
Const POS_HORIZONTAL:Int = 9005
Const POS_VERTICAL:Int = 9006
Const POS_FULL:Int = 9007


' widget types
Const vNULL:Int = 0
Const vALPHALIST:Int = 1
Const vBUTTON:Int = 2
Const vBUTTONBOX:Int = 3
Const vCALENDAR:Int = 4
Const vDIALOG:Int = 5
Const vDSCALE:Int = 6
Const vENTRY:Int = 7
Const vFSCALE:Int = 8
Const vFSELECT:Int = 9
Const vFSLIDER:Int = 10
Const vGRAPH:Int = 11
Const vHISTOGRAM:Int = 12
Const vITEMLIST:Int = 13
Const vLABEL:Int = 14
Const vMARQUEE:Int = 15
Const vMATRIX:Int = 16
Const vMENTRY:Int = 17
Const vMENU:Int = 18
Const vRADIO:Int = 19
Const vSCALE:Int = 20
Const vSCROLL:Int = 21
Const vSELECTION:Int = 22
Const vSLIDER:Int = 23
Const vSWINDOW:Int = 24
Const vTEMPLATE:Int = 25
Const vTRAVERSE:Int = 26
Const vUSCALE:Int = 27
Const vUSLIDER:Int = 28
Const vVIEWER:Int = 29

' display types
Const vINVALID:Int = 0
Const vCHAR:Int = 1
Const vHCHAR:Int = 2
Const vINT:Int = 3
Const vHINT:Int = 4
Const vMIXED:Int = 5
Const vHMIXED:Int = 6
Const vUCHAR:Int = 7
Const vLCHAR:Int = 8
Const vUHCHAR:Int = 9
Const vLHCHAR:Int = 10
Const vUMIXED:Int = 11
Const vLMIXED:Int = 12
Const vUHMIXED:Int = 13
Const vLHMIXED:Int = 14
Const vVIEWONLY:Int = 15

Const vEARLY_EXIT:Int = 0
Const vESCAPE_HIT:Int = 1
Const vNORMAL:Int = 2
Const vNEVER_ACTIVATED:Int = 3


Rem
bbdoc: Gets the user login name.
End Rem
Function GetUserName:String()
Extern
	Function bmx_curses_GetUserName(buf:Byte Ptr)
End Extern
	Local buf:Byte[256]
	bmx_curses_GetUserName(buf)
	Return String.fromUTF8String(buf)
End Function

Rem
bbdoc: Creates a centered array of strings, split by ~n
End Rem
Function StringToCenteredArray:String[](text:String)
	Local parts:String[] = text.Split("~n")
		
	For Local i:Int = 0 Until parts.length
		
		parts[i] = "<C>" + parts[i]
		
	Next

	Return parts
End Function
