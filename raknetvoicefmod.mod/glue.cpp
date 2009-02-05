#include "FMODVoiceAdapter.h"

extern "C" {

	FMODVoiceAdapter * bmx_raknetvoicefmod_adapter_new(FMOD_SYSTEM * system, RakVoice * rakVoice);
	void bmx_raknetvoicefmod_adapter_update(FMODVoiceAdapter * adapter);



}


FMODVoiceAdapter * bmx_raknetvoicefmod_adapter_new(FMOD_SYSTEM * system, RakVoice * rakVoice) {
	FMODVoiceAdapter * adapter = new FMODVoiceAdapter();
	if (!adapter->SetupAdapter(system, rakVoice)) {
		delete adapter;
		return 0;
	}
	return adapter;
}

void bmx_raknetvoicefmod_adapter_update(FMODVoiceAdapter * adapter) {
	adapter->Update();
}

