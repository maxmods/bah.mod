?win32
Framework brl.win32maxgui
?linux
Framework BaH.GTKMaxGUI
'Framework BRL.FLTKMaxGUI
?
Import BRL.Timer

Global wndMain:TGadget = CreateWindow("Hello",100,100,400,300,Null,15)
Global gadButton:TGadget = CreateButton("&Test Button",10,30,370,30,wndMain)
Global gadCheckBox:TGadget = CreateButton("T&est Button",10,70,370,30,wndMain, BUTTON_CHECKBOX)
Global gadRadioButton:TGadget = CreateButton("Te&st Button",10,110,370,30,wndMain, BUTTON_RADIO)
	
Repeat

	Select WaitEvent()
		
		Case EVENT_WINDOWCLOSE;End
		
	EndSelect
	
	DebugLog CurrentEvent.toString$()
	
Forever
