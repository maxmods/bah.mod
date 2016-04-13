SuperStrict

Framework bah.soloud

Local soloud:TSoloud = New TSoloud
Local speech:TSLSpeech = New TSLSpeech

speech.setText("1 2 3   1 2 3   Hello world. Welcome to So-Loud.")

soloud.init()

soloud.play(speech)
Delay 100 ' wait a wee bit for it to start playing, otherwise we might skip the while loop

While soloud.getActiveVoiceCount()
	Delay 100
Wend

soloud.deinit()

End
