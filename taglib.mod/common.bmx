' Copyright (c) 2009 Bruce A Henderson
'
'  The contents of this file are subject to the Mozilla Public License
'  Version 1.1 (the "License"); you may not use this file except in
'  compliance with the License. You may obtain a copy of the License at
'  http://www.mozilla.org/MPL/
'  
'  Software distributed under the License is distributed on an "AS IS"
'  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
'  License for the specific language governing rights and limitations
'  under the License.
'  
'  The Original Code is BaH.TagLib.
'  
'  The Initial Developer of the Original Code is Bruce A Henderson.
'
SuperStrict

Import BRL.Map
Import BRL.LinkedList

Import "source.bmx"

Extern

	Function bmx_taglib_fileref_create:Byte Ptr(filename:String, readAudioProperties:Int, audioPropertiesStyle:Int)
	Function bmx_taglib_fileref_tag:Byte Ptr(handle:Byte Ptr)
	Function bmx_taglib_fileref_audioproperties:Byte Ptr(handle:Byte Ptr)
	Function bmx_taglib_fileref_delete(handle:Byte Ptr)
	Function bmx_taglib_fileref_save:Int(handle:Byte Ptr)
	Function bmx_taglib_fileref_isnull:Int(handle:Byte Ptr)

	Function bmx_taglib_tag_title:String(handle:Byte Ptr)
	Function bmx_taglib_tag_artist:String(handle:Byte Ptr)
	Function bmx_taglib_tag_album:String(handle:Byte Ptr)
	Function bmx_taglib_tag_comment:String(handle:Byte Ptr)
	Function bmx_taglib_tag_genre:String(handle:Byte Ptr)
	Function bmx_taglib_tag_year:Int(handle:Byte Ptr)
	Function bmx_taglib_tag_track:Int(handle:Byte Ptr)
	Function bmx_taglib_tag_settitle(handle:Byte Ptr, value:String)
	Function bmx_taglib_tag_setartist(handle:Byte Ptr, value:String)
	Function bmx_taglib_tag_setalbum(handle:Byte Ptr, value:String)
	Function bmx_taglib_tag_setcomment(handle:Byte Ptr, value:String)
	Function bmx_taglib_tag_setgenre(handle:Byte Ptr, value:String)
	Function bmx_taglib_tag_setyear(handle:Byte Ptr, value:Int)
	Function bmx_taglib_tag_settrack(handle:Byte Ptr, value:Int)
	Function bmx_taglib_tag_isempty:Int(handle:Byte Ptr)

	Function bmx_taglib_audoproperties_length:Int(handle:Byte Ptr)
	Function bmx_taglib_audoproperties_bitrate:Int(handle:Byte Ptr)
	Function bmx_taglib_audoproperties_samplerate:Int(handle:Byte Ptr)
	Function bmx_taglib_audoproperties_channels:Int(handle:Byte Ptr)

	Function bmx_taglib_mpegproperties_version:Int(handle:Byte Ptr)
	Function bmx_taglib_mpegproperties_layer:Int(handle:Byte Ptr)
	Function bmx_taglib_mpegproperties_protectionenabled:Int(handle:Byte Ptr)
	Function bmx_taglib_mpegproperties_channelmode:Int(handle:Byte Ptr)
	Function bmx_taglib_mpegproperties_iscopyrighted:Int(handle:Byte Ptr)
	Function bmx_taglib_mpegproperties_isoriginal:Int(handle:Byte Ptr)

	Function bmx_taglib_mpegfile_audioproperties:Byte Ptr(handle:Byte Ptr)
	Function bmx_taglib_mpegfile_save:Int(handle:Byte Ptr)
	Function bmx_taglib_mpegfile_savetags:Int(handle:Byte Ptr, tags:Int, stripOthers:Int)
	Function bmx_taglib_mpegfile_id3v2tag:Byte Ptr(handle:Byte Ptr, _create:Int)
	Function bmx_taglib_mpegfile_id3v1tag:Byte Ptr(handle:Byte Ptr , _create:Int)
	Function bmx_taglib_mpegfile_apetag:Byte Ptr(handle:Byte Ptr , _create:Int)
	Function bmx_taglib_mpegfile_strip:Int(handle:Byte Ptr, tags:Int, freeMemory:Int)
	Function bmx_taglib_mpegfile_firstframeoffset:Int(handle:Byte Ptr)
	Function bmx_taglib_mpegfile_nextframeoffset:Int(handle:Byte Ptr, position:Int)
	Function bmx_taglib_mpegfile_previousframeoffset:Int(handle:Byte Ptr, position:Int)
	Function bmx_taglib_mpegfile_lastframeoffset:Int(handle:Byte Ptr)

	Function bmx_taglib_id3v2header_majorversion:Int(handle:Byte Ptr)
	Function bmx_taglib_id3v2header_revisionnumber:Int(handle:Byte Ptr)
	Function bmx_taglib_id3v2header_unsynchronisation:Int(handle:Byte Ptr)
	Function bmx_taglib_id3v2header_extendedheader:Int(handle:Byte Ptr)
	Function bmx_taglib_id3v2header_experimentalindicator:Int(handle:Byte Ptr)
	Function bmx_taglib_id3v2header_footerpresent:Int(handle:Byte Ptr)
	Function bmx_taglib_id3v2header_tagsize:Int(handle:Byte Ptr)
	Function bmx_taglib_id3v2header_completetagsize:Int(handle:Byte Ptr)
	Function bmx_taglib_id3v2header_settagsize(handle:Byte Ptr, size:Int)

	Function bmx_taglib_id3v2tag_header:Object(handle:Byte Ptr)

End Extern


Const STRINGTYPE_LATIN1:Int = 0
Const STRINGTYPE_UTF16:Int = 1
Const STRINGTYPE_UTF16BE:Int = 2
Const STRINGTYPE_UTF8:Int = 3
Const STRINGTYPE_UTF16LE:Int = 4

