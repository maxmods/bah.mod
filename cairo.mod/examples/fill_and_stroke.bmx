SuperStrict

Import BaH.Cairo

Local cairo:TCairo = TCairo.Create(TCairoImageSurface.CreateForPixmap(256,256))


Local normalizeMat:TCairoMatrix = TCairoMatrix.CreateScale(256.0,256.0)
cairo.SetMatrix(normalizeMat)
cairo.SetLineWidth(0.05)

'

cairo.MoveTo(0.5, 0.1)
cairo.LineTo(0.9, 0.9)
cairo.RelLineTo(-0.4, 0.0)
cairo.CurveTo(0.2, 0.9, 0.2, 0.5, 0.5, 0.5)
cairo.ClosePath()

cairo.SetSourceRGB(0, 0, 1)
cairo.FillPreserve()
cairo.SetSourceRGB(0, 0, 0)
cairo.Stroke()

'

cairo.Destroy()

Local image:TImage = LoadImage(TCairoImageSurface(cairo.getTarget()).pixmap())

Graphics 640,480,0

SetBlend ALPHABLEND

SetClsColor 255,255,255

Repeat
	Cls
	DrawImage image,MouseX() - 128,MouseY() - 128
	Flip
Until KeyHit(key_escape)

End