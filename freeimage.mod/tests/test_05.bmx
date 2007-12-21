' TEST 5
'
' Load and display an animated gif - in a very inefficient way.
' It would probably be better to use LoadAnimFreeImage() or TMultiFreeImage.toAnimImage()
' see test_06.bmx for an example of LoadAnimFreeImage()

SuperStrict


Framework BaH.FreeImage
Import brl.glmax2d

' load a multipage image.
Local anim:TMultiFreeImage = LoadMultiFreeImage("anim.gif")

' did the image load ?
If anim Then
	DebugLog("Loaded ok.")
Else
	DebugLog("Couldn't load...")
	End
End If

' Display the animation

Graphics 800,600,0

Local pix:TPixmap
Local image:TImage
Local count:Int = 0

While Not KeyDown(key_escape)

	Cls

	' This is quite inefficient, but is here as a guide to show one way of iterating through the pages.
	' You may also be interested in lockPage() and unlockPage(), as well as toAnimImage()
	If anim.hasNext() Then
		count:+ 1
		pix = anim.getNext().getPixmap()
		image = LoadImage(pix)
		
		If Not anim.hasNext() Or count = 18 Then
			anim.reset()
			count = 0
		End If
	End If
	
	DrawImage image, 400 - ImageWidth(image)/2, 300 - ImageHeight(image)/2

	Flip
	
Wend

End
