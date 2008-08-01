' createwindow.bmx

Strict

Framework bah.gtkmaxgui

Local window:TGadget

window=CreateWindow("My Window",40,40,320,240, Null)
Local combobox:TGadget = CreateComboBox(50, 20, 120, 28, window)

AddGadgetItem combobox,"POINTER_DEFAULT"
AddGadgetItem combobox,"POINTER_ARROW"
AddGadgetItem combobox,"POINTER_IBEAM"
AddGadgetItem combobox,"POINTER_WAIT"
AddGadgetItem combobox,"POINTER_CROSS"
AddGadgetItem combobox,"POINTER_UPARROW"
AddGadgetItem combobox,"POINTER_SIZENWSE"
AddGadgetItem combobox,"POINTER_SIZENESW"
AddGadgetItem combobox,"POINTER_SIZEWE"
AddGadgetItem combobox,"POINTER_SIZENS"
AddGadgetItem combobox,"POINTER_SIZEALL"
AddGadgetItem combobox,"POINTER_NO"
AddGadgetItem combobox,"POINTER_HAND"
AddGadgetItem combobox,"POINTER_APPSTARTING"
AddGadgetItem combobox,"POINTER_HELP"

While True
	WaitEvent 
	Print CurrentEvent.ToString()
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
		Case EVENT_GADGETACTION
			Local pointer:Int = POINTER_DEFAULT
			If SelectedGadgetItem(combobox) >= 0 Then
				pointer = SelectedGadgetItem(combobox)
			End If
			SetPointer(pointer)
	End Select
Wend


