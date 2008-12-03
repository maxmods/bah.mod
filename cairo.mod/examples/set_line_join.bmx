SuperStrict

Import Bah.Cairo

Local cairo:TCairo = TCairo.Create(TCairoImageSurface.CreateForPixmap(256,256))

Local normalizeMat:TCairoMatrix = TCairoMatrix.CreateScale(256.0,256.0)
cairo.SetMatrix(normalizeMat)


cairo.SetLineWidth(0.16)
cairo.MoveTo(0.3, 0.33)
cairo.RelLineTo(0.2, -0.2)
cairo.RelLineTo(0.2, 0.2)
cairo.SetLineJoin(CAIRO_LINE_JOIN_MITER) ' default 
cairo.Stroke()

cairo.MoveTo(0.3, 0.63)
cairo.RelLineTo(0.2, -0.2)
cairo.RelLineTo(0.2, 0.2)
cairo.SetLineJoin(CAIRO_LINE_JOIN_BEVEL)
cairo.Stroke()

cairo.MoveTo(0.3, 0.93)
cairo.RelLineTo(0.2, -0.2)
cairo.RelLineTo(0.2, 0.2)
cairo.SetLineJoin(CAIRO_LINE_JOIN_ROUND)
cairo.Stroke()


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