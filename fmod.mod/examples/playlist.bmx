'
' This example shows how to load a playlist and play the sounds in a playlist.
'
'
SuperStrict

Framework BaH.FMOD
Import BRL.GLMax2d

Local system:TFMODSystem = New TFMODSystem.Create()

system.Init(32)

Local sound:TFMODSound
Local playlist:TFMODSound = system.CreateSoundURL("media/playlist.m3u", FMOD_DEFAULT)

Local soundType:Int
playlist.GetSoundType(soundType)

Local title:String
Local count:Int
Local isPlaylist:Int = False
Local tag:TFMODTag = Null

Local channel:TFMODChannel

If soundType = FMOD_SOUND_TYPE_PLAYLIST Then
	isPlayList = True
	
	' Get the first song in the playlist, create the sound and then play it.
	
	tag = playlist.GetTag("FILE", count)
	sound = system.CreateSoundURL("media/" + tag.GetDataAsString(), FMOD_DEFAULT)
	channel = system.PlaySound(FMOD_CHANNEL_FREE, sound)

	tag = playlist.GetTag("TITLE", count)
	title = tag.GetDataAsString()

	count:+ 1
Else
	' This is just a normal sound, so just play it.
	
	sound = playlist
	sound.SetMode(FMOD_LOOP_NORMAL)
	channel = system.PlaySound(FMOD_CHANNEL_FREE, sound)


End If


Graphics 640, 480, 0

While Not KeyDown(KEY_ESCAPE)

	Cls
	
		DrawText "===================================================================", 50, 10
		DrawText "PlayList Example.", 50, 30
		DrawText "===================================================================", 50, 50
		DrawText "Press 'n'     to play next sound in playlist", 50, 70
		DrawText "Press 'space' to pause/unpause current sound", 50, 90
		DrawText "Press 'Esc' to quit", 50, 130
		
		Local isPlaying:Int = False

		If channel And isPlayList Then
			
			' When sound has finished playing, play the next sound in the playlist
			channel.IsPlaying(isPlaying)
			
			If Not isPlaying Then
				If sound Then
					sound.SoundRelease()
					sound = Null
				End If
				
				tag = playlist.GetTag("FILE", count)

				If Not tag Then
					count = 0
				Else
					sound = system.CreateSoundURL("media/" + tag.GetDataAsString(), FMOD_DEFAULT)
					channel = system.PlaySound(FMOD_CHANNEL_FREE, sound)

					tag = playlist.GetTag("TITLE", count)
					title = tag.GetDataAsString()
					
					count:+ 1

				End If
			End If
			
		End If
		
		If KeyHit(KEY_N) Then
		
			' Play the next song in the playlist
			If channel And isPlayList Then
				channel.Stop()
			End If
			
		End If
		
		If KeyHit(KEY_SPACE) Then
			If channel Then
				Local paused:Int
				
				channel.GetPaused(paused)
				channel.SetPaused(Not paused)
			End If
		End If
		
	
		system.Update()
	
		Local paused:Int
		Local pos:Int
		Local lenms:Int
		
		If channel Then
			channel.GetPaused(paused)
			
			channel.GetPosition(pos, FMOD_TIMEUNIT_MS)
			
			Local currentSound:TFMODSound = channel.GetCurrentSound()			
			If currentSound Then
				currentSound.GetLength(lenms, FMOD_TIMEUNIT_MS)
			End If

		End If
		
		Local s:String = "Time " + Pad(pos / 1000 / 60) + ":" + Pad(pos / 1000 Mod 60) + ..
			":" + Pad(pos / 10 Mod 100) + "/" + Pad(lenms / 1000 / 60) + ":" + Pad(lenms / 1000 Mod 60) + ..
			":" + Pad(lenms / 10 Mod 100)
			
		If paused Then
			s:+ " : Paused "
		Else If isPlaying Then
			s:+ " : Playing"
		Else
			s:+ " : Stopped"
		End If
		
		s:+ " : " + title

		DrawText s, 50, 200

	Flip

Wend

' shut down
If sound Then
	sound.SoundRelease()
End If

If isPlaylist Then
	playlist.SoundRelease()
End If

system.Close()
system.SystemRelease()


End

Function Pad:String(val:Int, size:Int = 2)
	Local s:String = "00" + val
	Return s[s.length - size..]
End Function

