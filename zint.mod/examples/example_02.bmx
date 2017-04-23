SuperStrict

Framework brl.standardio
Import BaH.Zint
Import brl.glmax2d

Local bc:TZBarcode = New TZBarcode

bc.SetSymbology(BARCODE_ISBNX)
bc.SetScale(2)

Local cairo:TCairo = TCairo.Create(TCairoImageSurface.CreateForPixmap(512, 512,PF_BGR888))

bc.CairoEncode(cairo, "9781484225226")

cairo.Destroy()

Local image:TImage = LoadImage(TCairoImageSurface(cairo.getTarget()).pixmap())

Graphics 800, 600, 0

SetBlend ALPHABLEND

SetClsColor 255,255,255
SetColor 255, 255, 255

	While Not KeyDown(key_escape)

		Cls
		
		DrawImage image, 100, 100
		
		Flip

	Wend
