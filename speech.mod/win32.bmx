' Copyright (c) 2010-2017 Bruce A Henderson
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

Import "win32_common.bmx"

CoInitialize(Null)
OnEnd(_tidyup)

Type TSpeech Extends TSpeechBase

	Field speech:Byte Ptr
	Field status:SPVOICESTATUS = New SPVOICESTATUS

	Method New()
		speech = bmx_speech_create()
	End Method

	Method speak:Int(text:String)
		Return bmx_speech_Speak(speech, text)
	End Method

	Method isSpeaking:Int()
		bmx_speech_GetStatus(speech, status)
		Return status.dwRunningState <> 1
	End Method
	
	Function availableVoices:String[]()
		Return bmx_speech_availableVoices()
	End Function
	
	Method setVoice(voice:String)
		bmx_speech_setVoice(speech, voice)
	End Method

	Method pause()
		bmx_speech_Pause(speech)
	End Method
	
	Method resume()
		bmx_speech_Resume(speech)
	End Method

	Method setVolume(value:Int)
		bmx_speech_SetVolume(speech, Short(Min(100, Max(0, value))))
	End Method
	
	Method getVolume:Int()
		Local v:Short
		bmx_speech_GetVolume(speech, Varptr v)
		Return v
	End Method

	Method getPosition(char:Int Var, length:Int Var)
		bmx_speech_GetStatus(speech, status)
		char = status.ulInputWordPos
		length = status.ulInputWordLen
	End Method
	
	Method setRate(rate:Int)
		bmx_speech_SetRate(speech, rate)
	End Method
	
	Method getRate:Int()
		Local v:Int
		bmx_speech_GetRate(speech, Varptr v)
		Return v
	End Method

	Method Delete()
		If speech Then
			bmx_speech_Release(speech)
			speech = Null
		End If
	End Method
	
End Type

Function _tidyup()
	CoUninitialize()
End Function
