Strict 

Framework BaH.gtkmaxgui

Local window:TGadget
Local tabber:TGadget
Local document:TGadget[3]
Local currentdocument:TGadget

' CreateDocument creates a hidden panel that fills entire tabber client area 

Function CreateDocument:TGadget(tabber:TGadget)
	Local	panel:TGadget
	panel=CreatePanel(0,0,ClientWidth(tabber),ClientHeight(tabber),tabber)
	SetGadgetLayout panel,1,1,1,1
	HideGadget panel
	Return panel
End Function

' create a default window with a tabber gadget that fills entire client area

window=CreateWindow("My Window",30,20,400,300)

tabber=CreateTabber(0,0,ClientWidth(window),ClientHeight(window) - 50,window)
SetGadgetLayout tabber,1,1,1,1 

Local iconstrip:TIconStrip = LoadIconStrip("toolbar.bmp")
SetGadgetIconStrip(tabber, iconstrip)

Local rembut:TGadget = CreateButton("Remove", 20, 245, 100, 28, window)
SetGadgetLayout rembut,1,0,0,1 

' add three items and corresponding document panels to the tabber

AddGadgetItem tabber,"Document 0",False,1,""
AddGadgetItem tabber,"Document 1",False,-1,"Tabber Tip 1"
AddGadgetItem tabber,"Document 2",False,3,"tips 4 2"

document[0]=CreateDocument(tabber)
document[1]=CreateDocument(tabber)
document[2]=CreateDocument(tabber)

Local but:TGadget = CreateButton("Toggle", 50, 50, 80, 26, document[1])
SetGadgetLayout but,1,0,1,0

SetPanelColor document[0],255,200,200
SetPanelColor document[1],200,255,200
SetPanelColor document[2],200,200,255

' our documents start off hidden so make first one current and show

currentdocument=document[0]
ShowGadget currentdocument
Local switch:Int = 0
' standard message loop with special tabber GADGET_ACTION handling

While WaitEvent()
	'Print currentevent.toString()
	Select EventID()
		Case EVENT_GADGETACTION
			Select EventSource()
				Case tabber
					HideGadget currentdocument
					currentdocument=document[EventData()]
					ShowGadget currentdocument
				Case but
					If Not switch Then
						ModifyGadgetItem(tabber, 1, "Document 1",0, 5,"New Tips!")
					Else
						ModifyGadgetItem(tabber, 1, "Document 1",0, -1,"New Tips!~nAgain!!")
					End If
					switch = Not switch
				Case rembut
					RemoveGadgetItem(tabber, SelectedGadgetItem(tabber))
			End Select
		Case EVENT_WINDOWCLOSE
			End
	End Select
Wend
