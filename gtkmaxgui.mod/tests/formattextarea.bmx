Strict

Framework BaH.gtkmaxgui

Local window:TGadget
Local textarea:TGadget

window=CreateWindow("My Window",130,20,200,200,,15|WINDOW_ACCEPTFILES)

Local button:TGadget = CreateButton("make red", 0, 0, 100, 26, window)
SetGadgetLayout button,1,0,1,0

Local button2:TGadget = CreateButton("make blue", 120, 0, 100, 26, window)
SetGadgetLayout button2,1,0,1,0

textarea=CreateTextArea(0,35,ClientWidth(window),ClientHeight(window)/2,window)
SetGadgetLayout textarea,1,1,1,1
SetGadgetText textarea,"a textarea gadget~none line~nandanother"
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
				Local _len:Int = TextAreaSelLen(textarea)
				If  _len > 0 Then
					Local _start:Int = TextAreaCursor(textarea)
					FormatTextAreaText(textarea,255,0,0,0,_start,_len)
				End If
			Case button2
				Local _len:Int = TextAreaSelLen(textarea)
				If  _len > 0 Then
					Local _start:Int = TextAreaCursor(textarea)
					FormatTextAreaText(textarea,0,0,255,0,_start,_len)
				End If
			End select
	End Select
Wend

