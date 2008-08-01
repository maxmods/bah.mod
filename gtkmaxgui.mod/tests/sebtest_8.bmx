Framework BaH.GTKMaxGUI

Global wndMain:TGadget = CreateWindow("Test Window",100,100,400,300,Null,15)

	Global gadTextbox:TGadget = CreateTextField(10,10,300,20,wndMain)

Repeat

	Select WaitEvent()

		Case EVENT_WINDOWCLOSE;End

	EndSelect

	Local g:TGadget = ActiveGadget()
	If g Then
		DebugLog g.xpos
	End If

Forever

