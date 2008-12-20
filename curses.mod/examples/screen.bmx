'
' REMEMBER : Compile as a Console app, not as GUI !
'

SuperStrict

Framework BaH.Curses
Import BRL.StandardIO

' This demonstrates how to create five different screens and flip between them.

' Create the screens...

' first screen is already here ;-)
Local screen1:TCursesScreen = GetCurrentScreen()
Local screen2:TCursesScreen = TCursesScreen.Create()
Local screen3:TCursesScreen = TCursesScreen.Create()
Local screen4:TCursesScreen = TCursesScreen.Create()
Local screen5:TCursesScreen = TCursesScreen.Create()

Local label1:TCursesLabel = TCursesLabel.Create(POS_CENTER, POS_TOP, ["<C><#HL(30)>", ..
	"<C></R>This is the first screen.", "<C>Hit space to go to the next screen", ..
	"<C><#HL(30)>"], False, False)

SetCurrentScreen(screen2)

Local label2:TCursesLabel = TCursesLabel.Create(POS_RIGHT, POS_CENTER, ["<C><#HL(30)>", ..
	"<C></R>This is the second screen.", "<C>Hit space to go to the next screen", ..
	"<C><#HL(30)>"], False, False)
	
SetCurrentScreen(screen3)

Local label3:TCursesLabel = TCursesLabel.Create(POS_CENTER, POS_BOTTOM, ["<C><#HL(30)>", ..
	"<C></R>This is the third screen.", "<C>Hit space to go to the next screen", ..
	"<C><#HL(30)>"], False, False)
	
SetCurrentScreen(screen4)

Local label4:TCursesLabel = TCursesLabel.Create(POS_LEFT, POS_CENTER, ["<C><#HL(30)>", ..
	"<C></R>This is the fourth screen.", "<C>Hit space to go to the next screen", ..
	"<C><#HL(30)>"], False, False)

SetCurrentScreen(screen5)

Local dialog:TCursesDialog = TCursesDialog.Create(POS_CENTER, POS_CENTER, [ "<C><#HL(30)>", "<C>Screen 5", ..
	"<C>This is the last of 5 screens. If you want", "<C>to continue press the 'Continue' button.", ..
	"<C>Otherwise press the 'Exit' button", "<C><#HL(30)>"], ..
	["Continue", "Exit"], A_REVERSE, True, True, False)


While True

	' Draw the first screen.
	screen1.show()
	label1.wait(" ")
	screen1.hide()

	' Draw the second screen.
	screen2.show()
	label2.wait(" ")
	screen2.hide()

	' Draw the third screen.
	screen3.show()
	label3.wait(" ")
	screen3.hide()

	' Draw the fourth screen.
	screen4.show()
	label4.wait(" ")
	screen4.hide()

	' Draw the fifth screen.
	screen5.show()
	Local answer:Int = dialog.activate()
	
	If answer = 1 Then
	
		Exit
	
	End If
Wend

End
