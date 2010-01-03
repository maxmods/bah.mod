SuperStrict

Framework BaH.Speech
Import brl.StandardIO

Local speech:TSpeech = New TSpeech

Local s:String[] = TSpeech.availableVoices()
For Local i:Int = 0 Until s.length
	Print s[i]
Next

If s Then ' Choose the first voice, or use the default if there are none found.
	speech.setVoice(s[0])
End If

speech.speak("Hello, this is BaH dot Speech")

While speech.isSpeaking()
	Delay 10
Wend

End
