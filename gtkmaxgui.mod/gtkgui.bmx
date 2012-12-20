' Copyright (c) 2006-2009 Bruce A Henderson
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

Import "gtkcommon.bmx"
Import "gtkgadget.bmx"


Global GTKDriver:TGTKGUIDriver =New TGTKGUIDriver

Type TGTKGuiSystemDriver Extends TGTKSystemDriver
	Field	gui:TGTKGUIDriver
	
	Method Poll()
		While gtk_events_pending()
			gtk_main_iteration_do(False)
			bbSystemPoll()
		Wend
	End Method
		
	Method Wait()
		gtk_main_iteration_do(True)
	End Method
			
	Function Create:TGTKGuiSystemDriver(host:TGTKGUIDriver)
		Local	guisystem:TGTKGuiSystemDriver		
		guisystem=New TGTKGuiSystemDriver
		guisystem.gui=host
		Return guisystem
	End Function
	
End Type

Type TGTKSystemDriver Extends TSystemDriver

	Field NativeDriver:TSystemDriver

	Method New()
		NativeDriver=brl.System.Driver
	End Method
	
	Method Poll()
		NativeDriver.Poll()
	End Method
		
	Method Wait()
		NativeDriver.Wait()
	End Method
	
	Method Emit( osevent:Byte Ptr,source:Object )
		Throw "oops"
	End Method

	Method IsFullScreen:Int()
		Return False
	End Method	

	Method SetMouseVisible(bool:Int)
		NativeDriver.SetMouseVisible bool
	End Method

	Method MoveMouse( x:Int, y:Int )
		NativeDriver.MoveMouse x,y
	End Method

	Rem
	internal: Notify user.
	End Rem
	Method Notify( text:String, serious:Int )
		If text = Null Then
			text = ""
		End If
		
		serious = Max(0,Min(serious, 1))

		Local req:Byte Ptr = gtk_message_dialog_new(Null, GTK_DIALOG_MODAL, serious, ..
			GTK_BUTTONS_OK, gtkCheckAndConvert(text))

		Local res:Int = gtk_dialog_run(req)

		gtk_widget_destroy(req)
	End Method
	
	Rem
	internal: Request user confirmation.
	End Rem
	Method Confirm:Int( text:String, serious:Int )
		If text = Null Then
			text = ""
		End If
		
		serious = Max(0,Min(serious, 1))

		Local req:Byte Ptr = gtk_message_dialog_new(Null, GTK_DIALOG_MODAL, serious, ..
			GTK_BUTTONS_YES_NO, gtkCheckAndConvert(text))

		Local res:Int = gtk_dialog_run(req)

		gtk_widget_destroy(req)

		If res = GTK_RESPONSE_YES Then
			Return True
		Else
			Return False
		End If
	End Method
	
	Rem
	internal: Request user confirmation or cancellation.
	End Rem
	Method Proceed:Int( text:String, serious:Int )
		If text = Null Then
			text = ""
		End If
		
		serious = Max(0,Min(serious, 1))
		If Not serious Then
			serious = GTK_MESSAGE_QUESTION
		End If

		Local req:Byte Ptr = gtk_message_dialog_new(Null, GTK_DIALOG_MODAL, serious, ..
			GTK_BUTTONS_YES_NO, gtkCheckAndConvert(text))

		' add a cancel button
		gtk_dialog_add_button(req, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL)

		Local res:Int = gtk_dialog_run(req)

		gtk_widget_destroy(req)

		Select res
			Case GTK_RESPONSE_YES
				Return True
			Case GTK_RESPONSE_NO
				Return False
			Case GTK_RESPONSE_CANCEL
				Return -1
		End Select
	End Method

	Method RequestFile:String( text:String,exts:String, Save:Int,file:String )
		Local req:Byte Ptr
		Local f:String = Null
		Local opensave:Int
		Local reqButton:String

		If Save Then
			opensave = GTK_FILE_CHOOSER_ACTION_SAVE
			reqButton = GTK_STOCK_SAVE
		Else
			opensave = GTK_FILE_CHOOSER_ACTION_OPEN
			reqButton = GTK_STOCK_OPEN
		End If

		If text = Null Or text.length = 0 Then
			req = gtk_file_chooser_dialog_new(Null, Null, ..
				opensave, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, ..
				reqButton, GTK_RESPONSE_ACCEPT, Null)
		Else
			req = gtk_file_chooser_dialog_new(gtkCheckAndConvert(text), Null, ..
				opensave, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, ..
				reqButton, GTK_RESPONSE_ACCEPT, Null)
		End If

		Local Current:String = CurrentDir()
		gtk_file_chooser_set_current_folder(req, gtkCheckAndConvert(Current))

		' set the path if there was one.
		If file <> Null And file.length > 0 Then
			If Not save And Not FileType(file) Then
				file = ""
			End If
			gtk_file_chooser_set_filename(req, gtkCheckAndConvert(file))
		End If

		' set up filters, if any
		Rem
		The optional extensions string can either be a comma separated list of file extensions
		or as in the following example groups of extensions that begin with a "group:" and separated
		by a semicolon.
		End Rem

		If exts Then

			Local groups:String[] = _stringSplit(exts, ";")

			For Local group:Int = 0 Until groups.length

				If groups[group].length > 0 Then

					Local pairs:String[] = _stringSplit(groups[group], ":")
	
					Local name:String = Null
					Local ex:String[]

					If pairs.length = 1 Then
						ex = _stringSplit(pairs[0], ",")
					Else
						name = pairs[0]
						ex = _stringSplit(pairs[1], ",")
					End If

					' add a filter
					Local filter:Byte Ptr = gtk_file_filter_new()
					
					If name <> Null Then
						gtk_file_filter_set_name(filter, gtkCheckAndConvert(name))
					End If

					For Local i:Int = 0 Until ex.length
						Local s:String = ex[i].Trim()

						If s <> "*" Then
							s = "*." + s
						End If

						gtk_file_filter_add_pattern(filter, gtkCheckAndConvert(s))

					Next

					gtk_file_chooser_add_filter(req, filter)
					' we don't need to clean up the filter object, as gtk takes ownership of it when
					' we add it to the chooser.

				End If

			Next

		End If

		Local res:Int = gtk_dialog_run(req)

		If res = GTK_RESPONSE_ACCEPT Then
			f = String.FromCString(gtk_file_chooser_get_filename(req))
		End If

		gtk_widget_destroy(req)

		Return f
	End Method
	
	Method RequestDir:String( text:String, path:String )
		Local p:String = Null
		Local req:Byte Ptr

		If text = Null Or text.length = 0 Then
			req = gtk_file_chooser_dialog_new(Null, Null, ..
				GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, ..
				GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, Null)
		Else
			req = gtk_file_chooser_dialog_new(gtkCheckAndConvert(text), Null, ..
				GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, ..
				GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, Null)
		End If

		' set the path if there was one.
		If path <> Null And path.length > 0 Then
			' just in case the path is bad.. we'll set it to current
			If Not FileType(path) Then
				path = CurrentDir()
			End If
		Else
			path = CurrentDir()
		End If

		gtk_file_chooser_set_current_folder(req, gtkCheckAndConvert(path))
		gtk_file_chooser_set_filename(req, gtkCheckAndConvert(path))


		Local res:Int = gtk_dialog_run(req)

		If res = GTK_RESPONSE_ACCEPT Then
			p = String.FromCString(gtk_file_chooser_get_filename(req))
		End If

		gtk_widget_destroy(req)

		Return p
	End Method

	Method OpenURL:Int( url:String )
		Return NativeDriver.OpenURL(url)
	End Method
End Type


Type TGTKGUIDriver Extends TMaxGUIDriver

	Method New()
		brl.System.Driver=TGTKGuiSystemDriver.Create(Self)
		gtk_init(Null, Null)
		maxgui_driver=Self
	End Method

	Method LoadFont:TGuiFont(name:String, size:Int, flags:Int)
		Local font:TGuiFont = New TGTKGuiFont
		font.name = name
		font.size = size
		font.style = flags
		
		Return DoLoadFont(font)

	End Method
	
	Method DoLoadFont:TGuiFont(font:TGuiFont)

		Local widget:Byte Ptr = gtk_label_new(Null)
		Local _context:Byte Ptr  = gtk_widget_get_pango_context(widget)

		Local fontdesc:Byte Ptr = getPangoDescriptionFromGuiFont(font)
		Local _fontset:Byte Ptr = pango_context_load_fontset(_context, fontdesc, Null)

		pango_fontset_foreach(_fontset, fontforeach, font)

		pango_font_description_free(fontdesc)

		gtk_widget_destroy(widget)
		g_object_unref(_context)

		If font.path = "OK!" Then
			font.path = ""
			Return font
		Else
			Select font.name
				Case "Lucida"
					' No Lucida? Try FreeSerif...
					font.name = "FreeSerif"
					font = DoLoadFont(font)
				Case "FreeSerif"
					Return Null
				Default ' try a default...
					font.name = "Lucida"
					font = DoLoadFont(font)
			End Select

			If font Then
				Return font
			End If
		
			Return Null
		End If

	End Method


	Method LoadFontWithDouble:TGuiFont(name:String, size:Double, flags:Int)
		Local font:TGuiFont = New TGTKGuiFont
		font.name = name
		font.size = size
		font.style = flags
		
		Return DoLoadFont(font)
	End Method

	Function fontforeach:Int(fontset:Byte Ptr, _font:Byte Ptr, data:Object)
		Local fontdesc:Byte Ptr = pango_font_describe(_font)
		Local thisfont:TGuiFont = getGuiFontFromPangoDescription(fontdesc)
		pango_font_description_free(fontdesc)

		If thisfont.name.toLower().find(TGuiFont(data).name.tolower()) >= 0  Then
			TGuiFont(data).name = thisfont.name

			TGuiFont(data).path = "OK!"
			Return True
		End If

	End Function

	Method CreateGadget:TGadget(GadgetClass:Int, name:String, x:Int, y:Int, w:Int, h:Int,group:TGadget, style:Int)
		Local gadget:TGTKGadget
		Local gtkclass:Int = -1
		Local gtkGroup:TGTKGadget

		Select GadgetClass
			Case GADGET_DESKTOP
				Return TGTKDesktop.CreateDesktop()
			Case GADGET_WINDOW
				gtkclass = GTK_WINDOW
			Case GADGET_BUTTON
				gtkclass = GTK_BUTTON
				Select style
					Case BUTTON_CHECKBOX
						gtkclass = GTK_CHECKBUTTON
					Case BUTTON_RADIO
						gtkclass = GTK_RADIOBUTTON
					Case BUTTON_OK
					Case BUTTON_CANCEL
				End Select
			Case GADGET_PANEL
				gtkclass = GTK_PANEL
			Case GADGET_TEXTFIELD
				gtkclass = GTK_TEXTFIELD
			Case GADGET_TEXTAREA
				gtkclass = GTK_TEXTAREA
			Case GADGET_COMBOBOX
				gtkclass = GTK_COMBOBOX
			Case GADGET_LISTBOX
				gtkclass = GTK_LISTBOX
			Case GADGET_TOOLBAR
				gtkclass = GTK_TOOLBAR
			Case GADGET_TABBER
				gtkclass = GTK_TABBER
			Case GADGET_TREEVIEW
				gtkclass = GTK_TREEVIEW
			Case GADGET_HTMLVIEW
				gtkclass = GTK_HTMLVIEW
			Case GADGET_LABEL
				gtkclass = GTK_LABEL
			Case GADGET_SLIDER
				If style & SLIDER_STEPPER Then
					gtkclass = GTK_STEPPER
				Else If style & SLIDER_TRACKBAR Then
					gtkclass = GTK_TRACKBAR
				Else
					gtkclass = GTK_SCROLLBAR
				End If
			Case GADGET_PROGBAR
				gtkclass = GTK_PROGRESSBAR
			Case GADGET_MENUITEM
				gtkclass = GTK_MENUITEM
			Case GADGET_CANVAS
				gtkclass = GTK_CANVAS
		End Select
		gtkgroup = TGTKGadget(group)

		If gtkclass > -1 Then
			gadget = TGTKGadget.Create(gtkclass, x, y, w, h, name, gtkgroup, style, GadgetClass)
		End If

		Return gadget

	End Method

	Rem
	internal: Returns the currently active gadget.
	End Rem
	Method ActiveGadget:TGadget()
		Local handle:Int
		For Local w:TGTKWindow = EachIn gtkWindows
			' get the focussed widget for the window
			Local widget:Byte Ptr = gtk_window_get_focus(w.handle)
			
			If widget Then ' we need a gadget to test!
				' but is this window currently in focus (belonging to the toplevel window?)
				Local _gflags:Int = Int Ptr(widget + _OFFSET_GTK_FLAGS)[0]
				If (1 & (_gflags Shr 12)) Then
					handle = Int Ptr(widget)[0]
				End If
	
				If handle Then
					Return GadgetFromHandle(handle)
				End If
			End If
		Next
	End Method

	Rem
	internal: Pops up a color requester
	about: FIXME: Seem to have rounding errors somewhere....
	End Rem
	Method RequestColor:Int(r:Int, g:Int, b:Int)
		Local argb:Int = Null

		' populate the color object, ensuring values in valid range, then scale up
		Local color:TGDKColor = New TGDKColor
		color.red = Max(0, Min(r, 255)) * 256
		color.green = Max(0, Min(g, 255)) * 256
		color.blue = Max(0, Min(b, 255)) * 256

		Local req:Byte Ptr = gtk_color_selection_dialog_new("Select color")
		Local colsel:Byte Ptr = Byte Ptr(Int Ptr(req + _OFFSET_GTK_DIALOG)[0])
		gtk_color_selection_set_current_color(colsel, color)

		Local res:Int = gtk_dialog_run(req)

		If res = GTK_RESPONSE_OK Then
			gtk_color_selection_get_current_color(colsel, color)
			' need to scale down and populate argb var
			argb = $ff000000 | (((color.red Shr 8)&255)Shl 16) | (((color.green Shr 8)&255)Shl 8) | ((color.blue Shr 8)&255)
		End If
		
		gtk_widget_destroy(req)

		Return argb
	End Method

	Rem
	internal: Pops up a font requester.
	End Rem
	Method RequestFont:TGuiFont(font:TGuiFont)
		Local req:Byte Ptr = gtk_font_selection_dialog_new("Choose font")
		If font Then
			Local fontdesc:Byte Ptr = getPangoDescriptionFromGuiFont(font)
			gtk_font_selection_dialog_set_font_name(req, pango_font_description_to_string(fontdesc))
			
			pango_font_description_free(fontdesc)
		End If

		Local res:Int = gtk_dialog_run(req)

		If res = GTK_RESPONSE_OK Then
			' the requestor returns a Pango font description... so we need to 
			Local fontdesc:Byte Ptr = pango_font_description_from_string(gtk_font_selection_dialog_get_font_name(req))

			font = getGuiFontFromPangoDescription(fontdesc)

			pango_font_description_free(fontdesc)
		End If

		gtk_widget_destroy(req)

		Return font
	End Method

	Method SetPointer(shape:Int)
		Local screen:Byte Ptr = gdk_screen_get_default()
		
		Local cursorType:Int
		Select shape
			'Case POINTER_DEFAULT
			'Case POINTER_ARROW
			Case POINTER_IBEAM
				cursorType = GDK_XTERM
			Case POINTER_WAIT
				cursorType = GDK_WATCH
			Case POINTER_CROSS
				cursorType = GDK_CROSSHAIR
			Case POINTER_UPARROW
				cursorType = GDK_CENTER_PTR
			Case POINTER_SIZENWSE
				cursorType = GDK_TOP_LEFT_CORNER
			Case POINTER_SIZENESW
				cursorType = GDK_TOP_RIGHT_CORNER
			Case POINTER_SIZEWE
				cursorType = GDK_SB_H_DOUBLE_ARROW
			Case POINTER_SIZENS
				cursorType = GDK_SB_V_DOUBLE_ARROW
			Case POINTER_SIZEALL
				cursorType = GDK_FLEUR
			Case POINTER_NO
				cursorType = GDK_PIRATE
			Case POINTER_HAND
				cursorType = GDK_HAND2
			Case POINTER_APPSTARTING
				cursorType = GDK_WATCH
			Case POINTER_HELP
				cursorType = GDK_QUESTION_ARROW
			Default
				cursorType = GDK_LEFT_PTR
		End Select
		
		Local cursor:Byte Ptr = gdk_cursor_new_for_display(gdk_screen_get_display(screen), cursorType)
		
		For Local window:TGTKWindow = EachIn gtkWindows
			If Byte Ptr(Int Ptr(window.handle + _OFFSET_GTK_WINDOW)[0]) Then
				gdk_window_set_cursor(Byte Ptr(Int Ptr(window.handle + _OFFSET_GTK_WINDOW)[0]), cursor)
			End If
		Next
	End Method

	Method LoadIconStrip:TIconStrip(source:Object)
		Return TGTKIconStrip.Create(source)
	End Method

	Method UserName$()
	End Method
	
	Method ComputerName$()
	End Method
	

End Type

Function GadgetFromHandle:TGTKGadget( handle:Int )
	Return TGTKGadget( GadgetMap.ValueForKey( TGTKInteger.Set(handle) ) )
End Function

Rem
bbdoc: Sets the tooltip text for the gadget.
about: Note: Until BRL add gadget tooltip functionality, consider this function API volatile.
<p>Parameters:
<ul>
<li><b>gadget</b> : the gadget to apply the tooltip</li>
<li><b>tip</b> : the tooltip text or Null to clear</li>
</ul>
</p>
End Rem
'Function SetGadgetToolTip(gadget:TGadget, tip:String)
'	TGTKGadget(gadget).setToolTip(tip)
'End Function

Rem
bbdoc: Sets the application icon.
about: Note: Until BRL add app icon functionality, consider this function API volatile.
End Rem
Function SetAppIcon(pix:TPixmap)
	If pix <> Null Then
		Local pixmap:TPixmap
		If pix.format <> PF_RGBA8888 Then
			pixmap = pix.convert( PF_RGBA8888 )
		Else
			pixmap = pix
		End If

		Local icon:Int = gdk_pixbuf_new_from_data(pixmap.pixels, GDK_COLORSPACE_RGB, True, 8, ..
					pixmap.width, pixmap.height, pixmap.Pitch, Null, Null)
		gtk_window_set_default_icon(icon)
	End If
End Function

Rem
bbdoc: Sets the button image using the specified pixmap.
about: Note: Until BRL add button image functionality, consider this function API volatile.
End Rem
Function SetButtonPixmap(gadget:TGadget, pixmap:TPixmap)
	If TGTKButtonPush(gadget) And pixmap Then
		TGTKButtonPush(gadget).setPixmap(pixmap)
	End If
End Function

Rem
bbdoc: Returns pointer coordinates relative to window.
End Rem
Function GetWindowPointerPos(window:TGadget, x:Int Var, y:Int Var)
	If TGTKWindow(window) Then
		gtk_widget_get_pointer(TGTKWindow(window).handle, x, y)
	End If
End Function

Rem
bbdoc: Returns pointer coordinates relative to the screen.
End Rem
Function GetScreenPointerPos(x:Int Var, y:Int Var)
	gdk_display_get_pointer(gdk_display_get_default(), Null, x, y, Null)
End Function
