'
' In-memory graph creation.
'
'
SuperStrict

Framework BaH.Graphviz
Import BRL.StandardIO
Import BRL.glmax2d

?linux
Import "glsupport.bmx"
?macos
Import "glsupport.bmx"
?

Local renderSupport:TGVRenderSupport

?linux
renderSupport = New TOpenGLRenderSupport
?macos
renderSupport = New TOpenGLRenderSupport
?

Local renderer:TGVGraphviz = TGVGraphviz.Create(800, 480, renderSupport) ' with optional render support

' we need to create an in-memory graph...
' something simple:
'
'    node2  -->   node1
'           /
'    node3 /

Local graph:TGVGraph = TGVGraph.Create()

' create some nodes
Local node1:TGVNode = graph.addNode("node1")
Local node2:TGVNode = graph.addNode("node2")
Local node3:TGVNode = graph.addNode("node3")

node2.setAttr(ATTR_FONTCOLOR, "red")
node2.setAttr(ATTR_HTML_LABEL, "<TABLE BORDER=~q0~q CELLBORDER=~q1~q CELLSPACING=~q0~q>" + ..
	"<TR><TD>Left</TD><TD PORT=~qf1~q>Mid dle</TD><TD PORT=~qf2~q>Right</TD></TR></TABLE>")
node3.setAttr(ATTR_NODE_SHAPE, "diamond")

' join the nodes together
graph.addEdge(node2, node1)
graph.addEdge(node3, node1)

' now to construct the graph..
renderer.buildGraph(graph)

' and display it..
showGraph(renderer)

End

Function showGraph(renderer:TGVGraphviz)
	Graphics 800, 480, 0
	
	SetBlend alphablend
	
	
	renderer.layout("dot")
	renderer.fit(800, 480)
	
	
	Local mx:Int
	Local my:Int
	Local mz:Int
	Local oldZ:Int
	Local buttons:Int[] = New Int[3]	
	
	While Not KeyDown(KEY_ESCAPE)

		Cls
		
		SetColor(255, 255, 255)
		
		mx = MouseX()
		my = MouseY()
		mz = MouseZ()
		
		' tell graphviz we've moved the mouse
		renderer.mouseMove(mx, my)
		
		For Local i:Int = 1 To 3
			If MouseDown(i)
				If Not buttons[i - 1] Then
					renderer.MouseDown(mx, my, i) ' mouse button press
					buttons[i - 1] = 1
				End If
			Else
				If buttons[i - 1] Then
					buttons[i - 1] = 0
					renderer.mouseUp(mx, my, i) ' mouse button release
				End If
			End If
		Next
		
		If mz <> oldZ Then
			If mz < oldZ Then
				renderer.mouseScroll(mx, my, -1) ' scroll up - zoom in
			Else
				renderer.mouseScroll(mx, my, 1) ' scroll down - zoom out
			End If
			oldZ = mz
		End If
		
		' refresh/redraw the graph
		renderer.refresh()
	
		' draw the current tooltip under the mouse - if there is one!
		renderer.drawTooltip(mx, my + 8)
		
		' get data for selected "node" (for anything else, returns Null)
		Local obj:TGViewObject = renderer.selectedObject(GV_VIEW_NODE)
		
		If obj Then
			SetImageFont(Null)
			SetColor 100, 100, 255
			
			DrawText obj.name, 400, 0
			DrawText obj.skind, 400, 15
			
			For Local i:Int = 0 Until obj.attributes.length
				DrawText obj.attributes[i].name + " : " + obj.attributes[i].value, 400, 60 + i * 15
			Next
		End If
	
		Flip
	Wend
End Function
