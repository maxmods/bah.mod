SuperStrict

Framework BaH.Magick
Import BRL.GLmax2D
Import BRL.RamStream


Incbin "hippo.jpg"


Graphics 800, 600, 0
SetBlend alphablend

Local image:TImage = LoadImage("incbin::hippo.jpg")

' did the image load ?
If image Then
	DebugLog("Loaded ok.")
Else
	DebugLog("Couldn't load...")
	End
End If


While Not KeyDown(KEY_ESCAPE)

	Cls
	
	DrawImage image, 50, 50

	Flip	

Wend

End