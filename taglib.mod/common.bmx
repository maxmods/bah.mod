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

End Extern



Extern "c"
	Function strlen_( str:Byte Ptr )="strlen"
End Extern

' converts a UTF character array from byte-size characters to short-size characters
' based on the TextStream UTF code...
Function stringFromUTF8String:String(s:Byte Ptr)
	If s Then
		Local l:Int = strlen_(s)
		Local b:Short[] = New Short[l]
		Local bc:Int = -1, c:Int, d:Int, e:Int, f:Int
		For Local i:Int = 0 Until l
			bc:+1
			c = s[i]
			If c<128 
				b[bc] = c
				Continue
			End If
			i:+1
			d=s[i]
			If c<224 
				b[bc] = ((c & 31) Shl 6) | (d & 63)
				Continue
			End If
			i:+1
			e = s[i]
			If c < 240 
				b[bc] = ((c & 15) Shl 12) | ((d & 63) Shl  6) | (e & 63)
				Continue
			End If
			i:+1
			f = s[i]
			Local v:Int = (((c & 7) Shr 18) | ((d & 63) Shl 12) | ((e & 63) Shl  6) | (f & 63))
			If v < 65536 Then
				b[bc] = v
				Continue
			End If
			' requires 2 16-bit characters!
			b[bc] = ((v - 65536) / 1024) + 55296
			bc:+1
			b[bc] = (v Mod 1024) + 56320
		Next
		Return String.fromshorts(b, bc + 1)
	End If
	
	Return ""
	
End Function

' converts a Max short-based String to a byte-based UTF-8 String.
' based on the TextStream UTF code...
Function stringToUTF8String:String(text:String)
	If Not text Then
		Return ""
	End If
	
	Local l:Int = text.length
	If l = 0 Then
		Return ""
	End If
	
	Local count:Int = 0
	Local s:Byte[] = New Byte[l * 4] ' max possible is 4 x original size.
	
	For Local i:Int = 0 Until l
		Local char:Int = text[i]

		If char < 128 Then
			s[count] = char
			count:+ 1
			Continue
		Else If char<2048
			s[count] = char/64 | 192
			count:+ 1
			s[count] = char Mod 64 | 128
			count:+ 1
			Continue
		Else If char < 55296 Or char > 56319 ' not a surrogate pair?
			s[count] =  char/4096 | 224
			count:+ 1
			s[count] = char/64 Mod 64 | 128
			count:+ 1
			s[count] = char Mod 64 | 128
			count:+ 1
			Continue
		Else
			' this character built from pair of 16-bit characters 
			If i < (l-1) Then ' valid character?
				i:+1
				char = ((char - $D800) Shl 10) + (text[i] - $DC00) + $10000
			End If
		
			s[count] = char/262144 | 240
			count:+ 1
			s[count] = char/4096 Mod 64 | 128
			count:+ 1
			s[count] = char/64 Mod 64 | 128
			count:+1
			s[count] = char Mod 64 | 128
			count:+1
			Continue
		EndIf
		
	Next

	Return String.fromBytes(s, count)
End Function
