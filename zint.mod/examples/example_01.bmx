SuperStrict

Framework brl.standardio
Import BaH.Zint
Import brl.glmax2d

Local bc:TZBarcode = New TZBarcode

bc.SetSymbology(BARCODE_ISBNX)
bc.SetScale(2)

Local pix:TPixmap = bc.Encode("9781484225226")

If pix Then

	Graphics 800, 600, 0
	
	Local image:TImage = LoadImage(pix)
	
	SetClsColor 255, 255, 255
	SetColor 0, 0, 0
	
	While Not KeyDown(key_escape)

		Cls
		
		DrawImage image, 100, 100
		
		Flip

	Wend
Else
	Print bc.error
End If
