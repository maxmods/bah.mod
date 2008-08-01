' createcombobox.bmx

Strict

Framework bah.gtkmaxgui
Import brl.random
Import brl.eventqueue

Local window:TGadget
Local combobox:TGadget

window=CreateWindow("My Window",30,20,200,200)

combobox=CreateComboBox(20,20,120,30,window)', COMBOBOX_EDITABLE)
Local button1:TGadget = CreateButton("Change", 10, 100, 90, 28, window)
Local button2:TGadget = CreateButton("Show", 110, 100, 90, 28, window)
Local button3:TGadget = CreateButton("Remove", 10, 140, 90, 28, window)
Local button4:TGadget = CreateButton("Add", 110, 140, 90, 28, window)

Local iconstrip:TIconStrip = LoadIconStrip("toolbar.bmp")
SetGadgetIconStrip(combobox, iconstrip)

AddGadgetItem combobox,"Short",,1
AddGadgetItem combobox,"Medium"
AddGadgetItem combobox,"Fat",True,3
AddGadgetItem combobox,"Humungous",,4

While WaitEvent()
	Select EventID()
		Case EVENT_GADGETACTION
			Select EventSource()
				Case combobox
					Print "eventdata="+EventData()
					Print GadgetItemText(combobox, EventData())
				Case button1
					ModifyGadgetItem(combobox, SelectedGadgetItem(combobox), "Changed!",0, 8)
				Case button2
					Print GadgetItemText(combobox, SelectedGadgetItem(combobox))
				Case button3
					If CountGadgetItems(combobox) > 1 Then
						RemoveGadgetItem(combobox, CountGadgetItems(combobox) - 1)
					End If
				Case button4
					AddGadgetItem(combobox, "New Item " + CountGadgetItems(combobox),,Rand(1,10))
			End Select
		Case EVENT_WINDOWCLOSE
			End
		Case EVENT_GADGETMENU
			Select EventSource()
				Case combobox
					
			End Select
	End Select
Wend
