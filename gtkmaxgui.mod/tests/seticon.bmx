Strict

Framework bah.gtkmaxgui
Import BRL.RamStream

Local window:TGadget

Incbin "bmax120.png"
'DebugStop
Local icon:TPixmap = LoadPixmap("incbin::bmax120.png")
'SetAppIcon(icon)

window=CreateWindow("My Window",40,40,320,240, Null, WINDOW_TITLEBAR | WINDOW_RESIZABLE)
?linux
'DebugStop
TGTKWindow(window).setIcon(icon)
?

While True
	WaitEvent 
	'Print CurrentEvent.ToString()
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
	End Select
	
'	Print "max = " + WindowMaximized(window)
'	Print "min = " + WindowMinimized(window)
Wend

