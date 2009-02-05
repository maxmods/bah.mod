#include "glue.h"

RakVoice * bmx_RakVoice_new() {
	return new RakVoice();
}

void bmx_RakVoice_Init(RakVoice * voice, int speexSampleRate, int bufferSizeBytes) {
	voice->Init(static_cast<unsigned short>(speexSampleRate), static_cast<unsigned>(bufferSizeBytes));
}

void bmx_RakVoice_SetEncoderComplexity(RakVoice * voice, int complexity) {
	voice->SetEncoderComplexity(complexity);
}

void bmx_RakVoice_SetVAD(RakVoice * voice, bool enable) {
	voice->SetVAD(enable);
}

void bmx_RakVoice_SetNoiseFilter(RakVoice * voice, bool enable) {
	voice->SetNoiseFilter(enable);
}

void bmx_RakVoice_SetVBR(RakVoice * voice, bool enable) {
	voice->SetVBR(enable);
}

int bmx_RakVoice_GetEncoderComplexity(RakVoice * voice) {
	return voice->GetEncoderComplexity();
}

bool bmx_RakVoice_IsVADActive(RakVoice * voice) {
	return voice->IsVADActive();
}

bool bmx_RakVoice_IsNoiseFilterActive(RakVoice * voice) {
	return voice->IsNoiseFilterActive();
}

bool bmx_RakVoice_IsVBRActive(RakVoice * voice) {
	return voice->IsVBRActive();
}

void bmx_RakVoice_Deinit(RakVoice * voice) {
	voice->Deinit();
}

void bmx_RakVoice_RequestVoiceChannel(RakVoice * voice, MaxSystemAddress * recipient) {
	voice->RequestVoiceChannel(recipient->Address());
}

void bmx_RakVoice_CloseVoiceChannel(RakVoice * voice, MaxSystemAddress * recipient) {
	voice->CloseVoiceChannel(recipient->Address());
}

void bmx_RakVoice_CloseAllChannels(RakVoice * voice) {
	voice->CloseAllChannels();
}


