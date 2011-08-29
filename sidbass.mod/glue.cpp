/*
  Copyright (c) 2009-2011 Bruce A Henderson
 
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

#include "../sid.mod/glue.h"

class SIDBASS;


extern "C" {

	SIDBASS * bmx_sidbass_new();
	void bmx_sidbass_free(SIDBASS * sid);
	HSTREAM bmx_sidbass_channel_create(DWORD freq, DWORD chans, DWORD flags, SIDBASS * sid);
	DWORD bass_cb(HSTREAM handle, void *buf, DWORD len, void *user);
	
}

class SIDBASS : MaxSID
{
public:
	SIDBASS()
		: nBufferLength(0)
	{
	}
	
	HSTREAM channel_create(DWORD freq, DWORD chans, DWORD flags) {
		stream = BASS_StreamCreate(freq, chans, flags, (STREAMPROC*)&bass_cb, this);
		return stream;
	}
	
	virtual void init() {
		MaxSID::init();
		
		nBufferLength = 0;
	}
	
	void render(word *audioBuffer, dword len) {
		int remaining;
		unsigned int nSamplesPerCall;
		
		remaining = len / sizeof(signed short);
		
		if (!play_active)
		{
			memset(audioBuffer, 0, remaining*sizeof(signed short));
		} else {
	
			while (nBufferLength < remaining)
			{
				sid->cpuJSR(play_addr, 0);
				
				int nRefreshCIA = (int)(20000*(sid->memory[0xdc04]|(sid->memory[0xdc05]<<8))/0x4c00);
				
				if ((nRefreshCIA == 0) || (play_speed == 0)) nRefreshCIA = 20000;
				nSamplesPerCall = 44100*nRefreshCIA/1000000*2;
				
				sid->synth_render((word*)&buffer[nBufferLength], nSamplesPerCall);
				nBufferLength += nSamplesPerCall;
			}
			
			memcpy(audioBuffer, buffer, remaining*sizeof(signed short));
			nBufferLength -= remaining;
			memmove(buffer, &buffer[remaining], nBufferLength*sizeof(signed short));
		}
	}
	

	HSTREAM stream;
	signed short buffer[64000];
	int nBufferLength;

};

SIDBASS * bmx_sidbass_new() {
	return new SIDBASS();
}

void bmx_sidbass_free(SIDBASS * sid) {
	delete sid;
}

HSTREAM bmx_sidbass_channel_create(DWORD freq, DWORD chans, DWORD flags, SIDBASS * sid) {
	return sid->channel_create(freq, chans, flags);
}

DWORD bass_cb(HSTREAM handle, void *buf, DWORD len, void *user) {
	((SIDBASS*)user)->render((word*)buf, len);
	return len;
}


