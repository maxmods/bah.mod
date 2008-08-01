' createhtmlview.bmx

Strict

Framework BaH.gtkmaxgui

Import BaH.gtkwebmozilla
'Import BaH.gtkwebgtkhtml

Local window:TGadget
Local htmlview:TGadget


window=CreateWindow("My Window",30,20,600,440,,15|WINDOW_ACCEPTFILES)

htmlview=CreateHTMLView(0,0,ClientWidth(window),ClientHeight(window),window)',HTMLVIEW_NONAVIGATE)
SetGadgetLayout htmlview,1,1,1,1

'HtmlViewGo htmlview,"http://blitzmax.com/Community/topics.php?forum=109"
'HtmlViewGo htmlview, "/home/brucey/programming/c-coding/gtkhtml-3.10.2/doc/html/htmlengine_8c-source.html"
'HtmlViewGo htmlview, "/home/brucey/programming/BlitzMax/mod/brl.mod/maxgui.mod/doc/commands.html#CreateButton"
HtmlViewGo htmlview,"/home/brucey/programming/BlitzMax/mod/brl.mod/maxgui.mod/doc/commands.html"
'HtmlViewGo htmlview,"/usr/share/gtk-doc/html/"'glib/glib-Datasets.html
While WaitEvent()
	Print CurrentEvent.ToString()
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
	End Select
Wend
