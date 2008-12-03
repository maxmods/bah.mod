SuperStrict

Import BaH.Cairo

Local cairo:TCairo = TCairo.Create(TCairoImageSurface.CreateForPixmap(256,256))


Local normalizeMat:TCairoMatrix = TCairoMatrix.CreateScale(256.0,256.0)
cairo.SetMatrix(normalizeMat)


cairo.SetLineWidth(0.1)
cairo.Arc(0.5, 0.5, 0.3, 0, 360)
cairo.Clip()
cairo.Stroke()
cairo.SetSourceRGB(0, 0, 0)

cairo.NewPath()  ' current path is not
                 ' consumed by cairo.Clip() 
cairo.Rectangle(0, 0, 1, 1)
cairo.Fill()
cairo.SetSourceRGB(0, 1, 0)
cairo.MoveTo(0, 0)
cairo.LineTo(1, 1)
cairo.MoveTo(1, 0)
cairo.LineTo(0, 1)
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
