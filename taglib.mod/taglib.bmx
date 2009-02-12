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

ModuleInfo "History: 1.00 Initial Release (TagLib 1.5 SVN rev 925226)"

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
		fileRefPtr = bmx_taglib_fileref_create(bbStringToUTF8String(filename), readAudioProperties, audioPropertiesStyle)
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
	
	Method Delete()
		If fileRefPtr Then
			bmx_taglib_fileref_delete(fileRefPtr)
			fileRefPtr = Null
		End If
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
		bmx_taglib_tag_settitle(tagPtr, bbStringToUTF8String(value))
	End Method
	
	Rem
	bbdoc: Sets the artist to @value.
	about: If @value is Null then this value will be cleared.
	End Rem
	Method setArtist(value:String)
		bmx_taglib_tag_setartist(tagPtr, bbStringToUTF8String(value))
	End Method
	
	Rem
	bbdoc: Sets the album to @value.
	about: If @value is Null then this value will be cleared.
	End Rem
	Method setAlbum(value:String)
		bmx_taglib_tag_setalbum(tagPtr, bbStringToUTF8String(value))
	End Method
	
	Rem
	bbdoc: Sets the comment to @value.
	about: If @value is Null then this value will be cleared.
	End Rem
	Method setComment(value:String)
		bmx_taglib_tag_setcomment(tagPtr, bbStringToUTF8String(value))
	End Method
	
	Rem
	bbdoc: Sets the genre to @value.
	about: If @value is Null then this value will be cleared.
	End Rem
	Method setGenre(value:String)
		bmx_taglib_tag_setgenre(tagPtr, bbStringToUTF8String(value))
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

