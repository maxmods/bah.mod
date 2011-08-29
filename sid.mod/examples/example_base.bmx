'
' SID player base for importing of examples.
' Build one of the audio-specific examples instead. e.g. sidbass
'
'
SuperStrict

Import bah.sid
Import brl.glmax2d

Const mpath:String = "sid.mod/examples/music/"

Type TPlayer

	Field sid:TSID
	Field name:String
	Field trackCount:Int
	Field currentTrack:Int
	
	Field tunes:String[] = ["Ocean_Loader_1.sid", "Hyper_Sports.sid", "Comic_Bakery.sid", "Armalyte.sid"]
	Field tracks:Int[] = [0, 25, 0, 0]
	Field currentTune:Int
	
	Method Run()
		Init()
		
		Local toggle:Int
		While Not KeyDown(key_escape)
		
			sid.Update()
		
			Cls
			
			If KeyHit(KEY_UP) Then
				If currentTrack < trackCount - 1 Then
					currentTrack :+ 1
		
					sid.Play(currentTrack)
		
				End If
				
			End If
		
			If KeyHit(KEY_DOWN) Then
				If currentTrack > 0 Then
					currentTrack :- 1
		
					sid.Play(currentTrack)
				End If
				
			End If
			
			If KeyHit(key_space) Then
				sid.stop()
			End If
			
			If KeyHit(KEY_RETURN) Then
				currentTune :+ 1
				
				If currentTune >= tunes.length Then
					currentTune = 0
				End If
				
				Load(tunes[currentTune])
			End If
		
			DrawText name, 10, 10
		
			DrawText "Total Tracks  = " + trackCount, 10, 30
			DrawText "Current Track = " + (currentTrack + 1), 10, 50
			
			
			Flip
			
			
		Wend
		
	End Method
	
	Method Init()
		Graphics 640, 480, 0

		Load(tunes[0])
	End Method
	
	Method Load(music:String)
		sid = TSID.Load("../../" + mpath + music)
		
		If sid Then
			name = sid.songName()
			trackCount = sid.TrackCount()
			currentTrack = tracks[currentTune]
		
			' start playing!
			sid.Play(currentTrack)
			sid.SetVolume(0.4)
		Else
			DebugLog "Loading failed : " + music
			End
		End If
	End Method


End Type
