
SuperStrict

Framework BaH.Magick
Import BRL.GLmax2D


Graphics 1024, 768, 0
SetBlend alphablend

Local master:TMImage = TMImage.CreateFromFile("horse.jpg")

Local Second:TMImage = master.copy()
Second.zoom("640x480")

Local Third:TMImage = master.copy()
Third.zoom("800x600")


Local imgMaster:TImage = LoadImage(master.getPixmap()) 
Local imgSecond:TImage = LoadImage(Second.getPixmap()) 
Local imgThird:TImage = LoadImage(Third.getPixmap()) 


' did the image load ?
If imgMaster Then
	DebugLog("Loaded ok.")
Else
	DebugLog("Couldn't load...")
	End
End If


While Not KeyDown(KEY_ESCAPE)

	Cls
	
	DrawImage imgMaster, 10, 10
	DrawImage imgSecond, 150, 150
	DrawImage imgThird, 300, 300

	Flip	

Wend

End

