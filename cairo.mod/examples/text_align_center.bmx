SuperStrict

Import BaH.Cairo

Local cairo:TCairo = TCairo.Create(TCairoImageSurface.CreateForPixmap(256,256))


Local normalizeMat:TCairoMatrix = TCairoMatrix.CreateScale(256.0,256.0)
cairo.SetMatrix(normalizeMat)

Local text:String = "cairo"

cairo.SelectFontFace("arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL)



cairo.SetFontSize(0.4)
Local extents:TCairoTextExtents = cairo.TextExtents(text)
Local x:Double = 0.5 - (extents.width / 2 + extents.x_bearing)
Local y:Double = 0.5 - (extents.height / 2 + extents.y_bearing)


cairo.MoveTo(x, y)
cairo.ShowText(text)


cairo.SetLineWidth(0.03)
' draw helping lines
cairo.SetSourceRGBA(1,0.2,0.2, 0.6)
cairo.Arc(x, y, 0.05, 0, 360)
cairo.Fill()

cairo.MoveTo(0.5, 0)
cairo.RelLineTo(0, 1)

cairo.MoveTo(0, 0.5)
cairo.RelLineTo(1, 0)
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

