' TEST 4
'
' Load an image into a pixmap, transform the pixmap into a new format, convert
' back into an FreeImage image, and display.

SuperStrict


Framework BaH.FreeImage
Import brl.glmax2d


' load an image using the standard LoadPixmap function (using our library ;-)
Local pix:TPixmap = LoadPixmap("logo.jpg")

' did the image load ?
If pix Then
	DebugLog("Loaded ok.")
Else
	DebugLog("Couldn't load...")
	End
End If

' create a copy by resizing, and changing the format..
pix = ConvertPixmap(ResizePixmap(pix, pix.width * 2, pix.height * 2), PF_I8)

' convert the pixmap into a FreeImage...
Local img:TFreeImage = TFreeImage.CreateFromPixmap(pix)

If Not img Then
	DebugLog("problem creating from pixmap!")
	End
End If


' Display the pixmap

Graphics 800,600,0

Local image:TImage = LoadImage(img.getPixmap())
'Local image:TImage = LoadImage(pix)


While Not KeyDown(key_escape)

	Cls
	
	DrawImage image, 400 - ImageWidth(image)/2, 300 - ImageHeight(image)/2
	
	Flip
	
Wend

End