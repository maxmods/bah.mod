' Copyright (c) 2009 Bruce A Henderson
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
bbdoc: Right-To-Left String Formatter
End Rem
Module BaH.RTL

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2009 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release"

?macos
ModuleInfo "LD_OPTS: -L%PWD%/lib/macos"
?win32
ModuleInfo "LD_OPTS: -L%PWD%/lib/win32"
?

ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H"



Import "common.bmx"

Private
Global _converter:TRTLConverter

Type TRTLConverter

	Field convertPtr:Byte Ptr
	
	Method New()
		_converter = Self
		convertPtr = bmx_rtlconverter_new()
	End Method

	Method Convert:String(text:String)
		Return bmx_rtlconverter_convert(convertPtr, text)
	End Method

End Type

New TRTLConverter

Public

Rem
bbdoc: Formats a String for Right-To-Left display.
about: Note that most native UIs will already do this formatting for you. Use of this function is generally useful for
drawing text onto a Graphics Context.
End Rem
Function FormatStringRTL:String(text:String)

	Return _converter.Convert(text)

End Function


