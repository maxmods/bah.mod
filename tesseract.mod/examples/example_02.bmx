SuperStrict


Framework BaH.tesseract
Import BaH.FreeImage

Local Tess:Tesseract = New Tesseract

' initialise the engine
If Not Tess.Init("tessdata") Then


	' load an image
	Local pix:TPixmap = LoadPixmap("eurotext.tif")
	
	If pix Then
	
		' process the image, returning the recognised text
		Local s:String = Tess.Rect(pix)
	
		Print s
	
	End If


End If