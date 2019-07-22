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

Import "-lsapi"

Import "common.bmx"
Import "win32_glue.cpp"

Extern "win32"

?bmxng
Function CoInitialize:Int(p:Byte Ptr)="int CoInitialize(LPVOID)!"
Function CoUninitialize:Int()="int CoUninitialize()!"
?Not bmxng
Function CoInitialize:Int(p:Byte Ptr)
Function CoUninitialize:Int()
?

End Extern

Extern
	Function bmx_speech_create:Byte Ptr()
	Function bmx_speech_Release(speech:Byte Ptr)
	Function bmx_speech_Speak:Int(speech:Byte Ptr, Text:String)
	Function bmx_speech_Pause(speech:Byte Ptr)
	Function bmx_speech_Resume(speech:Byte Ptr)
	Function bmx_speech_SetVolume(speech:Byte Ptr, volume:Short)
	Function bmx_speech_GetVolume(speech:Byte Ptr, volume:Short Ptr)
	Function bmx_speech_SetRate(speech:Byte Ptr, rate:Int)
	Function bmx_speech_GetRate(speech:Byte Ptr, rate:Int Ptr)
	Function bmx_speech_availableVoices:String[]()
	Function bmx_speech_setVoice(speech:Byte Ptr, voice$w)
	
	Function bmx_speech_isSpeaking:Int(speech:Byte Ptr)
	Function bmx_speech_getPosition(speech:Byte Ptr, character:Int Ptr, length:Int Ptr)
End Extern
