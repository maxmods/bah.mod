Framework bah.gtkmaxgui

SuperStrict

Rem
Run the following in debug mode and when the parent/child window appear, keep resizing the child window to
different positions and at some point, the child window will drop, and a  EVENT_WINDOWMOVE will be fired in
the debuglog.
EndRem

Global wndParent:TGadget = CreateWindow("Parent Window",50,50,700,500,Null,15|WINDOW_CLIENTCOORDS)	'Create a parent window

	Global gadPanel:TGadget = CreatePanel(0,0,700,500,wndParent) 'Just so that you can see the parent's client area
	SetPanelColor(gadPanel,255,0,0)
	SetGadgetLayout(gadPanel,1,1,1,1)
	
	Global wndChild:TGadget = CreateWindow("Child Window",50,50,300,200,wndParent,15|WINDOW_CHILD)	'Create a child window

MaximizeWindow(wndParent)

Repeat

	Select WaitEvent()
	
		Case EVENT_WINDOWCLOSE;If EventSource() = wndParent Then End
		
	EndSelect

	DebugLog CurrentEvent.ToString$()

Forever
