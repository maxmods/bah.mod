SuperStrict

Framework BaH.Curses
Import BRL.StandardIO

Local slider:TCursesSlider = TCursesSlider.Create(POS_CENTER, POS_CENTER, "<C></U>Enter a value:", ..
		"</B>Current Value:", A_REVERSE | COLOR_PAIR (29) | Asc(" ") , 50, 1, 1, 100, 1, 5)

If slider Then

	Local selection:Int = slider.activate()
	
	Select slider.exitType()
	
		Case vESCAPE_HIT
		
			' remove the slider widget
			slider.free()

			' display a message
			popupLabel(["<C>You hit escape. No information passed back.", "", "<C>Press any key to continue."])
			
		Case vNORMAL
		
			' remove the slider widget
			slider.free()
			
			' display a message
			popupLabel(["<C>You selected ", "<C>" + selection , "", "<C>Press any key to continue."])
	
		Default
		
			slider.free()
			
	End Select

End If
