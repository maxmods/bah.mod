' TEST 1
'
' Load and display an image

SuperStrict


Framework BaH.FreeImage
Import brl.glmax2d


' load an image using the standard LoadPixmap function (using our library ;-)
Local pix:TPixmap = LoadPixmap("logo.gif")

' did the image load ?
If pix Then
	DebugLog("Loaded ok.")
Else
	DebugLog("Couldn't load...")
	End
End If

' Display the pixmap

Graphics 800,600,0

Local image:TImage = LoadImage(pix)

While Not KeyDown(key_escape)

	Cls
	
	DrawImage image, 400 - ImageWidth(image)/2, 300 - ImageHeight(image)/2
	
	Flip
	
Wend

End