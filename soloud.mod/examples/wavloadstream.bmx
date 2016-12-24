SuperStrict

Framework bah.soloud

Local stream:TStream = ReadStream("audio/war_loop.ogg")

Local soloud:TSoloud = New TSoloud
Local wav:TSLWav = New TSLWav

'wav.loadMem(IncbinPtr("audio/war_loop.ogg"), IncbinLen("audio/war_loop.ogg"), False, False)
wav.loadStream(stream)


soloud.init()

soloud.play(wav)
Delay 100 ' wait a wee bit for it to start playing, otherwise we might skip the while loop

While soloud.getActiveVoiceCount()
	Delay 100
Wend

wav.destroy()

soloud.deinit()

End


