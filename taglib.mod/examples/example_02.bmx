'
' Show framelists, tags and properties for files in a folder.
'
SuperStrict

Framework BaH.TagLib
Import BRL.StandardIO
Import BRL.FileSystem

' A folder which contains some audio files
Local path:String = "/Volumes/Audio Library/Pink Floyd/Dark Side Of The Moon/"
ChangeDir(path)

Local dir:Int = ReadDir(path)

If dir Then
	Local file:String = NextFile(dir)
	While file

		If file <> "." And file <> ".." Then
			ShowTagInfo(file)
		End If

		file = NextFile(dir)
	Wend
	CloseDir(dir)
End If

End


Function ShowTagInfo(filename:String)

	Local file:TTLMPEGFile = New TTLMPEGFile.Create(filename)
	
	If file.isValid() Then

		Print "******************** ~q" + filename + "~q ********************"

		Local id3v2tag:TTLID3v2Tag = file.ID3v2Tag()
		
		If id3v2tag Then
			Print "ID3v2." + id3v2tag.header().majorVersion() + "." + ..
				id3v2tag.header().revisionNumber() + ", " + id3v2tag.header().tagSize() + " bytes in tag"
				
			' TODO
		Else
			Print "file does not have a valid id3v2 tag"
		End If

		Print "~nID3v1"
		
		Local id3v1tag:TTLID3v1Tag = file.ID3v1Tag()
		
		If id3v1tag Then
			Print "title   - ~q" + id3v1tag.title()   + "~q"
			Print "artist  - ~q" + id3v1tag.artist()  + "~q"
			Print "album   - ~q" + id3v1tag.album()   + "~q"
			Print "year    - ~q" + id3v1tag.year()    + "~q"
			Print "comment - ~q" + id3v1tag.comment() + "~q"
			Print "track   - ~q" + id3v1tag.track()   + "~q"
			Print "genre   - ~q" + id3v1tag.genre()   + "~q"
		Else
			Print "file does not have a valid id3v1 tag"

		End If
		
		Print "~nAPE"
		
		Local ape:TTLAPETag = file.APETag()
		
		If ape Then
			' TODO
		Else
			Print "file does not have a valid APE tag"
		End If

		Print "~n"

		' free the file
		file.Free()
	End If

End Function

