SuperStrict

Import BaH.Cairo

Local cairo:TCairo = TCairo.Create(TCairoImageSurface.CreateForPixmap(256,256))


Local normalizeMat:TCairoMatrix = TCairoMatrix.CreateScale(256.0,256.0)
cairo.SetMatrix(normalizeMat)
cairo.SetLineWidth(0.05)

'

cairo.SetLineWidth(0.1)

' a custom shape, that could be wrapped in a function 
Local x0:Double	   = 0.1   '< parameters like cairo.Rectangle 
Local y0:Double	   = 0.1
Local rect_width:Double  = 0.8
Local rect_height:Double = 0.8
Local radius:Double = 0.4   '< and an approximate curvature radius 

Local x1:Double,y1:Double

x1=x0+rect_width
y1=y0+rect_height
If (rect_width = 0 Or rect_height = 0)
    Return
Else If (rect_width/2<radius)
    If (rect_height/2<radius)
        cairo.MoveTo (x0, (y0 + y1)/2)
        cairo.CurveTo(x0 ,y0, x0, y0, (x0 + x1)/2, y0)
        cairo.CurveTo(x1, y0, x1, y0, x1, (y0 + y1)/2)
        cairo.CurveTo(x1, y1, x1, y1, (x1 + x0)/2, y1)
        cairo.CurveTo(x0, y1, x0, y1, x0, (y0 + y1)/2)
    Else
        cairo.MoveTo (x0, y0 + radius)
        cairo.CurveTo(x0 ,y0, x0, y0, (x0 + x1)/2, y0)
        cairo.CurveTo(x1, y0, x1, y0, x1, y0 + radius)
        cairo.LineTo(x1 , y1 - radius)
        cairo.CurveTo(x1, y1, x1, y1, (x1 + x0)/2, y1)
        cairo.CurveTo(x0, y1, x0, y1, x0, y1- radius)
    End If
Else
    If (rect_height/2<radius)
        cairo.MoveTo (x0, (y0 + y1)/2)
        cairo.CurveTo(x0 , y0, x0 , y0, x0 + radius, y0)
        cairo.LineTo(x1 - radius, y0)
        cairo.CurveTo(x1, y0, x1, y0, x1, (y0 + y1)/2)
        cairo.CurveTo(x1, y1, x1, y1, x1 - radius, y1)
        cairo.LineTo(x0 + radius, y1)
        cairo.CurveTo(x0, y1, x0, y1, x0, (y0 + y1)/2)
    Else
        cairo.MoveTo (x0, y0 + radius)
        cairo.CurveTo(x0 , y0, x0 , y0, x0 + radius, y0)
        cairo.LineTo(x1 - radius, y0)
        cairo.CurveTo(x1, y0, x1, y0, x1, y0 + radius)
        cairo.LineTo(x1 , y1 - radius)
        cairo.CurveTo(x1, y1, x1, y1, x1 - radius, y1)
        cairo.LineTo(x0 + radius, y1)
        cairo.CurveTo(x0, y1, x0, y1, x0, y1- radius)
    End If
End If
cairo.ClosePath()

cairo.SetSourceRGB(0.5,0.5,1)
cairo.FillPreserve()
cairo.SetSourceRGBA(0.5, 0, 0, 0.5)
cairo.ClosePath()
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