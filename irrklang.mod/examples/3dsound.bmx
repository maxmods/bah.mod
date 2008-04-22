SuperStrict

Framework BaH.irrKlang
Import BRL.StandardIO

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
Local music:TISound = _engine.Play3D("media/ophelia.mp3", Null, True, False, True)

' the following step isn't necessary, but to adjust the distance where
' the 3D sound can be heard, we set some nicer minimum distance
' (the default min distance is 1, for a small object). The minimum
' distance simply is the distance in which the sound gets played
' at maximum volume.
If music Then
	music.setMinDistance(5.0)
End If

' Print some help text and start the display loop


Delay(15000)

' don't forget to release the resources as explained above.
If music Then
	music.Drop() ' release music stream.
End If

_engine.Drop()

End
