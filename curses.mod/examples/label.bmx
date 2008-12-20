'
' REMEMBER : Compile as a Console app, not as GUI !
'

SuperStrict

Framework BaH.Curses
Import BRL.StandardIO

Local mesg:String[] = ["</29/B>This line should have a yellow foreground and a blue background.", ..
		"</5/B>This line should have a white  foreground and a blue background.", ..
		"</26/B>This line should have a yellow foreground and a red  background.", ..
		"<C>This line should be set to whatever the screen default is."]


Local label:TCursesLabel = TCursesLabel.Create(POS_CENTER, POS_CENTER, mesg)
	
If label Then

	refreshScreen()
	label.wait()

End If