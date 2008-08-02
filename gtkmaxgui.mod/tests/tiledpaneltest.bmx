SuperStrict

?linux
Framework bah.gtkmaxgui
?
Import brl.eventqueue

Local window:TGadget
Local panel:TGadget
Local group:TGadget

window = CreateWindow("My Window",40,40,305,400)

panel = CreatePanel(0,0,ClientWidth(window),ClientHeight(window),window)
SetGadgetLayout panel,1,1,1,1

Local pixmap:TPixmap = LoadPixmap("bmax120.png")

Rem
Local w:Float = GadgetWidth(panel)
Local h:Float = GadgetHeight(panel)

Local wfull:Int = w / pixmap.width
Local hfull:Int = h / pixmap.height

Local wpart:Int = w Mod pixmap.width
Local hpart:Int = h Mod pixmap.height


Print wfull + " : " + wpart
Print hfull  + " : " + hpart

Local tiledPixmap:TPixmap = CreatePixmap(w, h, PF_RGBA8888)

Local mainPixmap:TPixmap
Local botPixmap:TPixmap
Local sidePixmap:TPixmap

If wfull > 0 And hfull > 0 Then
	mainPixmap = pixmap
Else
	If wfull > 0 Then
		mainpixmap = pixmap.window(0, 0, pixmap.width, hpart)
	ElseIf hfull > 0 Then
		mainpixmap = pixmap.window(0, 0, wpart, pixmap.height)
	End If
End If

If wpart > 0 Or hpart > 0 Then
	If wpart > 0 Then
		If hfull > 0 Then
			sidePixmap = pixmap.window(0, 0, wpart, pixmap.height)
		Else
			'sidePixmap = pixmap.window(0, 0, wpart, hpart)
		End If
	End If
	If hpart > 0 Then
		If wfull > 0 Then
			botPixmap = pixmap.window(0, 0, pixmap.width, hpart)
		Else
			'botPixmap = pixmap.window(0, 0, wpart, hpart)
		End If
	End If
End If

For Local x:Int = 0 Until wfull

	For Local y:Int = 0 Until hfull
	
		tiledPixmap.paste(pixmap, x * pixmap.width, y * pixmap.height)
			
	Next
	
	If hpart > 0 Then
		tiledPixmap.paste(botPixmap, x * pixmap.width, hfull * pixmap.height)
	End If

Next

If wpart > 0 Then

	For Local y:Int = 0 Until hfull
	
		tiledPixmap.paste(sidePixmap, wfull * pixmap.width, y * pixmap.height)
			
	Next

	If hpart > 0 Then
		tiledPixmap.paste(pixmap.window(0, 0, wpart, hpart), wfull * pixmap.width, hfull * pixmap.height)
	End If

End If
End Rem
SetPanelPixmap(panel, pixmap, PANELPIXMAP_TILE)


While True
	WaitEvent 
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
	End Select
Wend
