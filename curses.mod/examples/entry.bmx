'
' REMEMBER : Compile as a Console app, not as GUI !
'

SuperStrict

Framework BaH.Curses
Import BRL.StandardIO

Local entry:TCursesEntry = TCursesEntry.Create(POS_CENTER, POS_CENTER, "<C>Enter a~n<C>directory name.", ..
		"</U/5>Directory:<!U!5>", A_NORMAL, ".", vMixed, 40, 0, 256)
	
If entry Then

	' populate the entry with some text
	entry.setEntry("/some/path/somewhere", 0, 256)


	' show the entry
	Local value:String = entry.activate()
	
	Select entry.exitType()
	
		Case vESCAPE_HIT
		
			' remove the entry widget
			entry.free()
			
			' display a message
			popupLabel(["<C>You hit escape. No information passed back.", "", "<C>Press any key to continue."])
			
		Case vNORMAL
		
			' remove the entry widget
			entry.free()
			
			' display a message
			popupLabel(["<C>You typed in the following", "<C>" + value , "", "<C>Press any key to continue."])
	
		Default
		
			entry.free()
			
	End Select
	

Else
	Print "Couldn't open the entry box. Perhaps the window is too small?"

End If


