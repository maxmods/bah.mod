/*
  Copyright (c) 2008-2015 Bruce A Henderson
 
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

#include "bass.h"

class MaxSyncData;

extern "C" {

#ifdef BMX_NG
#define CB_PREF(func) func
#else
#define CB_PREF(func) _##func
#endif

#include "blitz.h"

	void CB_PREF(bah_bass_TBassStream__fileCloseProc)(void * obj);
	void CB_PREF(bah_bass_TBassStream__fileLenProc)(void * obj, QWORD * len);
	DWORD CB_PREF(bah_bass_TBassStream__fileReadProc)(void * buffer, DWORD length, void * obj);
	BOOL CB_PREF(bah_bass_TBassStream__fileSeekProc)(QWORD offset, void * obj);
	void CB_PREF(bah_bass_TBassStream__dlstreamcallback)(const void * buffer, DWORD length, void * user);


	DWORD bmx_bassinfo_getflags(BASS_INFO * info);
	DWORD bmx_bassinfo_gethwsize(BASS_INFO * info);
	DWORD bmx_bassinfo_gethwfree(BASS_INFO * info);
	DWORD bmx_bassinfo_getfreesam(BASS_INFO * info);
	DWORD bmx_bassinfo_getfree3d(BASS_INFO * info);
	DWORD bmx_bassinfo_getminrate(BASS_INFO * info);
	DWORD bmx_bassinfo_getmaxrate(BASS_INFO * info);
	BOOL bmx_bassinfo_geteax(BASS_INFO * info);
	DWORD bmx_bassinfo_getminbuf(BASS_INFO * info);
	DWORD bmx_bassinfo_getdsver(BASS_INFO * info);
	DWORD bmx_bassinfo_getlatency(BASS_INFO * info);
	DWORD bmx_bassinfo_getinitflags(BASS_INFO * info);
	DWORD bmx_bassinfo_getspeakers(BASS_INFO * info);
	DWORD bmx_bassinfo_getfreq(BASS_INFO * info);
	void bmx_bassinfo_delete(BASS_INFO * info);

	void bmx_bass_channelgetlength(DWORD handle, QWORD * length, DWORD mode);
	DWORD bmx_bass_channelgetlevel(DWORD handle, DWORD * left, DWORD * right);
	void bmx_bass_channelgetposition(DWORD handle, QWORD * pos, DWORD mode);
	void bmx_bass_channelgetpositionlowhigh(DWORD handle, QWORD * pos, DWORD mode, DWORD * low, DWORD * high);
	void bmx_bass_channelseconds2bytes(DWORD handle, QWORD * bytes, double pos);
	BBArray * bmx_bass_channelgettags(DWORD handle, DWORD tags);

	//HSTREAM bmx_bass_streamcreatefile(filename:Byte Ptr, offset:Int, length:Int, flags:Int)
	void bmx_bass_streamgetfileposition(DWORD handle, QWORD * pos, DWORD mode);
	DWORD bmx_bass_streamcreatetstream(void * handle, DWORD system, DWORD flags);
	HSTREAM bmx_bass_streamcreateurlncb( char *url, DWORD offset, DWORD flags, void *user);
	HSTREAM bmx_bass_streamcreateurl( char *url, DWORD offset, DWORD flags, void *user);

	BASS_SAMPLE * bmx_bass_getsampleinfo(HSAMPLE handle);
	BOOL bmx_bass_setsampleinfo(HSAMPLE handle, BASS_SAMPLE * info);

	DWORD bmx_sampleinfo_getfreq(BASS_SAMPLE * info);
	float bmx_sampleinfo_getvolume(BASS_SAMPLE * info);
	float bmx_sampleinfo_getpan(BASS_SAMPLE * info);
	DWORD bmx_sampleinfo_getflags(BASS_SAMPLE * info);
	DWORD bmx_sampleinfo_getlength(BASS_SAMPLE * info);
	DWORD bmx_sampleinfo_getmax(BASS_SAMPLE * info);
	DWORD bmx_sampleinfo_getorigres(BASS_SAMPLE * info);
	DWORD bmx_sampleinfo_getchans(BASS_SAMPLE * info);
	DWORD bmx_sampleinfo_getmingap(BASS_SAMPLE * info);
	DWORD bmx_sampleinfo_getmode3d(BASS_SAMPLE * info);
	float bmx_sampleinfo_getmindist(BASS_SAMPLE * info);
	float bmx_sampleinfo_getmaxdist(BASS_SAMPLE * info);
	DWORD bmx_sampleinfo_getiangle(BASS_SAMPLE * info);
	DWORD bmx_sampleinfo_getoangle(BASS_SAMPLE * info);
	float bmx_sampleinfo_getoutvol(BASS_SAMPLE * info);
	DWORD bmx_sampleinfo_getvam(BASS_SAMPLE * info);
	DWORD bmx_sampleinfo_getpriority(BASS_SAMPLE * info);
	void bmx_sampleinfo_delete(BASS_SAMPLE * info);
	void bmx_sampleinfo_setfreq(BASS_SAMPLE * info, DWORD value);
	void bmx_sampleinfo_setvolume(BASS_SAMPLE * info, float value);
	void bmx_sampleinfo_setpan(BASS_SAMPLE * info, float value);
	void bmx_sampleinfo_setflags(BASS_SAMPLE * info, DWORD value);
	void bmx_sampleinfo_setmingap(BASS_SAMPLE * info, DWORD value);
	void bmx_sampleinfo_setmode3d(BASS_SAMPLE * info, DWORD value);
	void bmx_sampleinfo_setmindist(BASS_SAMPLE * info, float value);
	void bmx_sampleinfo_setmaxdist(BASS_SAMPLE * info, float value);
	void bmx_sampleinfo_setiangle(BASS_SAMPLE * info, DWORD value);
	void bmx_sampleinfo_setoangle(BASS_SAMPLE * info, DWORD value);
	void bmx_sampleinfo_setoutvol(BASS_SAMPLE * info, float value);
	void bmx_sampleinfo_setvam(BASS_SAMPLE * info, DWORD value);
	void bmx_sampleinfo_setpriority(BASS_SAMPLE * info, DWORD value);

	BASS_DEVICEINFO * bmx_bass_getdeviceinfo(int device);
	const char * bmx_deviceinfo_getname(BASS_DEVICEINFO * info);
	const char * bmx_deviceinfo_getdriver(BASS_DEVICEINFO * info);
	DWORD bmx_deviceinfo_getflags(BASS_DEVICEINFO * info);
	void bmx_deviceinfo_delete(BASS_DEVICEINFO * info);

	BASS_RECORDINFO * bmx_bass_recordgetinfo();
	DWORD bmx_recordinfo_getflags(BASS_RECORDINFO * info);
	DWORD bmx_recordinfo_getformats(BASS_RECORDINFO * info);
	DWORD bmx_recordinfo_getinputs(BASS_RECORDINFO * info);
	BOOL bmx_recordinfo_issinglein(BASS_RECORDINFO * info);
	DWORD bmx_recordinfo_getfreq(BASS_RECORDINFO * info);
	void bmx_recordinfo_delete(BASS_RECORDINFO * info);

	void bmx_bass_fxdelete(void * fx);

	BASS_3DVECTOR * bmx_bass_3dvector_new();
	void bmx_bass_3dvector_setxyz(BASS_3DVECTOR * vec, float x, float y, float z);
	void bmx_bass_3dvector_delete(BASS_3DVECTOR * vec);
	float bmx_bass_3dvector_getx(BASS_3DVECTOR * vec);
	float bmx_bass_3dvector_gety(BASS_3DVECTOR * vec);
	float bmx_bass_3dvector_getz(BASS_3DVECTOR * vec);
	void bmx_bass_3dvector_setx(BASS_3DVECTOR * vec, float value);
	void bmx_bass_3dvector_sety(BASS_3DVECTOR * vec, float value);
	void bmx_bass_3dvector_setz(BASS_3DVECTOR * vec, float value);
	void bmx_bass_3dvector_getxyz(BASS_3DVECTOR * vec, float * x, float * y, float * z);

	BASS_CHANNELINFO * bmx_bass_getchannelinfo(DWORD handle);
	DWORD bmx_channelinfo_getfreq(BASS_CHANNELINFO * info);
	DWORD bmx_channelinfo_getchannels(BASS_CHANNELINFO * info);
	DWORD bmx_channelinfo_getflags(BASS_CHANNELINFO * info);
	DWORD bmx_channelinfo_getctype(BASS_CHANNELINFO * info);
	DWORD bmx_channelinfo_getorigres(BASS_CHANNELINFO * info);
	HSAMPLE bmx_channelinfo_getsample(BASS_CHANNELINFO * info);
	const char * bmx_channelinfo_getfilename(BASS_CHANNELINFO * info);
	void bmx_channelinfo_delete(BASS_CHANNELINFO * info);

	BASS_INFO * bmx_bass_getinfo();

	void bmx_bass_setsync(DWORD handle, DWORD stype, QWORD param, MaxSyncData * syncData, DWORD * channel, DWORD * data, int * set);
	MaxSyncData * bmx_bass_syncdata_new();

}

class MaxSyncData
{
public:
	HSYNC handle;
	DWORD * channel;
	DWORD * data;
	int * set;
private:
};

DWORD bmx_bassinfo_getflags(BASS_INFO * info) {
    return info->flags;
}

DWORD bmx_bassinfo_gethwsize(BASS_INFO * info) {
    return info->hwsize;
}

DWORD bmx_bassinfo_gethwfree(BASS_INFO * info) {
    return info->hwfree;
}

DWORD bmx_bassinfo_getfreesam(BASS_INFO * info) {
    return info->freesam;
}

DWORD bmx_bassinfo_getfree3d(BASS_INFO * info) {
    return info->free3d;
}

DWORD bmx_bassinfo_getminrate(BASS_INFO * info) {
    return info->minrate;
}

DWORD bmx_bassinfo_getmaxrate(BASS_INFO * info) {
    return info->maxrate;
}

BOOL bmx_bassinfo_geteax(BASS_INFO * info) {
    return info->eax;
}

DWORD bmx_bassinfo_getminbuf(BASS_INFO * info) {
    return info->minbuf;
}

DWORD bmx_bassinfo_getdsver(BASS_INFO * info) {
    return info->dsver;
}

DWORD bmx_bassinfo_getlatency(BASS_INFO * info) {
    return info->latency;
}

DWORD bmx_bassinfo_getinitflags(BASS_INFO * info) {
    return info->initflags;
}

DWORD bmx_bassinfo_getspeakers(BASS_INFO * info) {
    return info->speakers;
}

DWORD bmx_bassinfo_getfreq(BASS_INFO * info) {
    return info->freq;
}

void bmx_bassinfo_delete(BASS_INFO * info) {
    delete info;
}

// *************************************************

void bmx_bass_channelgetlength(DWORD handle, QWORD * length, DWORD mode) {
	*length = BASS_ChannelGetLength(handle, mode);
}

DWORD bmx_bass_channelgetlevel(DWORD handle, DWORD * left, DWORD * right) {
	DWORD level = BASS_ChannelGetLevel(handle);
	*left = LOWORD(level);
	*right = HIWORD(level);
	return level;
}

void bmx_bass_channelgetposition(DWORD handle, QWORD * pos, DWORD mode) {
	*pos = BASS_ChannelGetPosition(handle, mode);
}

void bmx_bass_channelgetpositionlowhigh(DWORD handle, QWORD * pos, DWORD mode, DWORD * low, DWORD * high) {
	*pos = BASS_ChannelGetPosition(handle, mode);
	*low = LOWORD(*pos);
	*high = HIWORD(*pos);
}

void bmx_bass_channelseconds2bytes(DWORD handle, QWORD * bytes, double pos) {
	*bytes = BASS_ChannelSeconds2Bytes(handle, pos);
}

BBArray * bmx_bass_channelgettags(DWORD handle, DWORD tags) {
	const char * text;
	TAG_ID3 *id3;
	
	switch (tags) {
		case BASS_TAG_ID3:
			id3 = (TAG_ID3*) BASS_ChannelGetTags(handle, BASS_TAG_ID3); // get the ID3 tags
			if (id3) {
				
				char buffer[4];
				
				BBArray * p = bbArrayNew1D("$", 7);
				BBString **s = (BBString**)BBARRAYDATA( p,p->dims );
							
				sprintf(buffer, "%.3s", id3->id);
				s[0] = bbStringFromCString(buffer);
				BBRETAIN( s[0] );
				
				s[1] = bbStringFromCString(id3->title);
				BBRETAIN( s[1] );
				
				s[2] = bbStringFromCString(id3->artist);
				BBRETAIN( s[2] );
				
				s[3] = bbStringFromCString(id3->album);
				BBRETAIN( s[3] );
	
				sprintf(buffer, "%.4s", id3->year);
				s[4] = bbStringFromCString(buffer);
				BBRETAIN( s[4] );
				
				s[5] = bbStringFromCString(id3->comment);
				BBRETAIN( s[5] );
							
				sprintf(buffer, "%d", id3->genre);
				s[6] = bbStringFromCString(buffer);
				BBRETAIN( s[6] );
				
				return p;
			} else {
				return &bbEmptyArray;
			}
		case BASS_TAG_META:
		case BASS_TAG_LYRICS3:
		case BASS_TAG_VENDOR:
		case BASS_TAG_MUSIC_NAME:
		case BASS_TAG_MUSIC_MESSAGE:
			text = BASS_ChannelGetTags(handle, tags);

			if (text) {
				BBArray * p = bbArrayNew1D("$", 1);
				BBString **s = (BBString**)BBARRAYDATA( p,p->dims );
				
				s[0] = bbStringFromCString( text );
				BBRETAIN( s[0] );

				return p;
			} else {
				return &bbEmptyArray;
			}
		
		default:
			text = BASS_ChannelGetTags(handle, tags);
			
			if (text) {
				int count = 0;
				const char * current = text;
				
				while (*current) {
					current += strlen(current) + 1;
					count++;
				}
				
				BBArray * p = bbArrayNew1D("$", count);
				BBString **s = (BBString**)BBARRAYDATA( p,p->dims );
				
				count = 0;
				current = text;
				while (*current) {
					s[count] = bbStringFromCString( current );
					BBRETAIN( s[count] );
		
					current += strlen(current) + 1;
					count++;
				}
				
				return p;
			
			} else {
				return &bbEmptyArray;
			}
	}
}

// *************************************************

void bmx_bass_streamgetfileposition(DWORD handle, QWORD * pos, DWORD mode) {
	*pos = BASS_StreamGetFilePosition(handle, mode);
}

QWORD CALLBACK bmx_bass_fileLenProc(void *user) {
	QWORD len;
	CB_PREF(bah_bass_TBassStream__fileLenProc)(user, &len);
	return len;
}

void CALLBACK bmx_bass_fileCloseProc(void * obj) {
	CB_PREF(bah_bass_TBassStream__fileCloseProc)(obj);
}

DWORD CALLBACK bmx_bass_fileReadProc(void * buffer, DWORD length, void * obj) {
	return CB_PREF(bah_bass_TBassStream__fileReadProc)(buffer, length, obj);
}

BOOL CALLBACK bmx_bass_fileSeekProc(QWORD offset, void * obj) {
	return CB_PREF(bah_bass_TBassStream__fileSeekProc)(offset, obj);
}

DWORD bmx_bass_streamcreatetstream(void * handle, DWORD system, DWORD flags) {
	BASS_FILEPROCS fileprocs = {bmx_bass_fileCloseProc,
		bmx_bass_fileLenProc,
		bmx_bass_fileReadProc,
		bmx_bass_fileSeekProc};
	
	return BASS_StreamCreateFileUser(system, flags, &fileprocs, handle);
}

HSTREAM bmx_bass_streamcreateurlncb( char *url, DWORD offset, DWORD flags, void *user) {
	return BASS_StreamCreateURL(url, offset, flags, NULL, user);
}

void CALLBACK bmx_bass_dlstreamcallback(const void *buffer, DWORD length, void *user) {
	CB_PREF(bah_bass_TBassStream__dlstreamcallback)(buffer, length, user);
};

HSTREAM bmx_bass_streamcreateurl( char *url, DWORD offset, DWORD flags, void *user) {
	return BASS_StreamCreateURL(url, offset, flags, &bmx_bass_dlstreamcallback, user);
}

// *************************************************

BASS_SAMPLE * bmx_bass_getsampleinfo(HSAMPLE handle) {
	BASS_SAMPLE * info = new BASS_SAMPLE;
	BOOL success = BASS_SampleGetInfo(handle, info);
	if (!success) {
		delete info;
		return 0;
	}
	return info;
}

BOOL bmx_bass_setsampleinfo(HSAMPLE handle, BASS_SAMPLE * info) {
	return BASS_SampleSetInfo(handle, info);
}

DWORD bmx_sampleinfo_getfreq(BASS_SAMPLE * info) {
    return info->freq;
}

float bmx_sampleinfo_getvolume(BASS_SAMPLE * info) {
    return info->volume;
}

float bmx_sampleinfo_getpan(BASS_SAMPLE * info) {
    return info->pan;
}

DWORD bmx_sampleinfo_getflags(BASS_SAMPLE * info) {
    return info->flags;
}

DWORD bmx_sampleinfo_getlength(BASS_SAMPLE * info) {
    return info->length;
}

DWORD bmx_sampleinfo_getmax(BASS_SAMPLE * info) {
    return info->max;
}

DWORD bmx_sampleinfo_getorigres(BASS_SAMPLE * info) {
    return info->origres;
}

DWORD bmx_sampleinfo_getchans(BASS_SAMPLE * info) {
    return info->chans;
}

DWORD bmx_sampleinfo_getmingap(BASS_SAMPLE * info) {
    return info->mingap;
}

DWORD bmx_sampleinfo_getmode3d(BASS_SAMPLE * info) {
    return info->mode3d;
}

float bmx_sampleinfo_getmindist(BASS_SAMPLE * info) {
    return info->mindist;
}

float bmx_sampleinfo_getmaxdist(BASS_SAMPLE * info) {
    return info->maxdist;
}

DWORD bmx_sampleinfo_getiangle(BASS_SAMPLE * info) {
    return info->iangle;
}

DWORD bmx_sampleinfo_getoangle(BASS_SAMPLE * info) {
    return info->oangle;
}

float bmx_sampleinfo_getoutvol(BASS_SAMPLE * info) {
    return info->outvol;
}

DWORD bmx_sampleinfo_getvam(BASS_SAMPLE * info) {
    return info->vam;
}

DWORD bmx_sampleinfo_getpriority(BASS_SAMPLE * info) {
    return info->priority;
}

void bmx_sampleinfo_delete(BASS_SAMPLE * info) {
	delete info;
}

void bmx_sampleinfo_setfreq(BASS_SAMPLE * info, DWORD value) {
	info->freq = value;
}

void bmx_sampleinfo_setvolume(BASS_SAMPLE * info, float value) {
	info->volume = value;
}

void bmx_sampleinfo_setpan(BASS_SAMPLE * info, float value) {
	info->pan = value;
}

void bmx_sampleinfo_setflags(BASS_SAMPLE * info, DWORD value) {
	info->flags = value;
}

void bmx_sampleinfo_setmingap(BASS_SAMPLE * info, DWORD value) {
	info->mingap = value;
}

void bmx_sampleinfo_setmode3d(BASS_SAMPLE * info, DWORD value) {
	info->mode3d = value;
}

void bmx_sampleinfo_setmindist(BASS_SAMPLE * info, float value) {
	info->mindist = value;
}

void bmx_sampleinfo_setmaxdist(BASS_SAMPLE * info, float value) {
	info->maxdist = value;
}

void bmx_sampleinfo_setiangle(BASS_SAMPLE * info, DWORD value) {
	info->iangle = value;
}

void bmx_sampleinfo_setoangle(BASS_SAMPLE * info, DWORD value) {
	info->oangle = value;
}

void bmx_sampleinfo_setoutvol(BASS_SAMPLE * info, float value) {
	info->outvol = value;
}

void bmx_sampleinfo_setvam(BASS_SAMPLE * info, DWORD value) {
	info->vam = value;
}

void bmx_sampleinfo_setpriority(BASS_SAMPLE * info, DWORD value) {
	info->priority = value;
}


// *************************************************

const char * bmx_deviceinfo_getname(BASS_DEVICEINFO * info) {
	return info->name;
}

const char * bmx_deviceinfo_getdriver(BASS_DEVICEINFO * info) {
	return info->driver;
}

DWORD bmx_deviceinfo_getflags(BASS_DEVICEINFO * info) {
	return info->flags;
}

void bmx_deviceinfo_delete(BASS_DEVICEINFO * info) {
	delete info;
}

BASS_DEVICEINFO * bmx_bass_getdeviceinfo(int device) {
	BASS_DEVICEINFO * info = new BASS_DEVICEINFO;
	BOOL success = BASS_GetDeviceInfo(device, info);
	if (!success || (!info->flags & BASS_DEVICE_ENABLED)) {
		delete info;
		return 0;
	}
	return info;
}

// *************************************************

BASS_RECORDINFO * bmx_bass_recordgetinfo() {
	BASS_RECORDINFO * info = new BASS_RECORDINFO;
	BOOL success = BASS_RecordGetInfo(info);
	if (!success) {
		delete info;
		return 0;
	}
	return info;
}

DWORD bmx_recordinfo_getflags(BASS_RECORDINFO * info) {
	return info->flags;
}

DWORD bmx_recordinfo_getformats(BASS_RECORDINFO * info) {
	return info->formats;
}

DWORD bmx_recordinfo_getinputs(BASS_RECORDINFO * info) {
	return info->inputs;
}

BOOL bmx_recordinfo_issinglein(BASS_RECORDINFO * info) {
	return info->singlein;
}

DWORD bmx_recordinfo_getfreq(BASS_RECORDINFO * info) {
	return info->freq;
}

void bmx_recordinfo_delete(BASS_RECORDINFO * info) {
	delete info;
}

// *************************************************

void bmx_bass_fxdelete(void * fx) {
	delete fx;
}

// *************************************************

BASS_3DVECTOR * bmx_bass_3dvector_new() {
	return new BASS_3DVECTOR;
}

void bmx_bass_3dvector_setxyz(BASS_3DVECTOR * vec, float x, float y, float z) {
	vec->x = x;
	vec->y = y;
	vec->z = z;
}

void bmx_bass_3dvector_delete(BASS_3DVECTOR * vec) {
	delete vec;
}

float bmx_bass_3dvector_getx(BASS_3DVECTOR * vec) {
	return vec->x;
}

float bmx_bass_3dvector_gety(BASS_3DVECTOR * vec) {
	return vec->y;
}

float bmx_bass_3dvector_getz(BASS_3DVECTOR * vec) {
	return vec->z;
}

void bmx_bass_3dvector_setx(BASS_3DVECTOR * vec, float value) {
	vec->x = value;
}

void bmx_bass_3dvector_sety(BASS_3DVECTOR * vec, float value) {
	vec->y = value;
}

void bmx_bass_3dvector_setz(BASS_3DVECTOR * vec, float value) {
	vec->z = value;
}

void bmx_bass_3dvector_getxyz(BASS_3DVECTOR * vec, float * x, float * y, float * z) {
	*x = vec->x;
	*y = vec->y;
	*z = vec->z;
}

// *************************************************

BASS_CHANNELINFO * bmx_bass_getchannelinfo(DWORD handle) {
	BASS_CHANNELINFO * info = new BASS_CHANNELINFO;
	BOOL success = BASS_ChannelGetInfo(handle, info);
	if (!success) {
		delete info;
		return 0;
	}
	return info;
}

DWORD bmx_channelinfo_getfreq(BASS_CHANNELINFO * info) {
	return info->freq;
}

DWORD bmx_channelinfo_getchannels(BASS_CHANNELINFO * info) {
	return info->chans;
}

DWORD bmx_channelinfo_getflags(BASS_CHANNELINFO * info) {
	return info->flags;
}

DWORD bmx_channelinfo_getctype(BASS_CHANNELINFO * info) {
	return info->ctype;
}

DWORD bmx_channelinfo_getorigres(BASS_CHANNELINFO * info) {
	return info->origres;
}

HSAMPLE bmx_channelinfo_getsample(BASS_CHANNELINFO * info) {
	return info->sample;
}

const char * bmx_channelinfo_getfilename(BASS_CHANNELINFO * info) {
	return info->filename;
}

void bmx_channelinfo_delete(BASS_CHANNELINFO * info) {
	delete info;
}

// *************************************************

BASS_INFO * bmx_bass_getinfo() {
	BASS_INFO * info = new BASS_INFO;
	BOOL success = BASS_GetInfo(info);
	if (!success) {
		delete info;
		return 0;
	}
	return info;
}

// *************************************************

void CALLBACK bmx_sync_callback(HSYNC handle, DWORD channel, DWORD data, void *user) {
	MaxSyncData * sync = (MaxSyncData*)user;
	*sync->channel = channel;
	*sync->data = data;
	*sync->set = 1;
}

void bmx_bass_setsync(DWORD handle, DWORD stype, QWORD param, MaxSyncData * syncData, DWORD * channel, DWORD * data, int * set) {
	syncData->channel = channel;
	syncData->data = data;
	syncData->set = set;
	syncData->handle = BASS_ChannelSetSync(handle, stype, param, bmx_sync_callback, syncData);
}

MaxSyncData * bmx_bass_syncdata_new() {
	return new MaxSyncData();
}

