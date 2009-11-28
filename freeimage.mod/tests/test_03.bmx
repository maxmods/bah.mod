' TEST 3
'
' Load a jpg, modify the image, save as png, then load and display.

SuperStrict

Framework BaH.FreeImage
Import brl.glmax2d

' load an image
Local img:TFreeImage = LoadFreeImage("logo.jpg")

' did the image load ?
If img Then
	DebugLog("Loaded ok.")
Else
	DebugLog("Couldn't load...")
	End
End If


' do some manipulation...
img.flipHorizontal()

' for 32 bit images... it will also invert ALPHA !!! (making things invisible)
img.invert()
img = img.rotate(45)


' save the image as a png
If Not img.save("newtest.png", FIF_PNG) Then
	DebugLog("Couldn't save...")
	End
End If


' load it back !!!
img = LoadFreeImage("newtest.png")

If img Then
	DebugLog("Found an image !")
Else
	DebugLog("Don't recognise format")
	End
End If

' Finally, display the image...

Graphics 800,600,0

' get the Pixmap via the getPixmap() method.
Local image:TImage = LoadImage(img.getPixmap())

While Not KeyDown(key_escape)

	Cls
	
	DrawImage image, 400 - ImageWidth(image)/2, 300 - ImageHeight(image)/2
	
	Flip
	
Wend

End
