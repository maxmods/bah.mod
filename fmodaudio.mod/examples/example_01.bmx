SuperStrict

Framework BaH.FMODAudio
Import BRL.StandardIO

Local drivers:String[] = AudioDrivers()

Print "Audio Drivers : "
For Local i:Int = 0 Until drivers.length
	Print drivers[i]
Next

SetAudioDriver("FMOD")

Local sound:TSound = LoadSound("../../fmod.mod/examples/media/drumloop.wav")

PlaySound(sound)

Local i:Int

While i < 30

	PollSystem ' important this or something that uses it (keydown etc) is called 

	Delay(100)
	i:+ 1
Wend