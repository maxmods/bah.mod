SuperStrict

Framework BaH.GTKMaxGUI
'Framework BRL.FLTKMaxGUI

Global wndMain:TGadget = CreateWindow("Test Window",100,100,400,300,Null,15)
Global gadButton:TGadget = CreateButton("Test Button",20,20,100,30,wndMain)
	
SetGadgetHotKey(gadButton,KEY_T,MODIFIER_COMMAND)	'Set Gadget HotKey as Control+T


Repeat
	
	Select WaitEvent()
	
		Case EVENT_WINDOWCLOSE;End
		
		Case EVENT_GADGETACTION;If EventSource() = gadButton Then Notify("Button Pressed!!!")
		
	EndSelect
	
Forever
