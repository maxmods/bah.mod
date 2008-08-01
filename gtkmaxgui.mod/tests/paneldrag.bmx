Strict

Framework bah.gtkmaxgui
Import BRL.eventqueue

Local window:TGadget
'DebugStop
window=CreateWindow("My Window",40,40,320,240, Null, WINDOW_TITLEBAR | WINDOW_RESIZABLE)

Local panel:TGadget = CreatePanel(10, 10, ClientWidth(window) - 20 , ClientHeight(window) -20, window, PANEL_ACTIVE)
SetPanelColor(panel, 255, 0, 0)

SetGadgetLayout panel, 1,1,1,1
SetGadgetShape window, 300, 300, 800, 300

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

