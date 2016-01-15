' Copyright (c) 2016 Bruce A Henderson
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
	Function setupESCAPI:Int()
	
	Function bmx_escapi_countCaptureDevices:Int()
	Function bmx_escapi_initCapture:Int(deviceno:Int, params:Byte Ptr)
	Function bmx_escapi_deinitCapture(deviceno:Int)
	Function bmx_escapi_doCapture(deviceno:Int)
	Function bmx_escapi_isCaptureDone:Int(deviceno:Int)
	Function bmx_escapi_getCaptureDeviceName(deviceno:Int, buffer:Byte Ptr, length:Int)
	Function bmx_escapi_ESCAPIVersion:Int()
	Function bmx_escapi_getCapturePropertyValue:Float(deviceno:Int, prop:Int)
	Function bmx_escapi_getCapturePropertyAuto:Int(deviceno:Int, prop:Int)
	Function bmx_escapi_setCaptureProperty:Int(deviceno:Int, prop:Int, value:Float, autoval:Int)

	Function bmx_escapi_params_new:Byte Ptr()
	Function bmx_escapi_params_free(handle:Byte Ptr)
	Function bmx_escapi_params_SetWidth(handle:Byte Ptr, width:Int)
	Function bmx_escapi_params_SetHeight(handle:Byte Ptr, height:Int)
	Function bmx_escapi_params_SetBuffer(handle:Byte Ptr, buffer:Byte Ptr)
	Function bmx_escapi_params_Buffer:Byte Ptr(handle:Byte Ptr)
End Extern

Const CAPTURE_BRIGHTNESS:Int = 0
Const CAPTURE_CONTRAST:Int = 1
Const CAPTURE_HUE:Int = 2
Const CAPTURE_SATURATION:Int = 3
Const CAPTURE_SHARPNESS:Int = 4
Const CAPTURE_GAMMA:Int = 5
Const CAPTURE_COLORENABLE:Int = 6
Const CAPTURE_WHITEBALANCE:Int = 7
Const CAPTURE_BACKLIGHTCOMPENSATION:Int = 8
Const CAPTURE_GAIN:Int = 9
Const CAPTURE_PAN:Int = 10
Const CAPTURE_TILT:Int = 11
Const CAPTURE_ROLL:Int = 12
Const CAPTURE_ZOOM:Int = 13
Const CAPTURE_EXPOSURE:Int = 14
Const CAPTURE_IRIS:Int = 15
Const CAPTURE_FOCUS:Int = 16
