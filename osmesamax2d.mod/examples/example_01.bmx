'
' OSMesa drawing example
'
'
SuperStrict

Framework BaH.OSMesaMax2D
Import BRL.PNGLoader

' create a pixmap
Local pixmap:TPixmap = CreatePixmap(400, 400, PF_RGBA8888)


'
' Note: To remain compatible with the Graphics framework, we must pass the pixel buffer
'       as an address (integer).
'
Local g:TGraphics = CreateGraphics(400, 400, 32, Int Ptr(Varptr pixmap.pixels)[0], 0)

' Set the current graphics context, so that we can do stuff.
SetGraphics(g)


' do some drawing
Cls


DrawText "OSMesa Drawing!!", 100, 200

SetColor 255, 0, 0

DrawLine 50, 50, 100, 100

SetColor 0, 255, 0
DrawLine 50, 60, 100, 110

SetColor 0, 0, 255
DrawLine 50, 70, 100, 120

' forces any buffered drawing commands to finish
Flip


' Save our pixmap!
' Graphics are drawn on an inverted Y axis, so perform a y-flip when you want to see it properly.
'
SavePixmapPNG(YFlipPixmap(pixmap), "test.png")

