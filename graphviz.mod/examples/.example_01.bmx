'
' Shows rendering to a custom origin / viewport size.
' 
' Notes: mouse actions outside of the viewport are still passed to the renderer...
'
SuperStrict

Framework BaH.Graphviz
Import BaH.GraphvizMax2D

Import BRL.StandardIO
Import BRL.TextStream

Import BRL.max2d
Import BRL.glmax2d

Local renderer:TGVGraphviz = TGVGraphviz.Create(200, 200)


'renderer.loadGraphText(LoadText("../graphs/directed/pm2way.dot"))
'renderer.loadGraphText(LoadText("../graphs/directed/sdh.dot"))
renderer.loadGraphText(LoadText("../graphs/directed/fig6.dot"))
'renderer.loadGraphText(LoadText("example_01.dot"))

'renderer.loadGraphText("digraph R { rankdir=LR node [style=rounded] node1 [shape=box] node2 [fillcolor=yellow, style=~qrounded,filled~q, shape=diamond] node3 [shape=record, label=~q{ a | b | c }~q] node1 -> node2 -> node3 }")

' show the graph content.... just for fun.
Print renderer.graph

Graphics 800, 480, 0

glEnable GL_LINE_SMOOTH
SetBlend alphablend

SetOrigin 100, 100

renderer.layout("dot")
renderer.fit(200, 200)


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
	
	renderer.mouseMove(mx, my)
	
	For Local i:Int = 1 To 3
		If MouseDown(i)
			If Not buttons[i - 1] Then
				renderer.MouseDown(mx, my, i)
				buttons[i - 1] = 1
			End If
		Else
			If buttons[i - 1] Then
				buttons[i - 1] = 0
				renderer.mouseUp(mx, my, i)
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

	DrawLine 0, 0, 200, 0
	DrawLine 200, 0, 200, 200
	DrawLine 200, 200, 0, 200
	DrawLine 0, 200, 0, 0
	
	Flip
Wend

End


