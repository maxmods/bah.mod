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


Rem
bbdoc: UniMotion
about: MacOS - Sudden Motion Sensor capture.
End Rem
Module BaH.UniMotion

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: libunimotion - 2006 Lincoln Ramsay"
ModuleInfo "Copyright: Wrapper - 2008 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.00 Initial Release."

?macos
ModuleInfo "LD_OPTS: -L%PWD%/lib/"


Import "common.bmx"

Rem
bbdoc: A type for retrieving Sudden Motion Sensor (SMS) data.
End Rem
Type TSMS

	' the value of SMS hardware present or SMS_UNKOWN if no hardware is detected.
	Field smsType:Int

	Rem
	bbdoc: Returns a new TSMS object.
	End Rem
	Function Create:TSMS()
		Local this:TSMS = New TSMS
		this.smsType = detect_sms()
		Return this
	End Function
	
	Rem
	bbdoc: Reads raw, unmodified values.
	End Rem
	Method ReadRaw:Int(x:Int Var, y:Int Var, z:Int Var)
		Return read_sms_raw(smsType, Varptr x, Varptr y, Varptr z)
	End Method
	
	Rem
	bbdoc: Reads "calibrated" values (same as raw if no calibration data exists).
	End Rem
	Method Read:Int(x:Int Var, y:Int Var, z:Int Var)
		Return read_sms(smsType, Varptr x, Varptr y, Varptr z)
	End Method
	
	Rem
	bbdoc: Reads real (1.0 = 1G) values (requires calibration data).
	about: Note that this is the preferred API as it need not change with new machines.
	If no "scale" calibration data exists defaults will be used based on the machine type.
	End Rem
	Method ReadReal:Int(x:Double Var, y:Double Var, z:Double Var)
		Return read_sms_real(smsType:Int, Varptr x, Varptr y, Varptr z)
	End Method
	
	Rem
	bbdoc: Reads raw SMS data (useful for debugging).
	about: Note that endian issues make reading the raw bytes non-trivial
	End Rem
	Method ReadRawBytes:Byte[]()
		Return bmx_read_sms_raw_bytes(smsType)
	End Method
	
	Rem
	bbdoc: Reads scaled values, like real but easier to handle.
	about: This reverses the backwards polarity of x and increases the range of the older machines to match the MacBook [Pro] sensor.
	End Rem
	Method ReadScaled:Int(x:Int Var, y:Int Var, z:Int Var)
		Return read_sms_scaled(smsType, Varptr x, Varptr y, Varptr z)
	End Method
	
End Type

?

