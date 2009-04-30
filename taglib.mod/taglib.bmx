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

Rem
bbdoc: TagLib
about: Reading and editing audio meta data.
End Rem
Module BaH.TagLib

ModuleInfo "Version: 1.00"
ModuleInfo "License: MPL"
ModuleInfo "Copyright: TagLib - Scott Wheeler"
ModuleInfo "Copyright: Wrapper - 2009 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release (TagLib 1.5 SVN rev 960714)"

ModuleInfo "CC_OPTS: -DHAVE_ZLIB -DWITH_MP4 -DWITH_ASF"

Import "common.bmx"

'
' Build notes :
'
' taglib_export.h - added  "&& !defined(__MINGW32__)" to ifdef.
'

Rem
bbdoc: This type provides a simple abstraction for creating and handling files.
about: TTLFileRef exists to provide a minimal, generic and value-based wrapper around a File. It is lightweight and implicitly shared,
and as such suitable for pass-by-value use. This hides some of the uglier details of TagLib::File and the non-generic portions of the
concrete file implementations.
<p>
This type is useful in a "simple usage" situation where it is desirable to be able to get and set some of the tag information that is
similar across file types.
</p>
End Rem
Type TTLFileRef

	Field fileRefPtr:Byte Ptr

	Rem
	bbdoc: 
	End Rem
	Function CreateFileRef:TTLFileRef(filename:String, readAudioProperties:Int = True, audioPropertiesStyle:Int = TTLAudioProperties.READSTYLE_AVERAGE)
		Return New TTLFileRef.Create(filename, readAudioProperties, audioPropertiesStyle)
	End Function

	Rem
	bbdoc: 
	End Rem
	Method Create:TTLFileRef(filename:String, readAudioProperties:Int = True, audioPropertiesStyle:Int = TTLAudioProperties.READSTYLE_AVERAGE)
		fileRefPtr = bmx_taglib_fileref_create(filename, readAudioProperties, audioPropertiesStyle)
		Return Self
	End Method

	Rem
	bbdoc: Returns the file's tag.
	End Rem
	Method tag:TTLTag()
		Return TTLTag._create(bmx_taglib_fileref_tag(fileRefPtr))
	End Method
	
	Rem
	bbdoc: Returns the audio properties for this FileRef.
	about: If no audio properties were read then this will return Null.
	End Rem
	Method audioProperties:TTLAudioProperties()
		Return TTLAudioProperties._create(bmx_taglib_fileref_audioproperties(fileRefPtr))
	End Method
	
	Rem
	bbdoc: Saves the file.
	returns: True on success.
	End Rem
	Method save:Int()
		Return bmx_taglib_fileref_save(fileRefPtr)
	End Method
	
	Rem
	bbdoc: Returns true if the file (and as such other file references) are null.
	End Rem
	Method isNull:Int()
		Return bmx_taglib_fileref_isnull(fileRefPtr)
	End Method
	
	Rem
	bbdoc: Frees and closes the FileRef.
	End Rem
	Method Free()
		If fileRefPtr Then
			bmx_taglib_fileref_delete(fileRefPtr)
			fileRefPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type

Rem
bbdoc: A simple, generic interface to common audio meta data fields.
about: This is an attempt to abstract away the difference in the meta data formats of various audio codecs and tagging schemes.
As such it is generally a subset of what is available in the specific formats but should be suitable for most applications.
This is meant to compliment the generic APIs found in TTLAudioProperties, TTLFile and TTLFileRef.
End Rem
Type TTLTag

	Field tagPtr:Byte Ptr
	
	Function _create:TTLTag(tagPtr:Byte Ptr)
		If tagPtr Then
			Local this:TTLTag = New TTLTag
			this.tagPtr = tagPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Returns the track name.
	about: If no track name is present in the tag Null will be returned.
	End Rem
	Method title:String()
		Return bmx_taglib_tag_title(tagPtr)
	End Method
	
	Rem
	bbdoc: Returns the artist name.
	about: If no artist name is present in the tag Null will be returned.
	End Rem
	Method artist:String()
		Return bmx_taglib_tag_artist(tagPtr)
	End Method
	
	Rem
	bbdoc: Returns the album name.
	about: If no album name is present in the tag Null will be returned.
	End Rem
	Method album:String()
		Return bmx_taglib_tag_album(tagPtr)
	End Method
	
	Rem
	bbdoc: Returns the track comment.
	about: If no comment is present in the tag Null will be returned.
	End Rem
	Method comment:String()
		Return bmx_taglib_tag_comment(tagPtr)
	End Method
	
	Rem
	bbdoc: Returns the genre name.
	about: If no genre is present in the tag Null will be returned.
	End Rem
	Method genre:String()
		Return bmx_taglib_tag_genre(tagPtr)
	End Method
	
	Rem
	bbdoc: Returns the year.
	about: If there is no year set, this will return 0.
	End Rem
	Method year:Int()
		Return bmx_taglib_tag_year(tagPtr)
	End Method
	
	Rem
	bbdoc: Returns the track number.
	about: If there is no track number set, this will return 0.
	End Rem
	Method track:Int()
		Return bmx_taglib_tag_track(tagPtr)
	End Method
	
	Rem
	bbdoc: Sets the title to @value.
	about: If @value is Null then this value will be cleared.
	End Rem
	Method setTitle(value:String)
		bmx_taglib_tag_settitle(tagPtr, stringToUTF8String(value))
	End Method
	
	Rem
	bbdoc: Sets the artist to @value.
	about: If @value is Null then this value will be cleared.
	End Rem
	Method setArtist(value:String)
		bmx_taglib_tag_setartist(tagPtr, stringToUTF8String(value))
	End Method
	
	Rem
	bbdoc: Sets the album to @value.
	about: If @value is Null then this value will be cleared.
	End Rem
	Method setAlbum(value:String)
		bmx_taglib_tag_setalbum(tagPtr, stringToUTF8String(value))
	End Method
	
	Rem
	bbdoc: Sets the comment to @value.
	about: If @value is Null then this value will be cleared.
	End Rem
	Method setComment(value:String)
		bmx_taglib_tag_setcomment(tagPtr, stringToUTF8String(value))
	End Method
	
	Rem
	bbdoc: Sets the genre to @value.
	about: If @value is Null then this value will be cleared.
	End Rem
	Method setGenre(value:String)
		bmx_taglib_tag_setgenre(tagPtr, stringToUTF8String(value))
	End Method
	
	Rem
	bbdoc: Sets the year to @value.
	about: If s is 0 then this value will be cleared.
	End Rem
	Method setYear(value:Int)
		bmx_taglib_tag_setyear(tagPtr, value)
	End Method
	
	Rem
	bbdoc: Sets the track to @value.
	about: If s is 0 then this value will be cleared.
	End Rem
	Method setTrack(value:Int)
		bmx_taglib_tag_settrack(tagPtr, value)
	End Method
	
	Rem
	bbdoc: Returns true if the tag does not contain any data.
	End Rem
	Method isEmpty:Int()
		Return bmx_taglib_tag_isempty(tagPtr)
	End Method

End Type

Rem
bbdoc: A simple, abstract interface to common audio properties.
about: The values here are common to most audio formats. For more specific, codec dependant values, please see see the subclasses APIs.
This is meant to compliment the TTLFile and TTLTag APIs in providing a simple interface that is sufficient for most applications.
End Rem
Type TTLAudioProperties

	Rem
	bbdoc: Read as little of the file as possible.
	about: Reading audio properties from a file can sometimes be very time consuming and for the most accurate results can
	often involve reading the entire file. Because in many situations speed is critical or the accuracy of the values
	is not particularly important this allows the level of desired accuracy to be set.
	End Rem
	Const READSTYLE_FAST:Int = 0
	Rem
	bbdoc: Read more of the file and make better values guesses.
	about: Reading audio properties from a file can sometimes be very time consuming and for the most accurate results can
	often involve reading the entire file. Because in many situations speed is critical or the accuracy of the values
	is not particularly important this allows the level of desired accuracy to be set.
	End Rem
	Const READSTYLE_AVERAGE:Int = 1
	Rem
	bbdoc: Read as much of the file as needed to report accurate values.
	about: Reading audio properties from a file can sometimes be very time consuming and for the most accurate results can
	often involve reading the entire file. Because in many situations speed is critical or the accuracy of the values
	is not particularly important this allows the level of desired accuracy to be set.
	End Rem
	Const READSTYLE_ACCURATE:Int = 2

	Field apPtr:Byte Ptr

	Function _create:TTLAudioProperties(apPtr:Byte Ptr)
		If apPtr Then
			Local this:TTLAudioProperties = New TTLAudioProperties
			this.apPtr = apPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Returns the length of the file in seconds.
	End Rem
	Method length:Int()
		Return bmx_taglib_audoproperties_length(apPtr)
	End Method
	
	Rem
	bbdoc: Returns the most appropriate bit rate for the file in kb/s.
	about: For constant bitrate formats this is simply the bitrate of the file. For variable bitrate formats this is either the average
	or nominal bitrate.
	End Rem
	Method bitrate:Int()
		Return bmx_taglib_audoproperties_bitrate(apPtr)
	End Method
	
	Rem
	bbdoc: Returns the sample rate in Hz.
	End Rem
	Method sampleRate:Int()
		Return bmx_taglib_audoproperties_samplerate(apPtr)
	End Method
	
	Rem
	bbdoc: Returns the number of audio channels.
	End Rem
	Method channels:Int()
		Return bmx_taglib_audoproperties_channels(apPtr)
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TTLFile

	Field filePtr:Byte Ptr

	Method name:String()
	End Method
	
	Method audioProperties:TTLAudioProperties()
	End Method
	
	Method save:Int()
	End Method

End Type

Rem
bbdoc: An MPEG file type with some useful methods specific to MPEG.
about: This implements the generic TTLFile API and additionally provides access to properties that
are distinct to MPEG files, notably access to the different ID3 tags.
End Rem
Type TTLMPEGFile Extends TTLFile

	Const TAGTYPE_NOTAGS:Int = 0
	Const TAGTYPE_ID3V1:Int = 1
	Const TAGTYPE_ID3v2:Int = 2
	Const TAGTYPE_APE:Int = 3
	Const TAGTYPE_ALLTAGS:Int = 4

	Rem
	bbdoc: Returns the TTLMPEGProperties for this file.
	about: If no audio properties were read then this will return Null.
	End Rem
	Method audioProperties:TTLAudioProperties()
		Return TTLMPEGProperties._create(bmx_taglib_mpegfile_audioproperties(filePtr))
	End Method
	
	Rem
	bbdoc: Saves the file.
	about: If at least one tag -- ID3v1 or ID3v2 -- exists this will duplicate its content into the other tag.
	This returns true if saving was successful.
	<p>
	If neither exists or if both tags are empty, this will strip the tags from the file.
	</p>
	<p>
	This is the same as calling save(AllTags);
	</p>
	<p>
	If you would like more granular control over the content of the tags, with the concession of generality, use paramaterized saveTags call.
	</p>
	End Rem
	Method save:Int()
		Return bmx_taglib_mpegfile_save(filePtr)
	End Method

	Rem
	bbdoc: Saves the file.
	about: This will attempt to save all of the tag types that are specified by OR-ing together TagTypes values.
	The save() method above uses AllTags. This returns true if saving was successful.
	<p>
	If stripOthers is true this strips all tags not included in the mask, but does not modify them in memory,
	so later calls to saveTags() which make use of these tags will remain valid. This also strips empty tags. 
	</p>
	End Rem
	Method saveTags:Int(tags:Int, stripOthers:Int = False)
		Return bmx_taglib_mpegfile_savetags(filePtr, tags, stripOthers)
	End Method

	Rem
	bbdoc: Returns the ID3v2 tag of the file.
	about: If @_create is false (the default) this will return Null if there is no valid ID3v2 tag.
	If @_create is true it will create an ID3v2 tag if one does not exist.
	End Rem
	Method ID3v2Tag:TTLID3v2Tag(_create:Int = False)
		Return TTLID3v2Tag._create(bmx_taglib_mpegfile_id3v2tag(filePtr, _create))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method ID3v1Tag:TTLID3v1Tag(_create:Int = False)
		Return TTLID3v1Tag._create(bmx_taglib_mpegfile_id3v1tag(filePtr, _create))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method APETag:TTLAPETag(_create:Int = False)
		Return TTLAPETag._create(bmx_taglib_mpegfile_apetag(filePtr, _create))
	End Method
	
	Rem
	bbdoc: This will strip the tags that match the OR-ed together TagTypes from the file.
	about: By default it strips all tags. It returns true if the tags are successfully stripped.
	<p>
	If @freeMemory is true the ID3 and APE tags will be deleted and pointers to them will be invalidated. 
	</p>
	End Rem
	Method strip:Int(tags:Int = TAGTYPE_ALLTAGS, freeMemory:Int = True)
		Return bmx_taglib_mpegfile_strip(filePtr, tags, freeMemory)
	End Method
	
	Rem
	bbdoc: Returns the position in the file of the first MPEG frame.
	End Rem
	Method firstFrameOffset:Int()
		Return bmx_taglib_mpegfile_firstframeoffset(filePtr)
	End Method
	
	Rem
	bbdoc: Returns the position in the file of the next MPEG frame, using the current position as start.
	End Rem
	Method nextFrameOffset:Int(position:Int)
		Return bmx_taglib_mpegfile_nextframeoffset(filePtr, position)
	End Method
	
	Rem
	bbdoc: Returns the position in the file of the previous MPEG frame, using the current position as start.
	End Rem
	Method previousFrameOffset:Int(position:Int)
		Return bmx_taglib_mpegfile_previousframeoffset(filePtr, position)
	End Method
	
	Rem
	bbdoc: Returns the position in the file of the last MPEG frame.
	End Rem
	Method lastFrameOffset:Int()
		Return bmx_taglib_mpegfile_lastframeoffset(filePtr)
	End Method
	
End Type

Rem
bbdoc: An implementation of audio property reading for MP3.
about: This reads the data from an MPEG Layer III stream found in the AudioProperties API.
End Rem
Type TTLMPEGProperties Extends TTLAudioProperties

	Function _create:TTLMPEGProperties(apPtr:Byte Ptr)
		If apPtr Then
			Local this:TTLMPEGProperties = New TTLMPEGProperties
			this.apPtr = apPtr
			Return this
		End If
	End Function

	Method xingHeader:TTLMPEGXingHeader()
	' TODO
	End Method
	
	Rem
	bbdoc: Returns the MPEG Version of the file.
	End Rem
	Method version:Int()
		Return bmx_taglib_mpegproperties_version(apPtr)
	End Method
	
	Rem
	bbdoc: Returns the layer version.
	about: This will be between the values 1-3.
	End Rem
	Method layer:Int()
		Return bmx_taglib_mpegproperties_layer(apPtr)
	End Method
	
	Rem
	bbdoc: Returns true if the MPEG protection bit is enabled.
	End Rem
	Method protectionEnabled:Int()
		Return bmx_taglib_mpegproperties_protectionenabled(apPtr)
	End Method
	
	Rem
	bbdoc: Returns the channel mode for this frame.
	End Rem
	Method channelMode:Int()
		Return bmx_taglib_mpegproperties_channelmode(apPtr)
	End Method
	
	Rem
	bbdoc: Returns true if the copyrighted bit is set.
	End Rem
	Method isCopyrighted:Int()
		Return bmx_taglib_mpegproperties_iscopyrighted(apPtr)
	End Method
	
	Rem
	bbdoc: Returns true if the "original" bit is set.
	End Rem
	Method isOriginal:Int()
		Return bmx_taglib_mpegproperties_isoriginal(apPtr)
	End Method
	
End Type

Type TTLMPEGXingHeader

End Type

Type TTLFLACFile Extends TTLFile
End Type

Type TTLMPCFile Extends TTLFile
End Type

Type TTLOggFile Extends TTLFile
End Type

Type TTLOggFLACFile Extends TTLOggFile
End Type

Type TTLOggSpeexFile Extends TTLOggFile
End Type

Type TTLOggVorbisFile Extends TTLOggFile
End Type

Type TTLTrueAudioFile Extends TTLFile
End Type

Type TTLWavPackFile Extends TTLFile
End Type

Rem
bbdoc: The main type in the ID3v2 implementation.
about: ID3v2 tags have several parts, TagLib attempts to provide an interface for them all.
header(), footer() and extendedHeader() corespond to those data structures in the ID3v2 standard and the APIs for the types that they
return attempt to reflect this.
<p>
Also ID3v2 tags are built up from a list of frames, which are in turn have a header and a list of fields. TagLib provides two ways of
accessing the list of frames that are in a given ID3v2 tag. The first is simply via the frameList() method. This is just a list of references
to the frames. The second is a map from the frame type -- i.e. "COMM" for comments -- and a list of frames of that type. (In some cases
ID3v2 allows for multiple frames of the same type, hence this being a map to a list rather than just a map to an individual frame.)
<p>
More information on the structure of frames can be found in the #TTLID3v2Frame type.
</p>
End Rem
Type TTLID3v2Tag Extends TTLTag

	Function _create:TTLID3v2Tag(tagPtr:Byte Ptr)
		If tagPtr Then
			Local this:TTLID3v2Tag = New TTLID3v2Tag
			this.tagPtr = tagPtr
			Return this
		End If
	End Function

	Method header:TTLID3v2Header()
	End Method
	
	Method extendedHeader:TTLID3v2ExtendedHeader()
	End Method
	
	Method footer:TTLID3v2Footer()
	End Method
	
	Method frameListMap:TMap()
	End Method
	
	Method frameList:TList()
	End Method
	
	Method addFrame(frame:TTLID3v2Frame)
	End Method
	
	Method removeFrame(frame:TTLID3v2Frame, del:Int = True)
	End Method

End Type

Type TTLID3V1Tag Extends TTLTag

	Function _create:TTLID3v1Tag(tagPtr:Byte Ptr)
		If tagPtr Then
			Local this:TTLID3v1Tag = New TTLID3v1Tag
			this.tagPtr = tagPtr
			Return this
		End If
	End Function

End Type

Type TTLAPETag Extends TTLTag

	Function _create:TTLAPETag(tagPtr:Byte Ptr)
		If tagPtr Then
			Local this:TTLAPETag = New TTLAPETag
			this.tagPtr = tagPtr
			Return this
		End If
	End Function

End Type

Type TTLID3v2Header

	Field headerPtr:Byte Ptr

	Function _create:TTLID3v2Header(headerPtr:Byte Ptr)
		If headerPtr Then
			Local this:TTLID3v2Header = New TTLID3v2Header
			this.headerPtr = headerPtr
			Return this
		End If
	End Function

	Method majorVersion:Int()
	End Method
	
	Method setMajorVersion(version:Int)
	End Method

	Method revisionNumber:Int()
	End Method

	Method unsynchronisation:Int()
	End Method

	Method extendedHeader:Int()
	End Method

	Method experimentalIndicator:Int()
	End Method

	Method footerPresent:Int()
	End Method

	Method tagSize:Int()
	End Method

	Method completeTagSize:Int()
	End Method

	Method setTagSize(s:Int)
	End Method

	'Method setData(Const ByteVector &data)
	'End Method

End Type

Type TTLID3v2ExtendedHeader

End Type

Type TTLID3v2Footer

End Type

Type TTLID3v2Frame Extends TTLID3v2Header

	Function _create:TTLID3v2Frame(headerPtr:Byte Ptr)
		If headerPtr Then
			Local this:TTLID3v2Frame = New TTLID3v2Frame
			this.headerPtr = headerPtr
			Return this
		End If
	End Function


End Type

Rem
bbdoc: An ID3v2 attached picture frame implementation.
about: This is an implementation of ID3v2 attached pictures. Pictures may be included in tags, one per
APIC frame (but there may be multiple APIC frames in a single tag). These pictures are usually in either
JPEG or PNG format.
End Rem
Type TTLID3v2AttachedPictureFrame Extends TTLID3v2Frame

	Rem
	bbdoc: A type not enumerated below.
	End Rem
	Const TYPE_OTHER:Int = $00
	Rem
	bbdoc: 32x32 PNG image that should be used as the file icon
	End Rem
	Const TYPE_FILEICON:Int = $01
	Rem
	bbdoc: File icon of a different size or format.
	End Rem
	Const TYPE_OTHERFILEICON:Int = $02
	Rem
	bbdoc: Front cover image of the album.
	End Rem
	Const TYPE_FRONTCOVER:Int = $03
	Rem
	bbdoc: Back cover image of the album.
	End Rem
	Const TYPE_BACKCOVER:Int = $04
	Rem
	bbdoc: Inside leaflet page of the album.
	End Rem
	Const TYPE_LEAFLETPAGE:Int = $05
	Rem
	bbdoc: Image from the album itself.
	End Rem
	Const TYPE_MEDIA:Int = $06
	Rem
	bbdoc: Picture of the lead artist or soloist.
	End Rem
	Const TYPE_LEADARTIST:Int = $07
	Rem
	bbdoc: Picture of the artist or performer.
	End Rem
	Const TYPE_ARTIST:Int = $08
	Rem
	bbdoc: Picture of the conductor.
	End Rem
	Const TYPE_CONDUCTOR:Int = $09
	Rem
	bbdoc: Picture of the band or orchestra.
	End Rem
	Const TYPE_BAND:Int = $0A
	Rem
	bbdoc: Picture of the composer.
	End Rem
	Const TYPE_COMPOSER:Int = $0B
	Rem
	bbdoc: Picture of the lyricist or text writer.
	End Rem
	Const TYPE_LYRICIST:Int = $0C
	Rem
	bbdoc: Picture of the recording location or studio.
	End Rem
	Const TYPE_RECORDINGLOCATION:Int = $0D
	Rem
	bbdoc: Picture of the artists during recording.
	End Rem
	Const TYPE_DURINGRECORDING:Int = $0E
	Rem
	bbdoc: Picture of the artists during performance.
	End Rem
	Const TYPE_DURINGPERFORMANCE:Int = $0F
	Rem
	bbdoc: Picture from a movie or video related to the track.
	End Rem
	Const TYPE_MOVIESCREENCAPTURE:Int = $10
	Rem
	bbdoc: Picture of a large, coloured fish.
	End Rem
	Const TYPE_COLOUREDFISH:Int = $11
	Rem
	bbdoc: Illustration related to the track.
	End Rem
	Const TYPE_ILLUSTRATION:Int = $12
	Rem
	bbdoc: Logo of the band or performer.
	End Rem
	Const TYPE_BANDLOGO:Int = $13
	Rem
	bbdoc: Logo of the publisher (record company).
	End Rem
	Const TYPE_PUBLISHERLOGO:Int = $14

	Method textEncoding:String()
	End Method
	
	Method setTextEncoding(encoding:String)
	End Method
	
	Method mimeType:String()
	End Method
	
	Method setMimeType(m:String)
	End Method
	
	Method imageType:Int()
	End Method
	
	Method setImageType(t:Int)
	End Method
	
	Method description:String()
	End Method
	
	Method setDescription(desc:String)
	End Method
	
	Method picture:TTLByteVector()
	End Method
	
	Method setPicture(data:TTLByteVector)
	End Method
	
End Type

Type TTLID3v2CommentsFrame Extends TTLID3v2Frame

End Type

Type TTLID3v2GeneralEncapsulatedObjectFrame Extends TTLID3v2Frame

End Type

Type TTLID3v2RelativeVolumeFrame Extends TTLID3v2Frame

End Type

Type TTLID3v2TextIdentificationFrame Extends TTLID3v2Frame

End Type

Type UserTextIdentificationFrame Extends TTLID3v2TextIdentificationFrame

End Type

Type TTLID3v2UniqueFileIdentifierFrame Extends TTLID3v2Frame

End Type

Type TTLID3v2UnknownFrame Extends TTLID3v2Frame

End Type

Type TTLID3v2UnsynchronizedLyricsFrame Extends TTLID3v2Frame

End Type

Type TTLID3v2UrlLinkFrame Extends TTLID3v2Frame

End Type

Type TTLID3v2UserUrlLinkFrame Extends TTLID3v2UrlLinkFrame

End Type

Rem
bbdoc: 
End Rem
Type TTLByteVector

	Field bvPtr:Byte Ptr
	
	Function _create:TTLByteVector(bvPtr:Byte Ptr)
		If bvPtr Then
			Local this:TTLByteVector = New TTLByteVector
			this.bvPtr = bvPtr
			Return this
		End If
	End Function
	
	Function CreateByteVector:TTLByteVector(data:Byte Ptr, length:Int)
		Return New TTLByteVector.Create(data, length)
	End Function
	
	Method Create:TTLByteVector(data:Byte Ptr, length:Int)
		'bvPtr = ...
		Return Self
	End Method

	Method setData(data:Byte Ptr, length:Int)
	End Method
	
	Method data:Byte Ptr()
	End Method
	
	Method clear()
	End Method
	
	Method size:Int()
	End Method

	Method isEmpty:Int()
	End Method

End Type



