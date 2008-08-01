' windowpointertest.bmx

Strict

Framework bah.gtkmaxgui
Import BRL.Timer

Local window:TGadget

window=CreateWindow("My Window",40,40,320,240, Null)

SetMinWindowSize(window, 320, 240)

CreateTimer 10

While True
	WaitEvent 

	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
		Case EVENT_TIMERTICK
			Local x:Int
			Local y:Int
			GetWindowPointerPos(window, x, y)
			SetStatusText(window, "~t" + x + " , " + y)
	End Select

Wend

