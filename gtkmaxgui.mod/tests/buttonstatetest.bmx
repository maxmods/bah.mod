' buttonstatetest.bmx
SuperStrict

Framework bah.gtkmaxgui
Import brl.eventqueue

Local window:TGadget
Local button:TGadget[8]
'DebugStop
window=CreateWindow("Button State Test",40,40,400,400,Null,WINDOW_TITLEBAR)
button[0]=CreateButton("Radio Button 1",10,50,140,26,window,BUTTON_RADIO)
button[1]=CreateButton("Radio Button 2",10,80,140,26,window,BUTTON_RADIO)
button[2]=CreateButton("Radio Button 3",10,110,140,26,window,BUTTON_RADIO)
button[3]=CreateButton("Radio Button 4",10,140,140,26,window,BUTTON_RADIO)

SetGadgetTextColor(button[1], 255, 0, 0)

Local btnSelectNext:TGadget = CreateButton("Select Next", 30, 170, 100, 26, window)
Local btnSelectPrev:TGadget = CreateButton("Select Prev", 30, 200, 100, 26, window)
Local btnShowRadio:TGadget = CreateButton("Show Selected", 20, 230, 120, 26, window)

button[4]=CreateButton("Check Button 1",200,50,140,26,window,BUTTON_CHECKBOX)
button[5]=CreateButton("Check Button 2",200,80,140,26,window,BUTTON_CHECKBOX)
button[6]=CreateButton("Check Button 3",200,110,140,26,window,BUTTON_CHECKBOX)
button[7]=CreateButton("Check Button 4",200,140,140,26,window,BUTTON_CHECKBOX)

Local btnSelectAll:TGadget = CreateButton("Select All", 220, 170, 100, 26, window)
Local btnSelectNone:TGadget = CreateButton("Select None", 220, 200, 100, 26, window)
Local btnShowSelected:TGadget = CreateButton("Show Selected", 210, 230, 120, 26, window)

While True
	WaitEvent
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
		Case EVENT_GADGETACTION
			Select EventSource()
				Case button[0],button[1],button[2],button[3],button[4],button[5],button[6],button[7]
					If EventData() Then
						Print GadgetText(TGadget(EventSource())) + " selected"
					Else
						Print GadgetText(TGadget(EventSource())) + " de-selected"
					End If
				Case btnSelectNext
					For Local i:Int = 0 To 3
						If ButtonState(button[i]) Then
							SetButtonState(button[i], False)
							If i < 3 Then
								SetButtonState(button[i + 1], True)
							Else
								SetButtonState(button[0], True)
							End If
							Exit
						End If
					Next

				Case btnSelectPrev
					For Local i:Int = 3 To 0 Step - 1
						If ButtonState(button[i]) Then
							SetButtonState(button[i], False)
							If i > 0 Then
								SetButtonState(button[i - 1], True)
							Else
								SetButtonState(button[3], True)
							End If
							Exit
						End If
					Next

				Case btnSelectAll
					For Local i:Int = 4 To 7
						SetButtonState(button[i], True)
					Next
				Case btnSelectNone
					For Local i:Int = 4 To 7
						SetButtonState(button[i], False)
					Next
				Case btnShowSelected
					For Local i:Int = 4 To 7
						If ButtonState(button[i]) Then
							Print GadgetText(button[i]) + " is selected"
						End If
					Next
				Case btnShowRadio
					For Local i:Int = 0 To 3
						If ButtonState(button[i]) Then
							Print GadgetText(button[i]) + " is selected"
						End If
					Next
			End Select
	End Select
Wend

