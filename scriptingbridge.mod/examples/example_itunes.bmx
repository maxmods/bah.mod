SuperStrict

Framework BaH.ScriptingBridge
Import BRL.StandardIO

Local itunes:SBApplication = SBApplication.applicationWithBundleIdentifier("com.apple.iTunes")

Print itunes.propertyAsString("name")

Local track:SBObject = itunes.propertyAsObject("currentTrack")

If track Then

	Print "Album    : " + track.propertyAsString("album")
	Print "Track    : " + track.propertyAsString("name")
	Print "Duration : " + track.propertyAsDouble("duration")
	Print "Size     : " + track.propertyAsInt("size") + " bytes"


End If

' playlist?

Local playlist:SBObject = itunes.propertyAsObject("currentPlaylist")

If playlist Then
	Print "~n"
	
	Print "Playlist : " + playlist.propertyAsString("name")
	Print "Duration : " + playlist.propertyAsDouble("duration")
	
	' retrieve all tracks for the current playlist
	Local tracks:SBElementArray = playlist.propertyAsList("tracks")
	
	Local count:Int = 0
	
	' iterate through the tracks
	For track:SBObject = EachIn tracks
	
		Print "Name    : " + track.propertyAsString("name")
	
		If count > 10 Then ' don't want to go on forever!
			Exit
		End If
		count:+ 1
	Next
		
End If

