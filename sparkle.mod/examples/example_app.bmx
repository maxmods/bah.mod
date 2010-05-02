SuperStrict

Framework BaH.Sparkle
Import BRL.GLMax2D

SparkleInit

Graphics 800, 600, 0

Local version:String = "1.0.0"

While Not KeyDown(KEY_ESCAPE) And Not AppTerminate()

	Cls
	
	DrawText "Hello, I'm version " + version, 300, 300
	
	DrawText "Escape to Quit", 680, 580
	
	Flip

Wend

End
