'
' This example shows how to simply load and play multiple sounds.  This is about the simplest
' use of FMOD.
' This makes FMOD decode the into memory when it loads.  If the sounds are big and possibly take
' up a lot of ram, then it would be better to use the FMOD_CREATESTREAM flag so that it is 
' streamed in realtime as it plays.
'
'
SuperStrict

Framework BaH.FMOD
Import BRL.GLMax2d

Local system:TFMODSystem = New TFMODSystem.Create()

system.Init(32)

Local sound1:TFMODSound = system.CreateSoundURL("media/drumloop.wav", FMOD_SOFTWARE)
sound1.SetMode(FMOD_LOOP_OFF)

Local sound2:TFMODSound = system.CreateSoundURL("media/jaguar.wav", FMOD_SOFTWARE)

Local sound3:TFMODSound = system.CreateSoundURL("media/swish.wav", FMOD_SOFTWARE)

Local channel:TFMODChannel

Graphics 640, 480, 0

While Not KeyDown(KEY_ESCAPE)

	Cls
	
		DrawText "===================================================================", 50, 10
		DrawText "PlaySound Example.", 50, 30
		DrawText "===================================================================", 50, 50
		DrawText "Press '1' to Play a mono sound using software mixing", 50, 70
		DrawText "Press '2' to Play a mono sound using software mixing", 50, 90
		DrawText "Press '3' to Play a stereo sound using software mixing", 50, 110
		DrawText "Press 'Esc' to quit", 50, 130

		If KeyHit(KEY_1) Then
			channel = system.PlaySound(FMOD_CHANNEL_FREE, sound1)
		End If

		If KeyHit(KEY_2) Then
			channel = system.PlaySound(FMOD_CHANNEL_FREE, sound2)
		End If

		If KeyHit(KEY_3) Then
			channel = system.PlaySound(FMOD_CHANNEL_FREE, sound3)
		End If
	
		system.Update()
	
		Local playing:Int
		Local paused:Int
		Local pos:Int
		Local lenms:Int
		Local channelsPlaying:Int
		
		If channel Then
			channel.IsPlaying(playing)
			
			channel.GetPaused(paused)
			
			channel.GetPosition(pos, FMOD_TIMEUNIT_MS)
			
			Local currentSound:TFMODSound = channel.GetCurrentSound()			
			If currentSound Then
				currentSound.GetLength(lenms, FMOD_TIMEUNIT_MS)
			End If

		End If
		
		system.GetChannelsPlaying(channelsPlaying)
		
		Local s:String = "Time " + Pad(pos / 1000 / 60) + ":" + Pad(pos / 1000 Mod 60) + ..
			":" + Pad(pos / 10 Mod 100) + " /" + Pad(lenms / 1000 / 60) + ":" + Pad(lenms / 1000 Mod 60) + ..
			":" + Pad(lenms / 10 Mod 100)
			
		If paused Then
			s:+ " : Paused"
		Else If playing Then
			s:+ " : Playing"
		Else
			s:+ " : Stopped"
		End If
		
		s:+ " : Channels Playing : " + channelsPlaying
		
		DrawText s, 50, 200
		
	Flip

Wend

' shut down
sound1.SoundRelease()
sound2.SoundRelease()
sound3.SoundRelease()

system.Close()
system.SystemRelease()


End

Function Pad:String(val:Int, size:Int = 2)
	Local s:String = "00" + val
	Return s[s.length - size..]
End Function
