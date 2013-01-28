'
' REMEMBER : Compile as a Console app, not as GUI !
'

SuperStrict

Framework BaH.Curses
Import BRL.StandardIO

Local dialog:TCursesDialog = TCursesDialog.Create(POS_CENTER, POS_CENTER, [ "<C></U>Simple Command Interface", ..
	"Pick the command you wish to run.", ..
	"<C>Press </R>?<!R> for help."], ..
	["Who", "Time", "Date", "Quit"], A_REVERSE, True, True, False)


If dialog Then

	' Create the key binding.
	dialog.bindKey(Asc("?"), Null, dialogHelpCB)

	' Activate the dialog box.
	
	Local selection:Int

	While selection <> 3

		selection = dialog.activate()

		' Check the results.
		
		Select selection
			Case 0
				popupLabel(["<C>     </U>Login Name<!U>     ","<C><" + GetUserName() + ">"])
			Case 1
				popupLabel(["<C>   </U>Current Time<!U>   ", "<C>" + CurrentTime()])
			Case 2
				popupLabel(["<C>   </U>Current Date<!U>   ", "<C>" + CurrentDate()])
		End Select
	Wend
	
End If

' callback for dialog key binding
Function dialogHelpCB(widget:TCursesWidget, data:Object, key:Int)

	Select TCursesDialog(widget).currentButton()
		Case 0
			popupLabel(["<C></U>Help for </U>Who<!U>.", ..
				"<C>When this button is picked the name of the current", ..
				"<C>user is displayed on the screen in a popup window."])
		Case 1
			popupLabel(["<C></U>Help for </U>Time<!U>.", ..
				"<C>When this button is picked the current time is", ..
				"<C>displayed on the screen in a popup window."])
		Case 2
			popupLabel(["<C></U>Help for </U>Date<!U>.", ..
				"<C>When this button is picked the current date is", ..
				"<C>displayed on the screen in a popup window."])
		Case 3
			popupLabel(["<C></U>Help for </U>Quit<!U>.", ..
				"<C>When this button is picked the dialog box is exited."])
	End Select
	
End Function
