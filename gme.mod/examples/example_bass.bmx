' Example player for BaH.GME using BASS audio library.
'
' 
'
SuperStrict

Framework BaH.GMEBass
Import BRL.StandardIO
Import BRL.Max2D
Import BRL.GLMax2D

' initialize BASS
If Not TBass.Init(-1,44100,0,Null,Null) Then
	DebugLog "Can't initialize device : " + TBass.ErrorGetCode()
	End
End If

' create a player
Local player:TGMEPlayer = GetGMEPlayer()
' load some music
player.LoadFile("test.nsf", 44100)


' hopefully, everything loaded okay.
If Not player.IsOk() Then
	Print "Error..."
	End
End If

' how many tracks?
Local trackCount:Int = player.TrackCount()
Local currentTrack:Int = 0

' start playing!
player.Play(currentTrack)


Local playing:Int = True

	
Graphics 640, 480, 0

'		Local active:Int = stream.IsActive()

While Not KeyDown(key_escape)

	If KeyHit(KEY_UP) Then
		currentTrack:+ 1
		If currentTrack = trackCount Then
			currentTrack = 0
		End If
		
		player.Play(currentTrack)
		
	Else If KeyHit(KEY_DOWN) Then
		currentTrack:- 1
		If currentTrack < 0 Then
			currentTrack = trackCount - 1
		End If

		player.Play(currentTrack)
	ElseIf KeyDown(KEY_EQUALS) Then
		Local vol:Float = player.GetVolume()
		vol:+ 0.01
		If vol > 1 Then
			vol = 1
		End If
		player.SetVolume(vol)
	ElseIf KeyDown(KEY_MINUS) Then
		Local vol:Float = player.GetVolume()
		vol:- 0.01
		If vol < 0 Then
			vol = 0
		End If
		player.SetVolume(vol)
	End If
	
	If KeyHit(KEY_SPACE) Then
		If playing Then
			player.Pause()
		Else
			player.Resume()
		End If
		playing = Not playing
	End If


	Cls
	
		DrawText "Escape to Quit", 100, 10
		
		DrawText "UP - Next Track     DOWN - Previous Track", 30, 30
		DrawText "SPACE - Pause/Play          -/+ - Change Volume", 30, 50
		
		DrawText "Track " + (currentTrack+1) + " of " + trackCount, 30, 80
		If playing Then
			DrawText "(Playing)", 30, 100
		Else
			DrawText "(Paused)", 30, 100
		End If
		
		DrawText "Volume : " + player.GetVolume(), 30, 130
	Flip
	
Wend
	

TBass.Free()

End

