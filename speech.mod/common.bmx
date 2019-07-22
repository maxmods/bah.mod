' Copyright (c) 2010-2019 Bruce A Henderson
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

?macos
Import Pub.Macos
Import "glue.m"
?win32
Import pub.win32
Import "-lsapi"
Import "-lole32"
?linux
Import "-lFestival"
Import "-lestools"
Import "-lestbase"
Import "-leststring"
Import "/usr/include/festival/*.h"
Import "/usr/include/speech_tools/*.h"
Import "glue.cpp"
?


Type TSpeechBase
	Method speak:Int(Text:String) Abstract
	Method isSpeaking:Int() Abstract
	Function availableVoices:String[]() Abstract
	Method setVoice(voice:String) Abstract
	Method pause() Abstract
	Method resume() Abstract
	Method setVolume(value:Int) Abstract
	Method getVolume:Int() Abstract
	Method getPosition(char:Int Var, length:Int Var) Abstract
End Type


?macos
Extern
	Function bmx_speech_new:Byte Ptr()
	Function bmx_speech_speak:Int(handle:Byte Ptr, Text:String)
	Function bmx_speech_isSpeaking:Int(handle:Byte Ptr)
	Function bmx_speech_free(handle:Byte Ptr)
	Function bmx_speech_availableVoices:String[]()
	Function bmx_speech_setVoice(handle:Byte Ptr, voice:String)
	Function bmx_speech_pause(handle:Byte Ptr)
	Function bmx_speech_resume(handle:Byte Ptr)
	Function bmx_speech_setVolume(handle:Byte Ptr, v:Float)
	Function bmx_speech_getVolume:Float(handle:Byte Ptr)
	Function bmx_speech_setRate(handle:Byte Ptr, rate:Float)
	Function bmx_speech_rate:Float(handle:Byte Ptr)
End Extern

?linux
Extern
	Function bmx_festival_initialize(load_init_files:Int, heapsize:Int)
	Function bmx_festival_say_text:Int(s:Byte Ptr)
End Extern
?


