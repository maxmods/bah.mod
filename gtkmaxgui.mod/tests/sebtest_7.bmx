
Rem
A few more little bugs...

1) Adding a double ampersand "&" to the text of a gadget doesn't produce a single "&" to the button (as in other modules).
I.e. "Fish & Chips"
2) When a mouse move event is triggered on a panel, the EventData() value is supposed to tell us whether the mouse button
is down. In GTK, it only seems to do this for the first event. Try holding your mouse down while moving over the panel to see.
3) If you run the app, making sure the mouse isn't over the window as it loads, the button will be correctly positioned at 10x10,
however, move your mouse over the pane to generate an event and the whole panel/button jumps upwards... Weird, eh?

Thanks Brucey

EndRem

SuperStrict

Framework BAH.GTKMaxGUI

Global wndMain:TGadget = CreateWindow( "Test Window" , 220 , 140 , 400 , 300 , Null , WINDOW_TITLEBAR | WINDOW_STATUS | WINDOW_RESIZABLE | WINDOW_CLIENTCOORDS )
	Global gadPanel:TGadget = CreatePanel(0,0,ClientWidth(wndMain),ClientHeight(wndMain),wndMain,PANEL_ACTIVE)
	Global gadButton:TGadget = CreateButton("Fish && Chips",10,10,100,30,gadPanel,BUTTON_PUSH)
	
	SetPanelColor(gadPanel,200,255,200)	'So you can see the panel

Repeat

	Select WaitEvent()

		Case EVENT_WINDOWCLOSE;End

	EndSelect
	
	SetStatusText(wndMain,CurrentEvent.ToString$())

	DebugLog "Panel X: " + GadgetX(gadPanel) + " Panel Y: " + GadgetY(gadPanel)			'Watch the panel and button Y-pos fall to -9 when
	DebugLog "Button X: " + GadgetX(gadButton) + " Button Y: " + GadgetY(gadPanel)		'an event is triggered.

Forever