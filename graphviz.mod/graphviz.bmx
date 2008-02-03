SuperStrict

Rem
bbdoc: Graphviz
End Rem
Module BaH.Graphviz

ModuleInfo "Version: 1.00"
ModuleInfo "License: Common Public License"
ModuleInfo "Copyright: AT&T Research"
ModuleInfo "Copyright: Wrapper - 2007,2008 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Added support for different Graphviz Drivers (renderers) - eg. Max2D, Cairo, etc."
ModuleInfo "History: Added ATTR_HTML_LABEL for GraphBuilder for constructing complex nodes."
ModuleInfo "History: 0.96"
ModuleInfo "History: Added support for CURRENT object attribute retrieval."
ModuleInfo "History: Fixed problems using fallback (max default) image font."
ModuleInfo "History: Fixed issue with drawing duplicates."
ModuleInfo "History: Removed jVWidth/jvHeight attributes."
ModuleInfo "History: Added dot/dash line style rendering via TGVRenderSupport."
ModuleInfo "History: Fixed bezier to draw only 10 sections. (Optimization)"
ModuleInfo "History: 0.95 Initial Release"
ModuleInfo "History: Includes Graphviz 2.12 source."


ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H"
ModuleInfo "CC_OPTS: -fexceptions"

?win32
ModuleInfo "CC_OPTS: -DMSWIN32"
?

Import BRL.Map
Import BRL.LinkedList

Import "common.bmx"


' Source changes :
'
' gvevent.c
'  * gvevent_find_current_obj() : changed for correct calculation of p.x and p.y.
'  * gvevent_select_current_obj() : changed to not set as visited.
'
' globals.h
'  * added define EXTERN for __APPLE__ : problem with externing variables.
'
' gvc.c
' * gvRender() : commented out gvjobs_delete() call, as I need it to keep the references.
'
' dot_builtins.c
' * added gvplugin_bmx_LTX_library as a builtin plugin.
'
' labels.c
' * added "memset(para, 0, sizeof(textpara_t));" to storeline(), to stop crash later on cleanup graph.
'

'Global defaultGVDriver:TGVDriver = New TGVDefaultDriver

Global graphvizDriver:TGVDriver = Null

Function setGVDriver(driver:TGVDriver, context:TGVGraphviz)
	GraphvizDriver = driver
	driver.setContext(context)
End Function

Rem
bbdoc: A Graphviz renderer.
End Rem
Type TGVGraphviz

	Global fp:Byte Ptr = "/".toCString()

	Global fontList:TMap = New TMap
	Global renderList:TList = New TList
	
	Global userFontDirs:String

	' the size of the current "display"
	' used for zooming and suchlike.
	Field jWidth:Int
	Field jHeight:Int

	' current magnification value
	Field jZoom:Double
	' current graph offsets
	Field jXOffset:Double
	Field jYOffset:Double
	
	' context pointer
	Field gvContextPtr:Byte Ptr
	' graph pointer
	Field gvGraphPtr:Byte Ptr
	' job pointer
	Field jobPtr:Byte Ptr
	
	' graph text - of a loaded .dot
	Field graph:String
	
	Field renderSupport:TGVRenderSupport
	
	Rem
	bbdoc: The graph background color
	about: Defaults to white (255, 255, 255)
	End Rem
	Field bgColor:TGVColor = TGVColor.set(255, 255, 255)
	
	' whether or not we've been rendered yet
	Field rendered:Int = False
	
	' current draw style - pen, fill, pen width
	Field drawStyle:TGVDrawStyle = New TGVDrawStyle
	
	' current object information
	Field currentObj:Byte Ptr
	Field currentObjTypeList:Byte Ptr
	Field currentObjAttrList:Byte Ptr

	Field previousStyle:Int
	
	Field renderingContext:Object
	
	Rem
	bbdoc: Creates a new #TGVGraphviz of the specified dimensions.
	about:@width and @height are required for correct drawing of the graph at various magnifications.
	End Rem
	Function Create:TGVGraphviz(width:Int, height:Int, rendererSupport:TGVRenderSupport = Null)
		Local this:TGVGraphviz = New TGVGraphviz
		
		this.jWidth = width
		this.jHeight = height
		
		If rendererSupport Then
			this.renderSupport = rendererSupport
		End If
		
		Assert graphvizDriver, "You need to Import a Graphviz Renderer before you can use Graphviz"
		
		setGVDriver(graphvizDriver, this)
		
		this.init()
		
		Return this
	End Function
	
	Method Init()
		' get a new context
		gvContextPtr = bmx_gvc_new()
		
		currentObjTypeList = gvNEWargvlist()
		currentObjAttrList = gvNEWargvlist()
		
		' add self to the list of renderers
		renderList.addLast(Self)
	End Method
	
	Rem
	bbdoc: Loads the graph contained within @text.
	End Rem
	Method loadGraphText(text:String)

		If gvGraphPtr Then
			cleanupGraph()
		End If

		graph = text
		' load the graph into graphviz
		gvGraphPtr = bmx_agmemread(graph)
	End Method
	
	Rem
	bbdoc: Builds a graph from the supplied #TGVGraph structure.
	End Rem
	Method buildGraph(gvGraph:TGVGraph)

		If gvGraphPtr Then
			cleanupGraph()
		End If
				
		Local builder:TGVGraphBuilder = TGVGraphBuilder.Create(gvGraph)
		
		gvGraphPtr = builder.graphPtr
	
	End Method
	
	' frees layout, closes graph - generally, cleans up.
	Method cleanupGraph()
	
		bmx_gvFreeLayout(gvContextPtr, gvGraphPtr)

		agclose(gvGraphPtr)
		
		gvGraphPtr = Null
		
		jobPtr = Null
		rendered = False
	End Method
	
	Rem
	bbdoc: Processes the graph using the specified layout engine.
	about: Valid values for @layoutType are :
	<ul>
	<li> <a href="#desc_dot">dot</a> </li>
	<li> <a href="#desc_neatofdp">neato</a> </li>
	<li> <a href="#desc_twopi">twopi</a> </li>
	<li> <a href="#desc_circo">circo</a> </li>
	<li> <a href="#desc_neatofdp">fdp</a> </li>
	</ul>
	End Rem
	Method layout(layoutType:String, context:Object = Null, singlePass:Int = False)
	
		If context Then
			renderingContext = context
		End If

		If rendered Then
			'gvGraphPtr = bmx_gvevent_layout(jobPtr, layoutType, graph)
			bmx_gvFreeLayout(gvContextPtr, gvGraphPtr)
			jobPtr = Null
			gvLayout(gvContextPtr, gvGraphPtr, layoutType)		
			gvRender(gvContextPtr, gvGraphPtr, "bmx", Null)
		Else
			If Not singlePass Then
				rendered = True
			End If
			gvLayout(gvContextPtr, gvGraphPtr, layoutType)		
			gvRender(gvContextPtr, gvGraphPtr, "bmx", Null)
			rendered = True
		End If
	End Method

	' sets the current pen color / alpha
	Method setPenColor()

		Local r:Double, g:Double, b:Double, a:Double

		bmx_setpencolor(jobPtr, Varptr r, Varptr g, Varptr b, Varptr a)

		If rendered Then
			graphvizDriver.setPenColor(r, g, b, a)
		End If
		
	End Method

	' sets the current fill color / alpha
	Method setFillColor()

		Local r:Double, g:Double, b:Double, a:Double
		
		bmx_setfillcolor(jobPtr, Varptr r, Varptr g, Varptr b, Varptr a)

		If rendered Then
			graphvizDriver.setFillColor(r, g, b, a)
		End If
		
	End Method
	
	' sets the draw style	
	Method setDrawStyle()

		bmx_setstyle(jobPtr, Varptr drawStyle.pen, Varptr drawStyle.fill, Varptr drawStyle.penWidth)
		
	End Method
	
	' sets the pen style
	Method setPenStyle(style:Int)
	
		If renderSupport Then
		
			If Not previousStyle Then
				previousStyle = GV_PEN_SOLID
			End If
			
			If style <> previousStyle Then
				previousStyle = style
				Select style
					Case GV_PEN_DASHED
						renderSupport.setDashed()
					Case GV_PEN_DOTTED
						renderSupport.setDotted()
					Case GV_PEN_SOLID
						renderSupport.setSolid()
				End Select
			End If
		End If
		
	End Method

	Function beginJob(job:Byte Ptr)
		' nothing to see here...
	End Function
	
	Function endJob(job:Byte Ptr)
		' nothing to see here...
	End Function
	
	' callback - renders text at the specified location
	Function textPara(handle:TGVGraphviz, x:Double, y:Double, para:Byte Ptr)

		Local s:String
		Local name:String
		
		Local sPtr:Byte Ptr = bmx_para_str(para)
		If sPtr Then
			s = gvUTF8toISO8859(sPtr)
		Else
			s = ""
		End If
		
		sPtr = bmx_para_fontname(para)
		If sPtr Then
			name = String.fromCString(sPtr)
		Else
			name = "Arial"
		End If

		Local size:Double, width:Double, height:Double
		bmx_para_fontmetrics(para, Varptr size, Varptr width, Varptr height)
		
		Local font:TGVFont = TGVFont(fontList.valueForKey(TGVFont.key(name, size, 0)))

		Local just:Byte = bmx_para_just(para)

		If graphvizDriver.graphviz.rendered Then
			graphvizDriver.textPara(s, size, x, y, width, height, name, Int(just))
		End If

	End Function
	
	' callback - draws an ellipse
	Function ellipse(handle:TGVGraphviz, points:Double Ptr, filled:Int)
		
		If graphvizDriver.graphviz.rendered Then
			graphvizDriver.ellipse(points[0], points[1], points[2], points[3], filled)
		End If
			
	End Function
	
	' callback - draws a polygon
	Function polygon(handle:TGVGraphviz, points:Double Ptr, n:Int, filled:Int)
		
		Local xy:Float[] = New Float[n * 2]

		For Local i:Int = 0 Until n
			xy[i * 2] = handle.jZoom * (points[i * 2] - handle.jXOffset)
			xy[i * 2 + 1] = handle.jZoom * (handle.jYOffset - points[i * 2 + 1])
		Next
		
		handle.setDrawStyle()
		
		If graphvizDriver.graphviz.rendered Then
			graphvizDriver.polygon(xy, filled)
		End If
		
	End Function
	
	' callback - draws a bezier
	Function bezier(handle:TGVGraphviz, points:Double Ptr, n:Int, arrowAtStart:Int, arrowAtEnd:Int, filled:Int)

		handle.setDrawStyle()
		
		If handle.drawStyle.pen <> GV_PEN_NONE Then
		
			handle.setPenStyle(handle.drawStyle.pen)
			
			handle.setPenColor()
			
			If graphvizDriver.graphviz.rendered Then
				graphvizDriver.bezier(points, n, arrowAtStart, arrowAtEnd, filled)
			End If
		
			handle.setPenStyle(GV_PEN_SOLID)
		End If

	End Function
	
	' callback - draws a polyline
	Function polyline(handle:TGVGraphviz, points:Double Ptr, n:Int)
		
		Local xy:Float[] = New Float[n * 2]

		For Local i:Int = 0 Until n
			xy[i * 2] = handle.jZoom * (points[i * 2] - handle.jXOffset)
			xy[i * 2 + 1] = handle.jZoom * (handle.jYOffset - (points[i * 2 + 1]))
		Next

		handle.setDrawStyle()

		If handle.drawStyle.pen <> GV_PEN_NONE Then
		
			handle.setPenStyle(handle.drawStyle.pen)
			
			handle.setPenColor()
			
			graphvizDriver.PolyLine(xy)
			
			handle.setPenStyle(GV_PEN_SOLID)
		End If
		
	End Function

	' callback - calculates the size of text for a particular font
	Function textLayout:Byte Ptr(common:Byte Ptr, para:Byte Ptr)

		' the string to render
		Local s:String
		Local name:String
		
		Local sPtr:Byte Ptr = bmx_para_str(para)
		If sPtr Then
			s = gvUTF8toISO8859(sPtr)
		Else
			s = ""
		End If
		
		' font name
		sPtr = bmx_para_fontname(para)
		If sPtr Then
			name = String.fromCString(sPtr)
		Else
			name = "Arial"
		End If
		
		
		Local size:Double, width:Double, height:Double
		bmx_para_fontmetrics(para, Varptr size, Varptr width, Varptr height)

		' get the cached font
		Local font:TGVFont = TGVFont(fontList.valueForKey(TGVFont.key(name, size, 0)))
		
		' cache a new font
		If Not font Then
			font = graphvizDriver.loadFont(name, size, 0)
			fontList.insert(TGVFont.key(name, size, 0), font)
		End If

		' get the font metrics
		If graphvizDriver.graphviz.rendered Then
			graphvizDriver.getFontMetrics(font, s, width, height)
		End If

		bmx_para_setmetrics(para, size, width, height)
		
		Return font.fpc
		
	End Function

	Function textSize(txt:String, name:String, size:Int, width:Int Var, height:Int Var, scale:Float Var)

		' get the cached font
		Local font:TGVFont = TGVFont(fontList.valueForKey(TGVFont.key(name, size, 0)))
		
		' cache a new font
		If Not font Then
			font = graphvizDriver.loadFont(name, size, 0)
			fontList.insert(TGVFont.key(name, size, 0), font)
		End If

		' get the font metrics
		If graphvizDriver.graphviz.rendered Then
			graphvizDriver.textSize(txt, font, width, height, scale)
		End If

	End Function
	
	' callback - initialises
	Function deviceInitialize(job:Byte Ptr)

		bmx_gv_setdpi(job, 72, 72)

		Local obj:Object = bmx_job_getWindow(job)
		
		If Not obj Then

			Local context:Byte Ptr = bmx_job_getGVC(job)
		
			' find matching instance of gvcontext
			For Local r:TGVGraphviz = EachIn renderList
			
				' found the matching context ?
				If context = r.gvContextPtr Then
					bmx_job_setWindow(job, r)					
					r.jobPtr = job
					
					
					bmx_graph_bgcolor(job, Varptr r.bgColor.r, Varptr r.bgColor.g, Varptr r.bgColor.b, Varptr r.bgColor.a)
					
					Exit
				End If
				
			Next

		End If
		
	End Function
	
	Function deviceFinalize(job:Byte Ptr)
	
		Local obj:Object = bmx_job_getWindow(job)
		
		If obj Then
		
			Local context:Byte Ptr = bmx_job_getGVC(job)
		
			' find matching instance of gvcontext
			For Local r:TGVGraphviz = EachIn renderList
			
				' found the matching context ?
				If context = r.gvContextPtr Then

					If r.renderSupport Then
						r.renderSupport.init()
					End If
			
					r.refresh()
					Exit
				End If
				
			Next
			
		End If

	End Function
	
	Rem
	bbdoc: Refresh the graph, using the optional new display width and height.
	End Rem
	Method refresh(context:Object = Null, width:Int = 0, height:Int = 0, drawBackground:Int = True)

		If context Then
			renderingContext = context
		End If

		Local originX:Float, originY:Float, r:Int, g:Int, b:Int, a:Float', w:Double, h:Double
		Local vpX:Int, vpY:Int, vpW:Int, vpH:Int
		
		If graphvizDriver.graphviz.rendered Then
			graphvizDriver.currentSettings(originX, originY, r, g, b, a, vpX, vpY, vpW, vpH)
		End If

		bmx_gvj_zoom(jobPtr, Varptr jZoom)
		bmx_gvj_focus(jobPtr, Varptr jXOffset, Varptr jYOffset)
		
		If width And height Then
			jWidth = width
			jHeight = height
		End If
		
		If graphvizDriver.graphviz.rendered Then
			graphvizDriver.setViewPortAndOrigin(originX, originY, jWidth, jHeight, jWidth / 2 + originX, jHeight / 2 + originY)
		End If

		bmx_gvj_setSize(jobPtr, jWidth, jHeight)

		bmx_job_refresh(jobPtr)
		
		currentObj = bmx_job_checkCurrent(jobPtr, currentObj, currentObjTypeList, currentObjAttrList)


		If graphvizDriver.graphviz.rendered Then
			graphvizDriver.setSettings(originX, originY, r, g, b, a, vpX, vpY, vpW, vpH)
		End If

	End Method
	
	Rem
	bbdoc: Inform the engine that the mouse was moved.
	about: This is used for detecting the location of the mouse relative to the graph.
	End Rem
	Method mouseMove(x:Int, y:Int)
		Local originX:Float, originY:Float
		graphvizDriver.GetOriginXY(originX, originY)
		' Graphviz translates the x,y coordinates for us.
		bmx_job_setPointer(jobPtr, x - originX, y - originY)
	End Method
	
	Rem
	bbdoc: Inform the engine of a mouse button down.
	about:
	<ul>
	<li>button 1 = left</li>
	<li>button 2 = right</li>
	</ul>
	End Rem
	Method MouseDown(x:Int, y:Int, button:Int)
		Local originX:Float, originY:Float
		graphvizDriver.GetOriginXY(originX, originY)
		' Graphviz translates the x,y coordinates for us.
		bmx_job_mouseDown(jobPtr, x - originX, y - originY, button)
	End Method

	Rem
	bbdoc: Inform the engine of a mouse button release.
	about:
	<ul>
	<li>button 1 = left</li>
	<li>button 2 = right</li>
	</ul>
	End Rem
	Method mouseUp(x:Int, y:Int, button:Int)
		Local originX:Float, originY:Float
		graphvizDriver.GetOriginXY(originX, originY)
		' Graphviz translates the x,y coordinates for us.
		bmx_job_mouseUp(jobPtr, x - originX, y - originY, button)
	End Method
	
	Rem
	bbdoc: Provide mouse scroll (zoom) information to the engine.
	about: A negative @direction implies zooming in. A positive @direction, zooming out.
	End Rem
	Method mouseScroll(x:Int, y:Int, direction:Int)
		Local originX:Float, originY:Float
		graphvizDriver.GetOriginXY(originX, originY)
		' Graphviz translates the x,y coordinates for us.
		bmx_job_mouseScroll(jobPtr, x - originX, y - originY, direction)
	End Method
	
	
	Rem
	bbdoc: Returns the tooltip text of the currently selected graph object.
	End Rem
	Method toolTip:String()
		Local s:Byte Ptr = bmx_job_active_tooltip(jobPtr)
		
		If s Then
			Return String.fromCString(s)
		End If
		
		Return Null
	End Method
	
	Rem
	bbdoc: Draws the currently selected graph object at the specified location.
	End Rem
	Method drawTooltip(x:Int, y:Int, style:TGVStyle = Null)
		Local s:Byte Ptr = bmx_job_active_tooltip(jobPtr)
		
		If s Then
			Local width:Int, height:Int, scale:Float
			
			If Not style Then
				style = defaultTooltipStyle
			End If
			
			Local txt:String = String.fromCString(s)

			If txt Then

				textSize(txt, style.font, style.fsize, width, height, scale)
			
				' offset to tooltip center
				x:- width / 2 - 4
				y:- 2
				width:+ 8 ' padding
				height:+ 4 ' padding

				Local xy:Float[] = New Float[8]
	
				xy[0] = x          ' top left
				xy[1] = y
				xy[2] = x + width  ' top right
				xy[3] = y
				xy[4] = x + width  ' bottom right
				xy[5] = y + height
				xy[6] = x          ' bottom left
				xy[7] = y + height
			
			
				If graphvizDriver.graphviz.rendered Then
					graphvizDriver.drawTooltip(txt, xy, scale, style)
				End If
'			' fill
'			If style.fcColor Then
'				style.fcColor.doColor()
'				DrawPoly(xy)
'			End If
'			
'			' pen
'			If style.pcColor Then
'				style.pcColor.doColor()
'				drawUnfilledPoly(xy)
'			End If
'			
'			' text
'			style.foColor.doColor()
'			
'			SetScale scale , scale
'			
'			DrawText txt, x + 4, y + 2
'			
'			SetScale 1,1
'			
'			SetColor 255, 255, 255
'			SetAlpha 1

			End If
		End If
		
	End Method
	
	Rem
	bbdoc: Returns the selected graph object, or Null.
	about: @selection determines what graph object kinds to return. The default is all kinds, but can
	be one or more of #GV_VIEW_GRAPH, #GV_VIEW_SUBGRAPH, #GV_VIEW_NODE or #GV_VIEW_EDGE, or #GV_VIEW_ALL.<br>
	Valid graph objects are #TGViewGraph, #TGViewSubGraph, #TGViewNode and #TGViewEdge.
	End Rem
	Method selectedObject:TGViewObject(selection:Int = GV_VIEW_ALL)
	
		If bmx_job_hasSelection(jobPtr) Then
		
			Local nPtr:Byte Ptr
			Local vPtr:Byte Ptr
			Local kind:Int
			Local size:Int
			Local sk:String
	
			Local obj:TGViewObject
			
			' get count for object details - generally we are only interested in the first two
			' kind and name
			Local aCount:Int = bmx_job_attrCount(jobPtr, 1, 2)
					
			nPtr = bmx_job_attrsValue(jobPtr, 1, 0)
			vPtr = bmx_job_attrsValue(jobPtr, 1, 1)
			
			If nPtr Then
				sk = String.fromCString(nPtr)
			End If
			
			Select sk
				Case "digraph"
					If selection & GV_VIEW_GRAPH Then
						obj = New TGViewGraph
						obj.kind = GV_OBJ_DIGRAPH
						size = 3
					Else
						Return Null ' not interested in this kind
					End If
				Case "graph"
					If selection & GV_VIEW_GRAPH Then
						obj = New TGViewGraph
						obj.kind = GV_OBJ_GRAPH
						size = 3
					Else
						Return Null ' not interested in this kind
					End If
				Case "subgraph"
					If selection & GV_VIEW_SUBGRAPH Then
						obj = New TGViewSubGraph
						obj.kind = GV_OBJ_SUBGRAPH
						size = 3
					Else
						Return Null ' not interested in this kind
					End If
				Case "node"
					If selection & GV_VIEW_NODE Then
						obj = New TGViewNode
						obj.kind = GV_OBJ_NODE
						size = 2
					Else
						Return Null ' not interested in this kind
					End If
				Case "edge"
					If selection & GV_VIEW_EDGE Then
						obj = New TGViewEdge
						obj.kind = GV_OBJ_EDGE
						size = 2
					Else
						Return Null ' not interested in this kind
					End If
			End Select
			
			If vPtr Then
				obj.name = String.fromCString(vPtr)
			Else
				obj.name = ""
			End If

			' get count for object attribtues
			aCount = bmx_job_attrCount(jobPtr, 2, size)
			
			obj.attributes = New TGVAttribute[aCount]
			For Local i:Int = 0 Until aCount
				Local att:TGVAttribute = New TGVAttribute
				
				nPtr = bmx_job_attrsValue(jobPtr, 2, i * size)
				vPtr = bmx_job_attrsValue(jobPtr, 2, i * size + 1)
				
				If nPtr Then
					att.name = String.fromCString(nPtr)
				Else
					att.name = ""
				End If
				If vPtr Then
					att.value = String.fromCString(vPtr)
				Else
					att.value = ""
				End If
				If size = 3 Then
					vPtr = bmx_job_attrsValue(jobPtr, 2, i * size + 2)
					If vPtr Then
						att.kind = Int Ptr(vPtr)[0]
					End If
				End If
				
				obj.attributes[i] = att
				
			Next
		
			Return obj
		End If
		
		Return Null
	End Method

	Rem
	bbdoc: Returns the current graph object, or Null.
	about: @selection determines what graph object kinds to return. The default is all kinds, but can
	be one or more of #GV_VIEW_GRAPH, #GV_VIEW_SUBGRAPH, #GV_VIEW_NODE or #GV_VIEW_EDGE, or #GV_VIEW_ALL.<br>
	Valid graph objects are #TGViewGraph, #TGViewSubGraph, #TGViewNode and #TGViewEdge.
	End Rem
	Method currentObject:TGViewObject(selection:Int = GV_VIEW_ALL)
	
		If currentObj Then
		
			Local nPtr:Byte Ptr
			Local vPtr:Byte Ptr
			Local kind:Int
			Local size:Int
			Local sk:String
	
			Local obj:TGViewObject
			
			' get count for object details - generally we are only interested in the first two
			' kind and name
			Local aCount:Int = bmx_obj_attrCount(currentObjTypeList, 2)
					
			nPtr = bmx_obj_attrsValue(currentObjTypeList, 0)
			vPtr = bmx_obj_attrsValue(currentObjTypeList, 1)
			
			If nPtr Then
				sk = String.fromCString(nPtr)
			End If
			
			Select sk
				Case "digraph"
					If selection & GV_VIEW_GRAPH Then
						obj = New TGViewGraph
						obj.kind = GV_OBJ_DIGRAPH
						size = 3
					Else
						Return Null ' not interested in this kind
					End If
				Case "graph"
					If selection & GV_VIEW_GRAPH Then
						obj = New TGViewGraph
						obj.kind = GV_OBJ_GRAPH
						size = 3
					Else
						Return Null ' not interested in this kind
					End If
				Case "subgraph"
					If selection & GV_VIEW_SUBGRAPH Then
						obj = New TGViewSubGraph
						obj.kind = GV_OBJ_SUBGRAPH
						size = 3
					Else
						Return Null ' not interested in this kind
					End If
				Case "node"
					If selection & GV_VIEW_NODE Then
						obj = New TGViewNode
						obj.kind = GV_OBJ_NODE
						size = 2
					Else
						Return Null ' not interested in this kind
					End If
				Case "edge"
					If selection & GV_VIEW_EDGE Then
						obj = New TGViewEdge
						obj.kind = GV_OBJ_EDGE
						size = 2
					Else
						Return Null ' not interested in this kind
					End If
			End Select
			
			If vPtr Then
				obj.name = String.fromCString(vPtr)
			Else
				obj.name = ""
			End If

			' get count for object attribtues
			aCount = bmx_obj_attrCount(currentObjAttrList, size)
			
			obj.attributes = New TGVAttribute[aCount]
			For Local i:Int = 0 Until aCount
				Local att:TGVAttribute = New TGVAttribute
				
				nPtr = bmx_obj_attrsValue(currentObjAttrList, i * size)
				vPtr = bmx_obj_attrsValue(currentObjAttrList, i * size + 1)
				
				If nPtr Then
					att.name = String.fromCString(nPtr)
				Else
					att.name = ""
				End If
				If vPtr Then
					att.value = String.fromCString(vPtr)
				Else
					att.value = ""
				End If
				If size = 3 Then
					vPtr = bmx_obj_attrsValue(currentObjAttrList, i * size + 2)
					If vPtr Then
						att.kind = Int Ptr(vPtr)[0]
					End If
				End If
				
				obj.attributes[i] = att
				
			Next
		
			Return obj
		End If
		
		Return Null
	End Method
	
	Rem
	bbdoc: Resizes the graph to fit within the specified size.
	about: Will only "shrink to fit" unless @expand is True, wherein the graph will expanded as necessary.
	End Rem
	Method fit(width:Int, height:Int, expand:Int = False)
		Local jVWidth:Double, jVHeight:Double
		
		bmx_gvj_viewSize(jobPtr, Varptr jVWidth, Varptr jVHeight)

		Local zoomToFit:Double = Min(width / Double(jVWidth), height / Double(jVHeight) )

		If expand Or zoomToFit < 1.0 Then
			bmx_gvj_setZoom(jobPtr, zoomToFit)
		End If

	End Method
	
	
	Method Delete()
		' TODO - tidy up the mess
		
		If currentObjTypeList Then
			gv_argvlist_reset(currentObjTypeList)
			gv_argvlist_free(currentObjTypeList)
			currentObjTypeList = Null
		End If

		If currentObjAttrList Then
			gv_argvlist_reset(currentObjAttrList)
			gv_argvlist_free(currentObjAttrList)
			currentObjAttrList = Null
		End If
		
	End Method
	
End Type

' Loads and caches fonts used on the graph.
' Includes rudimentary fontpath searching...
Type TGVFont
	Field imageFont:Object
	Field name:String
	Field fontPath:String
	Field fpc:Byte Ptr
	Field size:Float
	Field flags:Int
	Field scale:Float
	
	Function key:String(name:String, size:Float, flags:Int)
		Return name + size
	End Function
	
	Method loadFont:TGVFont(_name:String, _size:Float, _flags:Int)
		
		name = _name
		size = _size
		flags = _flags
		
		init()
		
		Return Self
	End Method
	
	Method Init()
	End Method
	
	Method Delete()
		If fpc Then
			MemFree(fpc)
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TGViewObject
	Field kind:Int
	Field skind:String
	Field name:String
	
	Field attributes:TGVAttribute[]
	
End Type

Rem
bbdoc: 
End Rem
Type TGViewNode Extends TGViewObject
End Type

Rem
bbdoc: 
End Rem
Type TGViewEdge Extends TGViewObject
End Type

Rem
bbdoc: 
End Rem
Type TGViewGraph Extends TGViewObject
End Type

Rem
bbdoc: 
End Rem
Type TGViewSubGraph Extends TGViewGraph
End Type

Rem
bbdoc: 
End Rem
Type TGVAttribute
	Field name:String
	Field value:String
	Field kind:Int
End Type

Rem
bbdoc: 
End Rem
Type TGVStyle
	Field fillColor:String
	Field penColor:String
	Field fontColor:String

	Field font:String
	Field fsize:Int
	
	Field fcColor:TGVColor
	Field pcColor:TGVColor
	Field foColor:TGVColor
	
	Function set:TGVStyle(fillColor:String, penColor:String, fontColor:String = "black", font:String = "Arial", fsize:Int = 14)
		Local this:TGVStyle = New TGVStyle
		
		this.fillColor = fillColor
		this.penColor = penColor
		
		this.fcColor = TGVColor.setStr(fillColor)
		this.pcColor = TGVColor.setStr(penColor)
		this.foColor = TGVColor.setStr(fontColor)
		
		this.font = font
		this.fsize = fsize
		
		Return this
	End Function
	
End Type

Type TGVDrawStyle
	Field pen:Int
	Field fill:Int
	Field penWidth:Double
End Type

Rem
bbdoc: 
End Rem
Type TGVColor
	Field r:Int
	Field g:Int
	Field b:Int
	Field a:Int

	Function set:TGVColor(r:Int, g:Int, b:Int, a:Int = 255)
		Local this:TGVColor = New TGVColor
		
		this.r = r
		this.g = g
		this.b = b
		this.a = a
		
		Return this
	End Function
	
	' creates a new TGVColor instance based on txt
	Function setStr:TGVColor(txt:String)
		If Not txt Then
			Return Null
		End If
		
		Local this:TGVColor = New TGVColor
		
		If Not bmx_color_translate(txt, Varptr this.r, Varptr this.g, Varptr this.b, Varptr this.a) Then
			Return defaultFillColor
		End If
		
		Return this
	End Function
	
	' sets color and alpha
	Method doColor(driver:TGVDriver, kind:Int = 3)
		If kind & 1 Then
			driver.setPenColor(r / 255.0, g / 255.0, b / 255.0, a / 255.0 )
		End If
		If kind & 2 Then
			driver.SetFillColor(r / 255.0, g / 255.0, b / 255.0, a / 255.0 )
		End If
	End Method
	
End Type

Rem
bbdoc: Base type for building in-memory graph structures. (<a href="#GraphBuilder">GraphBuilder</a>)
End Rem
Type TGVGraphObject

	Field objName:String
	Field attributes:TMap = New TMap
	
	' the pointer to the actual graphviz object
	Field objPtr:Byte Ptr
	
	Rem
	bbdoc: Sets an attribute of the given @name and @value pair.
	about: Some useful attribute names include #ATTR_LABEL, #ATTR_NODE_SHAPE, #ATTR_NODE_FILLCOLOR,
	#ATTR_PENCOLOR, #ATTR_NODE_FONTCOLOR and #ATTR_STYLE.
	End Rem
	Method setAttr(name:String, value:String)
		attributes.insert(name, value)
	End Method
	
	Method setName(name:String)
		objName = name
	End Method
	
	Method free()
		If attributes Then
			attributes.clear()
			attributes = Null
		End If
	End Method

	Method Delete()
		free()
	End Method
	
End Type

Rem
bbdoc: A container of subgraphs, nodes and edges. (<a href="#GraphBuilder">GraphBuilder</a>)
about: Both the root graph ( #TGVGraph ) and subgraphs ( #TGVSubGraph ) are valid containers.
End Rem
Type TGVGraphContainer Extends TGVGraphObject

	Field parts:TMap = New TMap
	
	Rem
	bbdoc: Adds a #TGVNode to this container.
	returns: The new #TGVNode object.
	about: The name should be unique for the graph!!
	End Rem
	Method addNode:TGVNode(name:String)
		Local node:TGVNode = New TGVNode
		
		node.setName(name)
		
		parts.insert(name, node)
		
		Return node
	End Method
	
	Rem
	bbdoc: Adds a #TGVEdge to this container, creating a connection between @nodeFrom and @nodeTo.
	returns: The new #TGVEdge object.
	End Rem
	Method addEdge:TGVEdge(nodeFrom:TGVNode, nodeTo:TGVNode)
		Local edge:TGVEdge = New TGVEdge
		
		edge.nFrom = nodeFrom
		edge.nTo = nodeTo
		edge.setName(nodeFrom.objName + "->" + nodeTo.objName)
		
		parts.insert(edge.objName, edge)
		
		Return edge
	End Method
	
	Rem
	bbdoc: Adds a #TGVSubGraph to this container.
	returns: The new #TGVSubGraph object.
	about: The name should be unique for the graph!!
	End Rem
	Method addSubGraph:TGVSubGraph(name:String)
		Local subgraph:TGVSubGraph = New TGVSubGraph
		
		subgraph.setName(name)
		
		parts.insert(name, subgraph)
		
		Return subgraph
	End Method
	
	Method findNode:TGVNode(name:String)
		Local node:TGVNode = TGVNode(parts.ValueForKey(name))
		If node Then
			Return node
		End If
		For Local cont:TGVGraphContainer = EachIn parts.values()
			node = cont.findNode(name)
			If node Then
				Return node
			End If
		Next
	End Method

	Method findEdge:TGVEdge(name:String)
		Local edge:TGVEdge = TGVEdge(parts.ValueForKey(name))
		If edge Then
			Return edge
		End If
		For Local cont:TGVGraphContainer = EachIn parts.values()
			edge = cont.findEdge(name)
			If edge Then
				Return edge
			End If
		Next
	End Method
	
	Method GetParent:TGVGraphContainer(obj:TGVGraphObject)
		For Local part:TGVGraphObject = EachIn parts.values()
			If obj = part Then
				Return Self
			End If
			
			If TGVGraphContainer(part) Then
				Local cont:TGVGraphContainer = TGVGraphContainer(part).GetParent(obj)
				If cont Then
					Return cont
				End If
			End If
		Next 
	End Method
	
	Method free()
		If parts Then
			For Local part:TGVGraphObject = EachIn parts.values()
				part.free()
			Next
			parts.clear()
			parts = Null
		End If
	
		Super.free()
	End Method
	
End Type

Rem
bbdoc: The root graph. (<a href="#GraphBuilder">GraphBuilder</a>)
End Rem
Type TGVGraph Extends TGVGraphContainer

	Field graphType:Int
	
	Rem
	bbdoc: Creates a new #TGVGraph of the specified @graphType.
	about: @graphType should be one of #AGRAPH, #AGRAPHSTRICT, #AGDIGRAPH or #AGDIGRAPHSTRICT.<br>
	The deafult is #AGDIGRAPH.
	End Rem
	Function Create:TGVGraph(graphType:Int = AGDIGRAPH)
		Local this:TGVGraph = New TGVGraph
		
		this.graphType = graphType
		
		Return this
	End Function

End Type

Rem
bbdoc: A subgraph. (<a href="#GraphBuilder">GraphBuilder</a>)
about: Subgraphs play three roles in Graphviz.
<ul>
<li>First, a subgraph can be used to represent graph structure, indicating that certain nodes and edges
should be grouped together. This is the usual role for subgraphs and typically specifies semantic
information about the graph components.</li>
<li>In the second role, a subgraph can provide a context for setting attributes. In Graphviz, these are often
attributes used by the layout and rendering functions. For example, the application could specify that blue
is the default color for nodes. Then, every node within the subgraph will have color blue.<br>
In the context of
graph drawing, a more interesting example is:
<pre>subgraph {
  rank = same; A; B; C;
}</pre>
This (anonymous) subgraph specifies that the nodes A, B and C should all be placed on the same rank if
drawn using dot.</li>
<li>The third role for subgraphs combines the previous two. If the name of the subgraph begins with
&quot;cluster&quot;, Graphviz identifies the subgraph as a special cluster subgraph. The software will
do the layout of the graph so that the nodes belonging to the cluster are drawn together, with the entire
drawing of the cluster contained within a bounding rectangle.</li>
</ul>
End Rem
Type TGVSubGraph Extends TGVGraphContainer
End Type

Rem
bbdoc: A graph node. (<a href="#GraphBuilder">GraphBuilder</a>)
End Rem
Type TGVNode Extends TGVGraphObject
End Type

Rem
bbdoc: A graph edge. (<a href="#GraphBuilder">GraphBuilder</a>)
about: An edge connects two nodes.
End Rem
Type TGVEdge Extends TGVGraphObject

	Field nFrom:TGVNode
	Field nTo:TGVNode
	
	Method free()
		nFrom = Null
		nTo = Null
		
		Super.free()
	End Method
	
End Type

' graph builder delegate
Type TGVGraphBuilder

	Field graph:TGVGraph
	Field graphPtr:Byte Ptr
	
	Const DO_GRAPH:Int = 1
	Const DO_NODE:Int = 2
	Const DO_EDGE:Int = 3

	Function Create:TGVGraphBuilder(graph:TGVGraph)
		Local this:TGVGraphBuilder = New TGVGraphBuilder
		
		this.graph = graph
		this.buildGraph()
		
		Return this
	End Function
	
	Method buildGraph()
	
		' create the new graph
		graphPtr = agopen("G", graph.graphType)
		
		processContainer(graph, DO_GRAPH)
		processContainer(graph, DO_NODE)
		processContainer(graph, DO_EDGE)
		
	End Method

	Method processContainer(obj:TGVGraphContainer, kind:Int)
	
		If kind = DO_GRAPH Then

			If TGVGraph(obj) Then
				' the root graph
				processGraph(TGVGraph(obj))
				
			End If
		End If

		For Local child:TGVGraphObject = EachIn obj.parts.values()
		
			If TGVSubGraph(child) Then
			
				If kind = DO_GRAPH Then
			
					processSubGraph(TGVSubGraph(child), obj)
			
				End If

				processContainer(TGVGraphContainer(child), kind)
				
				
			Else If TGVNode(child) And (kind = DO_NODE) Then
					
				processNode(TGVNode(child), obj)

			Else If TGVEdge(child) And (kind = DO_EDGE) Then

				processEdge(TGVEdge(child), obj)

			End If
			
		Next
		
	End Method
	
	Method processGraph(g:TGVGraph)

		' connect the graph pointer
		If Not g.objPtr Then
			g.objPtr = graphPtr
			processAttributes(g)
		End If

	End Method
	
	Method processSubGraph(sg:TGVSubGraph, parent:TGVGraphContainer)
		' create the subgraph
		sg.objPtr = agsubg(parent.objPtr, sg.objName)
		processAttributes(sg)

	End Method
	
	Method processNode(node:TGVNode, parent:TGVGraphContainer)
		' create the node
		node.objPtr = agnode(parent.objPtr, node.objName)
		processAttributes(node)

	End Method

	Method processEdge(edge:TGVEdge, parent:TGVGraphContainer)

		' create the edge
		edge.objPtr = agedge(parent.objPtr, edge.nFrom.objPtr, edge.nTo.objPtr)
		processAttributes(edge)

	End Method
	
	Method processAttributes(obj:TGVGraphObject)
		For Local attr:String = EachIn obj.attributes.keys()
			Local value:String = String(obj.attributes.ValueForKey(attr))
			If Not value Then
				value = ""
			End If
			If attr <> ATTR_HTML_LABEL Then
				' create the attribute for this object
				agsafeset(obj.objPtr, attr, value, "")
			Else
				bmx_agsafesetHTML(obj.objPtr, ATTR_LABEL, value, "")
			End If
		Next	
	End Method
	
	Method Delete()
		graph = Null
	End Method
	
End Type

Rem
bbdoc: Implementation specific rendering support.
about: To remove graphics driver specific code from the module, the user can pass in an instance of
#TGVRenderSupport which adds driver-specific support for some drawing features.
End Rem
Type TGVRenderSupport Abstract
	Rem
	bbdoc: Provide any required initialization
	End Rem
	Method Init() Abstract
	Rem
	bbdoc: Enable dotted line drawing
	End Rem
	Method setDotted() Abstract
	Rem
	bbdoc: Enable dashed line drawing
	End Rem
	Method setDashed() Abstract
	Rem
	bbdoc: Enable solid line drawing
	End Rem
	Method setSolid() Abstract
End Type


Type TGVDriver Abstract

	Field graphviz:TGVGraphviz
	
	Method setContext(context:TGVGraphviz)
		graphviz = context
	End Method
	
	Method setPenColor(r:Double, g:Double, b:Double, a:Double) Abstract
	
	Method setFillColor(r:Double, g:Double, b:Double, a:Double) Abstract
	
	Method textPara(text:String, size:Double, x:Double, y:Double, width:Double, height:Double, Name:String, just:Int) Abstract

	Method ellipse(x0:Double, y0:Double, x1:Double, y1:Double, filled:Int) Abstract

	Method polygon(xy:Float[], filled:Int) Abstract

	Method bezier(points:Double Ptr, n:Int, arrowAtStart:Int, arrowAtEnd:Int, filled:Int) Abstract

	Method polyline(xy:Float[]) Abstract
	
	Method drawTooltip(txt:String, xy:Float[], scale:Double, style:TGVStyle) Abstract

	Method getFontMetrics(font:TGVFont, txt:String, width:Double Var, height:Double Var) Abstract

	Method textSize(txt:String, font:TGVFont, width:Int Var, height:Int Var, scale:Float Var) Abstract

	Method currentSettings(originX:Float Var, originY:Float Var, r:Int Var, g:Int Var, b:Int Var, a:Float Var, ..
			vpX:Int Var, vpY:Int Var, vpW:Int Var, vpH:Int Var) Abstract

	Method setViewPortAndOrigin(vpX:Int, vpY:Int, vpW:Int, vpH:Int, originX:Int, originY:Int) Abstract

	Method setSettings(originX:Float, originY:Float, r:Int, g:Int, b:Int, a:Float, ..
			vpX:Int, vpY:Int, vpW:Int, vpH:Int) Abstract

	Method loadFont:TGVFont(name:String, size:Float, flags:Int) Abstract

	Method GetOriginXY(originX:Float Var, originY:Float Var) Abstract
	
End Type

Rem
bbdoc: 
about: Synonym for &quot;label&quot;.
End Rem
Const ATTR_LABEL:String = "label"
Rem
bbdoc: 
about: Synonym for &quot;label&quot;, for displaying HTML.
End Rem
Const ATTR_HTML_LABEL:String = "hlabel"
Rem
bbdoc: Define a node shape.
about: Synonym for &quot;<a href="lang.html#d:shape">shape</a>&quot;.
End Rem
Const ATTR_NODE_SHAPE:String = "shape"
Rem
bbdoc: Node background color.
about: Synonym for &quot;<a href="lang.html#d:fillcolor">fillcolor</a>&quot;.
End Rem
Const ATTR_NODE_FILLCOLOR:String = "fillcolor"
Rem
bbdoc: Basic drawing color.
about: Synonym for &quot;<a href="lang.html#d:color">color</a>&quot;.
End Rem
Const ATTR_PENCOLOR:String = "color"
Rem
bbdoc: Color used for text.
about: Synonym for &quot;<a href="lang.html#d:fontcolor">fontcolor</a>&quot;.
End Rem
Const ATTR_FONTCOLOR:String = "fontcolor"
Rem
bbdoc: Style for node or edge.
about: Synonym for &quot;<a href="lang.html#d:style">style</a>&quot;.
End Rem
Const ATTR_STYLE:String = "style"

Rem
bbdoc: 
End Rem
Const GV_VIEW_GRAPH:Int = $01
Rem
bbdoc: 
End Rem
Const GV_VIEW_SUBGRAPH:Int = $02
Rem
bbdoc: 
End Rem
Const GV_VIEW_NODE:Int = $04
Rem
bbdoc: 
End Rem
Const GV_VIEW_EDGE:Int = $08
Rem
bbdoc: 
End Rem
Const GV_VIEW_ALL:Int = GV_VIEW_GRAPH | GV_VIEW_SUBGRAPH | GV_VIEW_NODE | GV_VIEW_EDGE




Global defaultPenColor:TGVColor = TGVColor.set(0, 0, 0)
Global defaultFillColor:TGVColor = TGVColor.set(255, 255, 255)
Global defaultTooltipStyle:TGVStyle = TGVStyle.set("/oranges8/1","/oranges8/8", "black")
