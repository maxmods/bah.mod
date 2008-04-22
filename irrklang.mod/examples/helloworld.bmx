SuperStrict

Framework BaH.irrKlang
Import BRL.StandardIO

' start the sound engine with default parameters
Local _engine:TISoundEngine = CreateIrrKlangDevice()

If Not _engine Then
	Print "Could not startup engine"
	End
End If

' To play a sound, we only to call play2D(). The second parameter tells the engine to play it looped.
_engine.Play2D("media/getout.ogg", True)

' In a loop, wait until user presses 'q' to exit or another key to play another sound.


Delay(15000)

_engine.Drop()

End