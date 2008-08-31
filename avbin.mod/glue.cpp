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

#include <stddef.h>

extern "C" {

#include "avbin.h"
#include "blitz.h"

	int bmx_avbin_havefeature(BBString * feature);
	
	AVbinFile * bmx_avbinfile_openfilename(BBString * filename);
	void bmx_avbinfile_closefile(AVbinFile * file);
	AVbinResult bmx_avbinfile_seekfile(AVbinFile * file, AVbinTimestamp timestamp);
	AVbinFileInfo * bmx_avbinfile_fileinfo(AVbinFile * file);
	AVbinResult bmx_avbinfile_read(AVbinFile * file, AVbinPacket *packet);
	AVbinStream * bmx_avbinfile_openstream(AVbinFile * file, int index);
	AVbinResult bmx_avbin_streaminfo(AVbinFile * file, int index, AVbinStreamInfo * info);

	int bmx_avbinfileinfo_getstreamcount(AVbinFileInfo * info);
	void bmx_avbinfileinfo_getstarttime(AVbinFileInfo * info, AVbinTimestamp * value);
	void bmx_avbinfileinfo_getduration(AVbinFileInfo * info, AVbinTimestamp * value);
	void bmx_avbinfileinfo_delete(AVbinFileInfo * info);
	BBString * bmx_avbinfileinfo_gettitle(AVbinFileInfo * info);
	BBString * bmx_avbinfileinfo_getauthor(AVbinFileInfo * info);
	BBString * bmx_avbinfileinfo_getcopyright(AVbinFileInfo * info);
	BBString * bmx_avbinfileinfo_getcomment(AVbinFileInfo * info);
	BBString * bmx_avbinfileinfo_getalbum(AVbinFileInfo * info);
	int bmx_avbinfileinfo_getyear(AVbinFileInfo * info);
	int bmx_avbinfileinfo_gettrack(AVbinFileInfo * info);
	BBString * bmx_avbinfileinfo_getgenre(AVbinFileInfo * info);

	AVbinPacket * bmx_avbinpacket_create();
	void bmx_avbinpacket_delete(AVbinPacket * packet);
	void bmx_avbinpacket_gettimestamp(AVbinPacket * packet, AVbinTimestamp * timestamp);
	int bmx_avbinpacket_getstreamindex(AVbinPacket * packet);
	uint8_t * bmx_avbinpacket_getdata(AVbinPacket * packet);
	size_t bmx_avbinpacket_getsize(AVbinPacket * packet);

	int bmx_avbinstream_decodeaudio(AVbinStream *stream, uint8_t * dataIn, size_t inSize, uint8_t * dataOut, int * outSize);
	int bmx_avbinstream_decodevideo(AVbinStream *stream, uint8_t * dataIn, size_t inSize, uint8_t * dataOut);
	void bmx_avbinstream_close(AVbinStream *stream);

	AVbinStreamInfo * bmx_avbinstreaminfo_create();
	AVbinStreamType bmx_avbinstreaminfo_gettype(AVbinStreamInfo * info);
	unsigned int bmx_avbinstreaminfo_getwidth(AVbinStreamInfo * info);
	unsigned int bmx_avbinstreaminfo_getheight(AVbinStreamInfo * info);
	unsigned int bmx_avbinstreaminfo_getaspectnum(AVbinStreamInfo * info);
	unsigned int bmx_avbinstreaminfo_getaspectden(AVbinStreamInfo * info);
	AVbinSampleFormat bmx_avbinstreaminfo_getaudioformat(AVbinStreamInfo * info);
	unsigned int bmx_avbinstreaminfo_getaudiorate(AVbinStreamInfo * info);
	unsigned int bmx_avbinstreaminfo_getaudiobits(AVbinStreamInfo * info);
	unsigned int bmx_avbinstreaminfo_getaudiochannels(AVbinStreamInfo * info);
	void bmx_avbinstreaminfo_delete(AVbinStreamInfo * info);

}

int bmx_avbin_havefeature(BBString * feature) {
	int ret = 0;
	char *p = bbStringToCString( feature );

	ret = avbin_have_feature(p);

	bbMemFree( p );
	
	return ret;
}


AVbinFile * bmx_avbinfile_openfilename(BBString * filename) {
	char *p = bbStringToCString( filename );

	AVbinFile * file = avbin_open_filename(p);

	bbMemFree( p );

	return file;
}

void bmx_avbinfile_closefile(AVbinFile * file) {
	avbin_close_file(file);
}

AVbinResult bmx_avbinfile_seekfile(AVbinFile * file, AVbinTimestamp timestamp) {
	return avbin_seek_file(file, timestamp);
}

AVbinFileInfo * bmx_avbinfile_fileinfo(AVbinFile * file) {
	AVbinFileInfo *info = new AVbinFileInfo;
	info->structure_size = sizeof(AVbinFileInfo);
	
	AVbinResult res = avbin_file_info(file, info);
	return info;
}

AVbinResult bmx_avbinfile_read(AVbinFile * file, AVbinPacket *packet) {
	return avbin_read(file, packet);
}

AVbinStream * bmx_avbinfile_openstream(AVbinFile * file, int index) {
	return avbin_open_stream(file, index);
}

AVbinResult bmx_avbin_streaminfo(AVbinFile * file, int index, AVbinStreamInfo * info) {
	return avbin_stream_info(file, index, info);
}

// ************************************************

int bmx_avbinfileinfo_getstreamcount(AVbinFileInfo * info) {
	return info->n_streams;
}

void bmx_avbinfileinfo_getstarttime(AVbinFileInfo * info, AVbinTimestamp * value) {
	*value = info->start_time;
}

void bmx_avbinfileinfo_getduration(AVbinFileInfo * info, AVbinTimestamp * value) {
	*value = info->duration;
}

void bmx_avbinfileinfo_delete(AVbinFileInfo * info) {
	delete info;
}

BBString * bmx_avbinfileinfo_gettitle(AVbinFileInfo * info) {
	return bbStringFromCString(info->title);
}

BBString * bmx_avbinfileinfo_getauthor(AVbinFileInfo * info) {
	return bbStringFromCString(info->author);
}

BBString * bmx_avbinfileinfo_getcopyright(AVbinFileInfo * info) {
	return bbStringFromCString(info->copyright);
}

BBString * bmx_avbinfileinfo_getcomment(AVbinFileInfo * info) {
	return bbStringFromCString(info->comment);
}

BBString * bmx_avbinfileinfo_getalbum(AVbinFileInfo * info) {
	return bbStringFromCString(info->album);
}

int bmx_avbinfileinfo_getyear(AVbinFileInfo * info) {
	return info->year;
}

int bmx_avbinfileinfo_gettrack(AVbinFileInfo * info) {
	return info->track;
}

BBString * bmx_avbinfileinfo_getgenre(AVbinFileInfo * info) {
	return bbStringFromCString(info->genre);
}


// ************************************************

AVbinPacket * bmx_avbinpacket_create() {
	AVbinPacket * packet = new AVbinPacket;
	packet->structure_size = sizeof(AVbinPacket);
	return packet;
}

void bmx_avbinpacket_delete(AVbinPacket * packet) {
	delete packet;
}

void bmx_avbinpacket_gettimestamp(AVbinPacket * packet, AVbinTimestamp * timestamp) {
	*timestamp = packet->timestamp;
}

int bmx_avbinpacket_getstreamindex(AVbinPacket * packet) {
	return packet->stream_index;
}

uint8_t * bmx_avbinpacket_getdata(AVbinPacket * packet) {
	return packet->data;
}

size_t bmx_avbinpacket_getsize(AVbinPacket * packet) {
	return packet->size;
}


// ************************************************

int bmx_avbinstream_decodeaudio(AVbinStream *stream, uint8_t * dataIn, size_t inSize, uint8_t * dataOut, int * outSize) {
	return avbin_decode_audio(stream, dataIn, inSize, dataOut, outSize);
}

int bmx_avbinstream_decodevideo(AVbinStream *stream, uint8_t * dataIn, size_t inSize, uint8_t * dataOut) {
	return avbin_decode_video(stream, dataIn, inSize, dataOut);
}

void bmx_avbinstream_close(AVbinStream *stream) {
	avbin_close_stream(stream);
}

// ************************************************

AVbinStreamInfo * bmx_avbinstreaminfo_create() {
	AVbinStreamInfo * info = new AVbinStreamInfo;
	info->structure_size = sizeof(AVbinStreamInfo);
	
	return info;	
}

AVbinStreamType bmx_avbinstreaminfo_gettype(AVbinStreamInfo * info) {
	return info->type;
}

unsigned int bmx_avbinstreaminfo_getwidth(AVbinStreamInfo * info) {
	return info->video.width;
}

unsigned int bmx_avbinstreaminfo_getheight(AVbinStreamInfo * info) {
	return info->video.height;
}

unsigned int bmx_avbinstreaminfo_getaspectnum(AVbinStreamInfo * info) {
	return info->video.sample_aspect_num;
}

unsigned int bmx_avbinstreaminfo_getaspectden(AVbinStreamInfo * info) {
	return info->video.sample_aspect_den;
}

AVbinSampleFormat bmx_avbinstreaminfo_getaudioformat(AVbinStreamInfo * info) {
	return info->audio.sample_format;
}

unsigned int bmx_avbinstreaminfo_getaudiorate(AVbinStreamInfo * info) {
	return info->audio.sample_rate;
}

unsigned int bmx_avbinstreaminfo_getaudiobits(AVbinStreamInfo * info) {
	return info->audio.sample_bits;
}

unsigned int bmx_avbinstreaminfo_getaudiochannels(AVbinStreamInfo * info) {
	return info->audio.channels;
}

void bmx_avbinstreaminfo_delete(AVbinStreamInfo * info) {
	delete info;
}

