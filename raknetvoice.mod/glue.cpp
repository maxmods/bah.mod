/*
  Copyright (c) 2009-2010 Bruce A Henderson
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/ 

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

void bmx_RakVoice_RequestVoiceChannel(RakVoice * voice, RakNetGUID * guid) {
	voice->RequestVoiceChannel(*guid);
}

void bmx_RakVoice_CloseVoiceChannel(RakVoice * voice, RakNetGUID * guid) {
	voice->CloseVoiceChannel(*guid);
}

void bmx_RakVoice_CloseAllChannels(RakVoice * voice) {
	voice->CloseAllChannels();
}

bool bmx_RakVoice_IsSendingVoiceDataTo(RakVoice * voice, RakNetGUID * guid) {
	return voice->IsSendingVoiceDataTo(*guid);
}

int bmx_RakVoice_GetSampleRate(RakVoice * voice) {
	return voice->GetSampleRate();
}

int bmx_RakVoice_GetBufferSizeBytes(RakVoice * voice) {
	return voice->GetBufferSizeBytes();
}

bool bmx_RakVoice_IsInitialized(RakVoice * voice) {
	return voice->IsInitialized();
}

RakPeerInterface * bmx_RakVoice_GetRakPeerInterface(RakVoice * voice) {
	return voice->GetRakPeerInterface();
}


