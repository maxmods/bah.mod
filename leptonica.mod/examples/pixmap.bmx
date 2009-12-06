SuperStrict

Framework BaH.Leptonica
Import BRL.GLMax2D
Import BRL.JPGLoader

Local pixmap:TPixmap = LoadPixmap("marge.jpg")

'DebugLog pixmap.width + ", " + pixmap.height
'DebugStop
Local pix:TLPIX = TLPIX.CreateWithPixmap(pixmap)
'Local pix:TLPIX = TLPIX.Create(100, 100, 32)
Local box:TLBOX = TLBOX.Create(10, 10, 20, 20)

'DebugStop
pix.RenderBoxArb(box, 1, 255, 255, 0)

Graphics 800, 600, 0

Local pixmap2:TPixmap = pix.GetPixmap()
Local image:TImage = LoadImage(pixmap2)


SetColor 255, 255, 255

While Not KeyDown(KEY_ESCAPE)

	Cls
	
	DrawImage image, 100, 100

	DrawLine 100, 100, 100 + pixmap.width , 100
	DrawLine 100 + pixmap.width, 100, 100 + pixmap.width, 100 + pixmap.height

	
	Flip

Wend
