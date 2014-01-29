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

Import "-lpangocairo-1.0"
Import "-lcairo"

Import "source.bmx"

Extern


	Function scintilla_new:Byte Ptr()

	Function bmx_mgta_scintilla_getsci:Byte Ptr(handle:Byte Ptr, id:Int)

	Function bmx_mgta_scintilla_gettext:String(handle:Byte Ptr)
	Function bmx_mgta_scintilla_settext(handle:Byte Ptr, text:String)
	Function bmx_mgta_scintilla_setfont(handle:Byte Ptr, name:String, size:Int)
	Function bmx_mgta_scintilla_positionfromline:Int(handle:Byte Ptr, line:Int)
	Function bmx_mgta_scintilla_setselectionstart(handle:Byte Ptr, pos:Int)
	Function bmx_mgta_scintilla_setselectionend(handle:Byte Ptr, pos:Int)
	Function bmx_mgta_scintilla_scrollcaret(handle:Byte Ptr)
	Function bmx_mgta_scintilla_setselel(handle:Byte Ptr, startPos:Int, endPos:Int)
	Function bmx_mgta_scintilla_replacesel(handle:Byte Ptr, text:Byte Ptr)
	Function bmx_mgta_scintilla_stylesetback(handle:Byte Ptr, col:Int)
	Function bmx_mgta_scintilla_stylesetfore(handle:Byte Ptr, style:Int, color:Int)
	Function bmx_mgta_scintilla_stylesetitalic(handle:Byte Ptr, style:Int, value:Int)
	Function bmx_mgta_scintilla_stylesetbold(handle:Byte Ptr, style:Int, value:Int)
	Function bmx_mgta_scintilla_stylesetunderline(handle:Byte Ptr, style:Int, value:Int)
	Function bmx_mgta_scintilla_startstyling(handle:Byte Ptr, startPos:Int, style:Int)
	Function bmx_mgta_scintilla_setstyling(handle:Byte Ptr, realLength:Int, style:Int)
	Function bmx_mgta_scintilla_gettextrange:String(handle:Byte Ptr, startPos:Int, endPos:Int)
	
End Extern
