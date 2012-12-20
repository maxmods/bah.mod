' Copyright (c) 2012 Bruce A Henderson
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

Import BRL.Blitz

Import "-lqrencode"

Import "src/*.h"
Import "glue.cpp"


Extern

	Function bmx_qrcode_getdata:Byte Ptr(handle:Byte Ptr)
	Function bmx_qrcode_getwidth:Int(handle:Byte Ptr)
	Function bmx_qrcode_getversion:Int(handle:Byte Ptr)
	Function bmx_qrcode_free(handle:Byte Ptr)

	Function bmx_qrcode_encodestring:Byte Ptr(text:String, version:Int, level:Int, casesensitive:Int, encoding:Int)
	Function bmx_qrcode_encodestringMQR:Byte Ptr(text:String, version:Int, level:Int, casesensitive:Int, encoding:Int)
	Function bmx_qrcode_encodedata:Byte Ptr(size:Int, data:Byte Ptr, version:Int, level:Int)
	Function bmx_qrcode_encodedataMQR:Byte Ptr(size:Int, data:Byte Ptr, version:Int, level:Int)
	
	Function bmx_qrcode_lasterror:Int()
	
End Extern



Const QR_MODE_NUL:Int = -1      ' Terminator (NUL character). Internal use only
Const QR_MODE_NUM:Int = 0       ' Numeric mode
Const QR_MODE_AN:Int = 1        ' Alphabet-numeric mode
Const QR_MODE_8:Int = 2         ' 8-bit data mode
Const QR_MODE_KANJI:Int = 3     ' Kanji (shift-jis) mode
Const QR_MODE_STRUCTURE:Int = 4 ' Internal use only

Const QR_ECLEVEL_L:Int = 0 ' lowest
Const QR_ECLEVEL_M:Int = 1
Const QR_ECLEVEL_Q:Int = 2
Const QR_ECLEVEL_H:Int = 3 ' highest

