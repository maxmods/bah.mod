SuperStrict

Framework BaH.Curses
Import BRL.StandardIO
Import BRL.EventQueue

Local old:Int
mousemask(ALL_MOUSE_EVENTS, old)

While True
	WaitEvent()
	Select CurrentEvent.id
		Case EVENT_GADGETACTION
			Select CurrentEvent.data
				'Case vNORMAL
				'	popupLabel(["<C>You typed", "", "<C>'" + entry.getValue() + "'"])
				'Case vESCAPE_HIT
				'	popupLabel(["<C> You aborted the entry by hitting escape!! "])
			End Select
			Exit	
	End Select
Wend

