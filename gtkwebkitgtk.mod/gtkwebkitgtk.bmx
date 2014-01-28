' Copyright (c) 2014 Bruce A Henderson
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
bbdoc: GTKMaxGUI Linux WebKitGtk Widget
End Rem
Module BaH.GTKWebKitGtk

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2014 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

Import BaH.GTKMaxGUI

?linux

Import "-lwebkitgtk-1.0"

Extern
	Function webkit_web_view_new:Byte Ptr()
	Function webkit_web_view_load_uri(handle:Byte Ptr, url:Byte Ptr)
	Function webkit_web_view_get_load_status:Int(handle:Byte Ptr)
	Function webkit_web_view_stop_loading(handle:Byte Ptr)
	Function webkit_web_view_can_go_forward:Int(handle:Byte Ptr)
	Function webkit_web_view_can_go_back:Int(handle:Byte Ptr)
	Function webkit_web_view_go_forward(handle:Byte Ptr)
	Function webkit_web_view_go_back(handle:Byte Ptr)
	Function webkit_web_view_get_uri:Byte Ptr(handle:Byte Ptr)
End Extern

Global GtkWebKitGtkWeb:TGTKWebKitGtkDriver = New TGTKWebKitGtkDriver

Type TGTKWebKitGtkDriver Extends TGTKWebDriver
	Method New()
		gtkmaxgui_htmlview = Self
	End Method
	
	Function CreateHTMLView:TGTKWebKitGtk(x:Int, y:Int, w:Int, h:Int, label:String, group:TGadget, style:Int)
		Return TGTKWebKitGtk.CreateHTMLView(x, y, w, h, label, group, style)
	End Function
End Type


Type TGTKWebKitGtk Extends TGTKHTMLView

	Field noNavigate:Int

	Function CreateHTMLView:TGTKWebKitGtk(x:Int, y:Int, w:Int, h:Int, label:String, group:TGadget, style:Int)
		Local this:TGTKWebKitGtk = New TGTKWebKitGtk

		this.initHTMLView(x, y, w, h, label, group, style)

		Return this
	End Function

	Method initHTMLView(x:Int, y:Int, w:Int, h:Int, label:String, group:TGadget, style:Int)

		handle = webkit_web_view_new()

		init(GTK_HTMLVIEW, x, y, w, h, style)

		noNavigate = (style & HTMLVIEW_NONAVIGATE)

		gtk_layout_put(TGTKContainer(group).container, handle, x, y)
		gtk_widget_set_size_request(handle, w, Max(h,0))

		SetShow(True)

		g_object_connect(handle, "signal::notify::load-status", OnLoadStatus, Self, 0)

	End Method
	
	Rem
	bbdoc: Stops loading of the current document.
	End Rem
	Method Stop()
		webkit_web_view_stop_loading(handle)
	End Method

	Method SetText(url:String)
		Local urlPtr:Byte Ptr = checkURL(url).ToUTF8String()
		webkit_web_view_load_uri(handle, urlPtr)
		MemFree(urlPtr)
	End Method
	
	Method GetText:String()
		If handle Then
			Return String.FromUTF8String(webkit_web_view_get_uri(handle))
		End If
	End Method

	Method Activate(cmd:Int)
		Super.Activate(cmd)

		Select cmd
			Case ACTIVATE_FORWARD
				If webkit_web_view_can_go_forward(handle) Then
					webkit_web_view_go_forward(handle)
				End If
			Case ACTIVATE_BACK
				If webkit_web_view_can_go_back(handle) Then
					webkit_web_view_go_back(handle)
				End If
		End Select
	End Method

	Function checkURL:String(url:String, forMax:Int=False)
		Local	anchor$,a:Int
		a=url.Find("#")
		If a<>-1 Then
			anchor=url[a..]
			url=url[..a]
		End If
		If url[0..7]<>"http://" And url[0..7]<>"file://" Then
			If FileType(url) Then
				url = "file://" + url
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

	Function OnLoadStatus(widget:Byte Ptr, pspec:Byte Ptr, obj:Object)
		Local status:Int = webkit_web_view_get_load_status(widget)
		Select status
			Case WEBKIT_LOAD_FINISHED
				PostGuiEvent(EVENT_GADGETDONE, TGadget(obj))
		End Select
	End Function

	Method free()
		Super.free()
		
		If handle Then
			gtk_widget_destroy(handle)
		EndIf
		handle = Null

	End Method

End Type

Const WEBKIT_LOAD_PROVISIONAL:Int = 0
Const WEBKIT_LOAD_COMMITTED:Int = 1
Const WEBKIT_LOAD_FINISHED:Int = 2
Const WEBKIT_LOAD_FIRST_VISUALLY_NON_EMPTY_LAYOUT:Int = 3
Const WEBKIT_LOAD_FAILED:Int = 4

?
