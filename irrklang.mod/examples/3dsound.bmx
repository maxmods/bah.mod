' This example will show how to play sounds in 3D space using irrKlang.
' An mp3 file file be played in 3D space and moved around the user and a
' sound will be played at a random 3D position every time the user presses
' a key.
'
SuperStrict

Framework BaH.irrKlang
Import BRL.StandardIO
Import BRL.GLMax2D
?win32
Import BRL.D3D7Max2D
?
Import BRL.Random

' start the sound engine with default parameters
Local _engine:TISoundEngine = CreateIrrKlangDevice()

If Not _engine Then
	Print "Could not startup engine"
	End
End If

' Now play some sound stream as music in 3d space, looped.
' We are setting the last parameter named 'track' to 'true' to
' make irrKlang return a pointer to the played sound. (This is also returned
' if the parameter 'startPaused' is set to true, by the way). Note that you
' MUST call ->drop to the returned pointer if you don't need it any longer and
' don't want to waste any memory. This is done in the end of the program.
Local music:TISound = _engine.Play3D("media/ophelia.mp3", TIVec3D.Zero(), True, False, True)

' the following step isn't necessary, but to adjust the distance where
' the 3D sound can be heard, we set some nicer minimum distance
' (the default min distance is 1, for a small object). The minimum
' distance simply is the distance in which the sound gets played
' at maximum volume.
If music Then
	music.setMinDistance(5.0)
End If

Local posOnCircle:Float = 0
Const radius:Float = 5.0


Graphics 640, 480, 0


While Not KeyDown(KEY_ESCAPE)

	Cls
	
	DrawText "Playing streamed sound in 3D.", 50, 30
	DrawText "Press ESCAPE to quit, SPACE to play sound at random position.", 50, 60
	
	DrawText "+ = Listener position", 50, 90
	DrawText "o = Playing sound", 50, 110

	' Each step we calculate the position of the 3D music.
	' For this example, we let the
	' music position rotate on a circle:
	posOnCircle :+ 2.3
	posOnCircle = posOnCircle Mod 360
	
	Local pos3d:TIVec3D = New TIVec3D.Create(radius * Cos(posOnCircle), 0, radius * Sin(posOnCircle * 0.5))

	' After we know the positions, we need to let irrKlang know about the
	' listener position (always position (0,0,0), facing forward in this example)
	' and let irrKlang know about our calculated 3D music position
	_engine.SetListenerPosition(TIVec3D.Zero(), New TIVec3D.Create(0,0,1))

	If music Then
		music.SetPosition(pos3d)
	End If

	' Now display the position of the sound
	DrawText "[          +         ]", 50, 150
	Local charpos:Int = (pos3d.X() + radius) / radius * 10.0
	If charpos >= 0 And charpos < 20 Then
		DrawText "o", 58 + charpos * 8, 150
	End If
	
	Local playpos:Int
	If music Then
		playpos = music.GetPlayPosition()
	End If
	
	DrawText "3dpos: " + pos3d.X() + " " + pos3d.Y() + " " + pos3d.Z(), 250, 150
	DrawText "playpos: " + Pad(playPos/60000) + ":" + Pad((playPos Mod 60000)/1000), 250, 170

	Flip

	Delay(100)
	
	
	If KeyHit(KEY_SPACE) Then
		' Play random sound at some random position.
		' Note that when calling Play3D(), no obejct is returned because we didn't
		' specify the sound to start paused or to track it (as we did above
		' with the music), so we also don't need to call Drop().
		
		Local pos:TIVec3D = New TIVec3D.Create(Rand(-radius, radius), 0, 0)
		Local filename:String
		If Rand(0) = 1 Then
			filename = "media/bell.wav"
		Else
			filename = "media/explosion.wav"
		End If
		
		_engine.Play3D(filename, pos)
	End If
	
Wend


' don't forget to release the resources as explained above.
If music Then
	music.Drop() ' release music stream.
End If

_engine.Drop()

End

Function Pad:String(val:Int, size:Int = 2)
	Local s:String = "00" + val
	Return s[s.length - size..]
End Function
