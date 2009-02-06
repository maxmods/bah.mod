#ifndef __FMODVOICEBRIDGE_H
#define __FMODVOICEBRIDGE_H

#include "RakVoice.h"

#include "fmod.h"

class FMODVoiceAdapter {

public:

	// --------------------------------------------------------------------------------------------
	// User functions
	// --------------------------------------------------------------------------------------------

	FMODVoiceAdapter();
	~FMODVoiceAdapter() {};

	bool SetupAdapter(FMOD_SYSTEM *fmodSystem, RakVoice *rakVoice);

	/// Release any resources used.
	void Release();

	/// You need to call this once in a while, depending on the parameters used. Ex: call once every 20-30 milliseconds
	void Update();

	/// Turns on/off outgoing traffic
	/// \param[in] true to mute, false to allow outgoing traffic.
	void SetMute(bool mute);

private:

	void UpdateSound(bool isRec);
	void BroadcastFrame(void *ptr);

	RakVoice *rakVoice;
	FMOD_SYSTEM *fmodSystem;
	FMOD_SOUND *recSound; // sound used for recording
	FMOD_SOUND *sound; // sound used to play what we hear
	FMOD_CHANNEL *channel;
	bool mute;
	unsigned int lastPlayPos;
	unsigned int lastRecordingPos;
};

#endif
