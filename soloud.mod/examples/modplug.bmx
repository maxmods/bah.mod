SuperStrict

Framework bah.soloud
Import brl.standardio

Local soloud:TSoloud = New TSoloud
Local m:TSLModplug = New TSLModplug

Local res:Int = m.Load("audio/BRUCE.S3M")
If res Then
	Print soloud.getErrorString(res)
	End
End If


soloud.init()

soloud.play(m)
Delay 500 ' wait a wee bit for it to start playing, otherwise we might skip the while loop

While soloud.getActiveVoiceCount()
	Delay 100
Wend

soloud.deinit()

End

