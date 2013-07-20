' Copyright (c) 2013 Bruce A Henderson
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

Import BRL.LinkedList
?win32
Import BaH.Regex
?linux
Import BRL.FileSystem
?

?macos
Import "-framework IOKit"
?win32
Import "-lsetupapi"
?

Import "source.bmx"

Extern

	Function bmx_serial_create:Byte Ptr(port:String, baudrate:Int, timeout:Byte Ptr, bytesize:Int, parity:Int, stopbits:Int, flowcontrol:Int)
	Function bmx_serial_open(handle:Byte Ptr)
	Function bmx_serial_close(handle:Byte Ptr)
	Function bmx_serial_isopen:Int(handle:Byte Ptr)
	Function bmx_serial_available:Int(handle:Byte Ptr)
	Function bmx_serial_read:Int(handle:Byte Ptr, buffer:Byte Ptr, size:Int)
	Function bmx_serial_readline:String(handle:Byte Ptr, size:Int, eol:String)
	Function bmx_serial_write:Int(handle:Byte Ptr, data:Byte Ptr, size:Int)
	Function bmx_serial_writestring:Int(handle:Byte Ptr, data:String)
	Function bmx_serial_setport(handle:Byte Ptr, port:String)
	Function bmx_serial_getport:String(handle:Byte Ptr)
	Function bmx_serial_setbaudrate(handle:Byte Ptr, baudrate:Int)
	Function bmx_serial_getbaudrate:Int(handle:Byte Ptr)
	Function bmx_serial_setbytesize(handle:Byte Ptr, bytesize:Int)
	Function bmx_serial_getbytesize:Int(handle:Byte Ptr)
	Function bmx_serial_setparity(handle:Byte Ptr, parity:Int)
	Function bmx_serial_getparity:Int(handle:Byte Ptr)
	Function bmx_serial_setstopbits(handle:Byte Ptr, stopbits:Int)
	Function bmx_serial_getstopbits:Int(handle:Byte Ptr)
	Function bmx_serial_setflowcontrol(handle:Byte Ptr, flowcontrol:Int)
	Function bmx_serial_getflowcontrol:Int(handle:Byte Ptr)
	Function bmx_serial_flush(handle:Byte Ptr)
	Function bmx_serial_flushinput(handle:Byte Ptr)
	Function bmx_serial_flushoutput(handle:Byte Ptr)
	Function bmx_serial_sendbreak(handle:Byte Ptr, duration:Int)
	Function bmx_serial_setbreak(handle:Byte Ptr, level:Int)
	Function bmx_serial_setrts(handle:Byte Ptr, level:Int)
	Function bmx_serial_setdtr(handle:Byte Ptr, level:Int)
	Function bmx_serial_waitforchange(handle:Byte Ptr)
	Function bmx_serial_getcts:Int(handle:Byte Ptr)
	Function bmx_serial_getdsr:Int(handle:Byte Ptr)
	Function bmx_serial_getri:Int(handle:Byte Ptr)
	Function bmx_serial_getcd:Int(handle:Byte Ptr)

	Function bmx_serial_timeout_max:Int()
	Function bmx_serial_timeout_free(handle:Byte Ptr)
	Function bmx_serial_timeout_create:Byte Ptr(interByteTimeout:Int, readTimeoutConstant:Int, readTimeoutMultiplier:Int, writeTimeoutConstant:Int, ..
			writeTimeoutMultiplier:Int)

End Extern


Const FIVEBITS:Int = 5
Const SIXBITS:Int = 6
Const SEVENBITS:Int = 7
Const EIGHTBITS:Int = 8

Const FLOWCONTROL_NONE:Int = 0
Const FLOWCONTROL_SOFTWARE:Int = 1
Const FLOWCONTROL_HARDWARE:Int = 2

Const PARITY_NONE:Int = 0
Const PARITY_ODD:Int = 1
Const PARITY_EVEN:Int = 2

Const STOPBITS_ONE:Int = 1
Const STOPBITS_ONE_POINT_FIVE:Int = 2
Const STOPBITS_TWO:Int = 2
