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

#include "gme/src/gme.h"

class MaxMusicEmu;

extern "C" {
#include "blitz.h"

	MaxMusicEmu * bmx_gme_emu_new();
	void bmx_gme_emu_delete(MaxMusicEmu * emu);

	BBString * bmx_gme_open_file( MaxMusicEmu * emu, BBString * path, long sample_rate);
	BBString * bmx_gme_open_data( MaxMusicEmu * emu, const void * data, long size, long sample_rate);
	BBString * bmx_gme_start_track(MaxMusicEmu * emu, int index);
	BBString * bmx_gme_play(MaxMusicEmu * emu, long count, short * out);
	int bmx_gme_track_count(MaxMusicEmu * emu);
	int bmx_gme_track_ended(MaxMusicEmu * emu);
	long bmx_gme_tell(MaxMusicEmu * emu);
	BBString * bmx_gme_seek(MaxMusicEmu * emu, long msec);
	void bmx_gme_set_fade(MaxMusicEmu * emu, long startMsec);
	void bmx_gme_set_stereo_depth(MaxMusicEmu * emu, double depth);
	void bmx_gme_ignore_silence(MaxMusicEmu * emu, int ignore);
	void bmx_gme_set_tempo(MaxMusicEmu * emu, double tempo);
	void bmx_gme_mute_voice(MaxMusicEmu * emu, int index, int mute);
	void bmx_gme_mute_voices(MaxMusicEmu * emu, int mutingMask);

}

// --------------------------------------------------

class MaxMusicEmu
{
public:

	MaxMusicEmu();
	
	~MaxMusicEmu();
	
	Music_Emu * emu;
};

