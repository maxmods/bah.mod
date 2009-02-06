#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "RakNetworkFactory.h"
#include "FMODVoiceAdapter.h"
#include "fmod_errors.h"


// Number of RakVoice frames in the fmod sound
#define FRAMES_IN_SOUND 4


FMODVoiceAdapter::FMODVoiceAdapter(){
	rakVoice=0;
	fmodSystem = 0;
	recSound=0;
	sound=0;
	channel=0;
	mute=false;
}

bool FMODVoiceAdapter::SetupAdapter(FMOD_SYSTEM *fmodSystem, RakVoice *rakVoice)
{
	FMOD_RESULT fmodErr;

	assert(fmodSystem);
	assert(rakVoice);
	// Make sure rakVoice was initialized
	assert((rakVoice->IsInitialized())&&(rakVoice->GetRakPeerInterface()!=NULL));

	this->fmodSystem = fmodSystem;
	this->rakVoice = rakVoice;
	lastPlayPos = 0;
	lastRecordingPos = 0;

	//
	// Create the FMOD sound used to record
	//
	FMOD_CREATESOUNDEXINFO exinfo;
	memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
	exinfo.cbsize           = sizeof(FMOD_CREATESOUNDEXINFO);
	exinfo.numchannels      = 1;
	exinfo.format           = FMOD_SOUND_FORMAT_PCM16;
	exinfo.defaultfrequency = rakVoice->GetSampleRate();
	exinfo.length			= rakVoice->GetBufferSizeBytes()*FRAMES_IN_SOUND;

	fmodErr = FMOD_System_CreateSound(fmodSystem, 0, FMOD_2D | FMOD_SOFTWARE | FMOD_OPENUSER, &exinfo, &recSound);
	if (fmodErr!=FMOD_OK)
		return false;

	// Create the FMOD sound used to play incoming sound data
	fmodErr = FMOD_System_CreateSound(fmodSystem, 0, FMOD_2D | FMOD_SOFTWARE | FMOD_OPENUSER, &exinfo, &sound);
	if (fmodErr!=FMOD_OK)
		return false;

	// Start playing the sound used for output
	FMOD_Sound_SetMode(sound, FMOD_LOOP_NORMAL);
	fmodErr= FMOD_System_PlaySound(fmodSystem, FMOD_CHANNEL_REUSE, sound, false, &channel);
	if (fmodErr!=FMOD_OK)
		return false;

	// Start recording
	fmodErr=FMOD_System_RecordStart(fmodSystem, recSound, true);
	if (fmodErr!=FMOD_OK)
		return false;

	return true;
}


void FMODVoiceAdapter::Update(void)
{
	assert(fmodSystem);
	UpdateSound(true);
	UpdateSound(false);
}

void FMODVoiceAdapter::Release(void)
{
	FMOD_RESULT err;

	if (fmodSystem==NULL) return;

	// Stop recording
	FMOD_BOOL recording=false;
	err = FMOD_System_IsRecording(fmodSystem, &recording);
	assert(err==FMOD_OK);
	if (recording){
		FMOD_System_RecordStop(fmodSystem);
	}

	// Stop what we hear
	FMOD_BOOL playing;
	err = FMOD_Channel_IsPlaying(channel, &playing);
	assert(err==FMOD_OK);
	if (playing){
		FMOD_Channel_Stop(channel);
	}

	if (recSound!=NULL)
	{
		FMOD_Sound_Release(recSound);
		recSound = NULL;
	}

	if (sound!=NULL)
	{
		FMOD_Sound_Release(sound);
		sound = NULL;
	}
	
}


void FMODVoiceAdapter::SetMute(bool mute)
{
	this->mute = mute;
}


void FMODVoiceAdapter::UpdateSound(bool isRec)
{
	FMOD_RESULT fmodErr;
	unsigned int soundLength;
	const int sampleSize = 2;

	FMOD_SOUND *snd = (isRec) ? recSound : sound;
	unsigned int& lastPos = (isRec) ? lastRecordingPos : lastPlayPos;

	// get current Play or recording position
	unsigned int currPos;
	if (isRec){
		fmodErr=FMOD_System_GetRecordPosition(fmodSystem, &currPos);
		assert(fmodErr==FMOD_OK);
	} else {
		fmodErr=FMOD_Channel_GetPosition(channel, &currPos, FMOD_TIMEUNIT_PCM);
		assert(fmodErr==FMOD_OK);
	}

	// Get length of sound in samples
	fmodErr=FMOD_Sound_GetLength(snd, &soundLength, FMOD_TIMEUNIT_PCM);
	assert(fmodErr==FMOD_OK);

	// calculate some variables we'll need ahead
	int bufferSizeBytes = rakVoice->GetBufferSizeBytes();

	// Round down the current position to a multiple of buffer size in samples
	currPos -= currPos % (bufferSizeBytes/sampleSize);

	if ( ((!isRec)||(isRec && !mute)) && (currPos != lastPos) ) 	
	{
		void *ptr1, *ptr2;
		unsigned int len1, len2;
		int blockLength;
	
		blockLength = (int)currPos - (int)lastPos;
		// Check for wrap around, and adjust
		if (blockLength < 0)
		{
			blockLength += soundLength;
		}

		// Lock to get access to the raw data
		FMOD_Sound_Lock(snd, lastPos * sampleSize, blockLength * sampleSize, &ptr1, &ptr2, &len1, &len2);

		//  Since the length and current position are both a multiple of bufferSizeBytes
		// just treat treat one full buffer at a time
		int numFrames = len1 / bufferSizeBytes;
		while(numFrames--){
			if (isRec) {
				BroadcastFrame(ptr1);
			} else {
				rakVoice->ReceiveFrame(ptr1);
			}
			ptr1 = (char*)ptr1 + bufferSizeBytes;
		}
		numFrames = len2 / bufferSizeBytes;
		while(numFrames--) {
			if (isRec){
				BroadcastFrame(ptr2);
			} else {
				rakVoice->ReceiveFrame(ptr2);
			}
			ptr2 = (char*)ptr2 + bufferSizeBytes;
		}

		FMOD_Sound_Unlock(snd, ptr1, ptr2, len1, len2);
	}

	lastPos = currPos;
}



void FMODVoiceAdapter::BroadcastFrame(void *ptr)
{
	unsigned i;

	unsigned int numPeers = rakVoice->GetRakPeerInterface()->GetMaximumNumberOfPeers();
	for (i=0; i < numPeers; i++)
	{
		rakVoice->SendFrame(rakVoice->GetRakPeerInterface()->GetSystemAddressFromIndex(i), ptr);
	}
}
