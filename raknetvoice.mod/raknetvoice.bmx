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
	bbdoc: Starts RakVoice.
	about: Parameters: 
	<ul>
	<li><b>speexSampleRate</b> : 8000, 16000, or 32000</li>
	<li><b>bufferSizeBytes</b> : How many bytes long inputBuffer and outputBuffer are in SendFrame and ReceiveFrame are.  Should be your sample size * the number of samples to encode at once.</li>
	<li><b>audioInterface</b> : An instance of the audio interface. eg. a TFMODSystem object.</li>
	</ul>
	End Rem
	Method Init(speexSampleRate:Int, bufferSizeBytes:Int, audioInterface:Object)
		bmx_RakVoice_Init(pluginPtr, speexSampleRate, bufferSizeBytes)
		_adapter.Init(audioInterface, Self)
	End Method
	
	Rem
	bbdoc: Changes encoder complexity
	about: Specifying higher values might help when encoding non-speech sounds.
	<p>Parameters: 
	<ul>
	<li><b>complexity</b> : 0 to 10. The higher the value, the more CPU it needs. Recommended values are from 2 to 4.</li>
	</ul>
	</p>
	End Rem
	Method SetEncoderComplexity(complexity:Int)
		bmx_RakVoice_SetEncoderComplexity(pluginPtr, complexity)
	End Method

	Rem
	bbdoc: Enables or disables VAD (Voice Activity Detection)
	about: Enabling VAD can help reduce the amount of data transmitted, by automatically disabling outgoing data, when no voice is detected.
	<p>
	Don't turn this off or the receive buffer fills up and you eventually get very long delays!!
	</p>
	<p>
	Only applies to encoder.
	</p>
	<p>Parameters: 
	<ul>
	<li><b>enable</b> : True to enable, False to disable. True by default</li>
	</ul>
	</p>

	End Rem
	Method SetVAD(enable:Int)
		bmx_RakVoice_SetVAD(pluginPtr, enable)
	End Method

	Rem
	bbdoc: Enables or disables the noise filter.
	about: Only applies to encoder.
	<p>Parameters: 
	<ul>
	<li><b>enable</b> : True to enable, False to disable.</li>
	</ul>
	</p>
	End Rem
	Method SetNoiseFilter(enable:Int)
		bmx_RakVoice_SetNoiseFilter(pluginPtr, enable)
	End Method


	Rem
	bbdoc: Enables or disables VBR
	about: VBR is variable bitrate. Uses less bandwidth but more CPU if on.
	<p>
	Only applies to encoder.
	</p>
	<p>Parameters: 
	<ul>
	<li><b>enable</b> : True to enable VBR, False to disable</li>
	</ul>
	</p>
	End Rem
	Method SetVBR(enable:Int)
		bmx_RakVoice_SetVBR(pluginPtr, enable)
	End Method

	Rem
	bbdoc: Returns the complexity of the encoder
	returns: A value from 0 to 10.
	about: Only applies to encoder.
	End Rem
	Method GetEncoderComplexity:Int()
		Return bmx_RakVoice_GetEncoderComplexity(pluginPtr)
	End Method

	Rem
	bbdoc: Returns current state of VAD.
	returns: True if VAD is enable, False otherwise.
	about: Only applies to encoder.
	End Rem
	Method IsVADActive:Int()
		Return bmx_RakVoice_IsVADActive(pluginPtr)
	End Method

	Rem
	bbdoc: Returns the current state of the noise filter
	returns: True if the noise filter is active, False otherwise.
	about: Only applies to encoder.
	End Rem
	Method IsNoiseFilterActive:Int()
		Return bmx_RakVoice_IsNoiseFilterActive(pluginPtr)
	End Method

	Rem
	bbdoc: Returns the current state of VBR.
	returns: True if VBR is active, False otherwise.
	about: Only applies to encoder.
	End Rem
	Method IsVBRActive:Int()
		Return bmx_RakVoice_IsVBRActive(pluginPtr)
	End Method

	Rem
	bbdoc: Shuts down RakVoice
	End Rem
	Method Deinit()
		bmx_RakVoice_Deinit(pluginPtr)
	End Method
	
	Rem
	bbdoc: Opens a channel to another connected system
	about: You will get ID_RAKVOICE_OPEN_CHANNEL_REPLY on success.
	<p>Parameters: 
	<ul>
	<li><b>recipient</b> : Which system to open a channel to</li>
	</ul>
	</p>
	End Rem
	Method RequestVoiceChannel(recipient:TRKSystemAddress)
		bmx_RakVoice_RequestVoiceChannel(pluginPtr, recipient.systemAddressPtr)
	End Method
	
	Rem
	bbdoc: Closes an existing voice channel.
	about: Other system will get ID_RAKVOICE_CLOSE_CHANNEL
	<p>Parameters: 
	<ul>
	<li><b>recipient</b> : Which system to close a channel with</li>
	</ul>
	</p>
	End Rem
	Method CloseVoiceChannel(recipient:TRKSystemAddress)
		bmx_RakVoice_CloseVoiceChannel(pluginPtr, recipient.systemAddressPtr)
	End Method
	
	Rem
	bbdoc: Closes all existing voice channels.
	about: Other systems will get ID_RAKVOICE_CLOSE_CHANNEL
	End Rem
	Method CloseAllChannels()
		bmx_RakVoice_CloseAllChannels(pluginPtr)
	End Method

	Rem
	bbdoc: Returns if we are currently sending voice data, accounting for voice activity detection.
	returns: True if we are sending voice data for the specified system.
	about: Parameters: 
	<ul>
	<li><b>recipient</b> :Which system to check</li>
	</ul>
	End Rem
	Method IsSendingVoiceDataTo:Int(recipient:TRKSystemAddress)
		Return bmx_RakVoice_IsSendingVoiceDataTo(pluginPtr, recipient.systemAddressPtr)
	End Method
	
	Rem
	bbdoc: Returns the value sample rate, as passed to Init.
	returns: The sample rate.
	End Rem
	Method GetSampleRate:Int()
		Return bmx_RakVoice_GetSampleRate(pluginPtr)
	End Method
	
	Rem
	bbdoc: Returns the buffer size in bytes, as passed to Init.
	returns: Buffer size in bytes.
	End Rem
	Method GetBufferSizeBytes:Int()
		Return bmx_RakVoice_GetBufferSizeBytes(pluginPtr)
	End Method
	
	Rem
	bbdoc: Returns true or false, indicating if the object has been initialized
	returns: True if initialized, False otherwise.
	End Rem
	Method IsInitialized:Int()
		Return bmx_RakVoice_IsInitialized(pluginPtr)
	End Method
	
	Rem
	bbdoc: Returns the RakPeerInterface that the object is attached to.
	returns: The respective RakPeerInterface, or null if not attached.
	End Rem
	Method GetRakPeerInterface:TRKRakPeerInterface()
		Return TRKRakPeer._create(bmx_RakVoice_GetRakPeerInterface(pluginPtr))
	End Method
	
	Rem
	bbdoc: You need to call this once in a while, depending on the parameters used.
	about: For example, call once every 20-30 milliseconds.
	End Rem
	Method Update()
		_adapter.Update()
	End Method
	
End Type


Global _voiceAdapter:TRKVoiceAdapter

' internal stuff
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

