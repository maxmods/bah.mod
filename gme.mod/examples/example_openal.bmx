' Example player for BaH.GME using OpenAL audio library.
'
' 
'
SuperStrict

Framework BaH.GMEOpenAL
Import BRL.StandardIO
Import BRL.Max2D
Import BRL.GLMax2D

' create a player
Local player:TGMEPlayer = GetGMEPlayer()
' load some music
player.LoadFile("Milkrace.ay", 44100)


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
Local info:TGMETrackInfo = player.GetTrackInfo(currentTrack)
	
Graphics 640, 480, 0

'		Local active:Int = stream.IsActive()

While Not KeyDown(key_escape)

	player.Update()
	
	If playing And player.TrackEnded() Then
		playing = False
	End If

	If KeyHit(KEY_UP) Then
		currentTrack:+ 1
		If currentTrack = trackCount Then
			currentTrack = 0
		End If
		
		player.Play(currentTrack)
		
		info = player.GetTrackInfo(currentTrack)
		
	Else If KeyHit(KEY_DOWN) Then
		currentTrack:- 1
		If currentTrack < 0 Then
			currentTrack = trackCount - 1
		End If

		player.Play(currentTrack)

		info = player.GetTrackInfo(currentTrack)

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
			If player.TrackEnded() Then
				player.Play(currentTrack)
			Else
				player.Resume()
			End If
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
			If player.TrackEnded() Then
				DrawText "(Finished)", 30, 100
			Else
				DrawText "(Paused)", 30, 100
			End If
		End If
		
		DrawText "Volume : " + player.GetVolume(), 30, 130
		
		If info Then
			DrawText "Info :", 30, 180
			DrawText "System    :  " + info.System(), 60, 200
			DrawText "Game      :  " + info.Game(), 60, 215
			DrawText "Song      :  " + info.Song(), 60, 230
			DrawText "Author    :  " + info.Author(), 60, 245
			DrawText "Copyright :  " + info.Copyright(), 60, 260
			DrawText "Comment   :  " + info.Comment(), 60, 275
			DrawText "Dumper    :  " + info.Dumper(), 60, 290
		End If
	Flip
	
Wend


End

