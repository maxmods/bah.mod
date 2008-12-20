'
' REMEMBER : Compile as a Console app, not as GUI !
'

SuperStrict

Framework BaH.Curses
Import BRL.Timer
Import BRL.EventQueue

Local timer:TTimer

Global timeLabel:TCursesLabel = TCursesLabel.Create(POS_RIGHT, POS_TOP, [CurrentTime()])

Local entry:TCursesEntry = TCursesEntry.Create(POS_CENTER, POS_CENTER, "<C>Enter some text.", ..
		"</U/5>Text:<!U!5>", A_NORMAL, ".", vMixed, 40, 0, 256)

If timeLabel Then

	timeLabel.show()

	timer = CreateTimer(1)
	AddHook EmitEventHook, eventHook, timer

	' entry will accept key input
	SetFocus(entry)
	
	While True
		WaitEvent()
		Select CurrentEvent.id
			Case EVENT_GADGETACTION
				Select CurrentEvent.data
					Case vNORMAL
						popupLabel(["<C>You typed", "", "<C>'" + entry.getValue() + "'"])
					Case vESCAPE_HIT
						popupLabel(["<C> You aborted the entry by hitting escape!! "])
				End Select
				Exit	
		End Select
	Wend

End If

Function eventHook:Object(id%, data:Object, context:Object)

	If TEvent(data) Then
		If TEvent(data).id = EVENT_TIMERTICK
			timeLabel.setText([CurrentTime()])
			timeLabel.getScreen().refresh()
		End If
	End If

	Return data
End Function
