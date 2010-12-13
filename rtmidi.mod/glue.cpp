/*
 Copyright (c) 2010 Bruce A Henderson
 
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

#include "RtMidi.h"

extern "C" {
#include "blitz.h"

	BBObject * _bah_rtmidi_TRtError__create(BBString * message, int type);

	RtMidiIn * bmx_rtmidiin_create(BBString * clientName);
	void bmx_rtmidiin_openPort(RtMidiIn * m, int portNumber, BBString * portName);
	void bmx_rtmidiin_closePort(RtMidiIn * m);
	void bmx_rtmidiin_openVirtualPort(RtMidiIn * m, BBString * portName);
	int bmx_rtmidiin_getPortCount(RtMidiIn * m);
	BBString * bmx_rtmidiin_getPortName(RtMidiIn * m, int portNumber);
	void bmx_rtmidiin_free(RtMidiIn * m);
	BBArray * bmx_rtmidiin_getMessage(RtMidiIn * m, double * timestamp);
	void bmx_rtmidiin_setQueueSizeLimit(RtMidiIn * m, int queueSize);
	void bmx_rtmidiin_ignoreTypes(RtMidiIn * m, int midiSysex, int midiTime, int midiSense);

	RtMidiOut * bmx_rtmidiout_create(BBString * clientName);
	void bmx_rtmidiout_openPort(RtMidiOut * m, int portNumber, BBString * portName);
	void bmx_rtmidiout_closePort(RtMidiOut * m);
	void bmx_rtmidiout_openVirtualPort(RtMidiOut * m, BBString * portName);
	int bmx_rtmidiout_getPortCount(RtMidiOut * m);
	BBString * bmx_rtmidiout_getPortName(RtMidiOut * m, int portNumber);
	void bmx_rtmidiout_sendMessage(RtMidiOut * m, unsigned char * message, int length);
	void bmx_rtmidiout_free(RtMidiOut * m);

}

void bmx_rtmidi_throw(RtError &error) {
	bbExThrow(_bah_rtmidi_TRtError__create(bbStringFromCString(error.getMessageString()), (int)error.getType()));
}

RtMidiIn * bmx_rtmidiin_create(BBString * clientName) {
	char * n = bbStringToUTF8String(clientName);
	RtMidiIn * in = 0;
	
	try {
		in = new RtMidiIn(n);
		bbMemFree(n);
	} catch (RtError &error) {
		bbMemFree(n);
		bmx_rtmidi_throw(error);
	}
	
	return in;
}

void bmx_rtmidiin_openPort(RtMidiIn * m, int portNumber, BBString * portName) {
	char * n = bbStringToUTF8String(portName);
	
	try {
		m->openPort(portNumber, n);
		bbMemFree(n);
	} catch (RtError &error) {
		bbMemFree(n);
		bmx_rtmidi_throw(error);
	}	
}

void bmx_rtmidiin_closePort(RtMidiIn * m) {
	try {
		m->closePort();
	} catch (RtError &error) {
		bmx_rtmidi_throw(error);
	}
}

void bmx_rtmidiin_openVirtualPort(RtMidiIn * m, BBString * portName) {
	char * n = bbStringToUTF8String(portName);
	
	try {
		m->openVirtualPort(n);
		bbMemFree(n);
	} catch (RtError &error) {
		bbMemFree(n);
		bmx_rtmidi_throw(error);
	}
}

int bmx_rtmidiin_getPortCount(RtMidiIn * m) {
	unsigned int n = 0;
	try {
		n = m->getPortCount();
	} catch (RtError &error) {
		bmx_rtmidi_throw(error);
	}
	return n;
}

BBString * bmx_rtmidiin_getPortName(RtMidiIn * m, int portNumber) {
	try {
		std::string s(m->getPortName(portNumber));
		return bbStringFromUTF8String(s.c_str());
	} catch (RtError &error) {
		bmx_rtmidi_throw(error);
	}
}

void bmx_rtmidiin_free(RtMidiIn * m) {
	try {
		delete m;
	} catch (RtError &error) {
		bmx_rtmidi_throw(error);
	}
}

BBArray * bmx_rtmidiin_getMessage(RtMidiIn * m, double * timestamp) {
	BBArray * arr = &bbEmptyArray;
	std::vector<unsigned char> message;
	int n;
	
	try {
		*timestamp = m->getMessage(&message);
	} catch (RtError &error) {
		bmx_rtmidi_throw(error);
	}
	
	n = message.size();
	
	if (n) {
		arr = bbArrayNew1D("b", n);
		unsigned char *c = (unsigned char*)BBARRAYDATA(arr, arr->dims);
		for(int i=0; i < n; ++i){
			c[i] = message[i];
		}
	}

	return arr;
}

void bmx_rtmidiin_setQueueSizeLimit(RtMidiIn * m, int queueSize) {
	try {
		m->setQueueSizeLimit(queueSize);
	} catch (RtError &error) {
		bmx_rtmidi_throw(error);
	}
}

void bmx_rtmidiin_ignoreTypes(RtMidiIn * m, int midiSysex, int midiTime, int midiSense) {
	try {
		m->ignoreTypes(midiSysex, midiTime, midiSense);
	} catch (RtError &error) {
		bmx_rtmidi_throw(error);
	}
}

// --------------------------------------------------------

RtMidiOut * bmx_rtmidiout_create(BBString * clientName) {
	char * n = bbStringToUTF8String(clientName);
	RtMidiOut * out = 0;
	
	try {
		out = new RtMidiOut(n);
		bbMemFree(n);
	} catch (RtError &error) {
		bbMemFree(n);
		bmx_rtmidi_throw(error);
	}
	
	return out;
}

void bmx_rtmidiout_openPort(RtMidiOut * m, int portNumber, BBString * portName) {
	char * n = bbStringToUTF8String(portName);
	
	try {
		m->openPort(portNumber, n);
		bbMemFree(n);
	} catch (RtError &error) {
		bbMemFree(n);
		bmx_rtmidi_throw(error);
	}	
}

void bmx_rtmidiout_closePort(RtMidiOut * m) {
	try {
		m->closePort();
	} catch (RtError &error) {
		bmx_rtmidi_throw(error);
	}
}

void bmx_rtmidiout_openVirtualPort(RtMidiOut * m, BBString * portName) {
	char * n = bbStringToUTF8String(portName);
	
	try {
		m->openVirtualPort(n);
		bbMemFree(n);
	} catch (RtError &error) {
		bbMemFree(n);
		bmx_rtmidi_throw(error);
	}
}

int bmx_rtmidiout_getPortCount(RtMidiOut * m) {
	unsigned int n = 0;
	try {
		n = m->getPortCount();
	} catch (RtError &error) {
		bmx_rtmidi_throw(error);
	}
	return n;
}

BBString * bmx_rtmidiout_getPortName(RtMidiOut * m, int portNumber) {
	try {
		std::string s(m->getPortName(portNumber));
		return bbStringFromUTF8String(s.c_str());
	} catch (RtError &error) {
		bmx_rtmidi_throw(error);
	}
}

void bmx_rtmidiout_sendMessage(RtMidiOut * m, unsigned char * message, int length) {
	try {
		std::vector<unsigned char> mess;
		mess.insert(mess.end(),message,message+length);
		
		m->sendMessage(&mess);
		
	} catch (RtError &error) {
		bmx_rtmidi_throw(error);
	}

}

void bmx_rtmidiout_free(RtMidiOut * m) {
	try {
		delete m;
	} catch (RtError &error) {
		bmx_rtmidi_throw(error);
	}
}

