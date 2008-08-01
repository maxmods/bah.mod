Strict 

Framework bah.gtkmaxgui

Local window:TGadget

Print "desktop width = " + GadgetWidth(Desktop())
Print "desktop height = " + GadgetHeight(Desktop())

Local width:Int = 320
Local height:Int = 607
'Local x:Int = GadgetWidth(Desktop()) / 2 - width / 2
'Local y:Int = GadgetHeight(Desktop()) / 2 - height / 2

window=CreateWindow("My Centered Window", 0, 0, width, height)

While True
	WaitEvent 
	Print CurrentEvent.ToString()
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
	End Select
Wend
