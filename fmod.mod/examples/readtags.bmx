'
' This example shows how to read tags from sound files
'
'
SuperStrict

Framework BaH.FMOD
Import BRL.StandardIO

Local system:TFMODSystem = New TFMODSystem.Create()

system.Init(100)

Print "=================================================================="
Print "ReadTags Example."
Print "==================================================================~n"


' Open the specified file. Use FMOD_CREATESTREAM and FMOD_OPENONLY so it opens quickly
Local sound:TFMODSound = system.CreateSoundURL("media/wave.mp3", FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM | FMOD_OPENONLY)

' Read and display all tags associated with this file
While True

	' An index of -1 means "get the first tag that's new or updated".
	' If no tags are new or updated then getTag will return FMOD_ERR_TAGNOTFOUND.
	' This is the first time we've read any tags so they'll all be new but after we've read them, 
	' they won't be new any more.
	Local tag:TFMODTag = sound.GetTag(Null, -1)
	If Not tag Then
		Exit
	End If
	
	If tag.GetDataType() = FMOD_TAGDATATYPE_STRING Then
		Print tag.GetName() + " = " +  tag.GetDataAsString()
	Else
		Print tag.GetName() + " = <binary> (" + tag.GetDataLength() + " bytes)"
	End If
	
Wend

Print ""

Local numTags:Int
Local numTagsUpdated:Int

' Read all the tags regardless of whether they're updated or not. Also show the tag type.
sound.GetNumTags(numTags, numTagsUpdated)

For Local count:Int = 0 Until numTags

	Local tag:TFMODTag = sound.GetTag(Null, count)
	
	Local tagType:String
	
	Select tag.GetTagType()
		Case FMOD_TAGTYPE_UNKNOWN
			tagType = "FMOD_TAGTYPE_UNKNOWN  "
		Case FMOD_TAGTYPE_ID3V1
			tagType = "FMOD_TAGTYPE_ID3V1  "
		Case FMOD_TAGTYPE_ID3V2
			tagType = "FMOD_TAGTYPE_ID3V2  "
		Case FMOD_TAGTYPE_VORBISCOMMENT
			tagType = "FMOD_TAGTYPE_VORBISCOMMENT  "
		Case FMOD_TAGTYPE_SHOUTCAST
			tagType = "FMOD_TAGTYPE_SHOUTCAST  "
		Case FMOD_TAGTYPE_ICECAST
			tagType = "FMOD_TAGTYPE_ICECAST  "
		Case FMOD_TAGTYPE_ASF
			tagType = "FMOD_TAGTYPE_ASF  "
		Case FMOD_TAGTYPE_FMOD
			tagType = "FMOD_TAGTYPE_FMOD  "
		Case FMOD_TAGTYPE_USER
			tagType = "FMOD_TAGTYPE_USER  "
	End Select

	If tag.GetDataType() = FMOD_TAGDATATYPE_STRING Then
		Print tagType + tag.GetName() + " = " +  tag.GetDataAsString()
	Else
		Print tagType + tag.GetName() + " = <binary> (" + tag.GetDataLength() + " bytes)"
	End If
	
Next

Print ""

' Find a specific tag by name. Specify an index > 0 to get access to multiple tags of the same name.
Local tag:TFMODTag = sound.GetTag("ARTIST", 0)
Print tag.GetName() + " = " + tag.GetDataAsString()


' shut down
sound.SoundRelease()

system.Close()
system.SystemRelease()


End

