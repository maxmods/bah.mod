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
#include <stdio.h>
#include "BASS_AudioInterface.h"


BASS_AudioInterface::BASS_AudioInterface(TheoraVideoClip* owner,int nChannels,int freq) :
	TheoraAudioInterface(owner,nChannels,freq), TheoraTimer(), mTempBuffer(0)
{
	mStream = BASS_StreamCreate(freq, nChannels, BASS_SAMPLE_FLOAT, STREAMPROC_PUSH, this);

	mMaxBuffSize=freq*mNumChannels*2;

	mTempBuffer = new float[mMaxBuffSize];

	owner->setTimer(this);
}

void BASS_AudioInterface::destroy()
{
	// todo
}

BASS_AudioInterface::~BASS_AudioInterface()
{
	// todo: delete buffers and source
	if (mTempBuffer) delete mTempBuffer;
	if (mStream) BASS_StreamFree(mStream);
}

void BASS_AudioInterface::insertData(float** data,int nSamples)
{
	int size = 0;
	
	for (int i=0;i<nSamples;i++)
	{
		if (size < mMaxBuffSize)
		{
			mTempBuffer[size ++] = data[0][i];

			if (mNumChannels == 2) {
				mTempBuffer[size ++] = data[1][i];
			}
		}
	
	}
	
	DWORD err = BASS_StreamPutData(mStream, mTempBuffer, nSamples * 8);
	
	DWORD state = BASS_ChannelIsActive(mStream);
	
	if (state != BASS_ACTIVE_PLAYING) {
		BASS_ChannelPlay(mStream, 0);
	}
	
}

void BASS_AudioInterface::update(float time_increase)
{
	// control playback and return time position
	int state  = BASS_ChannelIsActive(mStream);

	mTime = BASS_ChannelBytes2Seconds(mStream, BASS_ChannelGetPosition(mStream, BASS_POS_BYTE));

	float duration = mClip->getDuration();
	if (mTime > duration) {
		mTime=duration;
	}
}

void BASS_AudioInterface::pause()
{
	BASS_ChannelPause(mStream);
	TheoraTimer::pause();
}

void BASS_AudioInterface::play()
{
	BASS_ChannelPlay(mStream, 0);
	TheoraTimer::play();
}

void BASS_AudioInterface::seek(float time)
{
}

BASS_AudioInterfaceFactory::BASS_AudioInterfaceFactory()
{
}

BASS_AudioInterfaceFactory::~BASS_AudioInterfaceFactory()
{
}

BASS_AudioInterface* BASS_AudioInterfaceFactory::createInstance(TheoraVideoClip* owner,int nChannels,int freq)
{
	return new BASS_AudioInterface(owner, nChannels, freq);
}
