
' labelovertest.bmx

SuperStrict 

Framework BaH.GTKMaxGUI

Local window:TGadget

window = CreateWindow("create label test",30,20,320,480, Null,WINDOW_TITLEBAR|WINDOW_STATUS)'|WINDOW_RESIZABLE)

Local label:TGadget = CreateLabel("http://blitzmax.com",50,10,110,22,window)

Local btn:TGadget = CreateButton("Fish", 220, 400, 90, 26, window)
SetGadgetTextColor(btn, 255, 0, 0)
'SetGadgetToolTip(label, "Watch me change!")

While WaitEvent()<>EVENT_WINDOWCLOSE

	Print CurrentEvent.toString()
	
	Select EventSource()
		Case label
			Select EventID()
				Case EVENT_MOUSEENTER
					SetPointer(POINTER_HAND)
					SetGadgetTextColor(label, 0, 0, 255)
				Case EVENT_MOUSELEAVE
					SetPointer(POINTER_DEFAULT)
					SetGadgetTextColor(label, 0, 0, 0)
				Case EVENT_MOUSEDOWN
					OpenURL(GadgetText(label))
			End Select
	End Select


Wend

