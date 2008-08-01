Strict

Framework BaH.gtkmaxgui

Local window:TGadget
Local textarea:TGadget

window=CreateWindow("My Window",130,20,200,200,,15|WINDOW_ACCEPTFILES)

Local button:TGadget = CreateButton("Text", 0, 0, 85, 26, window)
SetGadgetLayout button,1,0,1,0

Local button2:TGadget = CreateButton("Add", 95, 0, 95, 26, window)
SetGadgetLayout button2,1,0,1,0
SetGadgetToolTip(button2, "Add some text to the end")

textarea=CreateTextArea(0,35,ClientWidth(window),ClientHeight(window)/2,window)
SetGadgetLayout textarea,1,1,1,1
SetGadgetText textarea,"a textarea gadget~none line~nandanother"
ActivateGadget textarea
SetGadgetToolTip(textarea, "Just some random noise")

SelectTextAreaText textarea,1,1,TEXTAREA_LINES

Print TextAreaCursor(textarea,TEXTAREA_LINES) 
Print TextAreaSelLen(textarea,TEXTAREA_LINES) 

While WaitEvent()
	Print CurrentEvent.ToString()+" "+EventSourceHandle()
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
		Case EVENT_APPTERMINATE
			End
		Case EVENT_GADGETACTION
			Select EventSource()
				Case button
					Print TextAreaText(textarea,1,1,TEXTAREA_LINES)
				Case button2
					AddTextAreaText(textarea, "added")
			End Select
	End Select
Wend
