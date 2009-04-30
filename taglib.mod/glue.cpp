/*
   Copyright (c) 2009 Bruce A Henderson
 
   The contents of this file are subject to the Mozilla Public License
   Version 1.1 (the "License"); you may not use this file except in
   compliance with the License. You may obtain a copy of the License at
   http://www.mozilla.org/MPL/
   
   Software distributed under the License is distributed on an "AS IS"
   basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
   License for the specific language governing rights and limitations
   under the License.
   
   The Original Code is BaH.TagLib.
   
   The Initial Developer of the Original Code is Bruce A Henderson.
*/
#include <fileref.h>
#include <tag.h>
#include <mpegheader.h>
#include <mpegfile.h>

extern "C" {

#include "blitz.h"

	BBString * bah_taglib_stringFromUTF8String(const char *);
	BBString * bah_taglib_stringToUTF8String(BBString *);


	TagLib::FileRef * bmx_taglib_fileref_create(BBString * filename, bool readAudioProperties, TagLib::AudioProperties::ReadStyle audioPropertiesStyle);
	TagLib::Tag * bmx_taglib_fileref_tag(TagLib::FileRef * ref);
	TagLib::AudioProperties * bmx_taglib_fileref_audioproperties(TagLib::FileRef * ref);
	void bmx_taglib_fileref_delete(TagLib::FileRef * ref);
	bool bmx_taglib_fileref_save(TagLib::FileRef * ref);
	bool bmx_taglib_fileref_isnull(TagLib::FileRef * ref);

	BBString * bmx_taglib_tag_title(TagLib::Tag * tag);
	BBString * bmx_taglib_tag_artist(TagLib::Tag * tag);
	BBString * bmx_taglib_tag_album(TagLib::Tag * tag);
	BBString * bmx_taglib_tag_comment(TagLib::Tag * tag);
	BBString * bmx_taglib_tag_genre(TagLib::Tag * tag);
	TagLib::uint bmx_taglib_tag_year(TagLib::Tag * tag);
	TagLib::uint bmx_taglib_tag_track(TagLib::Tag * tag);
	void bmx_taglib_tag_settitle(TagLib::Tag * tag, BBString * value);
	void bmx_taglib_tag_setartist(TagLib::Tag * tag, BBString * value);
	void bmx_taglib_tag_setalbum(TagLib::Tag * tag, BBString * value);
	void bmx_taglib_tag_setcomment(TagLib::Tag * tag, BBString * value);
	void bmx_taglib_tag_setgenre(TagLib::Tag * tag, BBString * value);
	void bmx_taglib_tag_setyear(TagLib::Tag * tag, TagLib::uint value);
	void bmx_taglib_tag_settrack(TagLib::Tag * tag, TagLib::uint value);
	bool bmx_taglib_tag_isempty(TagLib::Tag * tag);

	int bmx_taglib_audoproperties_length(TagLib::AudioProperties * prop);
	int bmx_taglib_audoproperties_bitrate(TagLib::AudioProperties * prop);
	int bmx_taglib_audoproperties_samplerate(TagLib::AudioProperties * prop);
	int bmx_taglib_audoproperties_channels(TagLib::AudioProperties * prop);

	TagLib::MPEG::Header::Version bmx_taglib_mpegproperties_version(TagLib::MPEG::Header * header);
	int bmx_taglib_mpegproperties_layer(TagLib::MPEG::Header * header);
	bool bmx_taglib_mpegproperties_protectionenabled(TagLib::MPEG::Header * header);
	TagLib::MPEG::Header::ChannelMode bmx_taglib_mpegproperties_channelmode(TagLib::MPEG::Header * header);
	bool bmx_taglib_mpegproperties_iscopyrighted(TagLib::MPEG::Header * header);
	bool bmx_taglib_mpegproperties_isoriginal(TagLib::MPEG::Header * header);

	TagLib::MPEG::Properties * bmx_taglib_mpegfile_audioproperties(TagLib::MPEG::File * file);
	bool bmx_taglib_mpegfile_save(TagLib::MPEG::File * file);
	bool bmx_taglib_mpegfile_savetags(TagLib::MPEG::File * file, int tags, bool stripOthers);
	TagLib::ID3v2::Tag * bmx_taglib_mpegfile_id3v2tag(TagLib::MPEG::File * file, bool create);
	TagLib::ID3v1::Tag * bmx_taglib_mpegfile_id3v1tag(TagLib::MPEG::File * file , bool create);
	TagLib::APE::Tag * bmx_taglib_mpegfile_apetag(TagLib::MPEG::File * file , bool create);
	bool bmx_taglib_mpegfile_strip(TagLib::MPEG::File * file, int tags, bool freeMemory);
	long bmx_taglib_mpegfile_firstframeoffset(TagLib::MPEG::File * file);
	long bmx_taglib_mpegfile_nextframeoffset(TagLib::MPEG::File * file, long position);
	long bmx_taglib_mpegfile_previousframeoffset(TagLib::MPEG::File * file, long position);
	long bmx_taglib_mpegfile_lastframeoffset(TagLib::MPEG::File * file);

}

// ****************************************

TagLib::FileRef * bmx_taglib_fileref_create(BBString * filename, bool readAudioProperties, TagLib::AudioProperties::ReadStyle audioPropertiesStyle) {

#ifdef WIN32
	wchar_t * f = (wchar_t*)bbStringToWString(filename);
#else
	char * f = bbStringToCString(brl_blitz_bbStringToUTF8String(filename));
#endif
	TagLib::FileRef * ref = new TagLib::FileRef(f, readAudioProperties, audioPropertiesStyle);
	bbMemFree(f);
	
	return ref;
}

TagLib::Tag * bmx_taglib_fileref_tag(TagLib::FileRef * ref) {
	return ref->tag();
}

TagLib::AudioProperties * bmx_taglib_fileref_audioproperties(TagLib::FileRef * ref) {
	return ref->audioProperties();
}

void bmx_taglib_fileref_delete(TagLib::FileRef * ref) {
	delete ref;
}

bool bmx_taglib_fileref_save(TagLib::FileRef * ref) {
	return ref->save();
}

bool bmx_taglib_fileref_isnull(TagLib::FileRef * ref) {
	return ref->isNull();
}

// ****************************************

BBString * bmx_taglib_tag_title(TagLib::Tag * tag) {
	return bah_taglib_stringFromUTF8String(tag->title().toCString(true));
}

BBString * bmx_taglib_tag_artist(TagLib::Tag * tag) {
	return bah_taglib_stringFromUTF8String(tag->artist().toCString(true));
}

BBString * bmx_taglib_tag_album(TagLib::Tag * tag) {
	return bah_taglib_stringFromUTF8String(tag->album().toCString(true));
}

BBString * bmx_taglib_tag_comment(TagLib::Tag * tag) {
	return bah_taglib_stringFromUTF8String(tag->comment().toCString(true));
}

BBString * bmx_taglib_tag_genre(TagLib::Tag * tag) {
	return bah_taglib_stringFromUTF8String(tag->genre().toCString(true));
}

TagLib::uint bmx_taglib_tag_year(TagLib::Tag * tag) {
	return tag->year();
}

TagLib::uint bmx_taglib_tag_track(TagLib::Tag * tag) {
	return tag->track();
}

void bmx_taglib_tag_settitle(TagLib::Tag * tag, BBString * value) {
	char * v = 0;
	if (value != &bbEmptyString) {
		v = bbStringToCString(value);
	}
	tag->setTitle((v) ? TagLib::String(v, TagLib::String::UTF8) : TagLib::String::null);
	if (v) bbMemFree(v);
}

void bmx_taglib_tag_setartist(TagLib::Tag * tag, BBString * value) {
	char * v = 0;
	if (value != &bbEmptyString) {
		v = bbStringToCString(value);
	}
	tag->setArtist((v) ? TagLib::String(v, TagLib::String::UTF8) : TagLib::String::null);
	if (v) bbMemFree(v);
}

void bmx_taglib_tag_setalbum(TagLib::Tag * tag, BBString * value) {
	char * v = 0;
	if (value != &bbEmptyString) {
		v = bbStringToCString(value);
	}
	tag->setAlbum((v) ? TagLib::String(v, TagLib::String::UTF8) : TagLib::String::null);
	if (v) bbMemFree(v);
}

void bmx_taglib_tag_setcomment(TagLib::Tag * tag, BBString * value) {
	char * v = 0;
	if (value != &bbEmptyString) {
		v = bbStringToCString(value);
	}
	tag->setComment((v) ? TagLib::String(v, TagLib::String::UTF8) : TagLib::String::null);
	if (v) bbMemFree(v);
}

void bmx_taglib_tag_setgenre(TagLib::Tag * tag, BBString * value) {
	char * v = 0;
	if (value != &bbEmptyString) {
		v = bbStringToCString(value);
	}
	tag->setGenre((v) ? TagLib::String(v, TagLib::String::UTF8) : TagLib::String::null);
	if (v) bbMemFree(v);
}

void bmx_taglib_tag_setyear(TagLib::Tag * tag, TagLib::uint value) {
	tag->setYear(value);
}

void bmx_taglib_tag_settrack(TagLib::Tag * tag, TagLib::uint value) {
	tag->setTrack(value);
}

bool bmx_taglib_tag_isempty(TagLib::Tag * tag) {
	return tag->isEmpty();
}


// ****************************************

int bmx_taglib_audoproperties_length(TagLib::AudioProperties * prop) {
	return prop->length();
}

int bmx_taglib_audoproperties_bitrate(TagLib::AudioProperties * prop) {
	return prop->bitrate();
}

int bmx_taglib_audoproperties_samplerate(TagLib::AudioProperties * prop) {
	return prop->sampleRate();
}

int bmx_taglib_audoproperties_channels(TagLib::AudioProperties * prop) {
	return prop->channels();
}


// ****************************************

TagLib::MPEG::Header::Version bmx_taglib_mpegproperties_version(TagLib::MPEG::Header * header) {
	return header->version();
}

int bmx_taglib_mpegproperties_layer(TagLib::MPEG::Header * header) {
	return header->layer();
}

bool bmx_taglib_mpegproperties_protectionenabled(TagLib::MPEG::Header * header) {
	return header->protectionEnabled();
}

TagLib::MPEG::Header::ChannelMode bmx_taglib_mpegproperties_channelmode(TagLib::MPEG::Header * header) {
	return header->channelMode();
}

bool bmx_taglib_mpegproperties_iscopyrighted(TagLib::MPEG::Header * header) {
	return header->isCopyrighted();
}

bool bmx_taglib_mpegproperties_isoriginal(TagLib::MPEG::Header * header) {
	return header->isOriginal();
}


// ****************************************

TagLib::MPEG::Properties * bmx_taglib_mpegfile_audioproperties(TagLib::MPEG::File * file) {
	return file->audioProperties();
}

bool bmx_taglib_mpegfile_save(TagLib::MPEG::File * file) {
	return file->save();
}

bool bmx_taglib_mpegfile_savetags(TagLib::MPEG::File * file, int tags, bool stripOthers) {
	return file->save(tags, stripOthers);
}

TagLib::ID3v2::Tag * bmx_taglib_mpegfile_id3v2tag(TagLib::MPEG::File * file, bool create) {
	return file->ID3v2Tag(create);
}

TagLib::ID3v1::Tag * bmx_taglib_mpegfile_id3v1tag(TagLib::MPEG::File * file , bool create) {
	return file->ID3v1Tag(create);
}

TagLib::APE::Tag * bmx_taglib_mpegfile_apetag(TagLib::MPEG::File * file , bool create) {
	return file->APETag(create);
}

bool bmx_taglib_mpegfile_strip(TagLib::MPEG::File * file, int tags, bool freeMemory) {
	return file->strip(tags, freeMemory);
}

long bmx_taglib_mpegfile_firstframeoffset(TagLib::MPEG::File * file) {
	return file->firstFrameOffset();
}

long bmx_taglib_mpegfile_nextframeoffset(TagLib::MPEG::File * file, long position) {
	return file->nextFrameOffset(position);
}

long bmx_taglib_mpegfile_previousframeoffset(TagLib::MPEG::File * file, long position) {
	return file->previousFrameOffset(position);
}

long bmx_taglib_mpegfile_lastframeoffset(TagLib::MPEG::File * file) {
	return file->lastFrameOffset();
}

