/*
 Copyright (c) 2011, Bruce A Henderson
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of Bruce A Henderson nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _BASS_AudioInterface_h
#define _BASS_AudioInterface_h

#include "TheoraAudioInterface.h"
#include "TheoraVideoClip.h"
#include "TheoraTimer.h"

#include "bass.h"
#include <queue>


class BASS_AudioInterface : public TheoraAudioInterface, TheoraTimer
{
	int mMaxBuffSize;
	float *mTempBuffer;

	HSTREAM mStream;
public:
	BASS_AudioInterface(TheoraVideoClip* owner,int nChannels,int freq);
	~BASS_AudioInterface();
	void insertData(float** data,int nSamples);
	void destroy();

	void update(float time_increase);

	void pause();
	void play();
	void seek(float time);
};



class BASS_AudioInterfaceFactory : public TheoraAudioInterfaceFactory
{
public:
	BASS_AudioInterfaceFactory();
	~BASS_AudioInterfaceFactory();
	BASS_AudioInterface* createInstance(TheoraVideoClip* owner,int nChannels,int freq);
};

#endif
