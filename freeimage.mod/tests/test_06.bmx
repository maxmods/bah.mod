' TEST 6
'
' Load and display an animated gif - loading all frames into a TImage

SuperStrict


Framework BaH.FreeImage
Import brl.glmax2d

Graphics 800,600,0

' load the animated image
Local image:TImage = LoadAnimFreeImage("anim.gif")
If Not image Then
	DebugLog("Couldn't load...")
	End
End If

Local count:Int = image.frames.length
Local pos:Int = -1


While Not KeyDown(key_escape)

	pos:+ 1
	
	If pos = count Then
		pos = 0
	End If

	Cls
	
	DrawImage image, 400 - ImageWidth(image)/2, 300 - ImageHeight(image)/2, pos

	Flip
	
Wend

End

