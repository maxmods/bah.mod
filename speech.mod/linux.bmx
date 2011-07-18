' Copyright (c) 2010-2011 Bruce A Henderson
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

Import "common.bmx"

Type TSpeech Extends TSpeechBase

	Field speechPtr:Byte Ptr

	Method New()
		bmx_festival_initialize(True, 210000)
	End Method

	Method speak:Int(text:String)
		Local s:Byte Ptr = text.ToUTF8String()
		Local res:Int = bmx_festival_say_text(s)
		MemFree(s)
		Return res
	End Method

	Method isSpeaking:Int()
		Return False
	End Method
	
	Function availableVoices:String[]()
		Return Null
	End Function
	
	Method setVoice(voice:String)
	End Method

	Method pause()
	End Method
	
	Method resume()
	End Method

	Method setVolume(value:Int)
	End Method
	
	Method getVolume:Int()
	End Method

	Method getPosition(char:Int Var, length:Int Var)
	End Method
	
	Method setRate(rate:Float)
	End Method
	
	Method getRate:Float()
	End Method

	Method Delete()
	End Method
	
End Type

