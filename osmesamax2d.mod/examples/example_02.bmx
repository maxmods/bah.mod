'
' OSMesa drawing example 2
'
'
SuperStrict

Framework BaH.OSMesaMax2D
Import BRL.PNGLoader
Import BRL.StandardIO

Local pix:TPixmap = LoadPixmap("star.png")
If Not pix Then
	Print "Error loading image"
	End
End If

' create a pixmap
Local pixmap:TPixmap = CreatePixmap(400, 400, PF_RGBA8888)


'
' Note: To remain compatible with the Graphics framework, we must pass the pixel buffer
'       as an address (integer).
'
Local g:TGraphics = CreateGraphics(400, 400, 32, Int Ptr(Varptr pixmap.pixels)[0], 0)

' Set the current graphics context, so that we can do stuff.
SetGraphics(g)

SetBlend ALPHABLEND
Local image:TImage = LoadImage(pix)

SetClsColor 50, 50, 50

' do some drawing
Cls


DrawText "OSMesa Drawing!!", 100, 200

SetColor 255, 0, 0

DrawLine 50, 50, 100, 100

SetColor 0, 255, 0
DrawLine 50, 60, 100, 110

SetColor 0, 0, 255
DrawLine 50, 70, 100, 120

SetColor 255, 255, 255
DrawImage image, 200, 50

' forces any buffered drawing commands to finish
Flip


' Save our pixmap!
' Graphics are drawn on an inverted Y axis, so perform a y-flip when you want to see it properly.
'
SavePixmapPNG(YFlipPixmap(pixmap), "test.png")

