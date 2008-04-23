'
' Play some incbin'd audio !
'
SuperStrict

Framework BaH.irrKlangAudio
Import BRL.StandardIO

' incbin the audio
Incbin "../../irrklang.mod/examples/media/getout.ogg"


Local drivers:String[] = AudioDrivers()

Print "Audio Drivers : "
For Local i:Int = 0 Until drivers.length
	Print drivers[i]
Next

SetAudioDriver("irrKlang")
'DebugStop
Local sound:TSound = LoadSound("incbin::../../irrklang.mod/examples/media/getout.ogg", SOUND_LOOP)

PlaySound(sound)

Local i:Int

While i < 30

	Delay(100)
	i:+ 1
Wend
