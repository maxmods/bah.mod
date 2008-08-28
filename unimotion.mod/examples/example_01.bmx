SuperStrict

Framework BaH.Unimotion
Import BRL.GLMax2D

Graphics 640, 480, 0

Local sms:TSMS = TSMS.Create()


While Not KeyDown(KEY_ESCAPE)

	Cls

		Local x:Double, y:Double, z:Double
		sms.ReadReal(x, y, z)
		
		DrawText "x = " + x, 150, 100
		DrawText "y = " + y, 150, 120
		DrawText "z = " + z, 150, 140
	
	Flip

Wend


