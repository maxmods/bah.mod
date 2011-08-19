/*
   Copyright (c) 2009-2011 Bruce A Henderson
 
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
#include <vorbisfile.h>
#include <vorbisproperties.h>
#include <mp4file.h>
#include <mp4tag.h>

class MaxID3v2FrameList;
class MaxMP4ItemListMap;
class MaxMP4CoverArtList;
class MaxByteVector;
class MaxOggFieldListMap;

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
	
	BBObject * _bah_taglib_TTLMP4Item__create(const TagLib::MP4::Item * item);
	
	BBObject * _bah_taglib_TTLMP4CoverArt__create(const TagLib::MP4::CoverArt * art);
	
	BBObject * getID3v2Header(TagLib::ID3v2::Header * header);
	BBObject * getID3v2Frame(const TagLib::ID3v2::Frame * header);
	
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

	TagLib::MPEG::File * bmx_taglib_mpegfile_create(BBString * filename, int readProperties, TagLib::AudioProperties::ReadStyle propertiesStyle);
	void bmx_taglib_mpegfile_free(TagLib::MPEG::File * file);
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
	int bmx_taglib_mpegfile_isopen(TagLib::MPEG::File * file);
	int bmx_taglib_mpegfile_isvalid(TagLib::MPEG::File * file);
	void bmx_taglib_mpegfile_clear(TagLib::MPEG::File * file);
	long bmx_taglib_mpegfile_length(TagLib::MPEG::File * file);

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
	MaxID3v2FrameList * bmx_taglib_id3v2tag_framelist(TagLib::ID3v2::Tag * tag);
	
	void bmx_taglib_id3v2framelist_free(MaxID3v2FrameList * list);
	BBObject * bmx_taglib_id3v2framelist_nextframe(MaxID3v2FrameList * list);
	void bmx_taglib_id3v2framelist_reset(MaxID3v2FrameList * list);
	BBObject * bmx_taglib_id3v2framelist_frame(MaxID3v2FrameList * list, int index);
	int bmx_taglib_id3v2framelist_isempty(MaxID3v2FrameList * list);
	int bmx_taglib_id3v2framelist_size(MaxID3v2FrameList * list);

	int bmx_taglib_file_isreadable(BBString * file);
	int bmx_taglib_file_iswritable(BBString * name);

	TagLib::Ogg::Vorbis::File * bmx_taglib_oggvorbisfile_create(BBString * filename, int readProperties, TagLib::AudioProperties::ReadStyle propertiesStyle);
	void bmx_taglib_oggvorbisfile_free(TagLib::Ogg::Vorbis::File * file);
	TagLib::Vorbis::Properties * bmx_taglib_oggvorbisfile_audioproperties(TagLib::Ogg::Vorbis::File * file);
	int bmx_taglib_oggvorbisfile_save(TagLib::Ogg::Vorbis::File * file);
	TagLib::Ogg::XiphComment * bmx_taglib_oggvorbisfile_tag(TagLib::Ogg::Vorbis::File * file);

	MaxOggFieldListMap * bmx_taglib_oggxiphcomment_fieldlistmap(TagLib::Ogg::XiphComment * comment);
	BBString * bmx_taglib_oggfieldlistmap_field(MaxOggFieldListMap * list, BBString * key);
	int bmx_taglib_oggfieldlistmap_isempty(MaxOggFieldListMap * list);
	int bmx_taglib_oggfieldlistmap_size(MaxOggFieldListMap * list);
	void bmx_taglib_oggfieldlistmap_reset(MaxOggFieldListMap * list);
	void bmx_taglib_oggfieldlistmap_free(MaxOggFieldListMap * list);
	BBArray * bmx_taglib_oggfieldlistmap_nextfield(MaxOggFieldListMap * list);

	int bmx_taglib_vorbisproperties_vorbisversion(TagLib::Vorbis::Properties * prop);
	int bmx_taglib_vorbisproperties_bitratemaximum(TagLib::Vorbis::Properties * prop);
	int bmx_taglib_vorbisproperties_bitratenominal(TagLib::Vorbis::Properties * prop);
	int bmx_taglib_vorbisproperties_bitrateminimum(TagLib::Vorbis::Properties * prop);

	BBString * bmx_taglib_bytevector_tostring(MaxByteVector * vec);
	void bmx_taglib_bytevector_free(MaxByteVector * vec);
	char * bmx_taglib_bytevector_data(MaxByteVector * vec);
	void bmx_taglib_bytevector_clear(MaxByteVector * vec);
	int bmx_taglib_bytevector_size(MaxByteVector * vec);
	int bmx_taglib_bytevector_isempty(MaxByteVector * vec);

	BBString * bmx_taglib_id3v2frame_tostring(TagLib::ID3v2::Frame * frame);
	MaxByteVector * bmx_taglib_id3v2frame_frameid(TagLib::ID3v2::Frame * frame);
	int bmx_taglib_id3v2frame_size(TagLib::ID3v2::Frame * frame);

	int bmx_taglib_id3v2attachedpictureframe_textencoding(TagLib::ID3v2::AttachedPictureFrame * frame);
	BBString * bmx_taglib_id3v2attachedpictureframe_mimetype(TagLib::ID3v2::AttachedPictureFrame * frame);
	int bmx_taglib_id3v2attachedpictureframe_imagetype(TagLib::ID3v2::AttachedPictureFrame * frame);
	BBString * bmx_taglib_id3v2attachedpictureframe_description(TagLib::ID3v2::AttachedPictureFrame * frame);
	MaxByteVector * bmx_taglib_id3v2attachedpictureframe_picture(TagLib::ID3v2::AttachedPictureFrame * frame);

	void bmx_taglib_id3v2textidentificationframe_settext(TagLib::ID3v2::TextIdentificationFrame * frame, BBString * text);
	void bmx_taglib_id3v2textidentificationframe_settextlist(TagLib::ID3v2::TextIdentificationFrame * frame, BBArray * text);
	BBString * bmx_taglib_id3v2textidentificationframe_tostring(TagLib::ID3v2::TextIdentificationFrame * frame);
	int bmx_taglib_id3v2textidentificationframe_textencoding(TagLib::ID3v2::TextIdentificationFrame * frame);
	void bmx_taglib_id3v2textidentificationframe_settextencoding(TagLib::ID3v2::TextIdentificationFrame * frame, TagLib::String::Type encoding);
	BBArray * bmx_taglib_id3v2textidentificationframe_fieldlist(TagLib::ID3v2::TextIdentificationFrame * frame);

	BBString * bmx_taglib_id3v2urllinkframe_url(TagLib::ID3v2::UrlLinkFrame * frame);
	void bmx_taglib_id3v2urllinkframe_seturl(TagLib::ID3v2::UrlLinkFrame * frame, BBString * text);
	void bmx_taglib_id3v2urllinkframe_settext(TagLib::ID3v2::UrlLinkFrame * frame, BBString * text);
	BBString * bmx_taglib_id3v2urllinkframe_tostring(TagLib::ID3v2::UrlLinkFrame * frame);

	BBString * bmx_taglib_id3v2commentsframe_tostring(TagLib::ID3v2::CommentsFrame * frame);
	BBString * bmx_taglib_id3v2commentsframe_description(TagLib::ID3v2::CommentsFrame * frame);
	BBString * bmx_taglib_id3v2commentsframe_text(TagLib::ID3v2::CommentsFrame * frame);
	int bmx_taglib_id3v2commentsframe_textencoding(TagLib::ID3v2::CommentsFrame * frame);
	void bmx_taglib_id3v2commentsframe_settextencoding(TagLib::ID3v2::CommentsFrame * frame, TagLib::String::Type encoding);
	void bmx_taglib_id3v2commentsframe_setdescription(TagLib::ID3v2::CommentsFrame * frame, BBString * description);
	void bmx_taglib_id3v2commentsframe_settext(TagLib::ID3v2::CommentsFrame * frame, BBString * text);

	TagLib::MP4::File * bmx_taglib_mp4file_create(BBString * filename, int readProperties, TagLib::AudioProperties::ReadStyle propertiesStyle);
	TagLib::MP4::Properties * bmx_taglib_mp4file_audioproperties(TagLib::MP4::File * file);
	int bmx_taglib_mp4file_save(TagLib::MP4::File * file);
	TagLib::MP4::Tag * bmx_taglib_mp4file_tag(TagLib::MP4::File * file);
	void bmx_taglib_mp4file_free(TagLib::MP4::File * file);
	int bmx_taglib_mp4file_isopen(TagLib::MP4::File * file);
	int bmx_taglib_mp4file_isvalid(TagLib::MP4::File * file);
	void bmx_taglib_mp4file_clear(TagLib::MP4::File * file);
	int bmx_taglib_mp4file_length(TagLib::MP4::File * file);

	MaxMP4ItemListMap * bmx_taglib_mp4tag_itemlist(TagLib::MP4::Tag * tag);

	BBObject * bmx_taglib_mp4itemlistmap_item(MaxMP4ItemListMap * list, BBString * key);
	int bmx_taglib_mp4itemlistmap_isempty(MaxMP4ItemListMap * list);
	int bmx_taglib_mp4itemlistmap_size(MaxMP4ItemListMap * list);
	void bmx_taglib_mp4itemlistmap_reset(MaxMP4ItemListMap * list);
	void bmx_taglib_mp4itemlistmap_free(MaxMP4ItemListMap * list);
	BBObject * bmx_taglib_mp4itemlistmap_nextitem(MaxMP4ItemListMap * list);

	BBObject * bmx_taglib_mp4coverartlist_coverart(MaxMP4CoverArtList * list, int index);
	int bmx_taglib_mp4coverartlist_isempty(MaxMP4CoverArtList * list);
	int bmx_taglib_mp4coverartlist_size(MaxMP4CoverArtList * list);
	void bmx_taglib_mp4coverartlist_reset(MaxMP4CoverArtList * list);
	void bmx_taglib_mp4coverartlist_free(MaxMP4CoverArtList * list);
	BBObject * bmx_taglib_mp4coverartlist_nextitem(MaxMP4CoverArtList * list);

	TagLib::MP4::CoverArt::Format bmx_taglib_mp4coverart_format(TagLib::MP4::CoverArt * art);
	MaxByteVector * bmx_taglib_mp4coverart_data(TagLib::MP4::CoverArt * art);

	MaxMP4CoverArtList * bmx_taglib_mp4item_coverartlist(TagLib::MP4::Item * item);
	int bmx_taglib_mp4item_isvalid(TagLib::MP4::Item * item);
	int bmx_taglib_mp4item_toint(TagLib::MP4::Item * item);
	int bmx_taglib_mp4item_tobool(TagLib::MP4::Item * item);
	void bmx_taglib_mp4item_tointpair(TagLib::MP4::Item * item, int * _first, int * _second);
	BBArray * bmx_taglib_mp4item_tostrings(TagLib::MP4::Item * item);

}

// ****************************************

BBArray * bmx_taglib_stringlistToBBArray(const TagLib::StringList & list) {
	int n = list.size();
	BBArray *p = bbArrayNew1D("$",n);
	BBString **s=(BBString**)BBARRAYDATA( p,p->dims );
	for( int i=0;i<n;++i ){
		s[i] = bbStringFromUTF8String(list[i].toCString(true));
		BBRETAIN( s[i] );
	}
	return p;
}

// ****************************************

class MaxID3v2FrameList
{
public:
	MaxID3v2FrameList(const TagLib::ID3v2::FrameList & f)
		: frameList(f)
	{
		it = frameList.begin();
	}
	
	~MaxID3v2FrameList()
	{
	}
	
	BBObject * nextFrame() {
		if (it != frameList.end()) {
			return getID3v2Frame(*it++);
		} else {
			return &bbNullObject;
		}
	}
	
	void reset() {
		it = frameList.begin();
	}
	
	TagLib::ID3v2::FrameList & List() {
		return frameList;
	}

private:
	TagLib::ID3v2::FrameList frameList;
	TagLib::ID3v2::FrameList::ConstIterator it;
};

// ****************************************

class MaxByteVector
{
public:
	MaxByteVector(const TagLib::ByteVector & v)
		: vector(v)
	{
	}
	
	~MaxByteVector()
	{
	}
	
	TagLib::ByteVector & Vector() {
		return vector;
	}

private:
	TagLib::ByteVector vector;
};

// ****************************************

class MaxMP4ItemListMap
{
public:
	MaxMP4ItemListMap(const TagLib::MP4::ItemListMap & f)
		: itemList(f)
	{
		it = itemList.begin();
	}
	
	~MaxMP4ItemListMap()
	{
	}
	
	BBObject * nextItem() {
		if (it != itemList.end()) {
			return _bah_taglib_TTLMP4Item__create(&(*it++).second);
		} else {
			return &bbNullObject;
		}
	}
	
	void reset() {
		it = itemList.begin();
	}
	
	TagLib::MP4::ItemListMap & List() {
		return itemList;
	}

private:
	TagLib::MP4::ItemListMap itemList;
	TagLib::MP4::ItemListMap::ConstIterator it;
};

// ****************************************

class MaxMP4CoverArtList
{
public:
	MaxMP4CoverArtList(const TagLib::MP4::CoverArtList & f)
		: coverArtList(f)
	{
		it = coverArtList.begin();
	}
	
	~MaxMP4CoverArtList()
	{
	}
	
	BBObject * nextItem() {
		if (it != coverArtList.end()) {
			return _bah_taglib_TTLMP4CoverArt__create(&(*it++));
		} else {
			return &bbNullObject;
		}
	}
	
	void reset() {
		it = coverArtList.begin();
	}
	
	TagLib::MP4::CoverArtList & List() {
		return coverArtList;
	}

private:
	TagLib::MP4::CoverArtList coverArtList;
	TagLib::MP4::CoverArtList::ConstIterator it;
};

// ****************************************

class MaxOggFieldListMap
{
public:
	MaxOggFieldListMap(const TagLib::Ogg::FieldListMap & f)
		: fieldList(f)
	{
		it = fieldList.begin();
	}
	
	~MaxOggFieldListMap()
	{
	}
	
	BBArray * nextField() {
		if (it != fieldList.end()) {
			return bmx_taglib_stringlistToBBArray((*it++).second);
		} else {
			return &bbEmptyArray;
		}
	}
	
	void reset() {
		it = fieldList.begin();
	}
	
	TagLib::Ogg::FieldListMap & List() {
		return fieldList;
	}

private:
	TagLib::Ogg::FieldListMap fieldList;
	TagLib::Ogg::FieldListMap::ConstIterator it;
};

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

BBObject * getID3v2Frame(const TagLib::ID3v2::Frame * header) {

	if (!header) {
		return &bbNullObject;
	}

	if (dynamic_cast<const TagLib::ID3v2::UserTextIdentificationFrame*>(header)) {
		return _bah_taglib_TTLID3v2UserTextIdentificationFrame__create((TagLib::ID3v2::Header*)header);
	}

	if (dynamic_cast<const TagLib::ID3v2::UserUrlLinkFrame*>(header)) {
		return _bah_taglib_TTLID3v2UserUrlLinkFrame__create((TagLib::ID3v2::Header*)header);
	}

	if (dynamic_cast<const TagLib::ID3v2::TextIdentificationFrame*>(header)) {
		return _bah_taglib_TTLID3v2TextIdentificationFrame__create((TagLib::ID3v2::Header*)header);
	}

	if (dynamic_cast<const TagLib::ID3v2::AttachedPictureFrame*>(header)) {
		return _bah_taglib_TTLID3v2AttachedPictureFrame__create((TagLib::ID3v2::Header*)header);
	}

	if (dynamic_cast<const TagLib::ID3v2::CommentsFrame*>(header)) {
		return _bah_taglib_TTLID3v2CommentsFrame__create((TagLib::ID3v2::Header*)header);
	}

	if (dynamic_cast<const TagLib::ID3v2::GeneralEncapsulatedObjectFrame*>(header)) {
		return _bah_taglib_TTLID3v2GeneralEncapsulatedObjectFrame__create((TagLib::ID3v2::Header*)header);
	}

	if (dynamic_cast<const TagLib::ID3v2::RelativeVolumeFrame*>(header)) {
		return _bah_taglib_TTLID3v2RelativeVolumeFrame__create((TagLib::ID3v2::Header*)header);
	}

	if (dynamic_cast<const TagLib::ID3v2::UniqueFileIdentifierFrame*>(header)) {
		return _bah_taglib_TTLID3v2UniqueFileIdentifierFrame__create((TagLib::ID3v2::Header*)header);
	}

	if (dynamic_cast<const TagLib::ID3v2::UnknownFrame*>(header)) {
		return _bah_taglib_TTLID3v2UnknownFrame__create((TagLib::ID3v2::Header*)header);
	}

	if (dynamic_cast<const TagLib::ID3v2::UnsynchronizedLyricsFrame*>(header)) {
		return _bah_taglib_TTLID3v2UnsynchronizedLyricsFrame__create((TagLib::ID3v2::Header*)header);
	}

	if (dynamic_cast<const TagLib::ID3v2::Frame*>(header)) {
		return _bah_taglib_TTLID3v2Frame__create((TagLib::ID3v2::Header*)header);
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

TagLib::MPEG::File * bmx_taglib_mpegfile_create(BBString * filename, int readProperties, TagLib::AudioProperties::ReadStyle propertiesStyle) {

#ifdef WIN32
	wchar_t * f = (wchar_t*)bbStringToWString(filename);
#else
	char * f = bbStringToUTF8String(filename);
#endif

	TagLib::MPEG::File * file = new TagLib::MPEG::File(f, static_cast<bool>(readProperties), propertiesStyle);
	bbMemFree(f);
	
	return file;
}

void bmx_taglib_mpegfile_free(TagLib::MPEG::File * file) {
	delete file;
}

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

int bmx_taglib_mpegfile_isopen(TagLib::MPEG::File * file) {
	return static_cast<int>(file->isOpen());
}

int bmx_taglib_mpegfile_isvalid(TagLib::MPEG::File * file) {
	return static_cast<int>(file->isValid());
}

void bmx_taglib_mpegfile_clear(TagLib::MPEG::File * file) {
	file->clear();
}

long bmx_taglib_mpegfile_length(TagLib::MPEG::File * file) {
	return file->length();
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

MaxID3v2FrameList * bmx_taglib_id3v2tag_framelist(TagLib::ID3v2::Tag * tag) {
	return new MaxID3v2FrameList(tag->frameList());
}

// ****************************************

int bmx_taglib_file_isreadable(BBString * file) {
	char * f = bbStringToCString(file);
	int res = static_cast<int>(TagLib::File::isReadable(f));
	bbMemFree(f);
	return res;
}

int bmx_taglib_file_iswritable(BBString * name) {
	char * f = bbStringToCString(name);
	int res = static_cast<int>(TagLib::File::isWritable(f));
	bbMemFree(f);
	return res;
}

// ****************************************

TagLib::Ogg::Vorbis::File * bmx_taglib_oggvorbisfile_create(BBString * filename, int readProperties, TagLib::AudioProperties::ReadStyle propertiesStyle) {

#ifdef WIN32
	wchar_t * f = (wchar_t*)bbStringToWString(filename);
#else
	char * f = bbStringToUTF8String(filename);
#endif

	TagLib::Ogg::Vorbis::File * file = new TagLib::Ogg::Vorbis::File(f, static_cast<bool>(readProperties), propertiesStyle);
	bbMemFree(f);
	
	return file;
}

void bmx_taglib_oggvorbisfile_free(TagLib::Ogg::Vorbis::File * file) {
	delete file;
}

TagLib::Vorbis::Properties * bmx_taglib_oggvorbisfile_audioproperties(TagLib::Ogg::Vorbis::File * file) {
	return file->audioProperties();
}

int bmx_taglib_oggvorbisfile_save(TagLib::Ogg::Vorbis::File * file){
	return static_cast<int>(file->save());
}

TagLib::Ogg::XiphComment * bmx_taglib_oggvorbisfile_tag(TagLib::Ogg::Vorbis::File * file) {
	return file->tag();
}


// ****************************************

int bmx_taglib_vorbisproperties_vorbisversion(TagLib::Vorbis::Properties * prop) {
	return prop->vorbisVersion();
}

int bmx_taglib_vorbisproperties_bitratemaximum(TagLib::Vorbis::Properties * prop) {
	return prop->bitrateMaximum();
}

int bmx_taglib_vorbisproperties_bitratenominal(TagLib::Vorbis::Properties * prop) {
	return prop->bitrateNominal();
}

int bmx_taglib_vorbisproperties_bitrateminimum(TagLib::Vorbis::Properties * prop) {
	return prop->bitrateMinimum();
}

// ****************************************

void bmx_taglib_id3v2framelist_free(MaxID3v2FrameList * list) {
	delete list;
}

BBObject * bmx_taglib_id3v2framelist_nextframe(MaxID3v2FrameList * list) {
	return list->nextFrame();
}

void bmx_taglib_id3v2framelist_reset(MaxID3v2FrameList * list) {
	list->reset();
}

BBObject * bmx_taglib_id3v2framelist_frame(MaxID3v2FrameList * list, int index) {
	return getID3v2Frame(list->List()[index]);
}

int bmx_taglib_id3v2framelist_isempty(MaxID3v2FrameList * list) {
	return static_cast<int>(list->List().isEmpty());
}

int bmx_taglib_id3v2framelist_size(MaxID3v2FrameList * list) {
	return list->List().size();
}

// ****************************************

BBString * bmx_taglib_bytevector_tostring(MaxByteVector * vec) {
	return bbStringFromBytes(vec->Vector().data(), vec->Vector().size());
}

void bmx_taglib_bytevector_free(MaxByteVector * vec) {
	delete vec;
}

char * bmx_taglib_bytevector_data(MaxByteVector * vec) {
	return vec->Vector().data();
}

void bmx_taglib_bytevector_clear(MaxByteVector * vec) {
	vec->Vector().clear();
}

int bmx_taglib_bytevector_size(MaxByteVector * vec) {
	return vec->Vector().size();
}

int bmx_taglib_bytevector_isempty(MaxByteVector * vec) {
	return static_cast<int>(vec->Vector().isEmpty());
}

// ****************************************

BBString * bmx_taglib_id3v2frame_tostring(TagLib::ID3v2::Frame * frame) {
	return bbStringFromUTF8String(frame->toString().toCString(true));
}

MaxByteVector * bmx_taglib_id3v2frame_frameid(TagLib::ID3v2::Frame * frame) {
	return new MaxByteVector(frame->frameID());
}

int bmx_taglib_id3v2frame_size(TagLib::ID3v2::Frame * frame) {
	return frame->size();
}

// ****************************************

int bmx_taglib_id3v2attachedpictureframe_textencoding(TagLib::ID3v2::AttachedPictureFrame * frame) {
	return frame->textEncoding();
}

BBString * bmx_taglib_id3v2attachedpictureframe_mimetype(TagLib::ID3v2::AttachedPictureFrame * frame) {
	return bbStringFromUTF8String(frame->mimeType().toCString(true));
}

int bmx_taglib_id3v2attachedpictureframe_imagetype(TagLib::ID3v2::AttachedPictureFrame * frame) {
	return frame->type();
}

BBString * bmx_taglib_id3v2attachedpictureframe_description(TagLib::ID3v2::AttachedPictureFrame * frame) {
	return bbStringFromUTF8String(frame->description().toCString(true));
}

MaxByteVector * bmx_taglib_id3v2attachedpictureframe_picture(TagLib::ID3v2::AttachedPictureFrame * frame) {
	return new MaxByteVector(frame->picture());
}


// ****************************************

void bmx_taglib_id3v2textidentificationframe_settext(TagLib::ID3v2::TextIdentificationFrame * frame, BBString * text) {
	char * t = 0;
	if (text != &bbEmptyString) {
		t = bbStringToUTF8String(text);
	}
	frame->setText((t) ? TagLib::String(t, TagLib::String::UTF8) : TagLib::String::null);
	if (t) bbMemFree(t);
}

void bmx_taglib_id3v2textidentificationframe_settextlist(TagLib::ID3v2::TextIdentificationFrame * frame, BBArray * text) {
	TagLib::StringList list;
	int n = text->scales[0];
	BBString **s = (BBString**)BBARRAYDATA(text,text->dims);
	for (int i = 0; i < n; i++) {
		char * t = bbStringToUTF8String(s[i]);
		list.append(t);
		bbMemFree(t);
	}
	frame->setText(list);
}

BBString * bmx_taglib_id3v2textidentificationframe_tostring(TagLib::ID3v2::TextIdentificationFrame * frame) {
	return bbStringFromUTF8String(frame->toString().toCString(true));
}

int bmx_taglib_id3v2textidentificationframe_textencoding(TagLib::ID3v2::TextIdentificationFrame * frame) {
	return frame->textEncoding();
}

void bmx_taglib_id3v2textidentificationframe_settextencoding(TagLib::ID3v2::TextIdentificationFrame * frame, TagLib::String::Type encoding) {
	frame->setTextEncoding(encoding);
}

BBArray * bmx_taglib_id3v2textidentificationframe_fieldlist(TagLib::ID3v2::TextIdentificationFrame * frame) {
	TagLib::StringList list = frame->fieldList();
	return bmx_taglib_stringlistToBBArray(list);
/*	
	int n = list.size();
	BBArray *p = bbArrayNew1D("$",n);
	BBString **s=(BBString**)BBARRAYDATA( p,p->dims );
	for( int i=0;i<n;++i ){
		s[i] = bbStringFromUTF8String(list[i].toCString(true));
		BBRETAIN( s[i] );
	}
	return p;
*/
}

// ****************************************

BBString * bmx_taglib_id3v2urllinkframe_url(TagLib::ID3v2::UrlLinkFrame * frame) {
	return bbStringFromUTF8String(frame->url().toCString(true));
}

void bmx_taglib_id3v2urllinkframe_seturl(TagLib::ID3v2::UrlLinkFrame * frame, BBString * text) {
	char * t = 0;
	if (text != &bbEmptyString) {
		t = bbStringToUTF8String(text);
	}
	frame->setUrl((t) ? TagLib::String(t, TagLib::String::UTF8) : TagLib::String::null);
	if (t) bbMemFree(t);
}

void bmx_taglib_id3v2urllinkframe_settext(TagLib::ID3v2::UrlLinkFrame * frame, BBString * text) {
	char * t = 0;
	if (text != &bbEmptyString) {
		t = bbStringToUTF8String(text);
	}
	frame->setText((t) ? TagLib::String(t, TagLib::String::UTF8) : TagLib::String::null);
	if (t) bbMemFree(t);
}

BBString * bmx_taglib_id3v2urllinkframe_tostring(TagLib::ID3v2::UrlLinkFrame * frame) {
	return bbStringFromUTF8String(frame->toString().toCString(true));
}

// ****************************************

BBString * bmx_taglib_id3v2commentsframe_tostring(TagLib::ID3v2::CommentsFrame * frame) {
	return bbStringFromUTF8String(frame->toString().toCString(true));
}

BBString * bmx_taglib_id3v2commentsframe_description(TagLib::ID3v2::CommentsFrame * frame) {
	return bbStringFromUTF8String(frame->description().toCString(true));
}

BBString * bmx_taglib_id3v2commentsframe_text(TagLib::ID3v2::CommentsFrame * frame) {
	return bbStringFromUTF8String(frame->text().toCString(true));
}

int bmx_taglib_id3v2commentsframe_textencoding(TagLib::ID3v2::CommentsFrame * frame) {
	return frame->textEncoding();
}

void bmx_taglib_id3v2commentsframe_settextencoding(TagLib::ID3v2::CommentsFrame * frame, TagLib::String::Type encoding) {
	frame->setTextEncoding(encoding);
}

void bmx_taglib_id3v2commentsframe_setdescription(TagLib::ID3v2::CommentsFrame * frame, BBString * description) {
	char * d = 0;
	if (description != &bbEmptyString) {
		d = bbStringToUTF8String(description);
	}
	frame->setDescription((d) ? TagLib::String(d, TagLib::String::UTF8) : TagLib::String::null);
	if (d) bbMemFree(d);
}

void bmx_taglib_id3v2commentsframe_settext(TagLib::ID3v2::CommentsFrame * frame, BBString * text) {
	char * t = 0;
	if (text != &bbEmptyString) {
		t = bbStringToUTF8String(text);
	}
	frame->setText((t) ? TagLib::String(t, TagLib::String::UTF8) : TagLib::String::null);
	if (t) bbMemFree(t);
}

// ****************************************

TagLib::MP4::File * bmx_taglib_mp4file_create(BBString * filename, int readProperties, TagLib::AudioProperties::ReadStyle propertiesStyle) {


#ifdef WIN32
	wchar_t * f = (wchar_t*)bbStringToWString(filename);
#else
	char * f = bbStringToUTF8String(filename);
#endif

	TagLib::MP4::File * file = new TagLib::MP4::File(f, static_cast<bool>(readProperties), propertiesStyle);
	bbMemFree(f);
	
	return file;
}

TagLib::MP4::Properties * bmx_taglib_mp4file_audioproperties(TagLib::MP4::File * file) {
	return file->audioProperties();
}

int bmx_taglib_mp4file_save(TagLib::MP4::File * file) {
	return static_cast<int>(file->save());
}

TagLib::MP4::Tag * bmx_taglib_mp4file_tag(TagLib::MP4::File * file) {
	return file->tag();
}

void bmx_taglib_mp4file_free(TagLib::MP4::File * file) {
	delete file;
}

int bmx_taglib_mp4file_isopen(TagLib::MP4::File * file) {
	return static_cast<int>(file->isOpen());
}

int bmx_taglib_mp4file_isvalid(TagLib::MP4::File * file) {
	return static_cast<int>(file->isValid());
}

void bmx_taglib_mp4file_clear(TagLib::MP4::File * file) {
	file->clear();
}

int bmx_taglib_mp4file_length(TagLib::MP4::File * file) {
	return file->length();
}


// ****************************************

MaxMP4ItemListMap * bmx_taglib_mp4tag_itemlist(TagLib::MP4::Tag * tag) {
	return new MaxMP4ItemListMap(tag->itemListMap());
}


// ****************************************

BBObject * bmx_taglib_mp4itemlistmap_item(MaxMP4ItemListMap * list, BBString * key) {

	char * k = 0;
	if (key != &bbEmptyString) {
		k = bbStringToUTF8String(key);
	}
	
	if (k) {
		BBObject * obj = _bah_taglib_TTLMP4Item__create(&list->List()[TagLib::String(k, TagLib::String::UTF8)]);
		bbMemFree(k);
		return obj;
	} else {
		return &bbNullObject;
	}
	
}

int bmx_taglib_mp4itemlistmap_isempty(MaxMP4ItemListMap * list) {
	return static_cast<int>(list->List().isEmpty());
}

int bmx_taglib_mp4itemlistmap_size(MaxMP4ItemListMap * list) {
	return list->List().size();
}

void bmx_taglib_mp4itemlistmap_reset(MaxMP4ItemListMap * list) {
	list->reset();
}

void bmx_taglib_mp4itemlistmap_free(MaxMP4ItemListMap * list) {
	delete list;
}

BBObject * bmx_taglib_mp4itemlistmap_nextitem(MaxMP4ItemListMap * list) {
	return list->nextItem();
}


// ****************************************

BBObject * bmx_taglib_mp4coverartlist_coverart(MaxMP4CoverArtList * list, int index) {
	return _bah_taglib_TTLMP4CoverArt__create(&list->List()[index]);
}

int bmx_taglib_mp4coverartlist_isempty(MaxMP4CoverArtList * list) {
	return static_cast<int>(list->List().isEmpty());
}

int bmx_taglib_mp4coverartlist_size(MaxMP4CoverArtList * list) {
	return list->List().size();
}

void bmx_taglib_mp4coverartlist_reset(MaxMP4CoverArtList * list) {
	list->reset();
}

void bmx_taglib_mp4coverartlist_free(MaxMP4CoverArtList * list) {
	delete list;
}

BBObject * bmx_taglib_mp4coverartlist_nextitem(MaxMP4CoverArtList * list) {
	return list->nextItem();
}

// ****************************************

TagLib::MP4::CoverArt::Format bmx_taglib_mp4coverart_format(TagLib::MP4::CoverArt * art) {
	return art->format();
}

MaxByteVector * bmx_taglib_mp4coverart_data(TagLib::MP4::CoverArt * art) {
	return new MaxByteVector(art->data());
}

// ****************************************

MaxMP4CoverArtList * bmx_taglib_mp4item_coverartlist(TagLib::MP4::Item * item) {
	return new MaxMP4CoverArtList(item->toCoverArtList());
}

int bmx_taglib_mp4item_isvalid(TagLib::MP4::Item * item) {
	return static_cast<int>(item->isValid());
}

int bmx_taglib_mp4item_toint(TagLib::MP4::Item * item) {
	return item->toInt();
}

int bmx_taglib_mp4item_tobool(TagLib::MP4::Item * item) {
	return static_cast<int>(item->toBool());
}

void bmx_taglib_mp4item_tointpair(TagLib::MP4::Item * item, int * _first, int * _second) {
	TagLib::MP4::Item::IntPair pair = item->toIntPair();
	*_first = pair.first;
	*_second = pair.second;
}

BBArray * bmx_taglib_mp4item_tostrings(TagLib::MP4::Item * item) {
	TagLib::StringList list = item->toStringList();
	
	int n = list.size();
	BBArray *p = bbArrayNew1D("$",n);
	BBString **s=(BBString**)BBARRAYDATA( p,p->dims );
	for( int i=0;i<n;++i ){
		s[i] = bbStringFromUTF8String(list[i].toCString(true));
		BBRETAIN( s[i] );
	}
	return p;
}

// ****************************************

MaxOggFieldListMap * bmx_taglib_oggxiphcomment_fieldlistmap(TagLib::Ogg::XiphComment * comment) {
	return new MaxOggFieldListMap(comment->fieldListMap());
}

BBString * bmx_taglib_oggfieldlistmap_field(MaxOggFieldListMap * list, BBString * key) {

}

int bmx_taglib_oggfieldlistmap_isempty(MaxOggFieldListMap * list) {

}

int bmx_taglib_oggfieldlistmap_size(MaxOggFieldListMap * list) {

}

void bmx_taglib_oggfieldlistmap_reset(MaxOggFieldListMap * list) {

}

void bmx_taglib_oggfieldlistmap_free(MaxOggFieldListMap * list) {

}

BBArray * bmx_taglib_oggfieldlistmap_nextfield(MaxOggFieldListMap * list) {

}

// ****************************************

