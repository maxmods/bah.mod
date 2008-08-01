?win32
Framework brl.win32maxgui
?linux
Framework BaH.GTKMaxGUI
?
'Framework BRL.FLTKMaxGUI
Import BRL.Timer

Global wndMain:TGadget = CreateWindow("Hello",100,100,400,300,Null,15)
	Global gadTextBox:TGadget = CreateTextField(10,10,370,20,wndMain)
	Global gadCheckBox:TGadget = CreateButton("Test Checkbox",10,30,370,20,wndMain,BUTTON_CHECKBOX)
	
	CreateTimer(1)
	
Global varSwitch:Int = 0

Repeat

	Select WaitEvent()
		
		Case EVENT_WINDOWCLOSE;End
		
		Case EVENT_TIMERTICK
		
			If varSwitch = 0 Then
		
				SetButtonState(gadCheckBox,1)
				SetGadgetText(gadTextBox,"Test Text")
				varSwitch = 1
			
			Else
			
				SetButtonState(gadCheckBox,0)
				SetGadgetText(gadTextBox,"")
				varSwitch = 0
			
			EndIf
		Case EVENT_GADGETACTION
			If EventSource() = gadTextBox Then
				DebugLog("gadTextBox!!")
			End If
	EndSelect
	
	DebugLog CurrentEvent.toString$()
	
Forever
