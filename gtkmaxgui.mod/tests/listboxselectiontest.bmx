Framework bah.gtkmaxgui



Local Window   :TGadget = CreateWindow("blub", ClientWidth(Desktop())/2-200,ClientHeight(Desktop())/2-150, 400,300, Null, WINDOW_TITLEBAR|WINDOW_CLIENTCOORDS)

Local Listbox  :TGadget = CreateListBox(5,5, 390,220, Window)

Local Textfield:TGadget = CreateTextField(5,230, 390,30, Window)



AddGadgetItem Listbox, "1"

AddGadgetItem Listbox, "2"

AddGadgetItem Listbox, "3"

AddGadgetItem Listbox, "4"

AddGadgetItem Listbox, "5"

AddGadgetItem Listbox, "6"





While WaitEvent()
	Print currentevent.toString()

	Select EventID()

		Case EVENT_WINDOWCLOSE
			End
		Case EVENT_GADGETSELECT

			If EventSource()=Listbox Then

				SetGadgetText Textfield, GadgetItemText(Listbox, EventData())

			EndIf



	End Select

Wend


