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

Rem
bbdoc: Extremely Simple Capture API
about: For Windows Vista or greater.
End Rem
Module BaH.ESCAPI

ModuleInfo "Version: 1.00"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: ESCAPI - 'unlicense'"
ModuleInfo "Copyright: Wrapper - 2016 Bruce A Henderson"
ModuleInfo "Copyright: ESCAPI - 2015 Jari Komppa http://iki.fi/sol"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

?win32
Import "common.bmx"


Rem
bbdoc: 
End Rem
Type TEscapi

	Field paramsPtr:Byte Ptr

	Rem
	bbdoc: Creates a TEscapi instance and initialises the library if required.
	End Rem
	Method Create:TEscapi()
		Global _inited:Int
		If Not _inited Then
			
			If setupESCAPI() < 0 Then
				Return Null
			End If
			
			_inited = True
		End If
		paramsPtr = bmx_escapi_params_new()
		
		Return Self
	End Method
	
	Method Delete()
		If paramsPtr Then
			bmx_escapi_params_free(paramsPtr)
			paramsPtr = Null
		End If
	End Method
	
	Rem
	bbdoc: Sets the width
	End Rem
	Method SetWidth(width:Int)
		bmx_escapi_params_SetWidth(paramsPtr, width)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetHeight(height:Int)
		bmx_escapi_params_SetHeight(paramsPtr, height)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetBuffer(buffer:Byte Ptr)
		bmx_escapi_params_SetBuffer(paramsPtr, buffer)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Buffer:Byte Ptr()
		Return bmx_escapi_params_Buffer(paramsPtr)
	End Method

	Rem
	bbdoc: Returns a count of available capture devices.
	End Rem
	Method Count:Int()
		Return bmx_escapi_countCaptureDevices()
	End Method
	
	Rem
	bbdoc: Tries to open the video capture device.
	returns: False on failure, True on success.
	End Rem
	Method Open:Int(device:Int)
		Return bmx_escapi_initCapture(device, paramsPtr)
	End Method
	
	Rem
	bbdoc: Closes the video capture device.
	End Rem
	Method Close(device:Int)
		bmx_escapi_deinitCapture(device)
	End Method
	
	Rem
	bbdoc: Requests a video frame to be captured.
	End Rem
	Method Capture(device:Int)
		bmx_escapi_doCapture(device)
	End Method
	
	Rem
	bbdoc: Returns True when the requested frame has been captured
	End Rem
	Method IsCaptureDone:Int(device:Int)
		Return bmx_escapi_isCaptureDone(device)
	End Method
	
	Rem
	bbdoc: Returns the printable name of the capture device.
	End Rem
	Method DeviceName:String(device:Int)
		Local buf:Byte[2048]
		bmx_escapi_getCaptureDeviceName(device, buf, buf.length)
		Return String.FromCString(buf)
	End Method
	
	Rem
	bbdoc: Returns the library version.
	End Rem
	Method Version:Int()
		Return bmx_escapi_ESCAPIVersion()
	End Method

	Rem
	bbdoc: Returns a property value [0..1] for the capture device.
	End Rem
	Method PropertyValue:Float(device:Int, prop:Int)
		Return bmx_escapi_getCapturePropertyValue(device, prop)
	End Method
	
	Rem
	bbdoc: Returns whether a property is set to automatic or not.
	End Rem
	Method PropertyIsAutomatic:Int(device:Int, prop:Int)
		Return bmx_escapi_getCapturePropertyAuto(device, prop)
	End Method
	
	Rem
	bbdoc: Sets the device property value [0..1].
	about: Optionally sets whether the property should be set to automatic or not.
	End Rem
	Method SetProperty:Int(device:Int, prop:Int, value:Float, auto:Int = False)
		Return bmx_escapi_setCaptureProperty(device, prop, value, auto)
	End Method
	
End Type


?
