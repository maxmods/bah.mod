#include "RakVoice.h"
#include "../raknet.mod/glue.h"

extern "C" {

#include "blitz.h"

	RakVoice * bmx_RakVoice_new();

	void bmx_RakVoice_Init(RakVoice * voice, int speexSampleRate, int bufferSizeBytes);
	void bmx_RakVoice_SetEncoderComplexity(RakVoice * voice, int complexity);
	void bmx_RakVoice_SetVAD(RakVoice * voice, bool enable);
	void bmx_RakVoice_SetNoiseFilter(RakVoice * voice, bool enable);
	void bmx_RakVoice_SetVBR(RakVoice * voice, bool enable);
	int bmx_RakVoice_GetEncoderComplexity(RakVoice * voice);
	bool bmx_RakVoice_IsVADActive(RakVoice * voice);
	bool bmx_RakVoice_IsNoiseFilterActive(RakVoice * voice);
	bool bmx_RakVoice_IsVBRActive(RakVoice * voice);
	void bmx_RakVoice_Deinit(RakVoice * voice);
	void bmx_RakVoice_RequestVoiceChannel(RakVoice * voice, MaxSystemAddress * recipient);
	void bmx_RakVoice_CloseVoiceChannel(RakVoice * voice, MaxSystemAddress * recipient);
	void bmx_RakVoice_CloseAllChannels(RakVoice * voice);



}
