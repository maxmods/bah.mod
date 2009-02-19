'
' Show tags and properties for files in a folder.
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

		ShowFileDetails(file)

		file = NextFile(dir)
	Wend
	CloseDir(dir)
End If

End

Function ShowFileDetails(filename:String)

	Local file:TTLFileRef = New TTLFileRef.Create(filename)
	
	If Not file.isNull() Then
	
		Print "******************** ~q" + filename + "~q ********************"
	
		Local tag:TTLTag = file.tag()
		
		If tag Then
		
			Print "-- TAG --"
			Print "title   - ~q" + tag.title()   + "~q"
			Print "artist  - ~q" + tag.artist()  + "~q"
			Print "album   - ~q" + tag.album()   + "~q"
			Print "year    - ~q" + tag.year()    + "~q"
			Print "comment - ~q" + tag.comment() + "~q"
			Print "track   - ~q" + tag.track()   + "~q"
			Print "genre   - ~q" + tag.genre()   + "~q"

		End If
	
		Local properties:TTLAudioProperties = file.audioProperties()
		
		If properties Then
		
			Local seconds:Int = properties.length() Mod 60
			Local minutes:Int = (properties.length() - seconds) / 60
			
			Print "-- AUDIO --"
			Print "bitrate     - " + properties.bitrate()
			Print "sample rate - " + properties.sampleRate()
			Print "channels    - " + properties.channels()
			If seconds < 10 Then
				Print "length      - " + minutes + ":0" + seconds
			Else
				Print "length      - " + minutes + ":" + seconds
			End If
		
		End If
	
		Print "~n"
		
		' free the file
		file.Free()
	End If

End Function
