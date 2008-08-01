' Copyright (c) 2006-2008 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
'
SuperStrict

Rem
bbdoc: GTKMaxGUI Linux HTMLView Widget
End Rem
Module BaH.GTKWebGTKHTML

ModuleInfo "Version: 1.02"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.02"
ModuleInfo "History: ProcessURL changes to use banks. [Simon Armstrong]"
ModuleInfo "History: 1.01"
ModuleInfo "History: Minor tweaks."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

Import BaH.GTKMaxGUI

?linux

Import "htmlhandler.bmx"

Import "-lgtkhtml-3.8"
Import "-lgailutil"

Extern
	Function gtk_html_new:Byte Ptr()
	Function gtk_html_set_allow_frameset(handle:Byte Ptr, bool:Int)
	Function gtk_html_load_empty(handle:Byte Ptr)
	Function gtk_html_begin_content:Byte Ptr(handle:Byte Ptr, _type:Byte Ptr)
	Function gtk_html_write(handle:Byte Ptr, stream:Byte Ptr, buffer:Byte Ptr, length:Int)
	Function gtk_html_end(handle:Byte Ptr, stream:Byte Ptr, stat:Int)
	Function gtk_html_get_base:Byte Ptr(handle:Byte Ptr)
	Function gtk_html_set_base(handle:Byte Ptr, base:Byte Ptr)
	Function gtk_html_jump_to_anchor(handle:Byte Ptr, anchor:Byte Ptr)
End Extern

Global GTKHTMLWeb:TGTKWebGTKHTMLDriver = New TGTKWebGTKHTMLDriver

Type TGTKWebGTKHTMLDriver Extends TGTKWebDriver
	Method New()
		gtkmaxgui_htmlview = Self
	End Method
	
	Function CreateHTMLView:TGTKWebGTKHTML(x:Int, y:Int, w:Int, h:Int, label:String, group:TGadget, style:Int)
		Return TGTKWebGTKHTML.CreateHTMLView(x, y, w, h, label, group, style)
	End Function
End Type


Const GTK_HTML_STREAM_OK:Int = 0
Const GTK_HTML_STREAM_ERROR:Int = 1


Rem
bbdoc: An HTMLView gadget, based on gtkhtml
about: Call it a kludge if you like.. but it mostly works.
The gtkhtml widget is wrapped inside a scrollwindow, which itself is placed inside a vbox.
The reason being that I couldn't get the gtkhtml/scrollwindow to resize when/how I wanted, but by
placing it inside a gtkbox widget, I can resize that and the rest will follow automagically :-)<br>
Of course, the gtkhtml widget doesn't do much in the way of URL processing, *nor* in fetching any of
its own content, all of which passes thru a LoadString() in a callback.<br>
I say - HACKY....  but it works, mostly :-p
End Rem
Type TGTKWebGTKHTML Extends TGTKHTMLView

	Field box:Byte Ptr
	Field scrollWindow:Byte Ptr
	Field noNavigate:Int
	Field anchor:String
	Field _cw:Int
	Field _ch:Int

	Field currenturl$
	Field history:TList
	Field historylink:TLink

	Global banks:TList=New TList

	Function CreateHTMLView:TGTKWebGTKHTML(x:Int, y:Int, w:Int, h:Int, label:String, group:TGadget, style:Int)
		Local this:TGTKWebGTKHTML = New TGTKWebGTKHTML

		this.initHTMLView(x, y, w, h, label, group, style)

		Return this
	End Function

	Method initHTMLView(x:Int, y:Int, w:Int, h:Int, label:String, group:TGadget, style:Int)

		handle = gtk_html_new()

		init(GTK_HTMLVIEW, x, y, w, h, style)

		gtk_container_set_resize_mode(handle, GTK_RESIZE_QUEUE)

		gtk_html_set_allow_frameset(handle, False)
		gtk_html_load_empty(handle)
		setShow(True)

		' create a scroll window for the html view
		scrollWindow = gtk_scrolled_window_new(Null, Null)
		' set container resize mode
		gtk_container_set_resize_mode(scrollWindow, GTK_RESIZE_QUEUE)
		' set scrollbar policy
		gtk_scrolled_window_set_policy(scrollWindow, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC)
		' show
		gtk_widget_show(scrollWindow)

		' add the html view to the scroll view
		gtk_container_add(scrollWindow, handle)


		' We need to put this inside a box...  Why?  because we can't resize the gtkhtml gadget 
		' in code - but we *can* resize a box that holds the scrollwindow + gtkhtml gadget...
		box = gtk_vbox_new(False, 0)
		gtk_widget_show(box)
		gtk_box_pack_start(box, scrollWindow, True, True, 0)

		gtk_layout_put(TGTKContainer(group).container, box, x, y)
		gtk_widget_set_size_request(box, w, Max(h,0))


		noNavigate = (style & HTMLVIEW_NONAVIGATE)

		g_signal_cb4(handle, "url_requested", processURL, Self, Destroy, 0)
		g_signal_cb2(handle, "load_done", PageLoaded, Self, Destroy, 0)
		g_signal_cb3(handle, "link_clicked", LinkClicked, Self, Destroy, 0)
	End Method

	Method Free()
		
	End Method

	Method BackURL()
		If historylink And historylink.PrevLink()
			historylink = historylink.PrevLink()
			SetURL(String(historylink.Value()),False)
		EndIf
	End Method
	
	Method ForwardURL()
		If historylink And historylink.NextLink()
			historylink = historylink.NextLink()
			SetURL(String(historylink.Value()),False)
		EndIf
	End Method

	Method SetURL(url:String, addhistory:Int = True)
		If url <> Null And url.length > 0 Then

			If addhistory Then
				If Not history Then
					history = New TList
				End If

				While historylink And historylink.NextLink()
					historylink.NextLink().Remove
				Wend

				historylink = history.AddLast(url)
			EndIf		


			Local html_stream_handle:Byte Ptr = gtk_html_begin_content(handle, "text/html; charset=utf-8")
	
			Local s:String = urlLessHTTP(checkURL(url))


			gtk_html_set_base(handle, urlLessAnchor(s).toCString())

			' set the url anchor, if any
			anchor = urlAnchor(s)

			processURL(handle, s.toCString(), html_stream_handle, Self)
		End If
	End Method

	Method SetText(url:String)
		SetURL(url)
	End Method
Rem
	Method Rethink()
		If scrollWindow
			If parent.clientwidth() <> ClientWidth() Or parent.clientheight() <> ClientHeight() Then

				Local alloc:TGTKAllocation = New TGTKAllocation
				alloc.x = xpos
				alloc.y = ypos
				alloc.width = width
				alloc.height = height
				gtk_widget_size_allocate(box, alloc)

				gtk_widget_set_size_request(box, width, height)

			End If
		EndIf
	End Method
End Rem
	Method ClientWidth:Int()
		Return width
		Local req:TGTKRequisition = New TGTKRequisition
		gtk_widget_size_request(box, req)
		Return req.width
	End Method

	Method ClientHeight:Int()
		Return height
		Local req:TGTKRequisition = New TGTKRequisition
		gtk_widget_size_request(box, req)
		Return req.height
	End Method

	Method GetText:String()
		If handle Then
			Return String.FromCString(gtk_html_get_base(handle))
		End If
	End Method

	Method Activate(cmd:Int)
		Super.Activate(cmd)

		Select cmd
			Case ACTIVATE_FORWARD
				ForwardURL()
			Case ACTIVATE_BACK
				BackURL()
		End Select
	End Method
	
	Method Stop()
	End Method

	Rem
	about: A bit of URL mashing, so we can use the correct Stream kind etc.
	End Rem
	Function checkURL:String(url:String, forMax:Int=False)
		Local	anchor$,a:Int
		a=url.Find("#")
		If a<>-1 Then
			anchor=url[a..]
			url=url[..a]
		End If
		If url[0..7]<>"http://" And url[0..7]<>"file://" Then
			If FileType(url) Then
				'url="file://"+url
			Else
				If forMax Then
					If url[0..6]<>"http::" Then
						url="http::" + url
					End If
				Else
					url = "http://" + url
				End If
			EndIf
		EndIf
		If forMax Then
			If url[0..7] = "http://" Then
				url = "http::" + url[7..]
			End If
		End If
		url:+anchor
		url = url.Replace(" ","%20")
		Return url
	End Function

	Rem
	bbdoc: Returns a URL minus an anchor, If there was one.
	End Rem
	Function urlLessAnchor:String(url:String)
		Local pos:Int = url.Find("#")
		If pos >= 0 Then
			url = url[..pos]
		End If

		Return url
	End Function

	Rem
	bbdoc: Returns a anchor from a full URL, If there was one.
	End Rem
	Function urlAnchor:String(url:String)
		Local pos:Int = url.Find("#")
		Local anchor:String

		If pos >= 0 Then
			anchor=url[pos+1..]
		End If

		Return anchor
	End Function

	Function urlLessHTTP:String(url:String)
		If url[0..6] = "http::" Then
			url = url[7..]
		End If
		Return url
	End Function

	Rem
	bbdoc: Called when a URL requires processing.
	about: This can Include images during a page load, etc.
	End Rem
	Function processURL(handle:Byte Ptr, url:Byte Ptr, html_stream:Byte Ptr, data:Object)

		Local u:String = parse_href(checkURL(String.FromCString(url)), String.FromCString(gtk_html_get_base(handle)))

		Local path:String = urlLessAnchor(checkURL(u, True))

		Local status:Int
		Local b:TBank=LoadBank(path)
		If b Then

			banks.addlast b

			gtk_html_write(handle, html_stream, LockBank(b), BankSize(b))
			
			status = GTK_HTML_STREAM_OK
		Else
			status = GTK_HTML_STREAM_ERROR
		End If

    	    gtk_html_end(handle, html_stream, status)
	End Function

	Rem
	bbdoc: Called when the page finishes loading
	End Rem
	Function PageLoaded(handle:Byte Ptr, obj:Object)
			PostGuiEvent(EVENT_GADGETDONE, TGadget(obj))
			
			' tidy up...
			For Local b:TBank = EachIn banks
				b.unlock()
			Next
			banks.clear()
			
			' if an anchor was provided, jump to it...
			' anchor NEEDs to be a <a...> tag to work
			Local anchor:String = TGTKWebGTKHTML(obj).anchor
			If anchor <> Null And anchor.length > 0 Then
				gtk_html_jump_to_anchor(handle, anchor)
			End If
	End Function

	Rem
	bbdoc: Called when a link is clicked
	about: If HTMLVIEW_NONAVIGATE is set For the view, an event is raised, rather than the link followed.
	End Rem
	Function LinkClicked(handle:Byte Ptr, link:Byte Ptr, obj:Object)
		Local url:String = parse_href(String.FromCString(link), String.FromCString(gtk_html_get_base(handle)))

		If TGTKWebGTKHTML(obj).noNavigate Then
			PostGuiEvent(EVENT_GADGETACTION, TGadget(obj),0,0,0,0,url)
		Else
			'Local url:String = parse_href(String.FromCString(link), String.FromCString(gtk_html_get_base(handle)))
			TGTKWebGTKHTML(obj).SetText(url)
		End If
	End Function

	Function parse_href:String(url:String, base:String)
'Print "url = " + url
		Local tmp:String
		Local tmpurl:THtmlHandler
		Local retval:String

		Local baseURL:THtmlHandler = THtmlHandler.newURL(base)

        If url = Null Or url.length = 0 Then
             Return ""
		End If

		Local anchorPos:Int = url.find("#")
		If anchorPos > 0 Then
			If url[anchorPos-1..anchorPos] = "/" Then
				url = url[anchorPos..]
			End If
		End If

        If url[0..1] = "#" Then
                tmpurl = baseURL.duplicate(THtmlHandler.REMOVE_REFERENCE)
                tmpurl.setReference(url[1..])

                tmp = tmpurl.toString()

                Return tmp
        End If

        tmpurl = THtmlHandler.newURL(url)
        If tmpurl.getProtocol() = Null Then
                If url[0..1] = "/" Then
                        If url[1..2] = "/" Then
                                Local t:String = baseURL.getProtocol() + ":" + url
                                tmpurl = THtmlHandler.newURL(t)
                                retval = tmpurl.toString()
                        Else
                                tmpurl = baseURL.duplicate(THtmlHandler.REMOVE_PATH)
                                tmpurl.setPath(url)
                                retval = tmpurl.toString()
                        End If
                Else
                        tmpurl = baseURL.appendPath(url)
                        retval = tmpurl.toString()
                End If
        Else
                retval = tmpurl.toString()
        End If

        Return retval
	End Function

	Method Rethink()
		If handle Then
			gtk_layout_move(TGTKContainer(parent).container, box, xpos, ypos)
			gtk_widget_set_size_request(box, Max(width,0), Max(height,0))
		End If
	End Method

	Method toString:String()
		Return "TGTKHTMLView"
	End Method

End Type

?