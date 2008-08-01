
' createpanel_pixmap.bmx

Strict 

Framework bah.gtkmaxgui
Import BRL.EventQueue

Local window:TGadget
Local panel:TGadget
Local panel2:TGadget
Local group:TGadget

window=CreateWindow("My Window",40,40,680,400)

' create a purple panel that occupies entire window client area
'DebugStop
panel=CreatePanel(0,0,ClientWidth(window),ClientHeight(window),window,PANEL_ACTIVE|PANEL_BORDER)'PANEL_ACTIVE| PANEL_BORDER)
SetGadgetLayout panel,1,1,1,1
SetPanelColor panel,100,0,200

' and a smaller box

panel2=CreatePanel(10,10,100,100,panel,PANEL_ACTIVE | PANEL_BORDER)
panel2.SetColor(160,255,160)

' and a group panel with a child button

group=CreatePanel(120,10,100,100,panel,PANEL_GROUP)
group.settext("My Group")
'DebugStop
Local btn:TGadget = CreateButton("Clear",5,5,64,24,group)

Local panel3:TGadget = CreatePanel(20, 200, 150, 160, panel)
Local panel4:TGadget = CreatePanel(180, 200, 150, 160, panel)
Local panel5:TGadget = CreatePanel(340, 200, 150, 160, panel)
Local panel6:TGadget = CreatePanel(500, 200, 150, 160, panel)

Local pixmap:TPixmap = LoadPixmap("bmax120.png")
SetPanelPixmap(panel3, pixmap, PANELPIXMAP_TILE)
SetPanelPixmap(panel4, pixmap, PANELPIXMAP_CENTER)
SetPanelPixmap(panel5, pixmap, PANELPIXMAP_FIT)
SetPanelPixmap(panel6, pixmap, PANELPIXMAP_STRETCH)

panel5.SetColor(255, 255, 255)


While True
	WaitEvent 
	'Print CurrentEvent.ToString()
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
		Case EVENT_GADGETACTION
			Select EventSource()
				Case btn
					SetPanelPixmap(panel6, Null, 0)
			End Select
	End Select
Wend
