SuperStrict


Framework BaH.tesseract
Import BaH.FreeImage

' initialise the engine
Tess.Init("tessdata")


' load an image
Local pix:TPixmap = LoadPixmap("phototest.tif")

If pix Then

	' process the image, returning the recognised text
	Local s:String = Tess.Rect(pix)

	Print s

End If


