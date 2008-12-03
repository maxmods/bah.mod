SuperStrict

Import BaH.Cairo


Local cairo:TCairo = TCairo.Create(TCairoImageSurface.CreateForPixmap(256,256))

Local normalizeMat:TCairoMatrix = TCairoMatrix.CreateScale(256.0,256.0)
cairo.SetMatrix(normalizeMat)
cairo.SetLineWidth(0.05)

'

Local pat:TCairoPattern = TCairoPattern.CreateLinear (0.0, 0.0,  0.0, 1.0)
pat.AddColorStopRGBA(1, 0, 0, 0, 1)
pat.AddColorStopRGBA(0, 1, 1, 1, 1)
cairo.Rectangle(0, 0, 1, 1)
cairo.SetSource(pat)
cairo.Fill()
pat.Destroy()

pat = TCairoPattern.CreateRadial (0.45, 0.4, 6, 0.4,  0.4, 30)
pat.AddColorStopRGBA(0, 1, 1, 1, 1)
pat.AddColorStopRGBA(1, 0, 0, 0, 1)
cairo.SetSource(pat)
cairo.Arc(0.5, 0.5, 0.3, 0, 360)
cairo.Fill()
pat.Destroy()

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