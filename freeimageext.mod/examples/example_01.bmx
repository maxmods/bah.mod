SuperStrict

Framework BaH.FreeImageExt
Import BRL.StandardIO
Import brl.glmax2d


' Load image from Nikon D70 marek - (original file from here : http://raw.fotosite.pl/index-Nikon_D70_marek.htm )
Local pix:TPixmap = LoadPixmap("DSC_0347.NEF")
Local pix2:TPixmap = LoadPixmap("DSC_0347.NEF.jpg") ' thumbnail jpg for comparison.

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
Local image2:TImage = LoadImage(pix2)

Local sx:Float = 600.0 / image.width
Local sy:Float = 600.0 / image.height


If sx > 1 Or sy > 1 Then
	sx = 1
	sy = 1
End If

If sy > sx Then
	sy = sx
Else
	sx = sy
End If


While Not KeyDown(key_escape)

	Cls

	SetScale sx, sy	
	DrawImage image, 400 - (ImageWidth(image) * sx)/2, 300 - (ImageHeight(image) * sy)/2
	
	SetScale 1, 1
	DrawImage image2, 0, 300
	
	Flip
	
Wend

End