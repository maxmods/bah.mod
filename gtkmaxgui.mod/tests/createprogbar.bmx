Strict 

Framework bah.gtkmaxgui
Import brl.timer


Local window:TGadget=CreateWindow("My Window",50,50,240,100,,WINDOW_TITLEBAR)

Local progbar:TGadget=CreateProgBar(10,10,200,20,window)

CreateLabel "Please Wait",10,40,200,20,window

CreateTimer 10

While WaitEvent()
	'Print currentevent.toString()
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
		Case EVENT_TIMERTICK
			Local t=EventData()
			If t=50 End
			UpdateProgBar progbar,t/50.0
			SetGadgetToolTip(progbar, "..current progress.. " + TGTKProgressBar(progbar).getValue())
	End Select
Wend
