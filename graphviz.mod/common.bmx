SuperStrict

Import BRL.Blitz
Import BaH.Expat

Import "source.bmx"

?linux
Import "src_vmalloc.bmx"
Import "src_sfio.bmx"
?
Import "src_circogen.bmx"
Import "src_dotgen.bmx"
Import "src_neatogen.bmx"
Import "src_fdpgen.bmx"
Import "src_twopigen.bmx"
Import "src_pack.bmx"

Import "src_plugin.bmx"

Extern

	Function _strlen:Int(s:Byte Ptr) = "strlen"

	Function bmx_gvc_new:Byte Ptr()
	Function bmx_agmemread:Byte Ptr(text:Byte Ptr)
	
	Function gvLayout(gvc:Byte Ptr, g:Byte Ptr, how:Byte Ptr)
	Function gvRender(gvc:Byte Ptr, g:Byte Ptr, how:Byte Ptr, where:Byte Ptr)
	Function bmx_gvFreeLayout(gvc:Byte Ptr, g:Byte Ptr)
	
	Function gvLayoutJobs(gvc:Byte Ptr, g:Byte Ptr)
	
	Function bmx_setpencolor(gvj:Byte Ptr, r:Double Ptr, g:Double Ptr, b:Double Ptr, a:Double Ptr)
	Function bmx_setfillcolor(gvj:Byte Ptr, r:Double Ptr, g:Double Ptr, b:Double Ptr, a:Double Ptr)
	Function bmx_setstyle(gvj:Byte Ptr, pen:Int Ptr, fill:Int Ptr, penwidth:Double Ptr)
	
	Function bmx_para_str:Byte Ptr(para:Byte Ptr)
	Function bmx_para_fontname:Byte Ptr(para:Byte Ptr)
	Function bmx_para_fontmetrics(para:Byte Ptr, size:Double Ptr, width:Double Ptr, height:Double Ptr)
	
	Function bmx_para_just:Byte(para:Byte Ptr)
	
	Function bmx_color_type:Int(color:Byte Ptr)
	Function bmx_para_setlayout(para:Byte Ptr, font:Object)
	Function bmx_para_setmetrics(para:Byte Ptr, size:Double, width:Double, height:Double)
	Function bmx_para_layout:Object(para:Byte Ptr)
	
	Function bmx_gvj_size:Int(gvj:Byte Ptr, width:Int Ptr, height:Int Ptr)
	
	'Function bmx_gvj_setSize(gvj:Byte Ptr, width:Int, height:Double)
	Function bmx_gvj_setSize(job:Byte Ptr, width:Int, height:Int)
	
	Function bmx_job_refresh(job:Byte Ptr)
	Function bmx_gv_setdpi(job:Byte Ptr, x:Int, y:Int)
	
	Function bmx_gvParseArgs(gvc:Byte Ptr, how:Byte Ptr)
'	Function gvLayoutJobs(gvc:Byte Ptr, g:Byte Ptr)
	Function gvRenderJobs(gvc:Byte Ptr, g:Byte Ptr)
	
	Function bmx_job_setPointer(gvj:Byte Ptr, x:Int, y:Int)
	Function bmx_job_mouseDown(gvj:Byte Ptr, x:Int, y:Int, button:Int)
	Function bmx_job_mouseUp(gvj:Byte Ptr, x:Int, y:Int, button:Int)
	Function bmx_job_mouseScroll(gvj:Byte Ptr, x:Int, y:Int, direction:Int)
	
	Function bmx_gvj_zoom(gvj:Byte Ptr, zoom:Double Ptr)
	Function bmx_gvj_setZoom(gvj:Byte Ptr, zoom:Double)
	Function bmx_gvj_pageOffset(gvj:Byte Ptr, x:Double Ptr, y:Double Ptr)
	Function bmx_gvj_focus(gvj:Byte Ptr, x:Double Ptr, y:Double Ptr)
	Function bmx_gvj_viewSize(gvj:Byte Ptr, width:Double Ptr, height:Double Ptr)
	
	Function bmx_job_getGVC:Byte Ptr(gvj:Byte Ptr)
	Function bmx_job_getWindow:Object(gvj:Byte Ptr)
	Function bmx_job_setWindow(gvj:Byte Ptr, win:Object)

	Function bmx_job_active_tooltip:Byte Ptr(gvj:Byte Ptr)
	Function bmx_color_translate:Int(txt:Byte Ptr, r:Int Ptr, g:Int Ptr, b:Int Ptr, a:Int Ptr)
	
	Function bmx_job_hasSelection:Int(gvj:Byte Ptr)
	Function bmx_job_attrCount:Int(gvj:Byte Ptr, kind:Int, count:Int)
	'Function bmx_job_attrsName:Byte Ptr(gvj:Byte Ptr, kind:Int, count:Int, i:Int)
	Function bmx_job_attrsValue:Byte Ptr(gvj:Byte Ptr, kind:Int, i:Int)
	
	Function bmx_gvjobs_delete(gvc:Byte Ptr)
	Function bmx_gvevent_layout:Byte Ptr(gvj:Byte Ptr, txt:Byte Ptr, graph:Byte Ptr)
	
	Function agopen:Byte Ptr(name:Byte Ptr, kind:Int)
	Function agraphattr:Byte Ptr(graph:Byte Ptr, name:Byte Ptr, defaultValue:Byte Ptr)
	Function agnodeattr:Byte Ptr(graph:Byte Ptr, name:Byte Ptr, defaultValue:Byte Ptr)
	Function agedgeattr:Byte Ptr(graph:Byte Ptr, name:Byte Ptr, defaultValue:Byte Ptr)
	Function agsubg:Byte Ptr(parent:Byte Ptr, name:Byte Ptr)
	Function agnode:Byte Ptr(parent:Byte Ptr, name:Byte Ptr)
	Function agedge:Byte Ptr(parent:Byte Ptr, nodeFrom:Byte Ptr, nodeTo:Byte Ptr)
	Function agsafeset(objPtr:Byte Ptr, attr:Byte Ptr, value:Byte Ptr, defaultValue:Byte Ptr)
	Function agclose(graph:Byte Ptr)

	Function bmx_agsafesetHTML(objPtr:Byte Ptr, attr:Byte Ptr, value:Byte Ptr, defaultValue:Byte Ptr)
	
	Function bmx_graph_bgcolor(job:Byte Ptr, r:Int Ptr, g:Int Ptr, b:Int Ptr, a:Int Ptr)
	
	Function gvNEWargvlist:Byte Ptr()
	Function gv_argvlist_reset(list:Byte Ptr)
	Function gv_argvlist_free(list:Byte Ptr)
	Function bmx_job_checkCurrent:Byte Ptr(job:Byte Ptr, current:Byte Ptr, nameList:Byte Ptr, attrList:Byte Ptr)
	Function bmx_obj_attrCount:Int(list:Byte Ptr, size:Int)
	Function bmx_obj_attrsValue:Byte Ptr(list:Byte Ptr, index:Int)
	
End Extern

?win32
Const kDEFAULT_FONTPATH:String = "C:/WINDOWS/FONTS;C:/WINNT/Fonts;C:/winnt/fonts"
Const kPATHSEPARATOR:String = ";"
?linux
Const kDEFAULT_FONTPATH:String = "/usr/share/X11/fonts/TTF:/usr/X11R6/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/truetype:" + ..
	"/usr/X11R6/lib/X11/fonts/TTF:/usr/share/fonts/bitstream-vera:" + ..
	"/usr/share/fonts/TrueType:/usr/share/fonts/truetype:/usr/openwin/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/Type1"
Const kPATHSEPARATOR:String = ":"
?macos
Const kDEFAULT_FONTPATH:String = "~~/Library/Fonts:/Library/Fonts:/Network/Library/Fonts:/System/Library/Fonts"
Const kPATHSEPARATOR:String = ":"
?

Function gvUTF8toISO8859:String(s:Byte Ptr)
	Local l:Int = _strlen(s)
	Local b:Short[] = New Short[l]
	Local bc:Int = -1
	Local c:Int
	Local d:Int
	Local e:Int
	For Local i:Int = 0 Until l
		bc:+1
		c = s[i]
		If c<128 
			b[bc] = c
			Continue
		End If
		i:+1
		d=s[i]
		If c<224 
			b[bc] = (c-192)*64+(d-128)
			Continue
		End If
		i:+1
		e = s[i]
		If c < 240 
			b[bc] = (c-224)*4096+(d-128)*64+(e-128)
			Continue
		End If
	Next

	Return String.fromshorts(b, bc + 1)
End Function

Const GV_FONT_SCALE:Float = 0.3333

Const GV_OBJ_DIGRAPH:Int = 1
Const GV_OBJ_GRAPH:Int = 2
Const GV_OBJ_SUBGRAPH:Int = 3
Const GV_OBJ_NODE:Int = 4
Const GV_OBJ_EDGE:Int = 5

Const GV_PEN_NONE:Int = 0
Const GV_PEN_DASHED:Int = 1
Const GV_PEN_DOTTED:Int = 2
Const GV_PEN_SOLID:Int = 3

Const GV_FILL_NONE:Int = 0
Const GV_FILL_SOLID:Int = 1

Const MAX2D_OPENGL:Int = 0
Const MAX2D_DIRECTX:Int = 1

Const AGFLAG_DIRECTED:Int = 1 Shl 0
Const AGFLAG_STRICT:Int = 1 Shl 1

Rem
bbdoc: A non-strict, undirected graph.
about: This graph type allows an arbitrary number of edges between two nodes.
End Rem
Const AGRAPH:Int = 0
Rem
bbdoc: A strict, undirected graph.
about: This graph type can have at most one edge between any pair of nodes.
End Rem
Const AGRAPHSTRICT:Int = AGRAPH | AGFLAG_STRICT
Rem
bbdoc: A non-strict, directed graph.
about: This graph type allows an arbitrary number of edges between two nodes.<br>
In a directed graph, the pair of nodes is ordered, so the graph can have edges from node A to node B
as well as edges from B to A.
End Rem
Const AGDIGRAPH:Int = AGFLAG_DIRECTED
Rem
bbdoc: A strict, directed graph.
about: This graph type can have at most one edge between any pair of nodes.
End Rem
Const AGDIGRAPHSTRICT:Int = AGDIGRAPH | AGFLAG_STRICT
