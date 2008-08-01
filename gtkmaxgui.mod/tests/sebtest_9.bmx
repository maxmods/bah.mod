Rem

Example Code for EventExtra() bug

When running on Windows, the code reports the text of the selected tree view node.

However, this method doesn't work for Linux using either Brucey's GTK mod or the 
official FLTK mod.


EndRem

'Framework BRL.Win32MaxGUI
Framework BaH.GTKMaxGUI
'Framework BRL.FLTKMaxGUI

SuperStrict

Local window:TGadget=CreateWindow("My Window",50,50,240,240,Null,WINDOW_TITLEBAR|WINDOW_CLIENTCOORDS)
Local treeview:TGadget=CreateTreeView(10,10,220,220,window)

SetGadgetLayout treeview,2,2,2,2

Local root:TGadget=TreeViewRoot(treeview)

Local help:TGadget=AddTreeViewNode("Help",root)

	AddTreeViewNode "topic 1",help
	AddTreeViewNode "topic 2",help
	AddTreeViewNode "topic 3",help

Local projects:TGadget=AddTreeViewNode("Projects",root)

	AddTreeViewNode "project 1",projects
	AddTreeViewNode("project 2",projects)
	AddTreeViewNode("project 3 is a big waste of time",projects)


'Event Loop

While WaitEvent()

	Print CurrentEvent.ToString()
	
	If EventExtra() Then Print "Gadget Text: " + GadgetText(TGadget(EventExtra()))
	
	Select EventID()
	
		Case EVENT_WINDOWCLOSE
		
			End
			
	End Select
	
Wend
