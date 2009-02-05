' Copyright (c) 2009 Bruce A Henderson
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
bbdoc: Raknet Voice
End Rem
Module BaH.RaknetVoice

ModuleInfo "Version: 1.00"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: Raknet - The Creative Commons Attribution - NonCommercial 2.5, or see http://www.rakkarsoft.com/SingleApplicationLicense.html"
ModuleInfo "Copyright: 2009 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Version"

ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H"

Import "common.bmx"


Rem
bbdoc: Encodes, decodes, and transmits voice data.
about: Voice compression and transmission interface.
End Rem
Type TRKRakVoice Extends TRKPluginInterface

	Field _adapter:TRKVoiceAdapter
	
	Method New()
		pluginPtr = bmx_RakVoice_new()
		_adapter = TRKVoiceAdapter.GetAdapter()
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Init(speexSampleRate:Int, bufferSizeBytes:Int, audioInterface:Object)
		bmx_RakVoice_Init(pluginPtr, speexSampleRate, bufferSizeBytes)
		_adapter.Init(audioInterface, Self)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetEncoderComplexity(complexity:Int)
		bmx_RakVoice_SetEncoderComplexity(pluginPtr, complexity)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetVAD(enable:Int)
		bmx_RakVoice_SetVAD(pluginPtr, enable)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetNoiseFilter(enable:Int)
		bmx_RakVoice_SetNoiseFilter(pluginPtr, enable)
	End Method


	Rem
	bbdoc: 
	End Rem
	Method SetVBR(enable:Int)
		bmx_RakVoice_SetVBR(pluginPtr, enable)
	End Method


	Rem
	bbdoc: 
	End Rem
	Method GetEncoderComplexity:Int()
		Return bmx_RakVoice_GetEncoderComplexity(pluginPtr)
	End Method


	Rem
	bbdoc: 
	End Rem
	Method IsVADActive:Int()
		Return bmx_RakVoice_IsVADActive(pluginPtr)
	End Method


	Rem
	bbdoc: 
	End Rem
	Method IsNoiseFilterActive:Int()
		Return bmx_RakVoice_IsNoiseFilterActive(pluginPtr)
	End Method


	Rem
	bbdoc: 
	End Rem
	Method IsVBRActive:Int()
		Return bmx_RakVoice_IsVBRActive(pluginPtr)
	End Method


	Rem
	bbdoc: 
	End Rem
	Method Deinit()
		bmx_RakVoice_Deinit(pluginPtr)
	End Method
	
	Method RequestVoiceChannel(recipient:TRKSystemAddress)
		bmx_RakVoice_RequestVoiceChannel(pluginPtr, recipient.systemAddressPtr)
	End Method
	
	Method CloseVoiceChannel(recipient:TRKSystemAddress)
		bmx_RakVoice_CloseVoiceChannel(pluginPtr, recipient.systemAddressPtr)
	End Method
	
	Method CloseAllChannels()
		bmx_RakVoice_CloseAllChannels(pluginPtr)
	End Method

	Method Update()
		_adapter.Update()
	End Method
	
End Type

Global _voiceAdapter:TRKVoiceAdapter

Type TRKVoiceAdapter

	Field rakVoice:TRKRakVoice


	Function GetAdapter:TRKVoiceAdapter()
		Return _voiceAdapter
	End Function
	
	Function  SetAdapter(adapter:TRKVoiceAdapter)
		_voiceAdapter = adapter
	End Function


	Method Init:Int(audioInterface:Object, rakVoice:TRKRakVoice) Abstract

	Method Update() Abstract

End Type

