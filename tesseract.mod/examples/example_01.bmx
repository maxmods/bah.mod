SuperStrict


Framework BaH.tesseract
Import BaH.FreeImage


Tess.Init("tessdata")


' load an image
Local pix:TPixmap = LoadPixmap("phototest.tif")

If pix Then

	Local s:String = Tess.Rect(pix)

	Print s

End If


