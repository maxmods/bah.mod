SuperStrict

Framework BaH.Leptonica
Import BRL.GLMax2D


Local pixI8:TPixmap = CreatePixmap(60, 60, PF_I8)
Local pixBGR888:TPixmap = CreatePixmap(60, 60, PF_BGR888)
Local pixRGB888:TPixmap = CreatePixmap(60, 60, PF_RGB888)
Local pixBGRA8888:TPixmap = CreatePixmap(60, 60, PF_BGRA8888)
Local pixRGBA8888:TPixmap = CreatePixmap(60, 60, PF_RGBA8888)

fillPixmap(pixI8)
fillPixmap(pixBGR888)
fillPixmap(pixRGB888)
fillPixmap(pixBGRA8888)
fillPixmap(pixRGBA8888)

'DebugStop
Local lp8:TLPIX = TLPIX.CreateWithPixmap(pixI8)
Local lp24a:TLPIX = TLPIX.CreateWithPixmap(pixBGR888, False)
Local lp24b:TLPIX = TLPIX.CreateWithPixmap(pixRGB888, False)
Local lp32a:TLPIX = TLPIX.CreateWithPixmap(pixBGRA8888)
Local lp32b:TLPIX = TLPIX.CreateWithPixmap(pixRGBA8888)

'DebugStop
Local pixI8lp:TPixmap = lp8.GetPixmap()
Local pixBGR888lp:TPixmap = lp24a.GetPixmap()
Local pixRGB888lp:TPixmap = lp24b.GetPixmap()
Local pixBGRA8888lp:TPixmap = lp32a.GetPixmap()
Local pixRGBA8888lp:TPixmap = lp32b.GetPixmap()


Graphics 800, 600, 0

While Not KeyDown(KEY_ESCAPE)

	Local y:Int = 20

	Cls
	
	imageDraw pixI8, 20, y
	imageDraw pixI8lp, 100, y
	
	DrawText pixI8.format, 30, y + 64
	DrawText pixI8lp.format, 110, y + 64
	
	y:+ 100

	imageDraw pixBGR888, 20, y
	imageDraw pixBGR888lp, 100, y

	DrawText pixBGR888.format, 30, y + 64
	DrawText pixBGR888lp.format, 110, y + 64
	
	y:+ 100

	imageDraw pixRGB888, 20, y
	imageDraw pixRGB888lp, 100, y

	DrawText pixRGB888.format, 30, y + 64
	DrawText pixRGB888lp.format, 110, y + 64
	
	y:+ 100

	imageDraw pixBGRA8888, 20, y
	imageDraw pixBGRA8888lp, 100, y

	DrawText pixBGRA8888.format, 30, y + 64
	DrawText pixBGRA8888lp.format, 110, y + 64
	
	y:+ 100

	imageDraw pixRGBA8888, 20, y
	imageDraw pixRGBA8888lp, 100, y
	
	DrawText pixRGBA8888.format, 30, y + 64
	DrawText pixRGBA8888lp.format, 110, y + 64
	
	Flip
	
Wend


Function imageDraw(pixmap:TPixmap, x:Int, y:Int)
	Local image:TImage = LoadImage(pixmap)
	DrawImage image, x, y
End Function


Function fillPixmap(pixmap:TPixmap)
	Local count:Int = 0

	For Local y:Int = 0 Until pixmap.height
		count = 0
		
		For Local x:Int = 0 Until pixmap.width Step 3

			Local colour:Int
		
			Select pixmap.format
				Case PF_I8
					Select count
						Case 0
							colour = 85
						Case 1
							colour = 85
						Case 2
							colour = 85
					End Select
				Case PF_BGR888
					Select count
						Case 0
							colour = $FF0000
						Case 1
							colour = $00FF00
						Case 2
							colour = $0000FF
					End Select
				Case PF_RGB888
					Select count
						Case 0
							colour = $FF0000
						Case 1
							colour = $00FF00
						Case 2
							colour = $0000FF
					End Select
				Case PF_BGRA8888
					Select count
						Case 0
							colour = $FFFF0000
						Case 1
							colour = $FF00FF00
						Case 2
							colour = $FF0000FF
					End Select
				Case PF_RGBA8888
					Select count
						Case 0
							colour = $FFFF0000
						Case 1
							colour = $FF00FF00
						Case 2
							colour = $FF0000FF
					End Select
			End Select
		
			For Local i:Int = 0 Until 3
				WritePixel(pixmap, x + i, y, colour)
			Next
			
			count:+ 1
			If count = 3 Then
				count = 0
			EndIf
		Next
	Next

End Function
