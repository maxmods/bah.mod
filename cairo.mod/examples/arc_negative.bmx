SuperStrict

Import BaH.Cairo

Local cairo:TCairo = TCairo.Create(TCairoImageSurface.CreateForPixmap(256,256))


Local normalizeMat:TCairoMatrix = TCairoMatrix.CreateScale(256.0,256.0)
cairo.SetMatrix(normalizeMat)


Local xc:Double = 0.5
Local yc:Double = 0.5
Local radius:Double = 0.4
Local angle1:Double = 45.0
Local angle2:Double = 180.0 

cairo.SetLineWidth(0.05)
cairo.ArcNegative(xc, yc, radius, angle1, angle2)
cairo.Stroke()

' draw helping lines 
cairo.SetSourceRGBA(1,0.2,0.2,0.6)
cairo.Arc(xc, yc, 0.05, 0, 360)
cairo.Fill()
cairo.SetLineWidth(0.03)
cairo.Arc(xc, yc, radius, angle1, angle1)
cairo.LineTo(xc, yc)
cairo.Arc(xc, yc, radius, angle2, angle2)
cairo.LineTo(xc, yc)
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