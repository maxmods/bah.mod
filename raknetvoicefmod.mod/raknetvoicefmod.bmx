' Copyright (c) 2009-2010 Bruce A Henderson
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
bbdoc: Raknet Voice - FMOD Driver
End Rem
Module BaH.RaknetVoiceFMOD

ModuleInfo "Version: 1.00"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: Raknet - The Creative Commons Attribution - NonCommercial 2.5, or see http://www.rakkarsoft.com/SingleApplicationLicense.html"
ModuleInfo "Copyright: 2009-2010 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Version"



Import BaH.RaknetVoice

Import "common.bmx"


New TRKFMODVoiceAdapter

Type TRKFMODVoiceAdapter Extends TRKVoiceAdapter

	Field system:TFMODSystem
	
	Field adapterPtr:Byte Ptr

	Method New()
		If Not GetAdapter() Then
			SetAdapter(Self)
		End If
	End Method

	Method Init:Int(audioInterface:Object, rakVoice:TRKRakVoice)
		system = TFMODSystem(audioInterface)

		If system Then
		
			Self.rakVoice = rakVoice
		
			adapterPtr = bmx_raknetvoicefmod_adapter_new(system.systemPtr, rakVoice.pluginPtr)
		
			Return True
		End If
		
		Return False
	End Method
	
	Method Update()
		If adapterPtr Then
			bmx_raknetvoicefmod_adapter_update(adapterPtr)
		End If
	End Method

End Type

