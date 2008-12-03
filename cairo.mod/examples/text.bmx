SuperStrict


Import BaH.Cairo


Local cairo:TCairo = TCairo.Create(TCairoImageSurface.CreateForPixmap(256,256))

Local normalizeMat:TCairoMatrix = TCairoMatrix.CreateScale(256.0,256.0)
cairo.SetMatrix(normalizeMat)
cairo.SetLineWidth(0.05)

'
cairo.SelectFontFace( "arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD)
cairo.SetFontSize ( 0.35)

cairo.MoveTo ( 0.04, 0.53)
cairo.ShowText ( "Héllo")

cairo.MoveTo( 0.27, 0.65)
cairo.TextPath( "void")
cairo.SetSourceRgb( 0.5, 0.5, 1)
cairo.FillPreserve()
cairo.SetSourceRgb( 0, 0, 0)
cairo.SetLineWidth( 0.01)
cairo.Stroke()

'/* draw helping lines */
cairo.SetSourceRgba ( 1,0.2,0.2, 0.6)
cairo.Arc ( 0.04, 0.53, 0.02, 0, 360)
cairo.Arc ( 0.27, 0.65, 0.02, 0, 360)
cairo.fill()

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


