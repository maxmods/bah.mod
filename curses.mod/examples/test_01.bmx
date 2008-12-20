SuperStrict

Framework BaH.Curses


Local file:String = RequestFile("Select a file")

If file Then
	popupLabel(["<C>You picked", "<C><" + file + ">"])
End If
