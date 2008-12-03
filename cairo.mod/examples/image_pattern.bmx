SuperStrict

Import BaH.Cairo



Local cairo:TCairo = TCairo.Create(TCairoImageSurface.CreateForPixmap(256,256))

Local normalizeMat:TCairoMatrix = TCairoMatrix.CreateScale(256.0,256.0)
cairo.SetMatrix(normalizeMat)

'
Local photo:TPixmap = LoadPixmap("romedalen.png")

Local photosurf:TCairoImageSurface = TCairoImageSurface.CreateFromPixmap(photo)

Local w:Int = photosurf.getWidth()
Local h:Int = photosurf.getHeight()

Local pattern:TCairoPattern = TCairoPattern.CreateForSurface(photosurf)
pattern.setExtend(CAIRO_EXTEND_REPEAT)



cairo.translate( 0.5, 0.5)
cairo.rotate( 45 )
cairo.scale( 1 / Sqr(2), 1 / Sqr(2))
cairo.translate(- 0.5, - 0.5)

normalizeMat.InitScale(w * 5.0 , h * 5.0)
pattern.setMatrix(normalizeMat)

cairo.setSource( pattern)

cairo.rectangle( 0, 0, 1.0, 1.0)
cairo.fill()

pattern.destroy()
photosurf.Destroy()

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
