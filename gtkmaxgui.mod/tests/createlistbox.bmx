' createlistbox.bmx
Strict 

Framework bah.gtkmaxgui

Local window:TGadget
Local listbox:TGadget

window=CreateWindow("My Window",30,20,200,200)

Const ETIP$="Great for lovers of rain, mushy peas and stomping beats.~r~nNew line..."

listbox=CreateListBox(4,4,160,160,window)

Local iconstrip:TIconStrip = LoadIconStrip("toolbar.bmp")
SetGadgetIconStrip(listbox, iconstrip)

'DebugStop
AddGadgetItem listbox,"German"
AddGadgetItem listbox,"English",False,0,ETIP
AddGadgetItem listbox,"French",False,4,"tip - goes here","mystringobject"
AddGadgetItem listbox,"Russian",False,-1
AddGadgetItem listbox,"Canadian",False,6
AddGadgetItem listbox,"Canadian",False,6
AddGadgetItem listbox,"Canadian",False,6
AddGadgetItem listbox,"Canadian",False,6
AddGadgetItem listbox,"Canadian",False,6
AddGadgetItem listbox,"Canadian",False,6
AddGadgetItem listbox,"Canadian",False,6
AddGadgetItem listbox,"Canadian",False,6
AddGadgetItem listbox,"Canadian",False,6

SelectGadgetItem listbox,1
SelectGadgetItem listbox,2
RemoveGadgetItem(listbox, 0)

ModifyGadgetItem(listbox, 0, "Scottish",0,6)
ModifyGadgetItem(listbox, 3, "Canadian - changed",0,-1)


'ClearGadgetItems(listbox)

While WaitEvent()
	Print CurrentEvent.ToString()
	If EventSource()=listbox
		Print "SelectedGadgetItem()="+SelectedGadgetItem(listbox)
		Print "EventData() = " + EventData()
		If EventData()<>SelectedGadgetItem(listbox) Then
			Print "error with skidracer"
		End If
	EndIf
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
	End Select
Wend
