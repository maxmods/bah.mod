' This example will show how to play sounds directly from memory using irrKlang.

' This is useful for embedding sounds directly in executables.
'
SuperStrict

Framework BaH.irrKlang
Import BRL.StandardIO
Import BRL.GLMax2D
?win32
Import BRL.D3D7Max2D
?

Incbin "media/getout.ogg"


' start the sound engine with default parameters
Local _engine:TISoundEngine = CreateIrrKlangDevice()

If Not _engine Then
	Print "Could not startup engine"
	End
End If


' To make irrKlang know about the memory we want to play, we register

' the memory chunk as a sound source. We specify the name "testsound", so

' we can use the name later for playing back the sound. Note that you

' could also specify a better fitting name like "ok.wav".

' The method addSoundSource() also returns a pointer to the created sound source,

' it can be used as parameter for play2D() later, if you don't want to

' play sounds via string names.
_engine.AddSoundSourceFromMemory(IncbinPtr("media/getout.ogg"), IncbinLen("media/getout.ogg"), "testsound")

' play the sound we added to memory
_engine.Play2D("testsound")

Graphics 640,480,0

' loop until user exits
While Not KeyDown(KEY_ESCAPE)

	Cls

	DrawText "Playing sound from memory.", 50, 20
	DrawText "Press any key to play, ESCAPE to end program.", 50, 40

	Flip

	Delay(100)	
Wend

' delete engine
_engine.Drop()

End


