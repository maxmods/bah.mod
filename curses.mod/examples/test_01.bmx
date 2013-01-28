SuperStrict

Framework BaH.Curses


Local file:String = RequestFile("Select a file")

If file Then
	Notify("You picked~n'" + file + "'")
End If

Notify Proceed("Are you sure?")