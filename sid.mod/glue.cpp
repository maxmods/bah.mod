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

#include "glue.h"

int bmx_sid_load(MaxSID * sid, unsigned char * data, int size) {
	int ret = sid->load(data, size);
	return ret;
}

int bmx_sid_trackcount(MaxSID * sid) {
	return sid->trackcount();
}

void bmx_sid_play(MaxSID * sid, int track, bool playing) {
	sid->play(track, playing);
}

void bmx_sid_stop(MaxSID * sid) {
	sid->stop();
}

BBString * bmx_sid_songname(MaxSID * sid) {
	return sid->songName();
}

BBString * bmx_sid_songauthor(MaxSID * sid) {
	return sid->songAuthor();
}

BBString * bmx_sid_songcopyright(MaxSID * sid) {
	return sid->songCopyright();
}


