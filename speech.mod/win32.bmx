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

CoInitialize(Null)
OnEnd(_tidyup)

Type TSpeech Extends TSpeechBase

	Field speech:ISpVoice
	Field status:SPVOICESTATUS = New SPVOICESTATUS

	Method New()
		CoCreateInstance(CLSID_SpVoice, Null, CLSCTX_ALL, IID_ISpVoice, Varptr speech)
	End Method

	Method speak:Int(text:String)
		Local v:Int
		Return speech.Speak(text, SPF_ASYNC, Varptr v)
	End Method

	Method isSpeaking:Int()
		speech.GetStatus(status, Null)
		Return status.dwRunningState <> 1
	End Method
	
	Function availableVoices:String[]()
		Local category:ISpObjectTokenCategory
		CoCreateInstance(CLSID_SpObjectTokenCategory, Null, CLSCTX_ALL, IID_ISpObjectTokenCategory, Varptr category)

		category.SetId(SPCAT_VOICES, False)


		Local enum:IEnumSpObjectTokens
		category.EnumTokens("", "", Varptr enum)
		
		Local count:Int
		
		enum.GetCount(Varptr count)
		
		Local list:String[] = New String[count]
	
		For Local i:Int = 0 Until count
			Local token:ISpObjectToken
			Local text:Short Ptr
			
			enum.GetNext(1, Varptr token, Null)
			
			token.GetStringValue("", Varptr text)
			list[i] = String.FromWString(text)
			
			token.release_
		Next
		
		category.release_
		
		Return list
	End Function
	
	Method setVoice(voice:String)
		Local category:ISpObjectTokenCategory
		CoCreateInstance(CLSID_SpObjectTokenCategory, Null, CLSCTX_ALL, IID_ISpObjectTokenCategory, Varptr category)

		category.SetId(SPCAT_VOICES, False)


		Local enum:IEnumSpObjectTokens
		category.EnumTokens("", "", Varptr enum)
		
		Local count:Int
		
		enum.GetCount(Varptr count)

		For Local i:Int = 0 Until count
			Local token:ISpObjectToken
			Local text:Short Ptr
			
			enum.GetNext(1, Varptr token, Null)
			
			token.GetStringValue("", Varptr text)
			Local v:String = String.FromWString(text)
			
			If v = voice Then
				speech.SetVoice(token)
				token.release_
				Exit
			End If
			
			token.release_
		Next
		
		category.release_
	End Method

	Method pause()
		speech.Pause()
	End Method
	
	Method resume()
		speech.Resume()
	End Method

	Method setVolume(value:Int)
		speech.SetVolume(Min(100, Max(0, value)))
	End Method
	
	Method getVolume:Int()
		Local v:Short
		speech.GetVolume(Varptr v)
		Return v
	End Method

	Method getPosition(char:Int Var, length:Int Var)
		speech.GetStatus(status, Null)
		char = status.ulInputWordPos
		length = status.ulInputWordLen
	End Method
	
	Method setRate(rate:Int)
		speech.SetRate(rate)
	End Method
	
	Method getRate:Int()
		Local v:Int
		speech.GetRate(Varptr v)
		Return v
	End Method

	Method Delete()
		If speech Then
			speech.release_
			speech = Null
		End If
	End Method
	
End Type

Function _tidyup()
	CoUninitialize()
End Function
