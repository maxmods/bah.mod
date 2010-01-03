' Copyright (c) 2010 Bruce A Henderson
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
		speechPtr = bmx_speech_new()
	End Method

	Method speak:Int(text:String)
		Return bmx_speech_speak(speechPtr, text)
	End Method

	Method isSpeaking:Int()
		Return bmx_speech_isSpeaking(speechPtr)
	End Method
	
	Function availableVoices:String[]()
		Return bmx_speech_availableVoices()
	End Function
	
	Method setVoice(voice:String)
		bmx_speech_setVoice(speechPtr, voice)
	End Method

	Method pause()
		bmx_speech_pause(speechPtr)
	End Method
	
	Method resume()
		bmx_speech_resume(speechPtr)
	End Method

	Method setVolume(value:Int)
		Local v:Float = Min(100, Max(0, value)) / 100.0
		bmx_speech_setVolume(speechPtr, v)
	End Method
	
	Method getVolume:Int()
		Return bmx_speech_getVolume(speechPtr) * 100
	End Method

	Method getPosition(char:Int Var, length:Int Var)
	End Method

	Method Delete()
		If speechPtr Then
			bmx_speech_free(speechPtr)
			speechPtr = Null
		End If
	End Method
	
End Type

