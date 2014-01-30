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
		
		' set some default monospaced font
		SetFont(LookupGuiFont(GUIFONT_MONOSPACED))

	End Method
	
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
		
	End Method

	Rem
	bbdoc: Set the text area selection
	End Rem
	Method SetSelection(pos:Int, length:Int, units:Int)
		ignoreChange = True
		Local startPos:Int
		Local endPos:Int

		If units = TEXTAREA_LINES Then
			startPos = bmx_mgta_scintilla_positionfromline(sciPtr, pos)
			endPos = bmx_mgta_scintilla_positionfromline(sciPtr, pos + length)
		Else ' must be TEXTAREA_CHARS
			startPos = pos
			endPos = pos + length
		End If
		
		bmx_mgta_scintilla_setselel(sciPtr, startPos, endPos)

		PostGuiEvent(EVENT_GADGETSELECT, Self)

		' scroll to the start of the selection
'		bmx_mgta_scintilla_scrollcaret(sciPtr)

	End Method

	Method ReplaceText(pos:Int, length:Int, text:String, units:Int)
		ignoreChange = True
		If length = TEXTAREA_ALL Then
			SetText(text)
		Else
			Local startPos:Int
			Local endPos:Int
	
			If units = TEXTAREA_LINES Then
				startPos = bmx_mgta_scintilla_positionfromline(sciPtr, pos)
				endPos = bmx_mgta_scintilla_positionfromline(sciPtr, pos + length)
			Else ' must be TEXTAREA_CHARS
				startPos = pos
				endPos = pos + length
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
			startPos = bmx_mgta_scintilla_positionfromline(sciPtr, pos)
			realLength = bmx_mgta_scintilla_positionfromline(sciPtr, pos + length) - startPos
		Else ' must be TEXTAREA_CHARS
			startPos = pos
			realLength = length
		End If

		bmx_mgta_scintilla_startstyling(sciPtr, startPos)
		bmx_mgta_scintilla_setstyling(sciPtr, realLength, style)

	End Method

	Method AreaText:String(pos:Int, length:Int, units:Int)
		Local startPos:Int
		Local endPos:Int

		If units = TEXTAREA_LINES Then
			startPos = bmx_mgta_scintilla_positionfromline(sciPtr, pos)
			endPos = bmx_mgta_scintilla_positionfromline(sciPtr, pos + length)
		Else ' must be TEXTAREA_CHARS
			startPos = pos
			endPos = pos + length
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

	Method SetTabs(tabs:Int)

		bmx_mgta_scintilla_settabwidth(sciPtr, tabs)

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
