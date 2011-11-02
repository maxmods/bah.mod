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

#include "TheoraPlayer.h"
#include "TheoraException.h"

extern "C" {

#include "blitz.h"

	BBObject * _bah_theoraplayer_TTheoraGenericException__create(BBString * mErrText, BBString * mFile, BBString * mType);

	TheoraVideoManager * bmx_TheoraVideoManager_new();
	void bmx_TheoraVideoManager_free(TheoraVideoManager * manager);
	TheoraVideoClip * bmx_TheoraVideoManager_createVideoClip(TheoraVideoManager * manager, BBString * filename, int outputMode, int numPrecachedOverride, int usePower2Stride);
	void bmx_TheoraVideoManager_update(TheoraVideoManager * manager, float timeIncrease);


	BBString * bmx_TheoraVideoClip_getName(TheoraVideoClip * clip);
	int bmx_TheoraVideoClip_getWidth(TheoraVideoClip * clip);
	int bmx_TheoraVideoClip_getHeight(TheoraVideoClip * clip);
	int bmx_TheoraVideoClip_getStride(TheoraVideoClip * clip);
	TheoraVideoFrame * bmx_TTheoraVideoClip_getNextFrame(TheoraVideoClip * clip);
	void bmx_TheoraVideoClip_popFrame(TheoraVideoClip * clip);

	void * bmx_TheoraVideoFrame_getBuffer(TheoraVideoFrame * frame);

}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void bmx_theoraplayer_throw_exception(_TheoraGenericException & e) {
	bbExThrow(_bah_theoraplayer_TTheoraGenericException__create(bbStringFromUTF8String(e.mErrText.data()),
		bbStringFromUTF8String(e.mFile.data()),
		bbStringFromUTF8String(e.mType.data())));
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TheoraVideoManager * bmx_TheoraVideoManager_new() {
	return new TheoraVideoManager();
}

void bmx_TheoraVideoManager_free(TheoraVideoManager * manager) {
	delete manager;
}

TheoraVideoClip * bmx_TheoraVideoManager_createVideoClip(TheoraVideoManager * manager, BBString * filename, int outputMode, int numPrecachedOverride, int usePower2Stride) {
	TheoraVideoClip * clip = 0;
	char * f = bbStringToUTF8String(filename);
	
	try {
		clip = manager->createVideoClip(f, (TheoraOutputMode)outputMode, numPrecachedOverride, usePower2Stride);
	} catch (_TheoraGenericException & e) {
		bbMemFree(f);
		bmx_theoraplayer_throw_exception(e);
	}
	
	bbMemFree(f);
	return clip;
}

void bmx_TheoraVideoManager_update(TheoraVideoManager * manager, float timeIncrease) {
	manager->update(timeIncrease);
}

TheoraVideoFrame * bmx_TTheoraVideoClip_getNextFrame(TheoraVideoClip * clip) {
	return clip->getNextFrame();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBString * bmx_TheoraVideoClip_getName(TheoraVideoClip * clip) {
	return bbStringFromUTF8String(clip->getName().data());
}

int bmx_TheoraVideoClip_getWidth(TheoraVideoClip * clip) {
	return clip->getWidth();
}

int bmx_TheoraVideoClip_getHeight(TheoraVideoClip * clip) {
	return clip->getHeight();
}

int bmx_TheoraVideoClip_getStride(TheoraVideoClip * clip) {
	return clip->getStride();
}

void bmx_TheoraVideoClip_popFrame(TheoraVideoClip * clip) {
	clip->popFrame();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void * bmx_TheoraVideoFrame_getBuffer(TheoraVideoFrame * frame) {
	return frame->getBuffer();
}

