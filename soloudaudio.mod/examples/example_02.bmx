'
' Cue the music !
'
SuperStrict

Framework BaH.SoloudAudio
Import BRL.StandardIO
'Import brl.polledinput

Local drivers:String[] = AudioDrivers()

Print "Audio Drivers : "
For Local i:Int = 0 Until drivers.length
	Print drivers[i]
Next

SetAudioDriver("SoLoud")

Local sound:TSound = LoadSound("../../soloud.mod/examples/audio/delphi_loop.ogg")

Local channel:TChannel = CueSound(sound)

 ' wait a while
Delay(500)

' start playing !
ResumeChannel(channel)

Local i:Int

While i < 30

	'PollSystem ' important this or something that uses it (keydown etc) is called 

	Delay(100)
	i:+ 1
Wend


