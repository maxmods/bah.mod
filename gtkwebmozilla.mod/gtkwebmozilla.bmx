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
Strict

Rem
bbdoc: GTKMaxGUI Linux Mozilla Widget
End Rem
Module BaH.GTKWebMozilla

ModuleInfo "Version: 1.03"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2006-2008 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.03"
ModuleInfo "History: Fixed mem issue in GetText."
ModuleInfo "History: Improved MOZ_DIST_BIN checking."
ModuleInfo "History: libgtkembedmoz.so is now runtime linked."
ModuleInfo "History: 1.02"
ModuleInfo "History: Added Free() method."
ModuleInfo "History: 1.01"
ModuleInfo "History: Minor tweaks."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

ModuleInfo "LD_OPTS: -L."

Import BaH.GTKMaxGUI

?linux

Import "-ldl"


Private

Global _mozDLL:Byte Ptr=LoadMozilla()

Extern
	Function dlopen:Byte Ptr(dll:Byte Ptr, flag:Int)
	Function dlsym:Byte Ptr(dll:Byte Ptr, fname:Byte Ptr)
End Extern

Function Link:Byte Ptr(func:String)
	If _mozDLL Then
		Return dlsym(_mozDLL, func)
	End If
End Function

Function LoadMozilla:Byte Ptr()
	Return dlopen( "libgtkembedmoz.so", 2)
End Function

Public

Rem
bbdoc: 
End Rem
Function GTKWebMozillaInstalled:Int()
	If _mozDLL Then
		Return True
	End If
End Function

' function pointers
Global gtk_moz_embed_new:Byte Ptr()=Link("gtk_moz_embed_new")
Global gtk_moz_embed_set_comp_path(aPath:Byte Ptr)=Link("gtk_moz_embed_set_comp_path")
Global gtk_moz_embed_load_url(handle:Byte Ptr, url:Byte Ptr)=Link("gtk_moz_embed_load_url")
Global gtk_moz_embed_can_go_forward:Int(handle:Byte Ptr)=Link("gtk_moz_embed_can_go_forward")
Global gtk_moz_embed_can_go_back:Int(handle:Byte Ptr)=Link("gtk_moz_embed_can_go_back")
Global gtk_moz_embed_go_forward(handle:Byte Ptr)=Link("gtk_moz_embed_go_forward")
Global gtk_moz_embed_go_back(handle:Byte Ptr)=Link("gtk_moz_embed_go_back")
Global gtk_moz_embed_get_location:Byte Ptr(handle:Byte Ptr)=Link("gtk_moz_embed_get_location")
Global gtk_moz_embed_stop_load(handle:Byte Ptr)=Link("gtk_moz_embed_stop_load")


Global MOZ_DIST_BIN:String = getenv_("MOZ_DIST_BIN")
If Not MOZ_DIST_BIN Or MOZ_DIST_BIN.length = 0 Then
	' we'll try several of them...
	MOZ_DIST_BIN = "/usr/lib/xulrunner:/usr/local/seamonkey:/usr/lib/mozilla-firefox:/usr/lib/firefox:/usr/lib"
End If

Global GTKMozillaWeb:TGTKWebMozDriver = New TGTKWebMozDriver

Type TGTKWebMozDriver Extends TGTKWebDriver
	Method New()
		gtkmaxgui_htmlview = Self
	End Method
	
	Function CreateHTMLView:TGTKWebMozilla(x:Int, y:Int, w:Int, h:Int, label:String, group:TGadget, style:Int)
		Return TGTKWebMozilla.CreateHTMLView(x, y, w, h, label, group, style)
	End Function
End Type

Type TGTKWebMozilla Extends TGTKHTMLView

	Field noNavigate:Int
	Global compPathSet:Int = False
	Field ignoreOpen:Int


	Function CreateHTMLView:TGTKWebMozilla(x:Int, y:Int, w:Int, h:Int, label:String, group:TGadget, style:Int)
		Local this:TGTKWebMozilla = New TGTKWebMozilla

		this.initHTMLView(x, y, w, h, label, group, style)

		Return this
	End Function

	Method initHTMLView(x:Int, y:Int, w:Int, h:Int, label:String, group:TGadget, style:Int)

		If Not compPathSet Then
		
			' Let's try and find a valid mozilla-type dist directory...
			If MOZ_DIST_BIN Then
				Local parts:String[] = MOZ_DIST_BIN.Split(":")
				MOZ_DIST_BIN = ""
				For Local i:Int = 0 Until parts.length
					If FileType(parts[i]) Then
						MOZ_DIST_BIN = parts[i]
						Exit
					End If
				Next
			End If
			
			If Not MOZ_DIST_BIN Or Not FileType(MOZ_DIST_BIN) Or Not GTKWebMozillaInstalled() Then
				' we need some kind of widget...  an empty box
				handle = gtk_vbox_new(False, 0)
				Return
			End If
			gtk_moz_embed_set_comp_path(MOZ_DIST_BIN)
			compPathSet = True
		End If

		handle = gtk_moz_embed_new()

		init(GTK_HTMLVIEW, x, y, w, h, style)

		noNavigate = (style & HTMLVIEW_NONAVIGATE)

		gtk_layout_put(TGTKContainer(group).container, handle, x, y)
		gtk_widget_set_size_request(handle, w, Max(h,0))

		SetShow(True)
		
		g_signal_cb3(handle, "open_uri", OnOpenURI, Self, Destroy, 0)
		g_signal_cb2(handle, "net_stop", OnNetStop, Self, Destroy, 0)

	End Method
	
	Rem
	bbdoc: Stops loading of the current document.
	End Rem
	Method Stop()
		If compPathSet Then
			gtk_moz_embed_stop_load(handle)
		End If
	End Method

	Method SetText(url:String)
		If url <> Null Then
			If compPathSet Then
				ignoreOpen = True
				gtk_moz_embed_load_url(handle, checkURL(url))
			End If
		End If
	End Method
	
	Method GetText:String()
		Local ret:String
		If compPathSet Then
			Local s:Byte Ptr = gtk_moz_embed_get_location(handle)
			
			If s <> Null Then
				ret = gtkUTF8toISO8859(s)
				g_free(s)
			End If
		End If
		Return ret
	End Method

	Method Activate(cmd:Int)
		Super.Activate(cmd)

		If compPathSet Then
			Select cmd
				Case ACTIVATE_FORWARD
					If gtk_moz_embed_can_go_forward(handle) Then
						gtk_moz_embed_go_forward(handle)
					End If
				Case ACTIVATE_BACK
					If gtk_moz_embed_can_go_back(handle) Then
						gtk_moz_embed_go_back(handle)
					End If
			End Select
		End If
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

	Function OnOpenURI:Int(widget:Byte Ptr, uri:Byte Ptr, obj:Object)
		Local url:String = String.fromCString(uri)
		
		If Not TGTKWebMozilla(obj).ignoreOpen And TGTKWebMozilla(obj).noNavigate Then
			PostGuiEvent(EVENT_GADGETACTION, TGadget(obj),0,0,0,0,url)
			Return True
		End If

		TGTKWebMozilla(obj).ignoreOpen = False
		
		Return False
	End Function

	Function OnNetStop:Int(widget:Byte Ptr, obj:Object)
		PostGuiEvent(EVENT_GADGETDONE, TGadget(obj))
	End Function
	
	Method free()
		Super.free()
		
		If handle Then
			gtk_widget_destroy(handle)
		EndIf
		handle = Null

	End Method
Rem
	Function OnMouseDown:Int(widget:Byte Ptr, event:Byte Ptr, obj:Object)
		Local button:Int = Int Ptr(event + 40)[0]

		If button = 3 Then ' right mouse button
			Local x:Int = Double Ptr(event + 16)[0]
			Local y:Int = Double Ptr(event + 24)[0]

			PostGuiEvent(EVENT_GADGETMENU, TGadget(obj),,,x,y)

			Return True
		End If

		Return False
	End Function
End Rem
End Type

?