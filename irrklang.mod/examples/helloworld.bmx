' This example will show how to play sounds using irrKlang.
' It will play a looped background music and a sound every
' time the user presses a key.
'
SuperStrict

Framework BaH.irrKlang
Import BRL.StandardIO
Import BRL.GLMax2D
?win32
Import BRL.D3D7Max2D
?

' start the sound engine with default parameters
Local _engine:TISoundEngine = CreateIrrKlangDevice()

If Not _engine Then
	Print "Could not startup engine"
	End
End If

' To play a sound, we only need to call Play2D(). The second parameter tells the engine to play it looped.
Local s:TISound = _engine.Play2D("media/getout.ogg", True, True)
'DebugStop

Graphics 640,480,0

' In a loop, wait until user presses 'q' to exit or another key to play another sound.

While Not KeyDown(KEY_Q)

	Cls

	DrawText "Press any key to play some sound, press 'q' to quit.", 50, 20

	Flip

	Local key:Int = WaitKey()

	If key <> KEY_Q Then
		' play a single sound
		_engine.Play2D("media/bell.wav")
	End If
	
Wend

' After we are finished, we have to delete the irrKlang Device created earlier
' with CreateIrrKlangDevice(). Use Drop() to do that. In irrKlang, you should
' delete all objects you created with a method or function that starts with 'create'.
' (an exception is the Play2D()- or Play3D()-method, see the documentation or the
' next example for an explanation)
' The object is deleted simply by calling .Drop().

_engine.Drop()

End

