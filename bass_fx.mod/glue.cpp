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

#include "../bass.mod/include/bass.h"
#include "bass_fx.h"

extern "C" {

	BASS_BFX_ECHO * bmx_bass_fxecho_new();
	BASS_BFX_FLANGER * bmx_bass_fxflanger_new();
	BASS_BFX_VOLUME * bmx_bass_fxvolume_new();
	BASS_BFX_PEAKEQ * bmx_bass_fxpeakeq_new();
	BASS_BFX_REVERB * bmx_bass_fxreverb_new();
	BASS_BFX_LPF * bmx_bass_fxlpf_new();
	BASS_BFX_MIX * bmx_bass_fxmix_new();
	BASS_BFX_DAMP * bmx_bass_fxdamp_new();
	BASS_BFX_AUTOWAH * bmx_bass_fxautowah_new();
	BASS_BFX_ECHO2 * bmx_bass_fxecho2_new();
	BASS_BFX_PHASER * bmx_bass_fxphaser_new();
	BASS_BFX_ECHO3 * bmx_bass_fxecho3_new();
	BASS_BFX_CHORUS * bmx_bass_fxchorus_new();
	BASS_BFX_APF * bmx_bass_fxapf_new();
	BASS_BFX_COMPRESSOR * bmx_bass_fxcompressor_new();
	BASS_BFX_DISTORTION * bmx_bass_fxdistortion_new();

	float bmx_bassfxecho_getlevel(BASS_BFX_ECHO * fx);
	void bmx_bassfxecho_setlevel(BASS_BFX_ECHO * fx, float level);
	int bmx_bassfxecho_getdelay(BASS_BFX_ECHO * fx);
	void bmx_bassfxecho_setdelay(BASS_BFX_ECHO * fx, int dlay);

	float bmx_bassfxflanger_getwetdry(BASS_BFX_FLANGER * fx);
	void bmx_bassfxflanger_setwetdry(BASS_BFX_FLANGER * fx, float wetdry);
	float bmx_bassfxflanger_getspeed(BASS_BFX_FLANGER * fx);
	void bmx_bassfxflanger_setspeed(BASS_BFX_FLANGER * fx, float speed);
	int bmx_bassfxflanger_getchannel(BASS_BFX_FLANGER * fx);
	void bmx_bassfxflanger_setchannel(BASS_BFX_FLANGER * fx, int channel);

}

BASS_BFX_ECHO * bmx_bass_fxecho_new() {
    return new BASS_BFX_ECHO;
}

BASS_BFX_FLANGER * bmx_bass_fxflanger_new() {
    return new BASS_BFX_FLANGER;
}

BASS_BFX_VOLUME * bmx_bass_fxvolume_new() {
    return new BASS_BFX_VOLUME;
}

BASS_BFX_PEAKEQ * bmx_bass_fxpeakeq_new() {
    return new BASS_BFX_PEAKEQ;
}

BASS_BFX_REVERB * bmx_bass_fxreverb_new() {
    return new BASS_BFX_REVERB;
}

BASS_BFX_LPF * bmx_bass_fxlpf_new() {
    return new BASS_BFX_LPF;
}

BASS_BFX_MIX * bmx_bass_fxmix_new() {
    return new BASS_BFX_MIX;
}

BASS_BFX_DAMP * bmx_bass_fxdamp_new() {
    return new BASS_BFX_DAMP;
}

BASS_BFX_AUTOWAH * bmx_bass_fxautowah_new() {
    return new BASS_BFX_AUTOWAH;
}

BASS_BFX_ECHO2 * bmx_bass_fxecho2_new() {
    return new BASS_BFX_ECHO2;
}

BASS_BFX_PHASER * bmx_bass_fxphaser_new() {
    return new BASS_BFX_PHASER;
}

BASS_BFX_ECHO3 * bmx_bass_fxecho3_new() {
    return new BASS_BFX_ECHO3;
}

BASS_BFX_CHORUS * bmx_bass_fxchorus_new() {
    return new BASS_BFX_CHORUS;
}

BASS_BFX_APF * bmx_bass_fxapf_new() {
    return new BASS_BFX_APF;
}

BASS_BFX_COMPRESSOR * bmx_bass_fxcompressor_new() {
    return new BASS_BFX_COMPRESSOR;
}

BASS_BFX_DISTORTION * bmx_bass_fxdistortion_new() {
    return new BASS_BFX_DISTORTION;
}

// ********************************

float bmx_bassfxecho_getlevel(BASS_BFX_ECHO * fx) {
	return fx->fLevel;
}

void bmx_bassfxecho_setlevel(BASS_BFX_ECHO * fx, float level) {
	fx->fLevel = level;
}

int bmx_bassfxecho_getdelay(BASS_BFX_ECHO * fx) {
	return fx->lDelay;
}

void bmx_bassfxecho_setdelay(BASS_BFX_ECHO * fx, int dlay) {
	fx->lDelay = dlay;
}

// ********************************

float bmx_bassfxflanger_getwetdry(BASS_BFX_FLANGER * fx) {
    return fx->fWetDry;
}

void bmx_bassfxflanger_setwetdry(BASS_BFX_FLANGER * fx, float wetdry) {
    fx->fWetDry = wetdry;
}

float bmx_bassfxflanger_getspeed(BASS_BFX_FLANGER * fx) {
    return fx->fSpeed;
}

void bmx_bassfxflanger_setspeed(BASS_BFX_FLANGER * fx, float speed) {
    fx->fSpeed = speed;
}

int bmx_bassfxflanger_getchannel(BASS_BFX_FLANGER * fx) {
    return fx->lChannel;
}

void bmx_bassfxflanger_setchannel(BASS_BFX_FLANGER * fx, int channel) {
    fx->lChannel = channel;
}

