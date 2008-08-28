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

Import BRL.Blitz

' the library
Import "-lUniMotion"

Import "src/*.h"

Import "glue.cpp"

Extern

	Function detect_sms:Int()
	Function read_sms_raw:Int(smsType:Int, x:Int Ptr, y:Int Ptr, z:Int Ptr)
	Function read_sms:Int(smsType:Int, x:Int Ptr, y:Int Ptr, z:Int Ptr)
	Function read_sms_real:Int(smsType:Int, x:Double Ptr, y:Double Ptr, z:Double Ptr)

	Function bmx_read_sms_raw_bytes:Byte[](smsType:Int)
	Function read_sms_scaled:Int(smsType:Int, x:Int Ptr, y:Int Ptr, z:Int Ptr)

End Extern


Const SMS_UNKNOWN:Int = 0
Const SMS_POWERBOOK:Int = 1
Const SMS_IBOOK:Int = 2
Const SMS_HIRESPB:Int = 3
Const SMS_MACBOOKPRO:Int = 4

