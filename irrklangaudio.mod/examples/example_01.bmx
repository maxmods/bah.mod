'
' Play some audio !
'
SuperStrict

Framework BaH.irrKlangAudio
Import BRL.StandardIO

Local drivers:String[] = AudioDrivers()

Print "Audio Drivers : "
For Local i:Int = 0 Until drivers.length
	Print drivers[i]
Next

SetAudioDriver("irrKlang")

Local sound:TSound = LoadSound("../../irrklang.mod/examples/media/getout.ogg", SOUND_LOOP)

PlaySound(sound)

Local i:Int

While i < 300

	Delay(100)
	i:+ 1
Wend
