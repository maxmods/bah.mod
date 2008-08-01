SuperStrict 

Framework BaH.GTKMaxGUI

Local window:TGadget

window=CreateWindow("My Window",100,100,320,240)

Local button:TGadget = CreateButton("Popupmenu !", 50, 50, 100, 28, window)

Local menu:TGadget = CreateMenu("Popup", 100, Null)
Local menu2:TGadget = CreateMenu("Yay!", 101, menu)
Local menu3:TGadget = CreateMenu("Woohoo", 102, menu)

While True
	WaitEvent
'Print CurrentEvent.toString()
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
		Case EVENT_GADGETMENU
			PopupWindowMenu(window, menu, "Hello")
		Case EVENT_MENUACTION
			Select EventData()
				Case 101
					Print "Yay!   = " + String(EventExtra())
				Case 102
					Print "Woohoo = " + String(EventExtra())
			End Select
	End Select
Wend