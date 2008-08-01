' createwindow.bmx

Strict

Framework bah.gtkmaxgui
Import brl.eventqueue

Local window:TGadget

window=CreateWindow("My Window",40,40,320,240, Null, WINDOW_TITLEBAR | WINDOW_RESIZABLE)

SetMinWindowSize(window, 320, 240)

Local childWindow1:TGadget = CreateWindow("Child 1", 500, 40, 100, 200, window, WINDOW_TITLEBAR | WINDOW_CHILD)

While True
	WaitEvent 
	Print CurrentEvent.ToString()
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
	End Select
	
'	Print "max = " + WindowMaximized(window)
'	Print "min = " + WindowMinimized(window)
Wend

