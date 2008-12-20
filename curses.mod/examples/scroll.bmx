'
' REMEMBER : Compile as a Console app, not as GUI !
'

SuperStrict

Framework BaH.Curses
Import BRL.StandardIO
Import BRL.FileSystem

Local items:String[] = LoadDir(".")


Local scroll:TCursesScrollList = TCursesScrollList.Create(POS_CENTER, POS_CENTER, 0, 10, 50, "<C></5>Pick a file", ..
		items, False, A_REVERSE)
	
If scroll Then

	Local selection:Int = scroll.activate()
	

End If
