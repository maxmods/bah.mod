SuperStrict

Framework BaH.qrencode
Import BRL.GLMax2D


Local code:TQRCode = TQRCode.encodeString("http://maps.google.co.uk/?ll=52.467822,-1.738903&spn=0.001247,0.003484&t=h&z=19")

Local pix:TPixmap = code.getPixmap(4)

Graphics 800, 600, 0

SetClsColor 255, 255, 255
SetColor 0, 0, 0

Local image:TImage = LoadImage(pix)

While Not KeyDown(KEY_ESCAPE)

	Cls

	DrawImage image, 100, 100

	Flip
Wend
