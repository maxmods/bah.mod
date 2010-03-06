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


Import "source.bmx"


Extern

	Function bmx_RakVoice_new:Byte Ptr()

	Function bmx_RakVoice_Init(handle:Byte Ptr, speexSampleRate:Int, bufferSizeBytes:Int)
	Function bmx_RakVoice_SetEncoderComplexity(handle:Byte Ptr, complexity:Int)
	Function bmx_RakVoice_SetVAD(handle:Byte Ptr, enable:Int)
	Function bmx_RakVoice_SetNoiseFilter(handle:Byte Ptr, enable:Int)
	Function bmx_RakVoice_SetVBR(handle:Byte Ptr, enable:Int)
	Function bmx_RakVoice_GetEncoderComplexity:Int(handle:Byte Ptr)
	Function bmx_RakVoice_IsVADActive:Int(handle:Byte Ptr)
	Function bmx_RakVoice_IsNoiseFilterActive:Int(handle:Byte Ptr)
	Function bmx_RakVoice_IsVBRActive:Int(handle:Byte Ptr)
	Function bmx_RakVoice_Deinit(handle:Byte Ptr)
	Function bmx_RakVoice_RequestVoiceChannel(handle:Byte Ptr, guid:Byte Ptr)
	Function bmx_RakVoice_CloseVoiceChannel(handle:Byte Ptr, guid:Byte Ptr)
	Function bmx_RakVoice_CloseAllChannels(handle:Byte Ptr)
	Function bmx_RakVoice_IsSendingVoiceDataTo:Int(handle:Byte Ptr, guid:Byte Ptr)
	Function bmx_RakVoice_GetSampleRate:Int(handle:Byte Ptr)
	Function bmx_RakVoice_GetBufferSizeBytes:Int(handle:Byte Ptr)
	Function bmx_RakVoice_IsInitialized:Int(handle:Byte Ptr)
	Function bmx_RakVoice_GetRakPeerInterface:Byte Ptr(handle:Byte Ptr)


End Extern
