' Copyright (c) 2008 Bruce A Henderson
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


Import "source.bmx"

Extern
	Function _strlen:Int(s:Byte Ptr) = "strlen"

	Function bmx_cegui_new_oglrenderer:Byte Ptr()
	Function bmx_cegui_new_system:Byte Ptr(renderer:Byte Ptr)
	Function bmx_cegui_delete_system(system:Byte Ptr)
	Function bmx_cegui_delete_renderer(renderer:Byte Ptr)
	
	Function bmx_cegui_schememanager_loadScheme:Byte Ptr(scheme:Byte Ptr, resourceGroup:Byte Ptr)
	Function bmx_cegui_schememanager_isschemepresent:Int(scheme:Byte Ptr)
	Function bmx_cegui_schememanager_getscheme:Byte Ptr(name:Byte Ptr)
	Function bmx_cegui_schememanager_unloadallschemes()
	
	Function bmx_cegui_system_setDefaultFont(handle:Byte Ptr, font:Byte Ptr)
	Function bmx_cegui_system_setDefaultMouseCursor(handle:Byte Ptr, look:Byte Ptr, kind:Byte Ptr)
	Function bmx_cegui_system_renderGUI(handle:Byte Ptr)
	Function bmx_cegui_system_setGUISheet(handle:Byte Ptr, window:Byte Ptr)
	Function bmx_cegui_system_injectTimePulse:Int(handle:Byte Ptr, time:Float)
	Function bmx_cegui_system_injectMousePosition:Int(handle:Byte Ptr, x:Int, y:Int)
	Function bmx_cegui_system_injectMouseButtonUp:Int(handle:Byte Ptr, button:Int)
	Function bmx_cegui_system_injectMouseButtonDown:Int(handle:Byte Ptr, button:Int)
	Function bmx_cegui_system_injectMouseWheelChange:Int(handle:Byte Ptr, delta:Int)
	Function bmx_cegui_system_injectkeydown:Int(handle:Byte Ptr, key:Int)
	Function bmx_cegui_system_injectkeyup:Int(handle:Byte Ptr, key:Int)
	Function bmx_cegui_system_injectchar:Int(handle:Byte Ptr, key:Int)
	Function bmx_cegui_system_getDefaultFont:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_system_signalredraw(handle:Byte Ptr)
	Function bmx_cegui_system_isredrawrequested:Int(handle:Byte Ptr)
	Function bmx_cegui_system_getsingleclicktimeout:Double(handle:Byte Ptr)
	Function bmx_cegui_system_getmulticlicktimeout:Double(handle:Byte Ptr)
	Function bmx_cegui_system_getmulticlicktoleranceareasize(handle:Byte Ptr, width:Float Ptr, height:Float Ptr)
	Function bmx_cegui_system_setsingleclicktimeout(handle:Byte Ptr, timeout:Double)
	Function bmx_cegui_system_setmulticlicktimeout(handle:Byte Ptr, timeout:Double)
	Function bmx_cegui_system_setmulticlicktoleranceareasize(handle:Byte Ptr, width:Float, height:Float)
	
	Function bmx_cegui_windowmanager_getsingleton:Byte Ptr()
	Function bmx_cegui_windowmanager_loadWindowLayout:Object(handle:Byte Ptr, filename:Byte Ptr, namePrefix:Byte Ptr, resourceGroup:Byte Ptr)
	Function bmx_cegui_windowmanager_getwindow:Object(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_windowmanager_createwindow:Object(handle:Byte Ptr, windowType:Byte Ptr, name:Byte Ptr, prefix:Byte Ptr)
	Function bmx_cegui_windowmanager_iswindowpresent:Int(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_windowmanager_destroyallwindows(handle:Byte Ptr)
	Function bmx_cegui_windowmanager_destroywindowwindow(handle:Byte Ptr, window:Byte Ptr)
	Function bmx_cegui_windowmanager_destroywindowname(handle:Byte Ptr, window:Byte Ptr)
	Function bmx_cegui_windowmanager_renamewindowwindow(handle:Byte Ptr, window:Byte Ptr, newName:Byte Ptr)
	Function bmx_cegui_windowmanager_renamewindowname(handle:Byte Ptr, window:Byte Ptr, newName:Byte Ptr)

	'Function bmx_cegui_eventhandler_new:Byte Ptr()
	'Function bmx_cegui_eventhandler_delete(handle:Byte Ptr)
	Function bmx_cegui_eventcallback_new:Byte Ptr(handle:Object)

	Function bmx_cegui_eventargs_delete(handle:Byte Ptr)

	Function bmx_cegui_eventset_subscribeevent:Byte Ptr(handle:Byte Ptr, name:Byte Ptr, cb:Byte Ptr)

	Function bmx_cegui_window_settext(handle:Byte Ptr, text:Byte Ptr)
	Function bmx_cegui_window_deactivate(handle:Byte Ptr)
	Function bmx_cegui_window_setposition(handle:Byte Ptr, x:Float, y:Float)
	Function bmx_cegui_window_setxposition(handle:Byte Ptr, x:Float)
	Function bmx_cegui_window_setyposition(handle:Byte Ptr, y:Float)
	Function bmx_cegui_window_setsize(handle:Byte Ptr, width:Float, height:Float)
	Function bmx_cegui_window_setwidth(handle:Byte Ptr, width:Float)
	Function bmx_cegui_window_setheight(handle:Byte Ptr, height:Float)
	Function bmx_cegui_window_setmaxsize(handle:Byte Ptr, width:Float, height:Float)
	Function bmx_cegui_window_setminsize(handle:Byte Ptr, width:Float, height:Float)
	Function bmx_cegui_window_addchildwindowwindow(handle:Byte Ptr, window:Byte Ptr)
	Function bmx_cegui_window_addchildwindow(handle:Byte Ptr, window:Byte Ptr)
	Function bmx_cegui_window_hide(handle:Byte Ptr)
	Function bmx_cegui_window_show(handle:Byte Ptr)
	Function bmx_cegui_window_gettype:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_window_activate(handle:Byte Ptr)

	Function bmx_cegui_window_setproperty(handle:Byte Ptr, name:Byte Ptr, value:Byte Ptr)

	Function bmx_cegui_connection_delete(handle:Byte Ptr)

	Function bmx_cegui_checkbox_isselected:Int(handle:Byte Ptr)
	Function bmx_cegui_checkbox_setselected(handle:Byte Ptr, selected:Int)

	Function bmx_cegui_progressbar_getprogress:Float(handle:Byte Ptr)
	Function bmx_cegui_progressbar_getstep:Float(handle:Byte Ptr)
	Function bmx_cegui_progressbar_setprogress(handle:Byte Ptr, progress:Float)
	Function bmx_cegui_progressbar_setstepsize(handle:Byte Ptr, stepVal:Float)
	Function bmx_cegui_progressbar_dostep(handle:Byte Ptr)
	Function bmx_cegui_progressbar_adjustprogress(handle:Byte Ptr, delta:Float)

	Function bmx_cegui_editbox_hasinputfocus:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_isreadonly:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_istextmasked:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_istextvalid:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_getvalidationstring:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_editbox_getcaratindex:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_getselectionstartindex:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_getselectionendindex:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_getselectionlength:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_getmaxtextlength:Int(handle:Byte Ptr)
	Function bmx_cegui_editbox_setreadonly(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_editbox_settextmasked(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_editbox_setvalidationstring(handle:Byte Ptr, validationString:Byte Ptr)
	Function bmx_cegui_editbox_setcaratindex(handle:Byte Ptr, caratPos:Int)
	Function bmx_cegui_editbox_setselection(handle:Byte Ptr, startPos:Int, endPos:Int)
	Function bmx_cegui_editbox_setmaxtextlength(handle:Byte Ptr, maxLen:Int)

	Function bmx_cegui_logger_setlogginglevel(level:Int)
	Function bmx_cegui_logger_setlogfilename(filename:Byte Ptr, append:Int)
	Function bmx_cegui_logger_logevent(message:Byte Ptr, level:Int)
	Function bmx_cegui_customlogger_create:Byte Ptr(handle:Object)
	Function bmx_cegui_customlogger_delete(handle:Byte Ptr)
	Function bmx_cegui_logger_getlogginglevel:Int()

	Function bmx_cegui_eventargs_gethandled:Int(handle:Byte Ptr)
	Function bmx_cegui_windoweventargs_getwindow:Object(handle:Byte Ptr)
	Function bmx_cegui_activationeventargs_getotherwindow:Object(handle:Byte Ptr)
	Function bmx_cegui_updateeventargs_gettimesincelastframe:Float(handle:Byte Ptr)

	Function bmx_cegui_framewindow_issizingenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_framewindow_isframeenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_framewindow_istitlebarenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_framewindow_isclosebuttonenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_framewindow_isrollupenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_framewindow_isrolledup:Int(handle:Byte Ptr)
	Function bmx_cegui_framewindow_getsizingborderthickness:Float(handle:Byte Ptr)
	Function bmx_cegui_framewindow_setsizingenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_framewindow_setframeenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_framewindow_settitlebarenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_framewindow_setclosebuttonenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_framewindow_rollupenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_framewindow_togglerollup(handle:Byte Ptr)
	Function bmx_cegui_framewindow_setsizingborderthickness(handle:Byte Ptr, pixels:Float)
	Function bmx_cegui_framewindow_isdragmovingenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_framewindow_setdragmovingenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_framewindow_ishit:Int(handle:Byte Ptr, x:Float, y:Float)
	Function bmx_cegui_framewindow_gettitlebar:Object(handle:Byte Ptr)
	Function bmx_cegui_framewindow_getclosebutton:Object(handle:Byte Ptr)

	Function bmx_cegui_titlebar_isdraggingenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_titlebar_setdraggingenabled(handle:Byte Ptr, setting:Int)

	Function bmx_cegui_buttonbase_ishovering:Int(handle:Byte Ptr)
	Function bmx_cegui_buttonbase_ispushed:Int(handle:Byte Ptr)

	Function bmx_cegui_radiobutton_isselected:Int(handle:Byte Ptr)
	Function bmx_cegui_radiobutton_getgroupid:Int(handle:Byte Ptr)
	Function bmx_cegui_radiobutton_getselectedbuttoningroup:Object(handle:Byte Ptr)
	Function bmx_cegui_radiobutton_setselected(handle:Byte Ptr, selected:Int)
	Function bmx_cegui_radiobutton_setgroupid(handle:Byte Ptr, group:Int)

	Function bmx_cegui_combodroplist_setarmed(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_combodroplist_isarmed:Int(handle:Byte Ptr)
	Function bmx_cegui_combodroplist_setautoarmenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_combodroplist_isautoarmenabled:Int(handle:Byte Ptr)

	Function bmx_cegui_multilineeditbox_hasinputfocus:Int(handle:Byte Ptr)
	Function bmx_cegui_multilineeditbox_isreadonly:Int(handle:Byte Ptr)
	Function bmx_cegui_multilineeditbox_getcaratindex:Int(handle:Byte Ptr)
	Function bmx_cegui_multilineeditbox_getselectionstartindex:Int(handle:Byte Ptr)
	Function bmx_cegui_multilineeditbox_getselectionendindex:Int(handle:Byte Ptr)
	Function bmx_cegui_multilineeditbox_getselectionlength:Int(handle:Byte Ptr)
	Function bmx_cegui_multilineeditbox_getmaxtextlength:Int(handle:Byte Ptr)
	Function bmx_cegui_multilineeditbox_iswordwrapped:Int(handle:Byte Ptr)
	Function bmx_cegui_multilineeditbox_getvertscrollbar:Object(handle:Byte Ptr)
	Function bmx_cegui_multilineeditbox_isvertscrollbaralwaysshown:Int(handle:Byte Ptr)
	Function bmx_cegui_multilineeditbox_gethorzscrollbar:Object(handle:Byte Ptr)
	Function bmx_cegui_multilineeditbox_gettextrenderarea(handle:Byte Ptr, x:Float Ptr, y:Float Ptr, w:Float Ptr, h:Float Ptr)
	Function bmx_cegui_multilineeditbox_getlinenumberfromindex:Int(handle:Byte Ptr, index:Int)
	Function bmx_cegui_multilineeditbox_setreadonly(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_multilineeditbox_setcaratindex(handle:Byte Ptr, caratPos:Int)
	Function bmx_cegui_multilineeditbox_setselection(handle:Byte Ptr, startPos:Int, endPos:Int)
	Function bmx_cegui_multilineeditbox_setmaxtextlength(handle:Byte Ptr, maxLen:Int)
	Function bmx_cegui_multilineeditbox_ensurecaratisvisible(handle:Byte Ptr)
	Function bmx_cegui_multilineeditbox_setwordwrapping(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_multilineeditbox_setshowvertscrollbar(handle:Byte Ptr, setting:Int)

	Function bmx_cegui_scrolleditemlistbase_isvertscrollbaralwaysshown:Int(handle:Byte Ptr)
	Function bmx_cegui_scrolleditemlistbase_ishorzscrollbaralwaysshown:Int(handle:Byte Ptr)
	Function bmx_cegui_scrolleditemlistbase_getvertscrollbar:Object(handle:Byte Ptr)
	Function bmx_cegui_scrolleditemlistbase_gethorzscrollbar:Object(handle:Byte Ptr)
	Function bmx_cegui_scrolleditemlistbase_setshowvertscrollbar(handle:Byte Ptr, mode:Int)
	Function bmx_cegui_scrolleditemlistbase_setshowhorzscrollbar(handle:Byte Ptr, mode:Int)

	Function bmx_cegui_itemlistbox_getselectedcount:Int(handle:Byte Ptr)
	Function bmx_cegui_itemlistbox_getlastselecteditem:Object(handle:Byte Ptr)
	Function bmx_cegui_itemlistbox_getfirstselecteditem:Object(handle:Byte Ptr, startIndex:Int)
	Function bmx_cegui_itemlistbox_getnextselecteditem:Object(handle:Byte Ptr)
	Function bmx_cegui_itemlistbox_getnextselecteditemafter:Object(handle:Byte Ptr, startItem:Byte Ptr)
	Function bmx_cegui_itemlistbox_ismultiselectenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_itemlistbox_isitemselected:Int(handle:Byte Ptr, index:Int)
	Function bmx_cegui_itemlistbox_setmultiselectenabled(handle:Byte Ptr, state:Int)
	Function bmx_cegui_itemlistbox_clearallselections(handle:Byte Ptr)
	Function bmx_cegui_itemlistbox_selectrange(handle:Byte Ptr, a:Int, z:Int)
	Function bmx_cegui_itemlistbox_selectallitems(handle:Byte Ptr)

	Function bmx_cegui_imagesetmanager_createimagesetfromimagefile:Byte Ptr(name:Byte Ptr, filename:Byte Ptr, resourceGroup:Byte Ptr)
	Function bmx_cegui_imagesetmanager_createimageset:Byte Ptr(filename:Byte Ptr, resourceGroup:Byte Ptr)
	Function bmx_cegui_imagesetmanager_createimagesetfromtexture:Byte Ptr(name:Byte Ptr, texture:Byte Ptr)
	Function bmx_cegui_imagesetmanager_destroyimageset(objectPtr:Byte Ptr)
	Function bmx_cegui_imagesetmanager_destroyimagesetname(imageset:Byte Ptr)
	Function bmx_cegui_imagesetmanager_destroyallimagesets()
	Function bmx_cegui_imagesetmanager_getimageset:Byte Ptr(name:Byte Ptr)
	Function bmx_cegui_imagesetmanager_isimagesetpresent:Int(name:Byte Ptr)
	Function bmx_cegui_imagesetmanager_notifyscreenresolution(width:Float, height:Float)

	Function bmx_cegui_fontmanager_createfont:Byte Ptr(filename:Byte Ptr, resourceGroup:Byte Ptr)
	Function bmx_cegui_fontmanager_isfontpresent:Int(name:Byte Ptr)

	Function bmx_cegui_spinner_getcurrentvalue:Float(handle:Byte Ptr)
	Function bmx_cegui_spinner_getstepsize:Float(handle:Byte Ptr)
	Function bmx_cegui_spinner_getmaximumvalue:Float(handle:Byte Ptr)
	Function bmx_cegui_spinner_getminimumvalue:Float(handle:Byte Ptr)
	Function bmx_cegui_spinner_gettextinputmode:Int(handle:Byte Ptr)
	Function bmx_cegui_spinner_setcurrentvalue(handle:Byte Ptr, value:Float)
	Function bmx_cegui_spinner_setstepsize(handle:Byte Ptr, stepSize:Float)
	Function bmx_cegui_spinner_setmaximumvalue(handle:Byte Ptr, maxValue:Float)
	Function bmx_cegui_spinner_setminimumvalue(handle:Byte Ptr, minValue:Float)
	Function bmx_cegui_spinner_settextinputmode(handle:Byte Ptr, mode:Int)

	Function bmx_cegui_scrollbar_getdocumentsize:Float(handle:Byte Ptr)
	Function bmx_cegui_scrollbar_getpagesize:Float(handle:Byte Ptr)
	Function bmx_cegui_scrollbar_getstepsize:Float(handle:Byte Ptr)
	Function bmx_cegui_scrollbar_getoverlapsize:Float(handle:Byte Ptr)
	Function bmx_cegui_scrollbar_getscrollposition:Float(handle:Byte Ptr)
	Function bmx_cegui_scrollbar_getincreasebutton:Object(handle:Byte Ptr)
	Function bmx_cegui_scrollbar_getdecreasebutton:Object(handle:Byte Ptr)
	Function bmx_cegui_scrollbar_setdocumentsize(handle:Byte Ptr, documentSize:Float)
	Function bmx_cegui_scrollbar_setpagesize(handle:Byte Ptr, pageSize:Float)
	Function bmx_cegui_scrollbar_setstepsize(handle:Byte Ptr, stepSize:Float)
	Function bmx_cegui_scrollbar_setoverlapsize(handle:Byte Ptr, overlapSize:Float)
	Function bmx_cegui_scrollbar_setscrollposition(handle:Byte Ptr, position:Float)

	Function bmx_cegui_slider_getcurrentvalue:Float(handle:Byte Ptr)
	Function bmx_cegui_slider_getmaxvalue:Float(handle:Byte Ptr)
	Function bmx_cegui_slider_getclickstep:Float(handle:Byte Ptr)
	Function bmx_cegui_slider_setmaxvalue(handle:Byte Ptr, maxVal:Float)
	Function bmx_cegui_slider_setcurrentvalue(handle:Byte Ptr, value:Float)
	Function bmx_cegui_slider_setclickstep(handle:Byte Ptr, clickStep:Float)

	Function bmx_cegui_tooltip_gettargetwindow:Object(handle:Byte Ptr)
	Function bmx_cegui_tooltip_gethovertime:Float(handle:Byte Ptr)
	Function bmx_cegui_tooltip_setdisplaytime(handle:Byte Ptr, seconds:Float)
	Function bmx_cegui_tooltip_getfadetime:Float(handle:Byte Ptr)
	Function bmx_cegui_tooltip_sethovertime(handle:Byte Ptr, seconds:Float)
	Function bmx_cegui_tooltip_getdisplaytime:Float(handle:Byte Ptr)
	Function bmx_cegui_tooltip_setfadetime(handle:Byte Ptr, seconds:Float)
	Function bmx_cegui_tooltip_positionself(handle:Byte Ptr)
	Function bmx_cegui_tooltip_sizeself(handle:Byte Ptr)
	Function bmx_cegui_tooltip_gettextsize(handle:Byte Ptr, width:Float Ptr, height:Float Ptr)

	Function bmx_cegui_tabcontrol_gettabcount:Int(handle:Byte Ptr)
	Function bmx_cegui_tabcontrol_gettabpaneposition:Int(handle:Byte Ptr)
	Function bmx_cegui_tabcontrol_settabpaneposition(handle:Byte Ptr, pos:Int)
	Function bmx_cegui_tabcontrol_setselectedtab(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_tabcontrol_setselectedtabforid(handle:Byte Ptr, ID:Int)
	Function bmx_cegui_tabcontrol_setselectedtabatindex(handle:Byte Ptr, index:Int)
	Function bmx_cegui_tabcontrol_maketabvisible(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_tabcontrol_maketabvisibleforid(handle:Byte Ptr, ID:Int)
	Function bmx_cegui_tabcontrol_maketabvisibleatindex(handle:Byte Ptr, index:Int)
	Function bmx_cegui_tabcontrol_gettabcontentsatindex:Object(handle:Byte Ptr, index:Int)
	Function bmx_cegui_tabcontrol_gettabcontents:Object(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_tabcontrol_gettabcontentsforid:Object(handle:Byte Ptr, ID:Int)
	Function bmx_cegui_tabcontrol_istabcontentsselected:Int(handle:Byte Ptr, wnd:Byte Ptr)
	Function bmx_cegui_tabcontrol_getselectedtabindex:Int(handle:Byte Ptr)
	Function bmx_cegui_tabcontrol_gettabheight:Float(handle:Byte Ptr)
	Function bmx_cegui_tabcontrol_gettabtextpadding:Float(handle:Byte Ptr)
	Function bmx_cegui_tabcontrol_settabheight(handle:Byte Ptr, height:Float)
	Function bmx_cegui_tabcontrol_settabtextpadding(handle:Byte Ptr, padding:Float)
	Function bmx_cegui_tabcontrol_addtab(handle:Byte Ptr, wnd:Byte Ptr)
	Function bmx_cegui_tabcontrol_removetab(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_tabcontrol_removetabforid(handle:Byte Ptr, ID:Int)

	Function bmx_cegui_renderer_createtexture:Object(handle:Byte Ptr, filename:Byte Ptr, resourceGroup:Byte Ptr)
	Function bmx_cegui_renderer_createtexturewithsize:Object(handle:Byte Ptr, size:Float)
	Function bmx_cegui_renderer_destroytexture(handle:Byte Ptr, texture:Byte Ptr)
	Function bmx_cegui_renderer_destroyalltextures(handle:Byte Ptr)
	Function bmx_cegui_renderer_isqueueingenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_renderer_getwidth:Float(handle:Byte Ptr)
	Function bmx_cegui_renderer_getheight:Float(handle:Byte Ptr)
	Function bmx_cegui_renderer_getsize(handle:Byte Ptr, width:Float Ptr, height:Float Ptr)
	Function bmx_cegui_renderer_getmaxtexturesize:Int(handle:Byte Ptr)
	Function bmx_cegui_renderer_gethorzscreendpi:Int(handle:Byte Ptr)
	Function bmx_cegui_renderer_getvertscreendpi:Int(handle:Byte Ptr)
	Function bmx_cegui_renderer_resetzvalue(handle:Byte Ptr)
	Function bmx_cegui_renderer_advancezvalue(handle:Byte Ptr)
	Function bmx_cegui_renderer_getcurrentz:Float(handle:Byte Ptr)
	Function bmx_cegui_renderer_getzlayer:Float(handle:Byte Ptr, layer:Int)
	Function bmx_cegui_renderer_getidentifierstring:Byte Ptr(handle:Byte Ptr)

	Function bmx_cegui_texture_getwidth:Int(handle:Byte Ptr)
	Function bmx_cegui_texture_getoriginalwidth:Int(handle:Byte Ptr)
	Function bmx_cegui_texture_getxscale:Float(handle:Byte Ptr)
	Function bmx_cegui_texture_getheight:Int(handle:Byte Ptr)
	Function bmx_cegui_texture_getoriginalheight:Int(handle:Byte Ptr)
	Function bmx_cegui_texture_getyscale:Float(handle:Byte Ptr)
	Function bmx_cegui_texture_loadfromfile(handle:Byte Ptr, filename:Byte Ptr, resourceGroup:Byte Ptr)
	Function bmx_cegui_texture_loadfrommemory(handle:Byte Ptr, buffer:Byte Ptr, width:Int, height:Int, pixelFormat:Int)

	Function bmx_cegui_scheme_loadresources(handle:Byte Ptr)
	Function bmx_cegui_scheme_unloadresources(handle:Byte Ptr)
	Function bmx_cegui_scheme_resourcesloaded:Int(handle:Byte Ptr)
	Function bmx_cegui_scheme_getname:Byte Ptr(handle:Byte Ptr)

	Function bmx_cegui_listboxitem_gettext:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_listboxitem_gettooltiptext:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_listboxitem_getid:Int(handle:Byte Ptr)
	Function bmx_cegui_listboxitem_isselected:Int(handle:Byte Ptr)
	Function bmx_cegui_listboxitem_isdisabled:Int(handle:Byte Ptr)
	Function bmx_cegui_listboxitem_isautodeleted:Int(handle:Byte Ptr)
	Function bmx_cegui_listboxitem_getownerwindow:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_listboxitem_settext(handle:Byte Ptr, text:Byte Ptr)
	Function bmx_cegui_listboxitem_settooltiptext(handle:Byte Ptr, text:Byte Ptr)
	Function bmx_cegui_listboxitem_setid(handle:Byte Ptr, itemId:Int)
	Function bmx_cegui_listboxitem_setselected(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_listboxitem_setdisabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_listboxitem_setautodeleted(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_listboxitem_setownerwindow(handle:Byte Ptr, owner:Byte Ptr)

End Extern

' converts a UTF character array from byte-size characters to short-size characters
' based on the TextStream UTF code...
Function _convertUTF8ToMax:String(s:Byte Ptr)
	If s Then
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
	End If
	
	Return ""
	
End Function

' converts a Max short-based String to a byte-based UTF-8 String.
' based on the TextStream UTF code...
Function _convertMaxToUTF8:String(text:String)
	If Not text Then
		Return ""
	End If
	
	Local l:Int = text.length
	If l = 0 Then
		Return ""
	End If
	
	Local count:Int = 0
	Local s:Byte[] = New Byte[l * 3] ' max possible is 3 x original size.
	
	For Local i:Int = 0 Until l
		Local char:Int = text[i]

		If char < 128 Then
			s[count] = char
			count:+ 1
			Continue
		Else If char<2048
			s[count] = char/64 | 192
			count:+ 1
			s[count] = char Mod 64 | 128
			count:+ 1
			Continue
		Else
			s[count] =  char/4096 | 224
			count:+ 1
			s[count] = char/64 Mod 64 | 128
			count:+ 1
			s[count] = char Mod 64 | 128
			count:+ 1
			Continue
		EndIf
		
	Next

	Return String.fromBytes(s, count)
End Function

Rem
bbdoc: Log Errors.
about: Only actual error conditions will be logged.
End Rem
Const LOG_ERRORS:Int = 0
Rem
bbdoc: Log Warnings.
about: Warnings will be logged as well.
End Rem
Const LOG_WARNINGS:Int = 1
Rem
bbdoc: Log Standard.
about: Basic events will be logged (Default level).
End Rem
Const LOG_STANDARD:Int = 2
Rem
bbdoc: Log informative.
about: Useful tracing (Object creations etc) information will be logged.
End Rem
Const LOG_INFORMATIVE:Int = 3
Rem
bbdoc: Log Insane.
about: Mostly everything gets logged (use For heavy tracing only, Log WILL be big).
End Rem
Const LOG_INSANE:Int = 4


