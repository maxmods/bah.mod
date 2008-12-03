SuperStrict

Import BaH.Cairo


Local cairo:TCairo = TCairo.Create(TCairoImageSurface.CreateForPixmap(256,256))

Local normalizeMat:TCairoMatrix = TCairoMatrix.CreateScale(256.0,256.0)
cairo.SetMatrix(normalizeMat)
cairo.SetLineWidth(0.05)

'
Local RAND_MAX:Int = 2147483647
Local radToDeg:Double = 57.295779513082320876798154814105

Local X_FUZZ:Double = 0.08
Local Y_FUZZ:Double = 0.08

Local X_INNER_RADIUS:Double = 0.3
Local Y_INNER_RADIUS:Double = 0.2

Local X_OUTER_RADIUS:Double = 0.45
Local Y_OUTER_RADIUS:Double = 0.35

Local spikes:Int = 10

Local x:Double
Local y:Double
Local text:String = "KAPOW!"

SeedRnd(45)
cairo.SetLineWidth(0.01)



For Local i:Int = 0 Until spikes * 2

	x = 0.5 + Cos((Pi * i / spikes) * radToDeg) * X_INNER_RADIUS +  Rnd(0, RAND_MAX) * X_FUZZ / RAND_MAX
	y = 0.5 + Sin((Pi * i / spikes) * radToDeg) * Y_INNER_RADIUS + Rnd(0, RAND_MAX) * Y_FUZZ / RAND_MAX
	
	If i = 0 Then
		cairo.MoveTo(x, y)
	Else
		cairo.LineTo(x, y)
	End If
	
	i:+ 1
	
	x = 0.5 + Cos ((Pi * i / spikes) * radToDeg) * X_OUTER_RADIUS + Rnd(0, RAND_MAX) * X_FUZZ / RAND_MAX
	y = 0.5 + Sin ((Pi * i / spikes) * radToDeg) * Y_OUTER_RADIUS + Rnd(0, RAND_MAX) * Y_FUZZ / RAND_MAX
	
	cairo.LineTo(x, y)
Next

cairo.ClosePath()

cairo.SetSourceRGB(1, 1, 0.5)
cairo.FillPreserve()
cairo.SetSourceRGB(0,0,0)

cairo.Stroke()


cairo.selectFontFace("arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD)

cairo.MoveTo(x, y)
cairo.TextPath(text)



cairo.SetFontSize(0.2)
Local extents:TCairoTextExtents = cairo.TextExtents(text)
x = 0.5 - (extents.width / 2 + extents.x_bearing)
y = 0.5 - (extents.height / 2 + extents.y_bearing)




cairo.MoveTo(x, y)
cairo.TextPath(text)

cairo.SetSourceRGB(1, 0, 0)
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
