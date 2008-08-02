' createpanel.bmx

Strict 

Framework bah.gtkmaxgui
Import brl.eventqueue

Local window:TGadget
Local panel1:TGadget
Local panel2:TGadget
'Local group:TGadget

window=CreateWindow("My Window",40,40,320,240)

' create a purple panel that occupies entire window client area
'DebugStop
panel1=CreatePanel(0,0,ClientWidth(window) / 2,ClientHeight(window) ,window , PANEL_BORDER)'PANEL_ACTIVE| PANEL_BORDER)
SetGadgetLayout panel1,1,0,1,1

' and a smaller box

panel2=CreatePanel(ClientWidth(window) / 2, 0,ClientWidth(window) / 2,ClientHeight(window) ,window,PANEL_BORDER)'PANEL_ACTIVE| PANEL_BORDER)
SetGadgetLayout panel2,1,1,1,1

Local area1:TGadget = CreateTextArea(0, 0, ClientWidth(panel1), ClientHeight(panel1), panel1)
SetGadgetLayout area1,1,1,1,1

Local area2:TGadget = CreateTextArea(0, 0, ClientWidth(panel2), ClientHeight(panel2), panel2)
SetGadgetLayout area2,1,1,1,1

' and a group panel with a child button

'group=CreatePanel(120,10,100,100,panel,PANEL_GROUP)
'group.settext("My Group")
'DebugStop
'CreateButton("hello",5,5,64,24,group)


While True
	WaitEvent 
	Print CurrentEvent.ToString()
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
	End Select
Wend

