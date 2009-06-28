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
#include <id3v2tag.h>
#include <id3v2header.h>
#include <textidentificationframe.h>
#include <attachedpictureframe.h>
#include <commentsframe.h>
#include <generalencapsulatedobjectframe.h>
#include <relativevolumeframe.h>
#include <uniquefileidentifierframe.h>
#include <unknownframe.h>
#include <unsynchronizedlyricsframe.h>
#include <urllinkframe.h>

extern "C" {

#include "blitz.h"

	BBObject * _bah_taglib_TTLID3v2UserTextIdentificationFrame__create(TagLib::ID3v2::Header * header);
	BBObject * _bah_taglib_TTLID3v2UserUrlLinkFrame__create(TagLib::ID3v2::Header * header);
	BBObject * _bah_taglib_TTLID3v2TextIdentificationFrame__create(TagLib::ID3v2::Header * header);
	BBObject * _bah_taglib_TTLID3v2AttachedPictureFrame__create(TagLib::ID3v2::Header * header);
	BBObject * _bah_taglib_TTLID3v2CommentsFrame__create(TagLib::ID3v2::Header * header);
	BBObject * _bah_taglib_TTLID3v2GeneralEncapsulatedObjectFrame__create(TagLib::ID3v2::Header * header);
	BBObject * _bah_taglib_TTLID3v2RelativeVolumeFrame__create(TagLib::ID3v2::Header * header);
	BBObject * _bah_taglib_TTLID3v2UniqueFileIdentifierFrame__create(TagLib::ID3v2::Header * header);
	BBObject * _bah_taglib_TTLID3v2UnknownFrame__create(TagLib::ID3v2::Header * header);
	BBObject * _bah_taglib_TTLID3v2UnsynchronizedLyricsFrame__create(TagLib::ID3v2::Header * header);
	BBObject * _bah_taglib_TTLID3v2Frame__create(TagLib::ID3v2::Header * header);
	BBObject * _bah_taglib_TTLID3v2Header__create(TagLib::ID3v2::Header * header);

	TagLib::FileRef * bmx_taglib_fileref_create(BBString * filename, int readAudioProperties, TagLib::AudioProperties::ReadStyle audioPropertiesStyle);
	TagLib::Tag * bmx_taglib_fileref_tag(TagLib::FileRef * ref);
	TagLib::AudioProperties * bmx_taglib_fileref_audioproperties(TagLib::FileRef * ref);
	void bmx_taglib_fileref_delete(TagLib::FileRef * ref);
	int bmx_taglib_fileref_save(TagLib::FileRef * ref);
	int bmx_taglib_fileref_isnull(TagLib::FileRef * ref);

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
	int bmx_taglib_tag_isempty(TagLib::Tag * tag);

	int bmx_taglib_audoproperties_length(TagLib::AudioProperties * prop);
	int bmx_taglib_audoproperties_bitrate(TagLib::AudioProperties * prop);
	int bmx_taglib_audoproperties_samplerate(TagLib::AudioProperties * prop);
	int bmx_taglib_audoproperties_channels(TagLib::AudioProperties * prop);

	TagLib::MPEG::Header::Version bmx_taglib_mpegproperties_version(TagLib::MPEG::Header * header);
	int bmx_taglib_mpegproperties_layer(TagLib::MPEG::Header * header);
	int bmx_taglib_mpegproperties_protectionenabled(TagLib::MPEG::Header * header);
	TagLib::MPEG::Header::ChannelMode bmx_taglib_mpegproperties_channelmode(TagLib::MPEG::Header * header);
	int bmx_taglib_mpegproperties_iscopyrighted(TagLib::MPEG::Header * header);
	int bmx_taglib_mpegproperties_isoriginal(TagLib::MPEG::Header * header);

	TagLib::MPEG::Properties * bmx_taglib_mpegfile_audioproperties(TagLib::MPEG::File * file);
	int bmx_taglib_mpegfile_save(TagLib::MPEG::File * file);
	int bmx_taglib_mpegfile_savetags(TagLib::MPEG::File * file, int tags, int stripOthers);
	TagLib::ID3v2::Tag * bmx_taglib_mpegfile_id3v2tag(TagLib::MPEG::File * file, int create);
	TagLib::ID3v1::Tag * bmx_taglib_mpegfile_id3v1tag(TagLib::MPEG::File * file , int create);
	TagLib::APE::Tag * bmx_taglib_mpegfile_apetag(TagLib::MPEG::File * file , int create);
	int bmx_taglib_mpegfile_strip(TagLib::MPEG::File * file, int tags, int freeMemory);
	long bmx_taglib_mpegfile_firstframeoffset(TagLib::MPEG::File * file);
	long bmx_taglib_mpegfile_nextframeoffset(TagLib::MPEG::File * file, long position);
	long bmx_taglib_mpegfile_previousframeoffset(TagLib::MPEG::File * file, long position);
	long bmx_taglib_mpegfile_lastframeoffset(TagLib::MPEG::File * file);

	TagLib::uint bmx_taglib_id3v2header_majorversion(TagLib::ID3v2::Header * header);
	TagLib::uint bmx_taglib_id3v2header_revisionnumber(TagLib::ID3v2::Header * header);
	int bmx_taglib_id3v2header_unsynchronisation(TagLib::ID3v2::Header * header);
	int bmx_taglib_id3v2header_extendedheader(TagLib::ID3v2::Header * header);
	int bmx_taglib_id3v2header_experimentalindicator(TagLib::ID3v2::Header * header);
	int bmx_taglib_id3v2header_footerpresent(TagLib::ID3v2::Header * header);
	TagLib::uint bmx_taglib_id3v2header_tagsize(TagLib::ID3v2::Header * header);
	TagLib::uint bmx_taglib_id3v2header_completetagsize(TagLib::ID3v2::Header * header);
	void bmx_taglib_id3v2header_settagsize(TagLib::ID3v2::Header * header, TagLib::uint size);

	BBObject * bmx_taglib_id3v2tag_header(TagLib::ID3v2::Tag * tag);

}

// ****************************************

BBObject * getID3v2Header(TagLib::ID3v2::Header * header) {

	if (!header) {
		return &bbNullObject;
	}

	if (dynamic_cast<TagLib::ID3v2::UserTextIdentificationFrame*>(header)) {
		return _bah_taglib_TTLID3v2UserTextIdentificationFrame__create(header);
	}

	if (dynamic_cast<TagLib::ID3v2::UserUrlLinkFrame*>(header)) {
		return _bah_taglib_TTLID3v2UserUrlLinkFrame__create(header);
	}

	if (dynamic_cast<TagLib::ID3v2::TextIdentificationFrame*>(header)) {
		return _bah_taglib_TTLID3v2TextIdentificationFrame__create(header);
	}

	if (dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(header)) {
		return _bah_taglib_TTLID3v2AttachedPictureFrame__create(header);
	}

	if (dynamic_cast<TagLib::ID3v2::CommentsFrame*>(header)) {
		return _bah_taglib_TTLID3v2CommentsFrame__create(header);
	}

	if (dynamic_cast<TagLib::ID3v2::GeneralEncapsulatedObjectFrame*>(header)) {
		return _bah_taglib_TTLID3v2GeneralEncapsulatedObjectFrame__create(header);
	}

	if (dynamic_cast<TagLib::ID3v2::RelativeVolumeFrame*>(header)) {
		return _bah_taglib_TTLID3v2RelativeVolumeFrame__create(header);
	}

	if (dynamic_cast<TagLib::ID3v2::UniqueFileIdentifierFrame*>(header)) {
		return _bah_taglib_TTLID3v2UniqueFileIdentifierFrame__create(header);
	}

	if (dynamic_cast<TagLib::ID3v2::UnknownFrame*>(header)) {
		return _bah_taglib_TTLID3v2UnknownFrame__create(header);
	}

	if (dynamic_cast<TagLib::ID3v2::UnsynchronizedLyricsFrame*>(header)) {
		return _bah_taglib_TTLID3v2UnsynchronizedLyricsFrame__create(header);
	}

	if (dynamic_cast<TagLib::ID3v2::Frame*>(header)) {
		return _bah_taglib_TTLID3v2Frame__create(header);
	}

	if (dynamic_cast<TagLib::ID3v2::Header*>(header)) {
		return _bah_taglib_TTLID3v2Header__create(header);
	}

}

// ****************************************

TagLib::FileRef * bmx_taglib_fileref_create(BBString * filename, int readAudioProperties, TagLib::AudioProperties::ReadStyle audioPropertiesStyle) {

#ifdef WIN32
	wchar_t * f = (wchar_t*)bbStringToWString(filename);
#else
	char * f = bbStringToUTF8String(filename);
#endif
	TagLib::FileRef * ref = new TagLib::FileRef(f, static_cast<bool>(readAudioProperties), audioPropertiesStyle);
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

int bmx_taglib_fileref_save(TagLib::FileRef * ref) {
	return static_cast<int>(ref->save());
}

int bmx_taglib_fileref_isnull(TagLib::FileRef * ref) {
	return static_cast<int>(ref->isNull());
}

// ****************************************

BBString * bmx_taglib_tag_title(TagLib::Tag * tag) {
	return bbStringFromUTF8String(tag->title().toCString(true));
}

BBString * bmx_taglib_tag_artist(TagLib::Tag * tag) {
	return bbStringFromUTF8String(tag->artist().toCString(true));
}

BBString * bmx_taglib_tag_album(TagLib::Tag * tag) {
	return bbStringFromUTF8String(tag->album().toCString(true));
}

BBString * bmx_taglib_tag_comment(TagLib::Tag * tag) {
	return bbStringFromUTF8String(tag->comment().toCString(true));
}

BBString * bmx_taglib_tag_genre(TagLib::Tag * tag) {
	return bbStringFromUTF8String(tag->genre().toCString(true));
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
		v = bbStringToUTF8String(value);
	}
	tag->setTitle((v) ? TagLib::String(v, TagLib::String::UTF8) : TagLib::String::null);
	if (v) bbMemFree(v);
}

void bmx_taglib_tag_setartist(TagLib::Tag * tag, BBString * value) {
	char * v = 0;
	if (value != &bbEmptyString) {
		v = bbStringToUTF8String(value);
	}
	tag->setArtist((v) ? TagLib::String(v, TagLib::String::UTF8) : TagLib::String::null);
	if (v) bbMemFree(v);
}

void bmx_taglib_tag_setalbum(TagLib::Tag * tag, BBString * value) {
	char * v = 0;
	if (value != &bbEmptyString) {
		v = bbStringToUTF8String(value);
	}
	tag->setAlbum((v) ? TagLib::String(v, TagLib::String::UTF8) : TagLib::String::null);
	if (v) bbMemFree(v);
}

void bmx_taglib_tag_setcomment(TagLib::Tag * tag, BBString * value) {
	char * v = 0;
	if (value != &bbEmptyString) {
		v = bbStringToUTF8String(value);
	}
	tag->setComment((v) ? TagLib::String(v, TagLib::String::UTF8) : TagLib::String::null);
	if (v) bbMemFree(v);
}

void bmx_taglib_tag_setgenre(TagLib::Tag * tag, BBString * value) {
	char * v = 0;
	if (value != &bbEmptyString) {
		v = bbStringToUTF8String(value);
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

int bmx_taglib_tag_isempty(TagLib::Tag * tag) {
	return static_cast<int>(tag->isEmpty());
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

int bmx_taglib_mpegproperties_protectionenabled(TagLib::MPEG::Header * header) {
	return static_cast<int>(header->protectionEnabled());
}

TagLib::MPEG::Header::ChannelMode bmx_taglib_mpegproperties_channelmode(TagLib::MPEG::Header * header) {
	return header->channelMode();
}

int bmx_taglib_mpegproperties_iscopyrighted(TagLib::MPEG::Header * header) {
	return static_cast<int>(header->isCopyrighted());
}

int bmx_taglib_mpegproperties_isoriginal(TagLib::MPEG::Header * header) {
	return static_cast<int>(header->isOriginal());
}


// ****************************************

TagLib::MPEG::Properties * bmx_taglib_mpegfile_audioproperties(TagLib::MPEG::File * file) {
	return file->audioProperties();
}

int bmx_taglib_mpegfile_save(TagLib::MPEG::File * file) {
	return static_cast<int>(file->save());
}

int bmx_taglib_mpegfile_savetags(TagLib::MPEG::File * file, int tags, int stripOthers) {
	return static_cast<int>(file->save(tags, static_cast<bool>(stripOthers)));
}

TagLib::ID3v2::Tag * bmx_taglib_mpegfile_id3v2tag(TagLib::MPEG::File * file, int create) {
	return file->ID3v2Tag(static_cast<bool>(create));
}

TagLib::ID3v1::Tag * bmx_taglib_mpegfile_id3v1tag(TagLib::MPEG::File * file , int create) {
	return file->ID3v1Tag(static_cast<bool>(create));
}

TagLib::APE::Tag * bmx_taglib_mpegfile_apetag(TagLib::MPEG::File * file , int create) {
	return file->APETag(static_cast<bool>(create));
}

int bmx_taglib_mpegfile_strip(TagLib::MPEG::File * file, int tags, int freeMemory) {
	return static_cast<int>(file->strip(tags, static_cast<bool>(freeMemory)));
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


// ****************************************

TagLib::uint bmx_taglib_id3v2header_majorversion(TagLib::ID3v2::Header * header) {
	return header->majorVersion();
}

TagLib::uint bmx_taglib_id3v2header_revisionnumber(TagLib::ID3v2::Header * header) {
	return header->revisionNumber();
}

int bmx_taglib_id3v2header_unsynchronisation(TagLib::ID3v2::Header * header) {
	return static_cast<int>(header->unsynchronisation());
}

int bmx_taglib_id3v2header_extendedheader(TagLib::ID3v2::Header * header) {
	return static_cast<int>(header->extendedHeader());
}

int bmx_taglib_id3v2header_experimentalindicator(TagLib::ID3v2::Header * header) {
	return static_cast<int>(header->experimentalIndicator());
}

int bmx_taglib_id3v2header_footerpresent(TagLib::ID3v2::Header * header) {
	return static_cast<int>(header->footerPresent());
}

TagLib::uint bmx_taglib_id3v2header_tagsize(TagLib::ID3v2::Header * header) {
	return header->tagSize();
}

TagLib::uint bmx_taglib_id3v2header_completetagsize(TagLib::ID3v2::Header * header) {
	return header->completeTagSize();
}

void bmx_taglib_id3v2header_settagsize(TagLib::ID3v2::Header * header, TagLib::uint size) {
	return header->setTagSize(size);
}


// ****************************************

BBObject * bmx_taglib_id3v2tag_header(TagLib::ID3v2::Tag * tag) {
	return getID3v2Header(tag->header());
}
