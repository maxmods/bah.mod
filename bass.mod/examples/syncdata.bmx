' sync data test
SuperStrict

Framework BaH.Bass
Import BRL.GLMax2D
Import brl.StandardIO

Graphics 640,480

If Not TBass.Init(-1,44100,0,Null,Null) Then
	Print "Can't initialize device : " + TBass.ErrorGetCode()
	End
End If

Global syncData:TBassSyncData = New TBassSyncData

Global file:TStream
Global stream:TBassStream
Global SongName:String[]=["resources/bass_track_baby.mp3","resources/bass_track_baby.mp3"]
Local _exit:Int=False
Local current_song:Int=0

Global stream_size:Long

InitStream(current_song)
Local active:Int

While _exit=False

	If stream

		active = stream.IsActive()
				
		While stream.play(False)' And active
			Local _left:Int, _right:Int

			stream.GetLevel(_left, _right)
			Local lwidth:Int = _left / 410.0
			Cls
			DrawText "Song     - " + songName[current_song]+" "+current_song,50,30

			DrawRect 100 - lwidth, 100, lwidth, 20
			DrawRect 110, 100, _right / 410.0, 20

			Local pos:Long = stream.GetPosition(BASS_POS_BYTE)
			Local time:Double = stream.Bytes2Seconds(pos)
			DrawText "position - " + pos, 50, 50
			DrawText "elapsed  - " + Int(time/60) + ":" + Int(time Mod 60), 50, 70
			DrawText "Active: "+active,5,0

			active = stream.IsActive()
		
			If KeyHit(key_enter) ' move quite the end of the songs...
				stream.SetPosition(95 *Float(stream_size:Long)/100, BASS_POS_BYTE)
			End If
				
			If syncData.IsSet() Or KeyHit(key_Space) 'change the music
				current_song:+1
				If current_song>1 current_song=0
				StopMusic()
				InitStream(current_song)
			End If
			Delay 50
			Flip


		If KeyHit(KEY_ESCAPE)
			_Exit=True
			Exit
		End If

		Wend
	Else
		Print "NO stream!"
	End If
	
	Print "Finish playing..."
	

Wend
TBass.Free()

End

Function StopMusic()
'	Print "Clear all..."
	If stream stream.Free();stream=Null
	If file CloseStream(file);file=Null
	
End Function
Function InitStream(song:Int=0)
file = ReadStream(SongName[song])
	Print "Read <"+songName[song]+">"
	If file
		stream= New TBassStream.CreateTStream(file, 0,0)' BASS_SAMPLE_FX)
		Print "Stream Init..."
		If stream=Null Print "Problem!!!"
		stream_size=stream.GetLength(BASS_POS_BYTE)

		stream.SetSync(BASS_SYNC_END, 0, syncData) ' sync for stream end
		
	End If
End Function

