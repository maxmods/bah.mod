' Copyright (c) 2008 Bruce A Henderson
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


Import "source.bmx"

Extern
	Function _strlen:Int(s:Byte Ptr) = "strlen"

	Function bmx_cegui_new_oglrenderer:Byte Ptr()
	Function bmx_cegui_new_system:Byte Ptr(renderer:Byte Ptr)
	Function bmx_cegui_delete_system(system:Byte Ptr)
	Function bmx_cegui_delete_renderer(renderer:Byte Ptr)
	
	Function bmx_cegui_schememanager_loadScheme:Byte Ptr(scheme:Byte Ptr, resourceGroup:Byte Ptr)
	
	Function bmx_cegui_system_setDefaultFont(font:Byte Ptr)
	Function bmx_cegui_system_setDefaultMouseCursor(look:Byte Ptr, kind:Byte Ptr)
	Function bmx_cegui_system_renderGUI()
	Function bmx_cegui_system_setGUISheet(window:Byte Ptr)
	Function bmx_cegui_system_injectTimePulse:Int(time:Float)
	Function bmx_cegui_system_injectMousePosition:Int(x:Int, y:Int)
	Function bmx_cegui_system_injectMouseButtonUp:Int(button:Int)
	Function bmx_cegui_system_injectMouseButtonDown:Int(button:Int)
	
	
	Function bmx_cegui_windowmanager_loadWindowLayout:Object(filename:Byte Ptr, namePrefix:Byte Ptr, resourceGroup:Byte Ptr)
	Function bmx_cegui_windowmanager_getwindow:Object(name:Byte Ptr)
	Function bmx_cegui_windowmanager_createwindow:Object(windowType:Byte Ptr, name:Byte Ptr, prefix:Byte Ptr)
	
	'Function bmx_cegui_eventhandler_new:Byte Ptr()
	'Function bmx_cegui_eventhandler_delete(handle:Byte Ptr)
	Function bmx_cegui_eventcallback_new:Byte Ptr(handle:Object)

	Function bmx_cegui_eventargs_delete(handle:Byte Ptr)

	Function bmx_cegui_eventset_subscribeevent:Byte Ptr(handle:Byte Ptr, name:Byte Ptr, cb:Byte Ptr)

	Function bmx_cegui_window_settext(handle:Byte Ptr, text:Byte Ptr)
	Function bmx_cegui_window_deactivate(handle:Byte Ptr)
	Function bmx_cegui_window_setposition(handle:Byte Ptr, x:Float, y:Float)
	Function bmx_cegui_window_setxposition(handle:Byte Ptr, x:Float)
	Function bmx_cegui_window_setyposition(handle:Byte Ptr, y:Float)
	Function bmx_cegui_window_setsize(handle:Byte Ptr, width:Float, height:Float)
	Function bmx_cegui_window_setwidth(handle:Byte Ptr, width:Float)
	Function bmx_cegui_window_setheight(handle:Byte Ptr, height:Float)
	Function bmx_cegui_window_setmaxsize(handle:Byte Ptr, width:Float, height:Float)
	Function bmx_cegui_window_setminsize(handle:Byte Ptr, width:Float, height:Float)
	Function bmx_cegui_window_addchildwindowwindow(handle:Byte Ptr, window:Byte Ptr)
	Function bmx_cegui_window_addchildwindow(handle:Byte Ptr, window:Byte Ptr)
	Function bmx_cegui_window_hide(handle:Byte Ptr)
	Function bmx_cegui_window_show(handle:Byte Ptr)
	Function bmx_cegui_window_gettype:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_window_activate(handle:Byte Ptr)

	Function bmx_cegui_window_setproperty(handle:Byte Ptr, name:Byte Ptr, value:Byte Ptr)

	Function bmx_cegui_connection_delete(handle:Byte Ptr)

	Function bmx_cegui_checkbox_isselected:Int(handle:Byte Ptr)
	Function bmx_cegui_checkbox_setselected(handle:Byte Ptr, selected:Int)

	Function bmx_cegui_progressbar_getprogress:Float(handle:Byte Ptr)
	Function bmx_cegui_progressbar_getstep:Float(handle:Byte Ptr)
	Function bmx_cegui_progressbar_setprogress(handle:Byte Ptr, progress:Float)
	Function bmx_cegui_progressbar_setstepsize(handle:Byte Ptr, stepVal:Float)
	Function bmx_cegui_progressbar_dostep(handle:Byte Ptr)
	Function bmx_cegui_progressbar_adjustprogress(handle:Byte Ptr, delta:Float)

	Function bmx_cegui_system_injectkeydown:Int(key:Int)
	Function bmx_cegui_system_injectkeyup:Int(key:Int)
	Function bmx_cegui_system_injectchar:Int(key:Int)

	Function bmx_cegui_editbox_hasinputfocus:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_isreadonly:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_istextmasked:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_istextvalid:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_getvalidationstring:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_editbox_getcaratindex:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_getselectionstartindex:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_getselectionendindex:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_getselectionlength:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_getmaxtextlength:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_setreadonly(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_editbox_settextmasked(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_editbox_setvalidationstring(handle:Byte Ptr, validationString:Byte Ptr)
	Function bmx_cegui_editbox_setcaratindex(handle:Byte Ptr, caratPos:Int)
	Function bmx_cegui_editbox_setselection(handle:Byte Ptr, startPos:Int, endPos:Int)
	Function bmx_cegui_editbox_setmaxtextlength(handle:Byte Ptr, maxLen:Int)

End Extern


' converts a UTF character array from byte-size characters to short-size characters
' based on the TextStream UTF code...
Function _convertUTF8ToMax:String(s:Byte Ptr)
	If s Then
		Local l:Int = _strlen(s)
		Local b:Short[] = New Short[l]
		Local bc:Int = -1
		Local c:Int
		Local d:Int
		Local e:Int
		For Local i:Int = 0 Until l
			bc:+1
			c = s[i]
			If c<128 
				b[bc] = c
				Continue
			End If
			i:+1
			d=s[i]
			If c<224 
				b[bc] = (c-192)*64+(d-128)
				Continue
			End If
			i:+1
			e = s[i]
			If c < 240 
				b[bc] = (c-224)*4096+(d-128)*64+(e-128)
				Continue
			End If
		Next
	
		Return String.fromshorts(b, bc + 1)
	End If
	
	Return ""
	
End Function

' converts a Max short-based String to a byte-based UTF-8 String.
' based on the TextStream UTF code...
Function _convertMaxToUTF8:String(text:String)
	If Not text Then
		Return ""
	End If
	
	Local l:Int = text.length
	If l = 0 Then
		Return ""
	End If
	
	Local count:Int = 0
	Local s:Byte[] = New Byte[l * 3] ' max possible is 3 x original size.
	
	For Local i:Int = 0 Until l
		Local char:Int = text[i]

		If char < 128 Then
			s[count] = char
			count:+ 1
			Continue
		Else If char<2048
			s[count] = char/64 | 192
			count:+ 1
			s[count] = char Mod 64 | 128
			count:+ 1
			Continue
		Else
			s[count] =  char/4096 | 224
			count:+ 1
			s[count] = char/64 Mod 64 | 128
			count:+ 1
			s[count] = char Mod 64 | 128
			count:+ 1
			Continue
		EndIf
		
	Next

	Return String.fromBytes(s, count)
End Function


