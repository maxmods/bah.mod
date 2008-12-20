'
' REMEMBER : Compile as a Console app, not as GUI !
'

SuperStrict

Framework BaH.Curses
Import BRL.StandardIO

Local dialog:TCursesDialog = TCursesDialog.Create(POS_CENTER, POS_CENTER, [ "<C></U>Dialog Widget Demo", ..
	" ", "<C>The dialog widget allows the programmer to create", ..
	"<C>a popup dialog box with buttons. The dialog box", ..
	"<C>can contain </B/32>colours<!B!32>, </R>character attributes<!R>", ..
	"<R>and even be right justified.", "<L>and left."], ..
	["</B/24>Ok", "<R></B16>Cancel"], COLOR_PAIR(2) | A_REVERSE, True, True, False)


If dialog Then

	dialog.activate()

	Print ":-)"
Else
	Print ":-("

End If