' createsplitter.bmx

Strict

Framework bah.gtkmaxgui

Local window:TGadget

window=CreateWindow("My Window",40,40,320,240, Null)
'DebugStop
Local splitter:TGadget = CreateSplitter(10, 10, 200, 200, window)

'Local area1:TGadget = CreateTextArea(0, 0, ClientWidth(TGTKSplitter(splitter).panel1), ..
'		ClientHeight(TGTKSplitter(splitter).panel1), TGTKSplitter(splitter).panel1)
'SetGadgetLayout area1,1,1,1,1

While True
	WaitEvent 
	Print CurrentEvent.ToString()
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
	End Select
Wend

