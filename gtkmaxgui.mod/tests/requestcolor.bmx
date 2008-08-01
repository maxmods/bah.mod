Strict

Framework BaH.gtkmaxgui

Local window:TGadget
Local panel:TGadget
Local red:Int = 120
Local green:Int = 120
Local blue:Int = 120

window=CreateWindow("RequestColor",40,40,320,240)
panel=CreatePanel(20,20,32,32,window,PANEL_ACTIVE|PANEL_BORDER)

While True
	WaitEvent
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
		Case EVENT_MOUSEDOWN
			If RequestColor(red,green,blue)
				red=RequestedRed()
'Print red
				green=RequestedGreen()
'Print green
				blue=RequestedBlue()
'Print blue
				SetPanelColor panel,red,green,blue
			EndIf				
	End Select
Wend
