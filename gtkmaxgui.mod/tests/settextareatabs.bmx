Strict

Framework BaH.gtkmaxgui

Local window:TGadget
Local textarea:TGadget

window=CreateWindow("My Window",130,20,200,200,,15|WINDOW_ACCEPTFILES)

Local button:TGadget = CreateButton("set 4", 0, 0, 100, 26, window)
SetGadgetLayout button,1,0,1,0

Local button2:TGadget = CreateButton("set 2", 120, 0, 100, 26, window)
SetGadgetLayout button2,1,0,1,0

textarea=CreateTextArea(0,35,ClientWidth(window),ClientHeight(window)/2,window)
SetGadgetLayout textarea,1,1,1,1
SetGadgetText textarea,"a textarea gadget~n~tone line~n~t~tandanother"
ActivateGadget textarea

'SelectTextAreaText textarea,1,1,TEXTAREA_LINES

'Print TextAreaCursor(textarea,TEXTAREA_LINES) 
'Print TextAreaSelLen(textarea,TEXTAREA_LINES) 

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
				SetTextAreaTabs(textarea, 4)
			Case button2
				SetTextAreaTabs(textarea, 2)
			End Select
	End Select
Wend

