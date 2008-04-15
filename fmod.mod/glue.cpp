/*
  Copyright (c) 2008 Bruce A Henderson
 
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

#include "fmod.h"
#include "blitz.h"

extern "C" {

	unsigned int bmx_fmod_getversion();

	FMOD_SYSTEM  * bmx_FMOD_System_Create(FMOD_RESULT * result);
	FMOD_SOUND * bmx_FMOD_System_CreateSound(FMOD_SYSTEM *system, const char * filename, FMOD_MODE mode,
		FMOD_CREATESOUNDEXINFO * exInfo, FMOD_RESULT * ret);
	FMOD_CHANNEL * bmx_FMOD_System_PlaySound(FMOD_SYSTEM *system, FMOD_CHANNELINDEX channelId, FMOD_SOUND * sound,
		FMOD_BOOL paused, FMOD_CHANNEL * reuse);
	FMOD_CHANNEL * bmx_FMOD_System_GetChannel(FMOD_SYSTEM *system, int channelId);
	FMOD_DSP * bmx_FMOD_System_CreateDSPByType(FMOD_SYSTEM *system, FMOD_DSP_TYPE dspType, FMOD_RESULT * ret);


	FMOD_RESULT bmx_FMOD_Channel_GetSpectrum(FMOD_CHANNEL *channel, BBArray * spectrumArray, int channelOffset, FMOD_DSP_FFT_WINDOW windowType);
	FMOD_SOUND * bmx_FMOD_Channel_GetCurrentSound(FMOD_CHANNEL *channel);

	FMOD_CREATESOUNDEXINFO * bmx_soundexinfo_create();
	void bmx_soundexinfo_setlength(FMOD_CREATESOUNDEXINFO * info, int length);	
	void bmx_soundexinfo_delete(FMOD_CREATESOUNDEXINFO * info);
	
	FMOD_SOUND * bmx_FMOD_SoundGroup_GetSound(FMOD_SOUNDGROUP *soundgroup, int index);

	FMOD_TAG * bmx_FMOD_Sound_GetTag(FMOD_SOUND * sound, const char * name, int index);
	
	void * bmx_fmodtag_getdata(FMOD_TAG * tag);
	void bmx_fmodtag_delete(FMOD_TAG * tag);


}

unsigned int bmx_fmod_getversion() {
	return FMOD_VERSION;
}



FMOD_SYSTEM  * bmx_FMOD_System_Create(FMOD_RESULT * result) {
	FMOD_SYSTEM * system;

	*result = FMOD_System_Create(&system);

	return system;
}

FMOD_SOUND * bmx_FMOD_System_CreateSound(FMOD_SYSTEM *system, const char * filename, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO * exInfo,
		FMOD_RESULT * result) {

	FMOD_SOUND * sound;
	
	*result = FMOD_System_CreateSound(system, filename, mode, exInfo, &sound);
	
	if (*result) {
		return 0;
	}
	
	return sound;

}

FMOD_CHANNEL * bmx_FMOD_System_PlaySound(FMOD_SYSTEM *system, FMOD_CHANNELINDEX channelId, FMOD_SOUND * sound,
		FMOD_BOOL paused, FMOD_CHANNEL * reuse) {

	FMOD_CHANNEL * channel;
	
	if (reuse) {
		channel = reuse;
	}

	FMOD_RESULT result = FMOD_System_PlaySound(system, channelId, sound, paused, &channel);
	
	if (result) {
		return 0;
	}
	
	return channel;	
}

FMOD_CHANNEL * bmx_FMOD_System_GetChannel(FMOD_SYSTEM *system, int channelId) {
	FMOD_CHANNEL * channel;
	
	FMOD_RESULT result = FMOD_System_GetChannel(system, channelId, &channel);
	
	if (result) {
		return 0;
	}
	
	return channel;
}

FMOD_DSP * bmx_FMOD_System_CreateDSPByType(FMOD_SYSTEM *system, FMOD_DSP_TYPE dspType, FMOD_RESULT * ret) {
	FMOD_DSP * dsp;
	
	*ret = FMOD_System_CreateDSPByType(system, dspType, &dsp);
	
	if (*ret) {
		return 0;
	}
	
	return dsp;
}

// ++++++++++++++++++++++++++++++++

FMOD_RESULT bmx_FMOD_Channel_GetSpectrum(FMOD_CHANNEL *channel, BBArray * spectrumArray, int channelOffset, FMOD_DSP_FFT_WINDOW windowType) {

	int size = spectrumArray->scales[0];
	float arr[size];
	
	FMOD_RESULT res = FMOD_Channel_GetSpectrum(channel, &arr[0], size, channelOffset, windowType);

	float *s=(float*)BBARRAYDATA( spectrumArray, spectrumArray->dims );
	for (int i = 0; i < size; i ++) {
		s[i] = arr[i];
	}
	
	return res;
}

FMOD_SOUND * bmx_FMOD_Channel_GetCurrentSound(FMOD_CHANNEL *channel) {

	FMOD_SOUND * sound;
	
	FMOD_RESULT result = FMOD_Channel_GetCurrentSound(channel, &sound);

	if (result) {
		return 0;
	}
	
	return sound;
}

// ++++++++++++++++++++++++++++++++

FMOD_CREATESOUNDEXINFO * bmx_soundexinfo_create() {
	FMOD_CREATESOUNDEXINFO * info = new FMOD_CREATESOUNDEXINFO;
	
	memset(info, 0, sizeof(FMOD_CREATESOUNDEXINFO)); // need to zerofy the data
	
	info->cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	return info;
}

void bmx_soundexinfo_setlength(FMOD_CREATESOUNDEXINFO * info, int length) {
	info->length = length;
}

void bmx_soundexinfo_delete(FMOD_CREATESOUNDEXINFO * info) {
	delete info;
}

// ++++++++++++++++++++++++++++++++

FMOD_SOUND * bmx_FMOD_SoundGroup_GetSound(FMOD_SOUNDGROUP *soundgroup, int index) {
	FMOD_SOUND * sound;
	FMOD_RESULT result = FMOD_SoundGroup_GetSound(soundgroup, index, &sound);
	
	if (result) {
		return 0;
	}
	
	return sound;
}


// ++++++++++++++++++++++++++++++++

FMOD_TAG * bmx_FMOD_Sound_GetTag(FMOD_SOUND * sound, const char * name, int index) {
	FMOD_TAG * tag = new FMOD_TAG;
	FMOD_RESULT result = FMOD_Sound_GetTag(sound, name, index, tag);
	if (result) {
		return 0;
	}
	
	return tag;
}

// ++++++++++++++++++++++++++++++++

void * bmx_fmodtag_getdata(FMOD_TAG * tag) {
	return tag->data;
}

void bmx_fmodtag_delete(FMOD_TAG * tag) {
	delete tag;
}
