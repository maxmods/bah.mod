SuperStrict

Framework BaH.Leptonica
Import BRL.GLMax2D
Import BRL.JPGLoader
Import brl.retro

Local pixmap:TPixmap = LoadPixmap("marge.jpg")
'Local pixmap:TPixmap = TPixmap.Create(200, 200, PF_RGBA8888)
'pixmap.ClearPixels($FFFFFFFF)
'DebugLog pixmap.format

DebugLog Hex(pixmap.ReadPixel(10, 10))
DebugLog Hex(pixmap.ReadPixel(70, 10))
DebugLog Hex(pixmap.ReadPixel(140, 10))
DebugLog ""

'DebugLog pixmap.width + ", " + pixmap.height
'DebugStop
Local pix:TLPIX = TLPIX.CreateWithPixmap(pixmap)
'Local pix:TLPIX = TLPIX.Create(100, 100, 32)
Local box1:TLBOX = TLBOX.Create(9, 9, 20, 20)
Local box2:TLBOX = TLBOX.Create(69, 9, 20, 20)
Local box3:TLBOX = TLBOX.Create(139, 9, 20, 20)

'DebugStop
pix.RenderBoxArb(box1, 1, 255, 0, 0)
pix.RenderBoxArb(box2, 1, 0, 255, 0)
pix.RenderBoxArb(box3, 1, 0, 0, 255)

Graphics 800, 600, 0

Local pixmap2:TPixmap = pix.GetPixmap()
Local image:TImage = LoadImage(pixmap2)

DebugLog Hex(pixmap2.ReadPixel(9, 9))
DebugLog Hex(pixmap2.ReadPixel(69, 9))
DebugLog Hex(pixmap2.ReadPixel(139, 9))
DebugLog ""
DebugLog Hex(pixmap2.ReadPixel(10, 10))
DebugLog Hex(pixmap2.ReadPixel(70, 10))
DebugLog Hex(pixmap2.ReadPixel(140, 10))


SetColor 255, 255, 255

While Not KeyDown(KEY_ESCAPE)

	Cls
	
	DrawImage image, 100, 100

	DrawLine 100, 100, 100 + pixmap.width , 100
	DrawLine 100 + pixmap.width, 100, 100 + pixmap.width, 100 + pixmap.height

	
	Flip

Wend
