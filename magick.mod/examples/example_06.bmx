'
' Load a jpg, modify the image, and display.

SuperStrict

Framework BaH.Magick
Import brl.glmax2d


' Load the image
Local img:TMImage = LoadMagickImage("hippo.jpg")

' did the image load?
If img Then
	DebugLog("Loaded ok.")
Else
	DebugLog("Couldn't load...")
	End
End If

img.draw(DrawableFont("Arial"))
img.draw(DrawableText(50, 50, "Hello"))



' Display the image

Graphics 800,600,0

' get the Pixmap via the getPixmap() method.
Local image:TImage = LoadImage(img.getPixmap())

While Not KeyDown(key_escape)

	Cls
	
	DrawImage image, 400 - ImageWidth(image)/2, 300 - ImageHeight(image)/2
	
	Flip
	
Wend

End

