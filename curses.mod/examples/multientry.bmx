'
' REMEMBER : Compile as a Console app, not as GUI !
'

SuperStrict

Framework BaH.Curses
Import BRL.StandardIO

' Create the entry widget
Local entry:TCursesMultiEntry = TCursesMultiEntry.Create(POS_CENTER, POS_CENTER, "<C></5>Enter a message.<!5>", ..
		"<R>Message ", A_BOLD, ".", vMixed, 20, 5, 20, 0)
	
If entry Then

	' Activate the entry field.
	Local info:String = entry.activate()
	entry.free()

	Notify(info)

End If



