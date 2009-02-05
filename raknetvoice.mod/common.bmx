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
	Function bmx_RakVoice_RequestVoiceChannel(handle:Byte Ptr, recipient:Byte Ptr)
	Function bmx_RakVoice_CloseVoiceChannel(handle:Byte Ptr, recipient:Byte Ptr)
	Function bmx_RakVoice_CloseAllChannels(handle:Byte Ptr)


End Extern
