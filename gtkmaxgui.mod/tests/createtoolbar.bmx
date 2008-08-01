' createtoolbar.bmx

Strict

Framework bah.gtkmaxgui


Local window:TGadget=CreateWindow("My Window",50,50,480,240)

Local toolbar:TGadget=CreateToolBar("toolbar.bmp",0,0,0,0,window)

SetToolBarTips toolbar,["New","Open","Save~nmy soul", "Bézier", "Yay!"] 

AddGadgetItem toolbar,"hello",GADGETITEM_TOGGLE,2,"This item is a quick test of GADGETITEM_TOGGLE"

DisableGadgetItem toolbar,2

While WaitEvent()
	Select EventID()
		Case EVENT_GADGETACTION
			If EventSource()=toolbar 
				Print "ToolBar GadgetAction~nEventData()="+EventData()
			EndIf
		Case EVENT_WINDOWCLOSE
			End
	End Select
Wend
