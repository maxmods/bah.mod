SuperStrict

?linux
Framework BaH.GTKMaxGUI
?Not linux
Framework MaxGUI.Drivers
?
Import BRL.EventQueue
Import BaH.MaxGUITextAreaScintilla

Global window:TGadget = CreateWindow( "TextArea Example", 130, 20, 300, 200 )

Global textarea:TGadget = CreateTextArea( 0, 0, ClientWidth(window), ClientHeight(window), window )
	SetGadgetLayout( textarea, EDGE_ALIGNED, EDGE_ALIGNED, EDGE_ALIGNED, EDGE_ALIGNED )
	SetGadgetText( textarea, "A TextArea gadget. :-)~n~nOne line...~n...and then another!")
	ActivateGadget( textarea )

' Select the entire third (index: 2 [base-0]) line.
SelectTextAreaText( textarea, 2, 1, TEXTAREA_LINES )

' Output the properties of the current text selection (should be 1, 1 as set above).
Print "TextAreaCursor(): " + TextAreaCursor( textarea, TEXTAREA_LINES )
Print "TextAreaSelLen(): " + TextAreaSelLen( textarea, TEXTAREA_LINES )

While WaitEvent()
	If EventSource() = textarea Then
		Print CurrentEvent.ToString()
	End If
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
		Case EVENT_APPTERMINATE
			End
	End Select
Wend
