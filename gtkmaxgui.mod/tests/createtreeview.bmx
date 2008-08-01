' createtreeview.bmx

Strict

Framework bah.gtkmaxgui

Local window:TGadget=CreateWindow("My Window",50,50,240,300)
Local treeview:TGadget=CreateTreeView(0,0,200,200,window)

Local but1:TGadget=CreateButton("Selected", 10, 210, 80, 26, window)
Local but2:TGadget=CreateButton("Change", 120, 210, 80, 26, window)
Local but3:TGadget=CreateButton("Select it!", 10, 240, 80, 26, window)

SetGadgetLayout treeview,2,2,2,2

Local iconstrip:TIconStrip = LoadIconStrip("toolbar.bmp")
SetGadgetIconStrip(treeview, iconstrip)


Local root:TGadget=TreeViewRoot(treeview)

'DebugStop
Local help:TGadget=AddTreeViewNode("Help",root, 1)
AddTreeViewNode "topic 1",help, 0
AddTreeViewNode "topic 2",help, 0
AddTreeViewNode "topic 3",help, 0

Local projects:TGadget=AddTreeViewNode("Projects",root, 1)
AddTreeViewNode "project 1",projects, 3
AddTreeViewNode("project 2",projects)
AddTreeViewNode("project 3 is a big waste of time",projects)
AddTreeViewNode("project 4",projects)
AddTreeViewNode("project 5",projects)
AddTreeViewNode("project 6",projects)
Local rememberMe:TGadget = AddTreeViewNode("project 7",projects)
AddTreeViewNode("project 8",projects)
AddTreeViewNode("project 9",projects)
AddTreeViewNode("project 10",projects)
AddTreeViewNode("project 11",projects)
AddTreeViewNode("project 12",projects)

' this shouldn't generate an event...
ExpandTreeViewNode(projects)

While WaitEvent()
	Print CurrentEvent.ToString()
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
		Case EVENT_GADGETACTION
			If TGTKTreeViewNode(EventExtra()) Then
				Print "clicked node at : " + TGTKTreeViewNode(EventExtra())._path

			Else
				Select EventSource()
					Case but1
						Local view:TGadget = SelectedTreeViewNode(treeview)
						If Not view Then
							Print "No row selected"
						Else
							Print "Row Selected has path = " + TGTKTreeViewNode(view)._path
						End If
					Case but2
						Local view:TGadget = SelectedTreeViewNode(treeview)
						If view Then
							ModifyTreeViewNode(view, "I've been Changed!", 5)
						End If
					Case but3
						SelectTreeViewNode(rememberMe)
				End Select
			End If
		Case EVENT_GADGETSELECT
			If TGTKTreeViewNode(EventExtra()) Then
				Print "selected node at : " + TGTKTreeViewNode(EventExtra())._path
			End If
		Case EVENT_GADGETMENU
			If TGTKTreeViewNode(EventExtra()) Then
				Print "menu node at : " + TGTKTreeViewNode(EventExtra())._path
			End If

	End Select
Wend
