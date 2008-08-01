' createbutton.bmx
Strict 

Framework bah.gtkmaxgui
Import brl.eventqueue

Local window:TGadget
Local button:TGadget[8]
'DebugStop
window=CreateWindow("MaxGui Buttons",40,40,240,300,Null,WINDOW_TITLEBAR)
button[0]=CreateButton("Push Button",10,10,140,24,window,BUTTON_PUSH)
button[1]=CreateButton("Checkbox Button",10,34,140,24,window,BUTTON_CHECKBOX)
button[2]=CreateButton("Radio Button 1",10,58,140,24,window,BUTTON_RADIO)
button[3]=CreateButton("Radio Button 2",10,82,140,24,window,BUTTON_RADIO)
button[4]=CreateButton("Radio Button 2",10,106,140,24,window,BUTTON_RADIO)
button[5]=CreateButton("OK",10,140,70,24,window,BUTTON_OK)
button[6]=CreateButton("Cancel",84,140,70,24,window,BUTTON_CANCEL)

' A button with an image!!
button[7] = CreateButton("Home!", 50, 170, 90, 28, window)
SetButtonPixmap(button[7], LoadPixmap("home.png"))

SetGadgetToolTip(button[3], "An interesting option~nWhy not?")
SetGadgetToolTip(button[4], "Let's do it!")
SetGadgetToolTip(button[5], "Cancel me!")

While True
	WaitEvent 
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
		Case EVENT_GADGETACTION
			Print "EVENT_GADGETACTION"
	End Select
Wend
