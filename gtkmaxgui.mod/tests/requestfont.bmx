Strict

Framework bah.gtkmaxgui

Local window:TGadget
Local button:TGadget
Local label:TGadget
Local font:TGuiFont

window=CreateWindow("RequestFont",30,20,200,250)
label=CreateLabel("font example",4,4,192,132,window)
button=CreateButton("Select Font",4,140,100,26,window)

While WaitEvent()
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
		Case EVENT_GADGETACTION
			font=RequestFont(font)
			If font
				SetGadgetFont label,font
				SetGadgetText label,FontName(font)+":"+FontSize(font)
			EndIf
	End Select
Wend
