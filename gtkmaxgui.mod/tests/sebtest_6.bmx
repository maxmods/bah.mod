SuperStrict

Framework bah.gtkmaxgui

' feature request....

Global wndMain:TGadget = CreateWindow("Test Window", 100, 100, 400, 300, Null, 15)
'DebugStop
SetStatusText(wndMain,"This is on the left~tThis is in the middle~tThis is on the right.")

Local but1:TGadget = CreateButton("One", 10, 10, 90, 28, wndMain)
Local but2:TGadget = CreateButton("Two", 10, 40, 90, 28, wndMain)
Local but3:TGadget = CreateButton("Three", 10, 70, 90, 28, wndMain)
Local but4:TGadget = CreateButton("Lots 1", 10, 100, 90, 28, wndMain)
Local but5:TGadget = CreateButton("Lots 2", 10, 130, 90, 28, wndMain)
Local but6:TGadget = CreateButton("Lots 3", 10, 160, 90, 28, wndMain)
Local but7:TGadget = CreateButton("All", 10, 190, 90, 28, wndMain)

Repeat
	WaitEvent
	
	Select EventSource()
		Case but1
			SetStatusText(wndMain,"This is on the left")
		Case but2
			SetStatusText(wndMain,"~tThis is in the middle")
		Case but3
			SetStatusText(wndMain,"~t~tThis is on the right.")
		Case but4
			SetStatusText(wndMain,"This is a lot of text in the first bit... fill it up!")
		Case but5
			SetStatusText(wndMain,"~tThis is a lot of text in the middle bit... fill it up!")
		Case but6
			SetStatusText(wndMain,"~t~tThis is a lot of text in the last bit... fill it up!")
		Case but7
			SetStatusText(wndMain,"This is on the left~tThis is in the middle~tThis is on the right.")
	End Select
Until EventID() = EVENT_WINDOWCLOSE
