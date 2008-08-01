' createtextfield.bmx

Strict 

Framework BaH.gtkmaxgui

Local window:TGadget
Local textfield1:TGadget
Local textfield2:TGadget
Local textfield3:TGadget
Local button:TGadget

window=CreateWindow("My Window",30,20,320,200)

textfield1=CreateTextField(4,4,120,22,window)
SetGadgetText( textfield1,"A textfield gadget" )
textfield2=CreateTextField(4,35,120,22,window)
textfield3=CreateTextField(4,65,120,22,window)

SetGadgetToolTip(textfield1, "Enter some text...")

' we need an OK button to catch return key

button=CreateButton("OK",130,4,80,24,window,BUTTON_OK)

SetGadgetToolTip(button, "Click me for some action!")

ActivateGadget textfield1

While WaitEvent()
Print CurrentEvent.toString()
	Select EventID()
	Case EVENT_GADGETACTION
		Select EventSource()
			Case textfield1
				Print "textfield updated"
			Case button
				Print "return key / OK button pressed"
				Print "Text = " + TextFieldText(textfield1)
		End Select
	Case EVENT_GADGETLOSTFOCUS
		Select EventSource()
			Case textfield2
				If GadgetText(textfield2) = "" Then
					Print "You need to put a value here!!"
					ActivateGadget(textfield2)
				End If
		End Select
	Case EVENT_WINDOWCLOSE
		End
	End Select
Wend

