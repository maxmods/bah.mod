'
' Cue the music !
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

Local channel:TChannel = CueSound(sound)

Print "waiting..."

 ' wait a while
Delay(500)

Print "playing..."

' start playing !
ResumeChannel(channel)

Local i:Int

While i < 30

	Delay(100)
	i:+ 1
Wend
