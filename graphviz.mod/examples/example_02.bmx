'
' Keys : 1-5 to switch layout engine
' Keys : A, B to load different graphs and re-layout.
'
SuperStrict

Framework BaH.Graphviz
Import BaH.GraphvizMax2D

Import BRL.StandardIO
Import BRL.TextStream
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


'renderer.loadGraphText(LoadText("../graphs/directed/sdh.dot"))


Local txt:String = "digraph G { ~n" + ..
	 "size=~q4,4~q; ~n" + ..
	 "main [shape=box]; /*this is a comment*/ ~n" + ..
	 "main-> parse [weight=8]; ~n" + ..
	 "parse -> execute; ~n" + ..
	 "main -> init [style=dotted]; ~n" + ..
	 "main -> cleanup; ~n" + ..
	 "execute -> {make_string; printf} ~n" + ..
	 "init -> make_string; ~n" + ..
	 "edge [color=red]; //so is this ~n" + ..
	 "main -> printf[style=bold,label=~q100 times~q]; ~n" + ..
	 "make_string[label=~qmake a string~q]; ~n" + ..
	 "node [shape=box,style=filled,color=~q.7 .3 1.0~q]; ~n" + ..
	 "execute -> compare; ~n" + ..
	"}"
Print txt
renderer.loadGraphText(txt)

Graphics 800, 480, 0

SetClsColor(255, 255, 255)
SetBlend alphablend

renderer.layout("dot")
renderer.fit(800, 480)


Local mx:Int
Local my:Int
Local mz:Int
Local oldZ:Int
Local buttons:Int[] = New Int[3]

Local currentGraph:Int = 1
Local currentLayout:Int = 1

While Not KeyDown(KEY_ESCAPE)

	If KeyDown(key_1) Then
		If currentLayout <> 1 Then
			renderer.layout("dot")
			renderer.fit(800, 480)
			currentLayout = 1
		End If
	End If

	If KeyDown(key_2) Then
		If currentLayout <> 2 Then
			renderer.layout("neato")
			renderer.fit(800, 480)
			currentLayout = 2
		End If
	End If

	If KeyDown(key_3) Then
		If currentLayout <> 3 Then
			renderer.layout("twopi")
			renderer.fit(800, 480)
			currentLayout = 3
		End If
	End If

	If KeyDown(key_4) Then
		If currentLayout <> 4 Then
			renderer.layout("fdp")
			renderer.fit(800, 480)
			currentLayout = 4
		End If
	End If

	If KeyDown(key_5) Then
		If currentLayout <> 5 Then
			renderer.layout("circo")
			renderer.fit(800, 480)
			currentLayout = 5
		End If
	End If

	If KeyDown(key_A) Then
		If currentGraph <> 1 Then
			renderer.loadGraphText(LoadText("../graphs/directed/sdh.dot"))
			renderer.layout("dot")
			renderer.fit(800, 480)
			currentLayout = 1
			currentGraph = 1
		End If
	End If

	If KeyDown(key_B) Then
		If currentGraph <> 2 Then
			renderer.loadGraphText(LoadText("example_02.dot"))
			renderer.layout("dot")
			renderer.fit(800, 480)
			currentLayout = 1
			currentGraph = 2
		End If
	End If
	
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
	
	If KeyHit(key_down) Then
		renderer.mouseScroll(mx, my, -1) ' scroll up - zoom in
	End If
	
	If KeyHit(key_up) Then
		renderer.mouseScroll(mx, my, 1) ' scroll down - zoom out
	End If

	' refresh/redraw the graph
	renderer.refresh()

	' draw the current tooltip under the mouse - if there is one!
	renderer.drawTooltip(mx, my + 16)
	
	' get data for current object
	Local obj:TGViewObject = renderer.currentObject()
	
	If obj Then
		SetImageFont(Null)
		
		SetColor 255, 100, 100
		
		DrawText obj.name, 400, 0
		DrawText obj.skind, 400, 15
		
		For Local i:Int = 0 Until obj.attributes.length			
			DrawText obj.attributes[i].name + " : " + obj.attributes[i].value, 400, 60 + i * 15
		Next
	End If
	

	Flip
Wend

End


