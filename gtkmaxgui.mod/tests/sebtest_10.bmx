SuperStrict
Framework BaH.GTKMAxGUI

Global wndMain:TGadget = CreateWindow("Test Window",100,100,400,300,Null,WINDOW_TITLEBAR|WINDOW_CLIENTCOORDS)

	Global gadPanel:TGadget = CreatePanel(10,10,100,30,wndMain,PANEL_ACTIVE)
	
		Global gadButton:TGadget = CreateButton("I'm A Button",0,0,100,30,gadPanel,BUTTON_PUSH)
		'Global gadButton:TGadget = CreateButton("I'm A Button",1,1,98,28,gadPanel,BUTTON_PUSH)		'Try with smaller size, coming from top is fine, coming from bottom still causes erratic behaviour.
		
		SetPanelColor(gadPanel,255,0,0)

Repeat

	Select WaitEvent()
	
		Case EVENT_WINDOWCLOSE
			End
		
		Case EVENT_MOUSEENTER
			Hover()
		
		Case EVENT_MOUSELEAVE
			LostHover()
		
		Case EVENT_GADGETACTION
			Notify "Button Clicked"
		
	EndSelect
	
	DebugLog CurrentEvent.ToString$()

Forever


Function Hover()

	SetPanelColor(gadPanel,0,0,255)
	HideGadget(gadButton)

EndFunction

Function LostHover()

	SetPanelColor(gadPanel,255,0,0)
	ShowGadget(gadButton)
	
EndFunction
