SuperStrict

Import BaH.Cairo

Local cairo:TCairo = TCairo.Create(TCairoImageSurface.CreateForPixmap(100,100))

'cairo.setantialias( CAIRO_ANTIALIAS_NONE )

Local normalizeMat:TCairoMatrix = TCairoMatrix.CreateScale(100.0,100.0)
cairo.SetMatrix(normalizeMat)


Local f:Double = 1.0 / 255

Local r:Double = f * $A0
Local g:Double = f * $B0
Local b:Double = f * $C0
Local a:Double = f * $D0

cairo.SetLineWidth(0.1)

cairo.SetSourceRGBA( 1, 0, 0, 1.0 )
cairo.MoveTo(0.2, 0)
cairo.LineTo(0.2, 1)
cairo.Stroke()

cairo.SetSourceRGBA( 0, 1, 0, 1.0 )
cairo.MoveTo(0.5, 0)
cairo.LineTo(0.5, 1)
cairo.Stroke()

cairo.SetSourceRGBA( 0, 0, 1, 1.0 )
cairo.MoveTo(0.8, 0)
cairo.LineTo(0.8, 1)
cairo.Stroke()


cairo.Destroy()


Local pix:TPixmap = CreatePixmap( 100, 100, PF_BGRA8888 )
pix = TCairoImageSurface( cairo.getTarget() ).pixmap()


Graphics 640,480,0
SetBlend ALPHABLEND
SetClsColor 255,255,255


Cls
DrawPixmap  pix,  0, 0
Flip


For Local x:Int=0 To 99
        For Local y:Int=0 To 0
                Local i:Int = ReadPixel( pix, x, y )
                If i<>$00000000 Then Print x +"  "+ y +"  "+ Hex( i )
        Next
Next


WaitKey()
