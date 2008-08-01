' createwindow.bmx

Strict

Framework bah.gtkmaxgui

Local window:TGadget

window=CreateWindow("My Window",40,40,320,240, Null, WINDOW_TITLEBAR | WINDOW_RESIZABLE | WINDOW_ACCEPTFILES)

SetMinWindowSize(window, 320, 240)

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
