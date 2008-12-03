SuperStrict

Import BaH.Cairo


Local cairo:TCairo = TCairo.Create(TCairoImageSurface.CreateForPixmap(256,256))

Local normalizeMat:TCairoMatrix = TCairoMatrix.CreateScale(256.0,256.0)
cairo.SetMatrix(normalizeMat)
cairo.SetLineWidth(0.05)

cairo.moveto ( 0.1, -50)
cairo.lineto ( 0.1,  50)
cairo.setsourcergb ( 1, 0 ,0)
cairo.stroke()

cairo.moveto ( 0.2, -60)
cairo.lineto ( 0.2,  60)
cairo.setsourcergb ( 1, 1 ,0)
cairo.stroke()

cairo.moveto ( 0.3, -70)
cairo.lineto ( 0.3,  70)
cairo.setsourcergb ( 0, 1 ,0)
cairo.stroke()

cairo.moveto ( 0.4, -80)
cairo.lineto ( 0.4,  80)
cairo.setsourcergb ( 0, 0 ,1)
cairo.stroke()
'

cairo.Destroy()


Local image:TImage = LoadImage(TCairoImageSurface(cairo.getTarget()).pixmap())

Graphics 640,480,0

SetBlend ALPHABLEND

SetClsColor 255,255,255

Repeat
	Cls
	DrawImage image,MouseX(),MouseY() - 128
	Flip
Until KeyHit(key_escape)

End