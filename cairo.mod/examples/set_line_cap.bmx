SuperStrict

Import BaH.Cairo

Local cairo:TCairo = TCairo.Create(TCairoImageSurface.CreateForPixmap(256,256))

Local normalizeMat:TCairoMatrix = TCairoMatrix.CreateScale(256.0,256.0)
cairo.SetMatrix(normalizeMat)


cairo.SetLineWidth(0.12)
cairo.SetLineCap(CAIRO_LINE_CAP_BUTT) ' default 
cairo.MoveTo(0.25, 0.2) cairo.LineTo(0.25, 0.8)
cairo.Stroke()
cairo.SetLineCap(CAIRO_LINE_CAP_ROUND)
cairo.MoveTo(0.5, 0.2) cairo.LineTo(0.5, 0.8)
cairo.Stroke()
cairo.SetLineCap(CAIRO_LINE_CAP_SQUARE)
cairo.MoveTo(0.75, 0.2) cairo.LineTo(0.75, 0.8)
cairo.Stroke()

' draw helping lines 
cairo.SetSourceRGB(1,0.2,0.2)
cairo.SetLineWidth(0.01)
cairo.MoveTo(0.25, 0.2) ; cairo.LineTo(0.25, 0.8)
cairo.MoveTo(0.5, 0.2)  ; cairo.LineTo(0.5, 0.8)
cairo.MoveTo(0.75, 0.2) ; cairo.LineTo(0.75, 0.8)
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