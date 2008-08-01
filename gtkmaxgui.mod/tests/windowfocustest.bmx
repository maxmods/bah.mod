' windowfocustest.bmx

Strict

Framework bah.gtkmaxgui

Local window1:TGadget


window1=CreateWindow("My Window",40,40,320,240, Null, WINDOW_TITLEBAR | WINDOW_RESIZABLE)
Local button:TGadget = CreateButton("Activate!", 20, 20, 100, 28, window1)

Local window2:TGadget =CreateWindow("Window 2",400,400,100,100, Null, WINDOW_TITLEBAR | WINDOW_RESIZABLE)


ActivateWindow(window1)

While True
	WaitEvent 
	Print CurrentEvent.ToString()
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
		Case EVENT_GADGETACTION
			ActivateWindow(window2)
	End Select
	
'	Print "max = " + WindowMaximized(window)
'	Print "min = " + WindowMinimized(window)
Wend

