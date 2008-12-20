'
' REMEMBER : Compile as a Console app, not as GUI !
'

SuperStrict

Framework BaH.Curses
Import BRL.StandardIO

Local buttons:String[] = [" OK ", " Cancel "]

' Create the entry widget
Local entry:TCursesEntry = TCursesEntry.Create(POS_CENTER, POS_CENTER, "<C>Enter a name", ..
		"Name ", A_NORMAL, ".", vMixed, 40, 0, 256)
	
If entry Then

	' Create the button box widget.	
	Local buttonBox:TCursesButtonBox = TCursesButtonBox.Create(entry.getbegx(), ..
				   entry.getbegy() + entry.boxHeight() - 1, entry.boxWidth() - 1, 1, ..
				   Null, 1, 2, buttons, A_REVERSE)
	
	If buttonBox Then
	
		' Set the lower left and right characters of the entry.
		entry.setLLChar(ACS_LTEE)
		entry.setLRChar(ACS_RTEE)
		' Set the upper left and right characters of the button box.
		buttonBox.setULChar(ACS_LTEE)
		buttonBox.setURChar(ACS_RTEE)

		' Bind the Tab key in the entry field to send a Tab key To the button box widget.
		entry.bindKey(KEY_TAB, buttonBox, entryCB)

		' display the button box
		buttonBox.show()
		
		' Activate the entry field.
		Local info:String = entry.activate()
		Local selection:Int = buttonBox.currentButton()
		
		Local s:String = "You typed in ("
		If info Then
			s:+ info
		Else
			s:+ "<null>"
		End If
		s:+ ") and selected button (" + buttons[selection] + ")"
		popupLabel([s])
	
	End If

End If

' callback for Entry field keys
Function entryCB:Int(widget:TCursesWidget, data:Object, key:Int)

	TCursesButtonBox(data).injectCharacter(key)
	Return True
	
End Function
