SuperStrict

Import BaH.Cairo


Local image:TImage = CreateImage(256, 256,1,DYNAMICIMAGE)
Local cairo:TCairo = TCairo.Create(TCairoImageSurface.CreateFromImage(image))

Local normalizeMat:TCairoMatrix = TCairoMatrix.CreateScale(256.0,256.0)
cairo.SetMatrix(normalizeMat)

'
Local photo:TPixmap = LoadPixmap("romedalen.png")

Local photosurf:TCairoImageSurface = TCairoImageSurface.CreateFromPixmap(photo)

Local w:Int = photosurf.getWidth()
Local h:Int = photosurf.getHeight()

Local pattern:TCairoPattern = TCairoPattern.CreateForSurface(photosurf)
pattern.setExtend(CAIRO_EXTEND_REPEAT)


normalizeMat.InitScale(w * 5.0 , h * 5.0)
pattern.setMatrix(normalizeMat)

Graphics 640,480,0

SetBlend ALPHABLEND

SetClsColor 255,255,255

Local angle:Float = 0
Local scale:Float = 0

Repeat
	Cls

	LockImage(image,,,True)
	
	cairo.rectangle(0, 0, 1, 1)
	cairo.clip()
	
	cairo.clear(1, 1, 1, 1, 1)
	
	
	cairo.save()
	
	cairo.translate( 0.5, 0.5)
	cairo.rotate( angle )
	cairo.scale( (1 + scale )/ Sqr(2), (1 + scale) / Sqr(2))
	cairo.translate(- 0.5, - 0.5)
	
	
	cairo.setSource( pattern)
	
	cairo.rectangle( 0, 0, 1.0, 1.0)
	cairo.fill()
	
	
	cairo.restore()
		

	DrawImage image,MouseX() - 128,MouseY() - 128
	
	SetColor(0,0,0)
	DrawText "Realtime Rotating Cairo", MouseX() - 99, MouseY() + 81
	SetColor(255, 0, 0)
	DrawText "Realtime Rotating Cairo", MouseX() - 100, MouseY() + 80
	SetColor(255, 255, 255)

	scale = 0.1 * MouseZ()
	
	angle:+ 0.75
	angle = angle Mod 360
	
	Flip
Until KeyHit(key_escape)

pattern.destroy()
photosurf.Destroy()

'

cairo.Destroy()

End

