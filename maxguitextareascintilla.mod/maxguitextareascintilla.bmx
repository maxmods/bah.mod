' Copyright (c) 2014 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
'
SuperStrict

Rem
bbdoc: Scintilla-based MaxGUI TextArea gadget.
End Rem
Module BaH.MaxGUITextAreaScintilla

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: (c) 2014 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."


ModuleInfo "CC_OPTS: -fexceptions"

?Linux

ModuleInfo "CC_OPTS: -DGTK"

' glib
ModuleInfo "CC_OPTS: -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/lib/i386-linux-gnu/glib-2.0/include -I/usr/lib/x86_64-linux-gnu/glib-2.0/include"
' gtk
ModuleInfo "CC_OPTS: -I/usr/include/gtk-2.0  -I/usr/lib/i386-linux-gnu/gtk-2.0/include -I/usr/lib/x86_64-linux-gnu/gtk-2.0/include"
' cairo
ModuleInfo "CC_OPTS: -I/usr/include/cairo"
' pango
ModuleInfo "CC_OPTS: -I/usr/include/pango-1.0"
' gdk
ModuleInfo "CC_OPTS: -I/usr/include/gdk-pixbuf-2.0"
' atk
ModuleInfo "CC_OPTS: -I/usr/include/atk-1.0"


Import BaH.GTKMaxGUI

Import "common.bmx"

Rem
bbdoc: 
End Rem
Type TGTKScintillaTextArea Extends TGTKTextArea

	Field sciPtr:Byte Ptr
	Field sciId:Int
	
	Field styleMap:TMap = New TMap
	Field styles:Int[] = New Int[31]
	Field styleIndex:Int = 0
	
	Field lastStyleValue:Int = -1
	Field lastStyle:Int

	Global sci_id_count:Int = 0
	
	Field ignoreChange:Int
	Field tabPixelWidth:Int
	
	Field lineDigits:Int
	
	' holder for the latest notification
	' keep one in the type rather than locally in the callback function so we don't have to create a new object for every notification
	Field notification:TSCNotification = New TSCNotification

	Function CreateTextArea:TGTKTextArea(x:Int, y:Int, w:Int, h:Int, label:String, group:TGadget, style:Int)
		Local this:TGTKScintillaTextArea = New TGTKScintillaTextArea

		this.initTextArea(x, y, w, h, label, group, style)

		Return this
	End Function

	Method initTextArea(x:Int, y:Int, w:Int, h:Int, label:String, group:TGadget, style:Int)

		parent = group

		sciId = sci_id_count

		handle = scintilla_new()
		sciPtr = bmx_mgta_scintilla_getsci(handle, sciId)

		' increment internal counter		
		sci_id_count :+ 1

		gtk_widget_show(handle)

		gtk_layout_put(TGTKContainer(parent).container, handle, x, y)
		gtk_widget_set_size_request(handle, w, Max(h,0))


		addConnection("sci-notify", g_signal_cbsci(handle, "sci-notify", OnSciNotify, Self, Destroy, 0))
		addConnection("button-press-event", g_signal_cb3(handle, "button-press-event", OnMouseDown, Self, Destroy, 0))
		addConnection("button-release-event", g_signal_cb3(handle, "button-release-event", OnMouseUp, Self, Destroy, 0))
		addConnection("key-press-event", g_signal_cb3(handle, "key-press-event", OnKeyDown, Self, Destroy, 0))
		
		' set some default monospaced font
		SetFont(LookupGuiFont(GUIFONT_MONOSPACED))

	End Method
	
	Function OnMouseDown:Int(widget:Byte Ptr, event:Byte Ptr, obj:Object)
		Local x:Double, y:Double, button:Int
		bmx_gtkmaxgui_gdkeventbutton(event, Varptr x, Varptr y, Varptr button)

		If button = 3 Then ' right mouse button
			' ignore this...  see MouseUp for menu event!
			Return True
		End If

		PostGuiEvent(EVENT_GADGETSELECT, TGadget(obj))
	End Function

	Rem
	bbdoc: Callback for mouse button release
	End Rem
	Function OnMouseUp:Int(widget:Byte Ptr, event:Byte Ptr, obj:Object)
		Local x:Double, y:Double, button:Int
		bmx_gtkmaxgui_gdkeventbutton(event, Varptr x, Varptr y, Varptr button)

		If button = 3 Then ' right mouse button
			PostGuiEvent(EVENT_GADGETMENU, TGadget(obj),,,x,y)
			Return True
		End If

		PostGuiEvent(EVENT_GADGETSELECT, TGadget(obj))
	End Function

	Method GetText:String()
		Return bmx_mgta_scintilla_gettext(sciPtr)
	End Method

	Rem
	bbdoc: Sets the text.
	End Rem
	Method SetText(txt:String)
		bmx_mgta_scintilla_settext(sciPtr, txt)
	End Method

	Method SetFont(font:TGuiFont)

		If font = Null Then
			Return
		End If

		_font = font

		bmx_mgta_scintilla_setfont(sciPtr, font.name, font.size)
		
		' set the margin size for line numbers
		bmx_mgta_scintilla_setlinedigits(sciPtr, Varptr lineDigits)
		
		SetTabs()
	End Method

	Rem
	bbdoc: Set the text area selection
	End Rem
	Method SetSelection(pos:Int, length:Int, units:Int)
		'ignoreChange = True
		Local startPos:Int
		Local endPos:Int

		If length = 0 Then
			If units = TEXTAREA_LINES Then
				startPos = bmx_mgta_scintilla_positionfromline(sciPtr, pos, True)
				endPos = startPos
			Else
				startPos = bmx_mgta_scintilla_bytefromchar(sciPtr, pos, 0, 0)
				endPos = startPos
			End If
		Else
			If units = TEXTAREA_LINES Then
				startPos = bmx_mgta_scintilla_positionfromline(sciPtr, pos, True)
				endPos = bmx_mgta_scintilla_positionfromline(sciPtr, pos + length, True)
			Else ' must be TEXTAREA_CHARS
				'startPos = pos
				'endPos = pos + length
				startPos = bmx_mgta_scintilla_bytefromchar(sciPtr, pos, 0, 0)
				endPos = bmx_mgta_scintilla_bytefromchar(sciPtr, length, startPos, pos)
			End If
		End If

		bmx_mgta_scintilla_setsel(sciPtr, startPos, endPos)


		PostGuiEvent(EVENT_GADGETSELECT, Self)

		' scroll to the start of the selection
'		bmx_mgta_scintilla_scrollcaret(sciPtr)

	End Method

	Method GetSelectionLength:Int(units:Int)
		Return bmx_mgta_scintilla_getselectionlength(sciPtr, units)
	End Method

	Method SetMargins(leftmargin:Int)
		' TODO
	End Method

	Method CharX:Int(char:Int)
		' TODO
	EndMethod

	Method CharY:Int(char:Int)
		' TODO
	EndMethod

	Method AddText(text:String)
		ignoreChange = True

		bmx_mgta_scintilla_appendtext(sciPtr, text)

		brl.System.Driver.Poll() ' update events, before scrolling to the end...
		bmx_mgta_scintilla_scrolltoend(sciPtr)
	End Method

	Method ReplaceText(pos:Int, length:Int, text:String, units:Int)
		ignoreChange = True
		If length = TEXTAREA_ALL Then
			SetText(text)
		Else
			Local startPos:Int
			Local endPos:Int
	
			If units = TEXTAREA_LINES Then
				startPos = bmx_mgta_scintilla_positionfromline(sciPtr, pos, True)
				endPos = bmx_mgta_scintilla_positionfromline(sciPtr, pos + length, True)
			Else ' must be TEXTAREA_CHARS
				'startPos = pos
				'endPos = pos + length
				startPos = bmx_mgta_scintilla_bytefromchar(sciPtr, pos, 0, 0)
				endPos = bmx_mgta_scintilla_bytefromchar(sciPtr, length, startPos, pos)
			End If

			bmx_mgta_scintilla_settargetstart(sciPtr, startPos)
			bmx_mgta_scintilla_settargetend(sciPtr, endPos)
	
			' insert new text
			Local textPtr:Byte Ptr = text.ToUTF8String()
			bmx_mgta_scintilla_replacetarget(sciPtr, textPtr)
			MemFree(textPtr)
		End If
	End Method

	Method SetColor(r:Int, g:Int, b:Int)
		bmx_mgta_scintilla_stylesetback(sciPtr, r | g Shl 8 | b Shl 16)
	End Method

	Method SetStyle(r:Int, g:Int, b:Int, flags:Int, pos:Int, length:Int, units:Int)
		' Build a style string
		Local s:Int = r Shl 24 | g Shl 16 | b Shl 8 | (flags & $ff)
		Local style:Int = lastStyle

		If s <> lastStyleValue Then
		
			Local styleText:String = String(s)
	
			Local st:String = String(styleMap.ValueForKey(styleText))
			If Not st Then

				' is there already an entry for this one?
				If styles[styleIndex] Then
					' remove it from the map
					styleMap.Remove(String(styles[styleIndex]))
				End If
				
				styles[styleIndex] = s
				
				styleMap.Insert(styleText, Chr(styleIndex + 65))
				style = styleIndex
				
				styleIndex :+ 1
				If styleIndex > 31 Then
					styleIndex = 0
				End If

				' create the styling
				bmx_mgta_scintilla_stylesetfore(sciPtr, style, r | g Shl 8 | b Shl 16)
	
				If flags & TEXTFORMAT_ITALIC Then
					bmx_mgta_scintilla_stylesetitalic(sciPtr, style, True)
				Else
					bmx_mgta_scintilla_stylesetitalic(sciPtr, style, False)
				End If
	
				If flags & TEXTFORMAT_BOLD Then
					bmx_mgta_scintilla_stylesetbold(sciPtr, style, True)
				Else
					bmx_mgta_scintilla_stylesetbold(sciPtr, style, False)
				End If
	
				If flags & TEXTFORMAT_UNDERLINE Then
					bmx_mgta_scintilla_stylesetunderline(sciPtr, style, True)
				Else
					bmx_mgta_scintilla_stylesetunderline(sciPtr, style, False)
				End If
				
			Else
				style = Asc(st) - 65
			End If
			
			lastStyle = style
			lastStyleValue = s

		End If
		
		applyStyle(pos, length, units, style)
		
	End Method
	
	Method applyStyle(pos:Int, length:Int, units:Int, style:Int)
		Local startPos:Int
		Local realLength:Int

		If units = TEXTAREA_LINES Then
			startPos = bmx_mgta_scintilla_positionfromline(sciPtr, pos, True)
			realLength = bmx_mgta_scintilla_positionfromline(sciPtr, pos + length, True) - startPos
		Else ' must be TEXTAREA_CHARS
			startPos = bmx_mgta_scintilla_bytefromchar(sciPtr, pos, 0, 0)
			realLength = bmx_mgta_scintilla_bytefromchar(sciPtr, length, startPos, pos) - startPos
		End If

		bmx_mgta_scintilla_startstyling(sciPtr, startPos)
		bmx_mgta_scintilla_setstyling(sciPtr, realLength, style)

	End Method

	Method AreaText:String(pos:Int, length:Int, units:Int)
		Local startPos:Int
		Local endPos:Int

		If units = TEXTAREA_LINES Then
			startPos = bmx_mgta_scintilla_positionfromline(sciPtr, pos, True)
			endPos = bmx_mgta_scintilla_positionfromline(sciPtr, pos + length, True)
		Else ' must be TEXTAREA_CHARS
			'startPos = pos
			'endPos = pos + length
			startPos = bmx_mgta_scintilla_bytefromchar(sciPtr, pos, 0, 0)
			endPos = bmx_mgta_scintilla_bytefromchar(sciPtr, length, startPos, pos)
		End If
		
		Return bmx_mgta_scintilla_gettextrange(sciPtr, startPos, endPos)
	End Method

	Method AreaLen:Int(units:Int)
		If units = TEXTAREA_LINES Then
			Return bmx_mgta_scintilla_getlinecount(sciPtr)
		Else
			Return bmx_mgta_scintilla_getlength(sciPtr)
		End If
	End Method

	Method GetCursorPos:Int(units:Int)
		If units = TEXTAREA_LINES Then
			Return bmx_mgta_scintilla_getcurrentline(sciPtr)
		Else
			Return bmx_mgta_scintilla_getcurrentpos(sciPtr)
		End If
	End Method

	Method SetTabs(tabWidth:Int = -1)

		If tabWidth >= 0 Then
			tabPixelWidth = tabWidth
		Else
			tabWidth = tabPixelWidth
		End If

		' convert from pixels to characters
		If _font Then
			tabWidth = tabWidth / _font.CharWidth(32)
		Else
			tabWidth = 4
		End If

		bmx_mgta_scintilla_settabwidth(sciPtr, tabWidth)

	End Method

	Method Activate(cmd:Int)
		Super.Activate(cmd)

		Select cmd
			Case ACTIVATE_CUT
				bmx_mgta_scintilla_cut(sciPtr)

			Case ACTIVATE_COPY
				bmx_mgta_scintilla_copy(sciPtr)

			Case ACTIVATE_PASTE
				bmx_mgta_scintilla_paste(sciPtr)

		End Select
	End Method

	Method CharAt:Int(line:Int)
		Return bmx_mgta_scintilla_positionfromline(sciPtr, line, False)
	End Method

	Method LineAt:Int(index:Int)
		Return bmx_mgta_scintilla_linefromposition(sciPtr, index)
	End Method

	Function OnSciNotify(widget:Byte Ptr, id:Int, notificationPtr:Byte Ptr, obj:Object)
		Local ta:TGTKScintillaTextArea = TGTKScintillaTextArea(obj)
		
		MemCopy ta.notification, notificationPtr, SizeOf TSCNotification

		Select ta.notification.code
			Case SCN_UPDATEUI
				If ta.notification.updated & SC_UPDATE_SELECTION Then
					PostGuiEvent(EVENT_GADGETSELECT, TGadget(obj))
				End If
			Case SCN_MODIFIED
				If ta.notification.modificationType & (SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT) Then
					If Not ta.ignoreChange Then
						PostGuiEvent(EVENT_GADGETSELECT, TGadget(obj))
						PostGuiEvent(EVENT_GADGETACTION, TGadget(obj))
					End If
					ta.ignoreChange = False
					
					bmx_mgta_scintilla_setlinedigits(ta.sciPtr, Varptr ta.lineDigits)
				End If
		End Select

	End Function

End Type


' scintilla text area driver
Type TGTKScintillaTextAreaDriver Extends TGTKTextAreaDriver
	Function CreateTextArea:TGTKTextArea(x:Int, y:Int, w:Int, h:Int, label:String, group:TGadget, style:Int)
		Return TGTKScintillaTextArea.CreateTextArea(x, y, w, h, label, group, style)
	End Function
End Type

gtkmaxgui_textarea = New TGTKScintillaTextAreaDriver


?
