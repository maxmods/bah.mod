SuperStrict

Import BaH.Cairo


Local cairo:TCairo = TCairo.Create(TCairoImageSurface.CreateForPixmap(256,256))

Local normalizeMat:TCairoMatrix = TCairoMatrix.CreateScale(256.0,256.0)
cairo.SetMatrix(normalizeMat)
cairo.SetLineWidth(0.05)

'

Local x:Double=0.1,  y:Double=0.5
Local x1:Double=0.4, y1:Double=0.9
Local x2:Double=0.6, y2:Double=0.1
Local x3:Double=0.9, y3:Double=0.5
cairo.SetLineWidth(0.05)
cairo.MoveTo(x, y)
cairo.CurveTo(x1, y1, x2, y2, x3, y3)

cairo.Stroke()

cairo.SetSourceRGBA(1,0.2,0.2,0.6)
cairo.SetLineWidth(0.03)
cairo.MoveTo(x,y) ; cairo.LineTo(x1,y1)
cairo.MoveTo(x2,y2) ; cairo.LineTo(x3,y3)
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