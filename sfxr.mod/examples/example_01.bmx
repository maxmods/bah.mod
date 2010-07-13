SuperStrict

Framework BaH.Sfxr
Import BRL.FreeAudioAudio

' use our basic example settings handler.
Import "basic_handler.bmx"


' defaults never seem to work here, so set to something that does for me..
' your mileage of course, may vary :-)
?win32
SetAudioDriver("FreeAudio DirectSound")
?


' create
Local sfx:TSfxr = TSfxr.Create()


' load some saved data using our basic handler
Local stream:TStream = ReadStream("coin.txt")
sfx.LoadSettings(stream, True)
stream.close()


' generate the audio
Local bank:TBank = sfx.GenerateSoundData(SND_SHORT, FREQ_44100)



' second parameter is number of samples... for 'short' samples, that's 2 bytes per sample... so we divide by 2
Local sample:TAudioSample = TAudioSample.CreateStatic( bank.buf(), bank.size() / 2 , FREQ_44100, SF_MONO16LE )



' usual sound/channel stuff so that we can hear it :-)
Local sound:TSound=LoadSound( sample, False )

Local channel:TChannel = PlaySound(sound)

While channel.playing()
	Delay 1000
Wend

End
