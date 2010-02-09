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

Import BRL.Blitz

?macos
Import Pub.Macos
Import "glue.m"
?win32
Import pub.win32
Import "-lsapi"
Import "-lole32"
?


Type TSpeechBase
	Method speak:Int(text:String) Abstract
	Method isSpeaking:Int() Abstract
	Function availableVoices:String[]() Abstract
	Method setVoice(voice:String) Abstract
	Method pause() Abstract
	Method resume() Abstract
	Method setVolume(value:Int) Abstract
	Method getVolume:Int() Abstract
	Method getPosition(char:Int Var, length:Int Var) Abstract
End Type




?Not win32
Extern
	Function bmx_speech_new:Byte Ptr()
	Function bmx_speech_speak:Int(handle:Byte Ptr, text:String)
	Function bmx_speech_isSpeaking:Int(handle:Byte Ptr)
	Function bmx_speech_free(handle:Byte Ptr)
	Function bmx_speech_availableVoices:String[]()
	Function bmx_speech_setVoice(handle:Byte Ptr, voice:String)
	Function bmx_speech_pause(handle:Byte Ptr)
	Function bmx_speech_resume(handle:Byte Ptr)
	Function bmx_speech_setVolume(handle:Byte Ptr, v:Float)
	Function bmx_speech_getVolume:Float(handle:Byte Ptr)
End Extern

?win32
Extern "win32"

Function CoInitialize:Int(p:Byte Ptr)
Function CoUninitialize:Int()
Function CoCreateInstance(rclsid:Byte Ptr,pUnkOuter:Byte Ptr,dwClsContext:Int,riid:Byte Ptr,ppv:Byte Ptr)

Type ISpNotifySource Extends IUnknown
	Method SetNotifySink:Int(pNotifySink:Byte Ptr)
	Method SetNotifyWindowMessage:Int(hWnd:Int, Msg:Int, wParam:Int, lParam:Int)
	Method SetNotifyCallbackFunction:Int(pfnCallback(wParam:Int, lParam:Int), wParam:Int, lParam:Int)
	Method SetNotifyCallbackInterface:Int(pSpCallback:Int(wParam:Int, lParam:Int), wParam:Int, lParam:Int)
	Method SetNotifyWin32Event:Int()
	Method WaitForNotifyEvent:Int(dwMilliseconds:Int)
	Method GetNotifyEventHandle:Int()
End Type

Type ISpEventSource Extends ISpNotifySource
	Method SetInterest:Int(ullEventInterest:Long, ullQueuedInterest:Long)
	Method GetEvents:Int(ulCount:Int, pEventArray:Byte Ptr, pulFetched:Int Ptr)
	Method GetInfo:Int(pInfo:Byte Ptr)
End Type

Type ISpVoice Extends ISpEventSource
	Method SetOutput:Int(pUnkOutput:Byte Ptr, fAllowFormatChanges:Int)
	Method GetOutputObjectToken:Int(ppObjectToken:Byte Ptr Ptr)
	Method GetOutputStream:Int(ppStream:Byte Ptr Ptr)
	Method Pause:Int()
	Method Resume:Int()
	Method SetVoice:Int(pToken:Byte Ptr)
	Method GetVoice:Int(ppToken:Byte Ptr Ptr)
	Method Speak:Int(pwcs$w, dwFlags:Int, pulStreamNumber:Int Ptr)
	Method SpeakStream:Int(pStream:Byte Ptr, dwFlags:Int, pulStreamNumber:Int Ptr)
	Method GetStatus:Int(pStatus:Byte Ptr, ppszLastBookmark:Short Ptr Ptr)
	Method Skip:Int(pItemType$w, lNumItems:Int, pulNumSkipped:Int Ptr)
	Method SetPriority:Int(ePriority:Int)
	Method GetPriority:Int(pePriority:Int Ptr)
	Method SetAlertBoundary:Int(eBoundary:Int)
	Method GetAlertBoundary:Int(peBoundary:Int Ptr)
	Method SetRate:Int(RateAdjust:Int)
	Method GetRate:Int(pRateAdjust:Int Ptr)
	Method SetVolume:Int(usVolume:Short)
	Method GetVolume:Int(pusVolume:Short Ptr)
	Method WaitUntilDone:Int(msTimeout:Int)
	Method SetSyncSpeakTimeout:Int(msTimeout:Int)
	Method GetSyncSpeakTimeout:Int(pmsTimeout:Int Ptr)
	Method SpeakCompleteEvent:Int()
	Method IsUISupported:Int(pszTypeOfUI$w, pvExtraData:Byte Ptr, cbExtraData:Int, pfSupported:Int Ptr)
	Method DisplayUI:Int(hwndParent:Int, pszTitle$w, pszTypeOfUI$w, pvExtraData:Byte Ptr, cbExtraData:Int)
End Type

End Extern

Type SPVOICESTATUS
    Field ulCurrentStream:Int
    Field ulLastStreamQueued:Int
    Field hrLastResult:Int
    Field dwRunningState:Int
    Field ulInputWordPos:Int
    Field ulInputWordLen:Int
    Field ulInputSentPos:Int
    Field ulInputSentLen:Int
    Field lBookmarkId:Int
    Field PhonemeId:Int
    Field VisemeId:Int
    Field dwReserved1:Int
    Field dwReserved2:Int
End Type

Const SPF_DEFAULT:Int = 0
Const SPF_ASYNC:Int = 1 Shl 0
Const SPF_PURGEBEFORESPEAK:Int = 1 Shl 1
Const SPF_IS_FILENAME:Int = 1 Shl 2
Const SPF_IS_XML:Int = 1 Shl 3
Const SPF_IS_NOT_XML:Int = 1 Shl 4
Const SPF_PERSIST_XML:Int = 1 Shl 5
Const SPF_NLP_SPEAK_PUNC:Int = 1 Shl 6
Const SPF_NLP_MASK:Int = SPF_NLP_SPEAK_PUNC
Const SPF_VOICE_MASK:Int = SPF_ASYNC | SPF_PURGEBEFORESPEAK | SPF_IS_FILENAME | SPF_IS_XML | SPF_IS_NOT_XML | SPF_NLP_MASK | SPF_PERSIST_XML
Const SPF_UNUSED_FLAGS:Int = ~SPF_VOICE_MASK


Global IID_ISpVoice:Int[]=[$6c44df74, $499272b9, $99efeca1, $d422046e]
Global CLSID_SpVoice:Int[]=[$96749377, $11d23391, $c000e39e, $9673794f]
Rem
Dim IID_ISpVoice As GUID => ( &H6c44df74, &H72b9, &H4992, {&Ha1, &Hec, &Hef, &H99, &H6e, &H04, &H22, &Hd4 })
Dim CLSID_SpVoice As GUID => ( &H96749377, &H3391, &H11d2, {&H9e, &He3, &H00, &Hc0, &H4f, &H79, &H73, &H96 })
End Rem
?