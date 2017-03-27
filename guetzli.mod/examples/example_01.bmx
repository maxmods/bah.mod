SuperStrict

Framework brl.standardio
Import BaH.Guetzli
Import BRL.PNGLoader

Local pix:TPixmap = LoadPixmap("bees.png")

' please wait... ;-)
SavePixmapJPeg(pix, "bees.jpg")

