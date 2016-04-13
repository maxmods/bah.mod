SuperStrict

Framework bah.soloud

Incbin "audio/war_loop.ogg"

Local soloud:TSoloud = New TSoloud
Local wav:TSLWavStream = New TSLWavStream

wav.loadMem(IncbinPtr("audio/war_loop.ogg"), IncbinLen("audio/war_loop.ogg"), False, False)


soloud.init()

soloud.play(wav)
Delay 100 ' wait a wee bit for it to start playing, otherwise we might skip the while loop

While soloud.getActiveVoiceCount()
	Delay 100
Wend

soloud.deinit()

End

