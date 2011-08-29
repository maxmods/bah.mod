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

#include <bah.mod/bass.mod/include/bass.h>
#include <string.h>

class MaxSID;

extern "C" {

#include "blitz.h"
#include "sidengine.h"

	int bmx_sid_load(MaxSID * sid, unsigned char * data, int size);
	int bmx_sid_trackcount(MaxSID * sid);
	void bmx_sid_play(MaxSID * sid, int track, bool playing);
	void bmx_sid_stop(MaxSID * sid);
	BBString * bmx_sid_songname(MaxSID * sid);
	BBString * bmx_sid_songauthor(MaxSID * sid);
	BBString * bmx_sid_songcopyright(MaxSID * sid);

}

class MaxSID
{
public:

	MaxSID()
		: init_addr(0), play_addr(0), actual_subsong(0), max_subsong(0), play_speed(0), play_active(0), sid(0)
	{
	}
	
	~MaxSID()
	{
		if (sid) {
			delete sid;
			sid = 0;
		}
	}
	
	virtual void init() {
		if (sid) {
			delete sid;
			sid = 0;
		}
	
		sid = new SID();
		
		sid->c64Init();
		
		init_addr = 0;
		play_addr = 0;
		actual_subsong = 0;
		max_subsong = 0;
		play_speed = 0;
		play_active = 0;
	}
	
	int load(unsigned char * data, int size) {
		init();
		
		int res = sid->c64SidLoad(data, size, &init_addr, &play_addr, &actual_subsong, &max_subsong, &play_speed, song_name, song_author, song_copyright);
		return res;
	}
		
	void stop() {
		sid->cpuJSR(init_addr, actual_subsong);
		play_active = false;
	}
	
	void play(int track, bool playing) {
//		if (actual_subsong != track) {
//		if (playing || !ever_played) {
			actual_subsong = track;
			stop();
//		}
		play_active = true;
		ever_played = true;
	}
	
	int trackcount() {
		return max_subsong + 1;
	}
	
	BBString * songName() {
		return bbStringFromCString(&song_name[0]);
	}

	BBString * songAuthor() {
		return bbStringFromCString(&song_author[0]);
	}

	BBString * songCopyright() {
		return bbStringFromCString(&song_copyright[0]);
	}

	unsigned short init_addr, play_addr;
	unsigned char actual_subsong, max_subsong, play_speed;
	char song_name[32], song_author[32], song_copyright[32];
	int play_active;
	int ever_played;
	
	SID * sid;
	
};


