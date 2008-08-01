' createpanel.bmx

Strict 

Framework bah.gtkmaxgui

Local window:TGadget
Local panel:TGadget
Local panel2:TGadget
Local group:TGadget

window=CreateWindow("My Window",40,40,320,240)

' create a purple panel that occupies entire window client area
'DebugStop
panel=CreatePanel(0,0,ClientWidth(window),ClientHeight(window),window,PANEL_ACTIVE|PANEL_BORDER)'PANEL_ACTIVE| PANEL_BORDER)
SetGadgetLayout panel,1,1,1,1
SetPanelColor panel,100,0,200

' and a smaller box

panel2=CreatePanel(10,10,100,100,panel,PANEL_ACTIVE | PANEL_BORDER)
panel2.SetColor(160,255,160)

' and a group panel with a child button

group=CreatePanel(120,10,100,100,panel,PANEL_GROUP, "My Group")
'group.settext("My Group")
'DebugStop
CreateButton("hello",5,5,64,24,group)


While True
	WaitEvent 
	Print CurrentEvent.ToString()
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
	End Select
Wend
