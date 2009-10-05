' Copyright (c) 2008-2009 Bruce A Henderson
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
	Function bmx_cegui_new_system:Byte Ptr(renderer:Byte Ptr, resourceProvider:Byte Ptr)
	Function bmx_cegui_system_destroy()
	Function bmx_cegui_delete_renderer(renderer:Byte Ptr)
	
	Function bmx_cegui_system_setdefaultfont(handle:Byte Ptr, font:Byte Ptr)
	Function bmx_cegui_system_setdefaultfonttxt(handle:Byte Ptr, font:Byte Ptr)
	Function bmx_cegui_system_setDefaultMouseCursor(handle:Byte Ptr, look:Byte Ptr, kind:Byte Ptr)
	Function bmx_cegui_system_renderGUI(handle:Byte Ptr)
	Function bmx_cegui_system_setGUISheet:Object(handle:Byte Ptr, window:Byte Ptr)
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
	Function bmx_cegui_system_setdefaulttooltip(handle:Byte Ptr, tooltip:Byte Ptr)
	Function bmx_cegui_system_setdefaulttooltiptext(handle:Byte Ptr, tooltip:Byte Ptr)
	Function bmx_cegui_system_getdefaulttooltip:Object(handle:Byte Ptr)
	Function bmx_cegui_system_getmousemovescaling:Float(handle:Byte Ptr)
	Function bmx_cegui_system_setmousemovescaling(handle:Byte Ptr, scaling:Float)
	Function bmx_cegui_system_getresourceprovider:Object(handle:Byte Ptr)

	Function bmx_cegui_windowmanager_getsingleton:Byte Ptr()
	Function bmx_cegui_windowmanager_loadWindowLayout:Object(handle:Byte Ptr, filename:Byte Ptr, namePrefix:Byte Ptr, resourceGroup:Byte Ptr)
	Function bmx_cegui_windowmanager_getwindow:Object(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_windowmanager_createwindow:Object(handle:Byte Ptr, windowType:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_windowmanager_iswindowpresent:Int(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_windowmanager_destroyallwindows(handle:Byte Ptr)
	Function bmx_cegui_windowmanager_destroywindowwindow(handle:Byte Ptr, window:Byte Ptr)
	Function bmx_cegui_windowmanager_destroywindowname(handle:Byte Ptr, window:Byte Ptr)
	Function bmx_cegui_windowmanager_renamewindowwindow(handle:Byte Ptr, window:Byte Ptr, newName:Byte Ptr)
	Function bmx_cegui_windowmanager_renamewindowname(handle:Byte Ptr, window:Byte Ptr, newName:Byte Ptr)
	Function bmx_cegui_windowmanager_isdeadpoolempty:Int(handle:Byte Ptr)
	Function bmx_cegui_windowmanager_cleandeadpool(handle:Byte Ptr)
	Function bmx_cegui_windowmanager_lock(handle:Byte Ptr)
	Function bmx_cegui_windowmanager_unlock(handle:Byte Ptr)
	Function bmx_cegui_windowmanager_islocked:Int(handle:Byte Ptr)
	Function bmx_cegui_windowmanager_setdefaultresourcegroup(resourceGroup:Byte Ptr)
	Function bmx_cegui_windowmanager_getdefaultresourcegroup:String()

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
	Function bmx_cegui_window_gettype:String(handle:Byte Ptr)
	Function bmx_cegui_window_activate(handle:Byte Ptr)

	Function bmx_cegui_window_getname:String(handle:Byte Ptr)
	Function bmx_cegui_window_getprefix:String(handle:Byte Ptr)
	Function bmx_cegui_window_isdestroyedbyparent:Int(handle:Byte Ptr)
	Function bmx_cegui_window_isalwaysontop:Int(handle:Byte Ptr)
	Function bmx_cegui_window_isdisabled:Int(handle:Byte Ptr, localOnly:Int)
	Function bmx_cegui_window_isvisible:Int(handle:Byte Ptr, localOnly:Int	)
	Function bmx_cegui_window_isactive:Int(handle:Byte Ptr)
	Function bmx_cegui_window_isclippedbyparent:Int(handle:Byte Ptr)
	Function bmx_cegui_window_getid:Int(handle:Byte Ptr)
	Function bmx_cegui_window_getchildcount:Int(handle:Byte Ptr)
	Function bmx_cegui_window_ischild:Int(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_window_ischildid:Int(handle:Byte Ptr, ID:Int)
	Function bmx_cegui_window_ischildrecursive:Int(handle:Byte Ptr, ID:Int)
	Function bmx_cegui_window_ischildwindow:Int(handle:Byte Ptr, window:Byte Ptr)
	Function bmx_cegui_window_getchild:Object(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_window_getchildid:Object(handle:Byte Ptr, ID:Int)
	Function bmx_cegui_window_getchildrecursive:Object(handle:Byte Ptr, ID:Int)
	Function bmx_cegui_window_getchildatidx:Object(handle:Byte Ptr, idx:Int)
	Function bmx_cegui_window_getactivechild:Object(handle:Byte Ptr)
	Function bmx_cegui_window_isancestor:Int(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_window_isancestorid:Int(handle:Byte Ptr, ID:Int)
	Function bmx_cegui_window_isancestorwindow:Int(handle:Byte Ptr, window:Byte Ptr)
	Function bmx_cegui_window_gettext:String(handle:Byte Ptr)
	Function bmx_cegui_window_inheritsalpha:Int(handle:Byte Ptr)
	Function bmx_cegui_window_getalpha:Float(handle:Byte Ptr)
	Function bmx_cegui_window_geteffectivealpha:Float(handle:Byte Ptr)
	Function bmx_cegui_window_getunclippedinnerrect(handle:Byte Ptr, x:Float Ptr, y:Float Ptr, w:Float Ptr, h:Float Ptr)
	Function bmx_cegui_window_iscapturedbythis:Int(handle:Byte Ptr)
	Function bmx_cegui_window_iscapturedbyancestor:Int(handle:Byte Ptr)
	Function bmx_cegui_window_iscapturedbychild:Int(handle:Byte Ptr)
	Function bmx_cegui_window_ishit:Int(handle:Byte Ptr, x:Float, y:Float, allowDisabled:Int)
	Function bmx_cegui_window_getchildatposition:Object(handle:Byte Ptr, x:Float, y:Float)
	Function bmx_cegui_window_gettargetchildatposition:Object(handle:Byte Ptr, x:Float, y:Float)
	Function bmx_cegui_window_getparent:Object(handle:Byte Ptr)
	Function bmx_cegui_window_getpixelsize(handle:Byte Ptr, width:Float Ptr, height:Float Ptr)
	Function bmx_cegui_window_restoresoldcapture:Int(handle:Byte Ptr)
	Function bmx_cegui_window_iszorderingenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_window_wantsmulticlickevents:Int(handle:Byte Ptr)
	Function bmx_cegui_window_ismouseautorepeatenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_window_getautorepeatdelay:Float(handle:Byte Ptr)
	Function bmx_cegui_window_getautorepeatrate:Float(handle:Byte Ptr)
	Function bmx_cegui_window_distributescapturedinputs:Int(handle:Byte Ptr)
	Function bmx_cegui_window_isusingdefaulttooltip:Int(handle:Byte Ptr)
	Function bmx_cegui_window_gettooltiptype:String(handle:Byte Ptr)
	Function bmx_cegui_window_gettooltiptext:String(handle:Byte Ptr)
	Function bmx_cegui_window_inheritstooltiptext:Int(handle:Byte Ptr)
	Function bmx_cegui_window_isriseonclickenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_window_testclassname:Int(handle:Byte Ptr, className:Byte Ptr)
	Function bmx_cegui_window_getverticalalignment:Int(handle:Byte Ptr)
	Function bmx_cegui_window_gethorizontalalignment:Int(handle:Byte Ptr)
	Function bmx_cegui_window_getlooknfeel:String(handle:Byte Ptr)
	Function bmx_cegui_window_getmodalstate:Int(handle:Byte Ptr)
	Function bmx_cegui_window_getuserstring:String(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_window_isuserstringdefined:Int(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_window_getparentpixelwidth:Float(handle:Byte Ptr)
	Function bmx_cegui_window_getparentpixelheight:Float(handle:Byte Ptr)
	Function bmx_cegui_window_ismousepassthroughenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_window_isautowindow:Int(handle:Byte Ptr)
	Function bmx_cegui_window_iswritingxmlallowed:Int(handle:Byte Ptr)
	Function bmx_cegui_window_isdragdroptarget:Int(handle:Byte Ptr)
	Function bmx_cegui_window_setclippedbyparent(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_window_setid(handle:Byte Ptr, ID:Int)
	Function bmx_cegui_window_inserttext(handle:Byte Ptr, text:Byte Ptr, position:Int)
	Function bmx_cegui_window_appendtext(handle:Byte Ptr, text:Byte Ptr)
	Function bmx_cegui_window_setfont(handle:Byte Ptr, font:Byte Ptr)
	Function bmx_cegui_window_setfontbyname(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_window_removechildwindow(handle:Byte Ptr, window:Byte Ptr)
	Function bmx_cegui_window_removechildwindowname(handle:Byte Ptr, window:Byte Ptr)
	Function bmx_cegui_window_removechildwindowforid(handle:Byte Ptr, ID:Int)
	Function bmx_cegui_window_movetofront(handle:Byte Ptr)
	Function bmx_cegui_window_movetoback(handle:Byte Ptr)
	Function bmx_cegui_window_captureinput:Int(handle:Byte Ptr)
	Function bmx_cegui_window_releaseinput(handle:Byte Ptr)
	Function bmx_cegui_window_setrestorecapture(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_window_setalpha(handle:Byte Ptr, alpha:Float)
	Function bmx_cegui_window_setinheritsalpha(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_window_invalidate(handle:Byte Ptr)
	Function bmx_cegui_window_invalidaterenderingsurface(handle:Byte Ptr)
	Function bmx_cegui_window_setmousecursor(handle:Byte Ptr, image:Byte Ptr)
	Function bmx_cegui_window_setmousecursormode(handle:Byte Ptr, image:Int)
	Function bmx_cegui_window_setmousecursorbyname(handle:Byte Ptr, imageset:Byte Ptr, imageName:Byte Ptr)
	Function bmx_cegui_window_setzorderingenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_window_setwantsmulticlickevents(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_window_setmouseautorepeateenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_window_setautorepeatdelay(handle:Byte Ptr, _delay:Float)
	Function bmx_cegui_window_setautorepeatrate(handle:Byte Ptr, rate:Float)
	Function bmx_cegui_window_setdistributescapturedinputs(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_window_notifydragdropitementers(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_window_notifydragdropitemleaves(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_window_notifydragdropitemdropped(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_window_destroy(handle:Byte Ptr)
	Function bmx_cegui_window_settooltip(handle:Byte Ptr, tooltip:Byte Ptr)
	Function bmx_cegui_window_settooltiptype(handle:Byte Ptr, tooltipType:Byte Ptr)
	Function bmx_cegui_window_settooltiptext(handle:Byte Ptr, tip:Byte Ptr)
	Function bmx_cegui_window_setinheritstooltiptext(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_window_setriseonclickenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_window_setverticalaligmnent(handle:Byte Ptr, alignment:Int)
	Function bmx_cegui_window_sethorizontalalignment(handle:Byte Ptr, alignment:Int)
	Function bmx_cegui_window_setlooknfeel(handle:Byte Ptr, look:Byte Ptr)
	Function bmx_cegui_window_setmodalstate(handle:Byte Ptr, state:Int)
	Function bmx_cegui_window_performchildwindowlayout(handle:Byte Ptr)
	Function bmx_cegui_window_setuserstring(handle:Byte Ptr, name:Byte Ptr, value:Byte Ptr)
	Function bmx_cegui_window_setpositionu(handle:Byte Ptr, x:Byte Ptr, y:Byte Ptr)
	Function bmx_cegui_window_setxpositionu(handle:Byte Ptr, x:Byte Ptr)
	Function bmx_cegui_window_setypositionu(handle:Byte Ptr, y:Byte Ptr)
	Function bmx_cegui_window_setsizeu(handle:Byte Ptr, width:Byte Ptr, height:Byte Ptr)
	Function bmx_cegui_window_setwidthu(handle:Byte Ptr, width:Byte Ptr)
	Function bmx_cegui_window_setheightu(handle:Byte Ptr, height:Byte Ptr)
	Function bmx_cegui_window_getmousecursor:Object(handle:Byte Ptr, useDefault:Int)
	Function bmx_cegui_window_rename(handle:Byte Ptr, newName:Byte Ptr)
	Function bmx_cegui_window_setdestroyedbyparent(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_window_setalwaysontop(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_window_setenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_window_enable(handle:Byte Ptr)
	Function bmx_cegui_window_disable(handle:Byte Ptr)
	Function bmx_cegui_window_setvisible(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_window_getxposition:Float(handle:Byte Ptr)
	Function bmx_cegui_window_getyposition:Float(handle:Byte Ptr)
	Function bmx_cegui_window_getwidth:Float(handle:Byte Ptr)
	Function bmx_cegui_window_getheight:Float(handle:Byte Ptr)
	Function bmx_cegui_window_render(handle:Byte Ptr)
	Function bmx_cegui_window_getsize(handle:Byte Ptr, w:Float Ptr, h:Float Ptr)
	Function bmx_cegui_window_getmaxsize(handle:Byte Ptr, w:Float Ptr, h:Float Ptr)
	Function bmx_cegui_window_getminsize(handle:Byte Ptr, w:Float Ptr, h:Float Ptr)
	Function bmx_cegui_window_setmousepassthroughenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_window_setwindowrenderer(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_window_getwindowrenderername:String(handle:Byte Ptr)
	Function bmx_cegui_window_setwritingxmlallowed(handle:Byte Ptr, allow:Int)
	Function bmx_cegui_window_notifyscreenareachanged(handle:Byte Ptr)
	Function bmx_cegui_window_setfalagardtype(handle:Byte Ptr, _type:Byte Ptr, rendererType:Byte Ptr)
	Function bmx_cegui_window_setdragdroptarget(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_window_getfont:Byte Ptr(handle:Byte Ptr, useDefault:Int)
	Function bmx_cegui_window_gettooltip:Object(handle:Byte Ptr)
	Function bmx_cegui_window_getactivesibling:Object(handle:Byte Ptr)
	Function bmx_cegui_window_getparentpixelsize(handle:Byte Ptr, width:Float Ptr, height:Float Ptr)
	Function bmx_cegui_window_setarearel(handle:Byte Ptr, xpos:Float, ypos:Float, width:Float, height:Float)

	Function bmx_cegui_window_setproperty(handle:Byte Ptr, name:Byte Ptr, value:Byte Ptr)
	Function bmx_cegui_window_removeproperty(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_window_clearproperties(handle:Byte Ptr)
	Function bmx_cegui_window_ispropertypresent:Int(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_window_getpropertyhelp:String(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_window_getproperty:String(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_window_ispropertydefault:Int(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_window_getpropertydefault:String(handle:Byte Ptr, name:Byte Ptr)

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
	Function bmx_cegui_editbox_getvalidationstring:String(handle:Byte Ptr)
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
	Function bmx_cegui_editbox_setmaskcodepoint(handle:Byte Ptr, codePoint:Int)

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
	Function bmx_cegui_framewindow_ishit:Int(handle:Byte Ptr, x:Float, y:Float, allowDisabled:Int)
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
	Function bmx_cegui_scrolleditemlistbase_setshowvertscrollbar(handle:Byte Ptr, Mode:Int)
	Function bmx_cegui_scrolleditemlistbase_setshowhorzscrollbar(handle:Byte Ptr, Mode:Int)

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

	Function bmx_cegui_imagesetmanager_createimagesetfromtexture:Byte Ptr(name:Byte Ptr, texture:Byte Ptr, action:Int)
	Function bmx_cegui_imagesetmanager_createimagesetfromimagefile:Byte Ptr(name:Byte Ptr, filename:Byte Ptr, resourceGroup:Byte Ptr, action:Int)
	Function bmx_cegui_imagesetmanager_notifydisplaysizechanged(width:Float, height:Float)
	Function bmx_cegui_imagesetmanager_createimageset:Object(filename:Byte Ptr, resourceGroup:Byte Ptr, action:Int)
	Function bmx_cegui_imagesetmanager_destroyname(imageset:Byte Ptr)
	Function bmx_cegui_imagesetmanager_destroyobj(imageset:Byte Ptr)
	Function bmx_cegui_imagesetmanager_destroyall()
	Function bmx_cegui_imagesetmanager_get:Object(name:Byte Ptr)
	Function bmx_cegui_imagesetmanager_isdefined:Int(name:Byte Ptr)
	Function bmx_cegui_imagesetmanager_createall(pattern:Byte Ptr, resourceGroup:Byte Ptr)

	Function bmx_cegui_fontmanager_createfont:Object(filename:Byte Ptr, resourceGroup:Byte Ptr, action:Int)
	Function bmx_cegui_fontmanager_destroyname(name:Byte Ptr)
	Function bmx_cegui_fontmanager_destroyobj(font:Byte Ptr)
	Function bmx_cegui_fontmanager_destroyall()
	Function bmx_cegui_fontmanager_get:Object(name:Byte Ptr)
	Function bmx_cegui_fontmanager_isdefined:Int(name:Byte Ptr)
	Function bmx_cegui_fontmanager_createall(pattern:Byte Ptr, resourceGroup:Byte Ptr)

	Function bmx_cegui_fontmanager_createfreetypefont:Object(font_name:Byte Ptr, pointSize:Float, antialiased:Int, fontFilename:Byte Ptr, resourceGroup:Byte Ptr, ..
			autoScaled:Int, nativeHorzRes:Float, nativeVertRes:Float, action:Int)
	Function bmx_cegui_fontmanager_createpixmapfont:Object(font_name:Byte Ptr, imagesetFilename:Byte Ptr, resourceGroup:Byte Ptr, autoScaled:Int, ..
			nativeHorzRes:Float, nativeVertRes:Float, action:Int)
	Function bmx_cegui_fontmanager_notifydislaysizechanged(width:Float, height:Float)

	Function bmx_cegui_spinner_getcurrentvalue:Float(handle:Byte Ptr)
	Function bmx_cegui_spinner_getstepsize:Float(handle:Byte Ptr)
	Function bmx_cegui_spinner_getmaximumvalue:Float(handle:Byte Ptr)
	Function bmx_cegui_spinner_getminimumvalue:Float(handle:Byte Ptr)
	Function bmx_cegui_spinner_gettextinputmode:Int(handle:Byte Ptr)
	Function bmx_cegui_spinner_setcurrentvalue(handle:Byte Ptr, value:Float)
	Function bmx_cegui_spinner_setstepsize(handle:Byte Ptr, stepSize:Float)
	Function bmx_cegui_spinner_setmaximumvalue(handle:Byte Ptr, maxValue:Float)
	Function bmx_cegui_spinner_setminimumvalue(handle:Byte Ptr, minValue:Float)
	Function bmx_cegui_spinner_settextinputmode(handle:Byte Ptr, Mode:Int)

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
	Function bmx_cegui_tabcontrol_settabheightu(handle:Byte Ptr, height:Byte Ptr)

	Function bmx_cegui_texture_getsize(handle:Byte Ptr, w:Int Ptr, h:Int Ptr)
	Function bmx_cegui_texture_getoriginaldatasize(handle:Byte Ptr, w:Int Ptr, h:Int Ptr)
	Function bmx_cegui_texture_loadfromfile(handle:Byte Ptr, filename:Byte Ptr, resourceGroup:Byte Ptr)
	Function bmx_cegui_texture_loadfrommemory(handle:Byte Ptr, buffer:Byte Ptr, width:Int, height:Int, pixelFormat:Int)
	Function bmx_cegui_texture_savetomemory(handle:Byte Ptr, buffer:Byte Ptr)

	Function bmx_cegui_scheme_loadresources(handle:Byte Ptr)
	Function bmx_cegui_scheme_unloadresources(handle:Byte Ptr)
	Function bmx_cegui_scheme_resourcesloaded:Int(handle:Byte Ptr)
	Function bmx_cegui_scheme_getname:String(handle:Byte Ptr)
	Function bmx_cegui_scheme_setdefaultresourcegroup(resourceGroup:Byte Ptr)
	Function bmx_cegui_scheme_getdefaultresourcegroup:String()

	Function bmx_cegui_listboxitem_gettext:String(handle:Byte Ptr)
	Function bmx_cegui_listboxitem_gettooltiptext:String(handle:Byte Ptr)
	Function bmx_cegui_listboxitem_getid:Int(handle:Byte Ptr)
	Function bmx_cegui_listboxitem_isselected:Int(handle:Byte Ptr)
	Function bmx_cegui_listboxitem_isdisabled:Int(handle:Byte Ptr)
	Function bmx_cegui_listboxitem_isautodeleted:Int(handle:Byte Ptr)
	Function bmx_cegui_listboxitem_getownerwindow:Object(handle:Byte Ptr)
	Function bmx_cegui_listboxitem_settext(handle:Byte Ptr, text:Byte Ptr)
	Function bmx_cegui_listboxitem_settooltiptext(handle:Byte Ptr, text:Byte Ptr)
	Function bmx_cegui_listboxitem_setid(handle:Byte Ptr, itemId:Int)
	Function bmx_cegui_listboxitem_setselected(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_listboxitem_setdisabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_listboxitem_setautodeleted(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_listboxitem_setownerwindow(handle:Byte Ptr, owner:Byte Ptr)
	Function bmx_cegui_listboxitem_setuserdata(handle:Byte Ptr, itemData:Object)
	Function bmx_cegui_listboxitem_getselectioncolours:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_listboxitem_getselectionbrushimage:Object(handle:Byte Ptr)
	Function bmx_cegui_listboxitem_setselectioncolours(handle:Byte Ptr, cols:Byte Ptr)
	Function bmx_cegui_listboxitem_setselectioncoloursforcorners(handle:Byte Ptr, topLeftColour:Byte Ptr, topRightColour:Byte Ptr, bottomLeftColour:Byte Ptr, bottomRightColour:Byte Ptr)
	Function bmx_cegui_listboxitem_setselectioncoloursforcolour(handle:Byte Ptr, col:Byte Ptr)
	Function bmx_cegui_listboxitem_setselectionbrushimage(handle:Byte Ptr, image:Byte Ptr)
	Function bmx_cegui_listboxitem_setselectionbrushimagebyname(handle:Byte Ptr, imageset:Byte Ptr, image:Byte Ptr)
	Function bmx_cegui_listboxitem_getpixelsize(handle:Byte Ptr, width:Float Ptr, height:Float Ptr)
	Function bmx_cegui_listboxitem_getuserdata:Object(handle:Byte Ptr)

	Function bmx_cegui_multicolumnlist_isusersortcontrolenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_isusercolumnsizingenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_isusercolumndraggingenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_getcolumncount:Int(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_getrowcount:Int(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_getsortcolumn:Int(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_getcolumnwithid:Int(handle:Byte Ptr, colId:Int)
	Function bmx_cegui_multicolumnlist_getcolumnwithheadertext:Int(handle:Byte Ptr, text:Byte Ptr)
	Function bmx_cegui_multicolumnlist_gettotalcolumnheaderswidth:Float(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_getcolumnheaderwidth:Float(handle:Byte Ptr, colIdx:Int)
	Function bmx_cegui_multicolumnlist_getsortdirection:Int(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_getheadersegmentforcolumn:Object(handle:Byte Ptr, colIdx:Int)
	Function bmx_cegui_multicolumnlist_getitemrowindex:Int(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_multicolumnlist_getitemcolumnindex:Int(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_multicolumnlist_getitemgridreference(handle:Byte Ptr, item:Byte Ptr, row:Int Ptr, col:Int Ptr)
	Function bmx_cegui_multicolumnlist_getitematgridreference:Object(handle:Byte Ptr, row:Int, col:Int)
	Function bmx_cegui_multicolumnlist_islistboxitemincolumn:Int(handle:Byte Ptr, item:Byte Ptr, colIdx:Int)
	Function bmx_cegui_multicolumnlist_islistboxiteminrow:Int(handle:Byte Ptr, item:Byte Ptr, rowIdx:Int)
	Function bmx_cegui_multicolumnlist_islistboxiteminlist:Int(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_multicolumnlist_findcolumnitemwithtext:Object(handle:Byte Ptr, text:Byte Ptr, colIdx:Int, startItem:Byte Ptr)
	Function bmx_cegui_multicolumnlist_findrowitemwithtext:Object(handle:Byte Ptr, text:Byte Ptr, rowIdx:Int, startItem:Byte Ptr)
	Function bmx_cegui_multicolumnlist_findlistitemwithtext:Object(handle:Byte Ptr, text:Byte Ptr, startItem:Byte Ptr)
	Function bmx_cegui_multicolumnlist_getfirstselecteditem:Object(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_getnextselected:Object(handle:Byte Ptr, startItem:Byte Ptr)
	Function bmx_cegui_multicolumnlist_getselectedcount:Int(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_isitemselected:Int(handle:Byte Ptr, row:Int, col:Int)
	Function bmx_cegui_multicolumnlist_getnominatedselectioncolumnid:Int(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_getnominatedselectioncolum:Int(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_getnominatedselectionrow:Int(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_getselectionmode:Int(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_isvertscrollbaralwaysshown:Int(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_ishorzscrollbaralwaysshown:Int(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_getcolumnid:Int(handle:Byte Ptr, colIdx:Int)
	Function bmx_cegui_multicolumnlist_getrowid:Int(handle:Byte Ptr, rowIdx:Int)
	Function bmx_cegui_multicolumnlist_getrowwithid:Int(handle:Byte Ptr, rowId:Int)
	Function bmx_cegui_multicolumnlist_getlistrenderarea(handle:Byte Ptr, x:Float Ptr, y:Float Ptr, w:Float Ptr, h:Float Ptr)
	Function bmx_cegui_multicolumnlist_getvertscrollbar:Object(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_gethorzscrollbar:Object(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_getlistheader:Object(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_gettotalrowsheight:Float(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_getwidestcolumnitemwidth:Float(handle:Byte Ptr, colIdx:Int)
	Function bmx_cegui_multicolumnlist_gethighestrowitemheight:Float(handle:Byte Ptr, rowIdx:Int)
	Function bmx_cegui_multicolumnlist_resetlist(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_addcolumn(handle:Byte Ptr, text:Byte Ptr, colId:Int, width:Float)
	Function bmx_cegui_multicolumnlist_addcolumnu(handle:Byte Ptr, text:Byte Ptr, colId:Int, width:Byte Ptr)
	Function bmx_cegui_multicolumnlist_insertcolumn(handle:Byte Ptr, text:Byte Ptr, colId:Int, width:Float, position:Int)
	Function bmx_cegui_multicolumnlist_insertcolumnu(handle:Byte Ptr, text:Byte Ptr, colId:Int, width:Byte Ptr, position:Int)
	Function bmx_cegui_multicolumnlist_removecolumn(handle:Byte Ptr, colIdx:Int)
	Function bmx_cegui_multicolumnlist_removecolumnwithid(handle:Byte Ptr, colId:Int)
	Function bmx_cegui_multicolumnlist_movecolumn(handle:Byte Ptr, colIdx:Int, position:Int)
	Function bmx_cegui_multicolumnlist_movecolumnwithid(handle:Byte Ptr, colId:Int, position:Int)
	Function bmx_cegui_multicolumnlist_addrow:Int(handle:Byte Ptr, rowId:Int)
	Function bmx_cegui_multicolumnlist_addrowitem:Int(handle:Byte Ptr, item:Byte Ptr, colId:Int, rowId:Int)
	Function bmx_cegui_multicolumnlist_insertrow:Int(handle:Byte Ptr, rowIdx:Int, rowId:Int)
	Function bmx_cegui_multicolumnlist_insertrowitem:Int(handle:Byte Ptr, item:Byte Ptr, colId:Int, rowIdx:Int, rowId:Int)
	Function bmx_cegui_multicolumnlist_removerow(handle:Byte Ptr, rowIdx:Int)
	Function bmx_cegui_multicolumnlist_setitemforgridref(handle:Byte Ptr, item:Byte Ptr, row:Int, col:Int)
	Function bmx_cegui_multicolumnlist_setitem(handle:Byte Ptr, item:Byte Ptr, colId:Int, rowIdx:Int)
	Function bmx_cegui_multicolumnlist_setselectionmode(handle:Byte Ptr, selMode:Int)
	Function bmx_cegui_multicolumnlist_setnominatedselectioncolumnid(handle:Byte Ptr, colId:Int)
	Function bmx_cegui_multicolumnlist_setnominatedselectioncolumn(handle:Byte Ptr, colIdx:Int)
	Function bmx_cegui_multicolumnlist_setnominatedselectionrow(handle:Byte Ptr, rowIdx:Int)
	Function bmx_cegui_multicolumnlist_setsortdirection(handle:Byte Ptr, direction:Int)
	Function bmx_cegui_multicolumnlist_setsortcolumn(handle:Byte Ptr, colIdx:Int)
	Function bmx_cegui_multicolumnlist_setsortcolumnbyid(handle:Byte Ptr, colId:Int)
	Function bmx_cegui_multicolumnlist_setshowvertscrollbar(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_multicolumnlist_setshowhorzscrollbar(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_multicolumnlist_clearallselections(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_setitemselectstate(handle:Byte Ptr, item:Byte Ptr, state:Int)
	Function bmx_cegui_multicolumnlist_setitemselectstateforgridref(handle:Byte Ptr, row:Int, col:Int, state:Int)
	Function bmx_cegui_multicolumnlist_handleupdateditemdata(handle:Byte Ptr)
	Function bmx_cegui_multicolumnlist_setcolumnheaderwidth(handle:Byte Ptr, colIdx:Int, width:Float)
	Function bmx_cegui_multicolumnlist_setusersortcontrolenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_multicolumnlist_setusercolumnsizingenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_multicolumnlist_setusercolumndraggingenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_multicolumnlist_autosizecolumnheader(handle:Byte Ptr, colIdx:Int)
	Function bmx_cegui_multicolumnlist_setrowid(handle:Byte Ptr, rowIdx:Int, rowId:Int)

	Function bmx_cegui_dragcontainer_isdraggingenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_dragcontainer_setdraggingenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_dragcontainer_isbeingdragged:Int(handle:Byte Ptr)
	Function bmx_cegui_dragcontainer_getpixeldragthreshold:Float(handle:Byte Ptr)
	Function bmx_cegui_dragcontainer_setpixeldragthreshold(handle:Byte Ptr, pixels:Float)
	Function bmx_cegui_dragcontainer_getdragalpha:Float(handle:Byte Ptr)
	Function bmx_cegui_dragcontainer_setdragalpha(handle:Byte Ptr, alpha:Float)
	Function bmx_cegui_dragcontainer_getdragcursorimage:Object(handle:Byte Ptr)
	Function bmx_cegui_dragcontainer_setdragcursorimage(handle:Byte Ptr, image:Byte Ptr)
	Function bmx_cegui_dragcontainer_setdragcursorimagemode(handle:Byte Ptr, image:Int)
	Function bmx_cegui_dragcontainer_setdragcursorimagebyname(handle:Byte Ptr, imageset:Byte Ptr, image:Byte Ptr)
	Function bmx_cegui_dragcontainer_getcurrentdroptarget:Object(handle:Byte Ptr)

	Function bmx_cegui_colour_create:Byte Ptr(red:Float, green:Float, blue:Float, alpha:Float)
	Function bmx_cegui_colour_delete(handle:Byte Ptr)
	Function bmx_cegui_colour_getargb:Int(handle:Byte Ptr)
	Function bmx_cegui_colour_getalpha:Float(handle:Byte Ptr)
	Function bmx_cegui_colour_getred:Float(handle:Byte Ptr)
	Function bmx_cegui_colour_getgreen:Float(handle:Byte Ptr)
	Function bmx_cegui_colour_getblue:Float(handle:Byte Ptr)
	Function bmx_cegui_colour_gethue:Float(handle:Byte Ptr)
	Function bmx_cegui_colour_getsaturation:Float(handle:Byte Ptr)
	Function bmx_cegui_colour_getlumination:Float(handle:Byte Ptr)
	Function bmx_cegui_colour_setargb(handle:Byte Ptr, argb:Int)
	Function bmx_cegui_colour_setalpha(handle:Byte Ptr, alpha:Float)
	Function bmx_cegui_colour_setred(handle:Byte Ptr, red:Float)
	Function bmx_cegui_colour_setgreen(handle:Byte Ptr, green:Float)
	Function bmx_cegui_colour_setblue(handle:Byte Ptr, blue:Float)
	Function bmx_cegui_colour_set(handle:Byte Ptr, red:Float, green:Float, blue:Float, alpha:Float)
	Function bmx_cegui_colour_setrgb(handle:Byte Ptr, red:Float, green:Float, blue:Float)
	Function bmx_cegui_colour_sethsl(handle:Byte Ptr, hue:Float, saturation:Float, luminance:Float, alpha:Float)
	Function bmx_cegui_colour_inverColour(handle:Byte Ptr)
	Function bmx_cegui_colour_invertColourWithAlpha(handle:Byte Ptr)


	Function bmx_cegui_colourrect_create:Byte Ptr(colour:Byte Ptr)
	Function bmx_cegui_colourrect_setalpha(handle:Byte Ptr, alpha:Float)
	Function bmx_cegui_colourrect_settopalpha(handle:Byte Ptr, alpha:Float)
	Function bmx_cegui_colourrect_setbottomalpha(handle:Byte Ptr, alpha:Float)
	Function bmx_cegui_colourrect_setleftalpha(handle:Byte Ptr, alpha:Float)
	Function bmx_cegui_colourrect_setrightalpha(handle:Byte Ptr, alpha:Float)
	Function bmx_cegui_colourrect_ismonochromatic:Int(handle:Byte Ptr)
	Function bmx_cegui_colourrect_getsubrectangle:Byte Ptr(handle:Byte Ptr, _left:Float, _right:Float, _top:Float, _bottom:Float)
	Function bmx_cegui_colourrect_getcolouratpoint:Byte Ptr(handle:Byte Ptr, x:Float, y:Float)
	Function bmx_cegui_colourrect_setcolours(handle:Byte Ptr, col:Byte Ptr)
	Function bmx_cegui_colourrect_modulatealpha(handle:Byte Ptr, alpha:Float)
	Function bmx_cegui_colourrect_settopleft(handle:Byte Ptr, colour:Byte Ptr)
	Function bmx_cegui_colourrect_settopright(handle:Byte Ptr, colour:Byte Ptr)
	Function bmx_cegui_colourrect_setbottomleft(handle:Byte Ptr, colour:Byte Ptr)
	Function bmx_cegui_colourrect_setbottomright(handle:Byte Ptr, colour:Byte Ptr)
	Function bmx_cegui_colourrect_gettopleft:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_colourrect_gettopright:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_colourrect_getbottomleft:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_colourrect_getbottomright:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_colourrect_delete(handle:Byte Ptr)

	Function bmx_cegui_menubase_getitemspacing:Float(handle:Byte Ptr)
	Function bmx_cegui_menubase_ismultiplepopupsallowed:Int(handle:Byte Ptr)
	Function bmx_cegui_menubase_getpopupmenuitem:Object(handle:Byte Ptr)
	Function bmx_cegui_menubase_setitemspacing(handle:Byte Ptr, spacing:Float)
	Function bmx_cegui_menubase_changepopupmenuitem(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_menubase_setallowmultiplepopups(handle:Byte Ptr, setting:Int)

	Function bmx_cegui_popupmenu_getfadeintime:Float(handle:Byte Ptr)
	Function bmx_cegui_popupmenu_getfadeouttime:Float(handle:Byte Ptr)
	Function bmx_cegui_popupmenu_ispopupmenuopen:Int(handle:Byte Ptr)
	Function bmx_cegui_popupmenu_setfadeintime(handle:Byte Ptr, fadetime:Float)
	Function bmx_cegui_popupmenu_setfadeouttime(handle:Byte Ptr, fadetime:Float)
	Function bmx_cegui_popupmenu_openpopupmenu(handle:Byte Ptr, _notify:Int)
	Function bmx_cegui_popupmenu_closepopupmenu(handle:Byte Ptr, _notify:Int)

	Function bmx_cegui_itementry_getitempixelsize(handle:Byte Ptr, w:Float Ptr, h:Float Ptr)
	Function bmx_cegui_itementry_getownerlist:Object(handle:Byte Ptr)
	Function bmx_cegui_itementry_isselected:Int(handle:Byte Ptr)
	Function bmx_cegui_itementry_isselectable:Int(handle:Byte Ptr)
	Function bmx_cegui_itementry_setselected(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_itementry_selectentry(handle:Byte Ptr)
	Function bmx_cegui_itementry_deselect(handle:Byte Ptr)
	Function bmx_cegui_itementry_setselectable(handle:Byte Ptr, setting:Int)

	Function bmx_cegui_menuitem_ishovering:Int(handle:Byte Ptr)
	Function bmx_cegui_menuitem_ispushed:Int(handle:Byte Ptr)
	Function bmx_cegui_menuitem_isopened:Int(handle:Byte Ptr)
	Function bmx_cegui_menuitem_getpopupmenu:Object(handle:Byte Ptr)
	Function bmx_cegui_menuitem_setpopupmenu(handle:Byte Ptr, popup:Byte Ptr)
	Function bmx_cegui_menuitem_openpopupmenu(handle:Byte Ptr, _notify:Int)
	Function bmx_cegui_menuitem_closepopupmenu(handle:Byte Ptr, _notify:Int)
	Function bmx_cegui_menuitem_togglepopupmenu:Int(handle:Byte Ptr)

	Function bmx_cegui_treeitem_settextcolours(handle:Byte Ptr, cols:Byte Ptr)
	Function bmx_cegui_treeitem_settextcoloursforcorners(handle:Byte Ptr, topLeftColour:Byte Ptr, topRightColour:Byte Ptr, bottomLeftColour:Byte Ptr, bottomRightColour:Byte Ptr)
	Function bmx_cegui_treeitem_settextcoloursforcolour(handle:Byte Ptr, col:Byte Ptr)
	Function bmx_cegui_treeitem_gettext:String(handle:Byte Ptr)
	Function bmx_cegui_treeitem_gettooltiptext:String(handle:Byte Ptr)
	Function bmx_cegui_treeitem_getid:Int(handle:Byte Ptr)
	Function bmx_cegui_treeitem_getuserdata:Object(handle:Byte Ptr)
	Function bmx_cegui_treeitem_isselected:Int(handle:Byte Ptr)
	Function bmx_cegui_treeitem_isdisabled:Int(handle:Byte Ptr)
	Function bmx_cegui_treeitem_isautodeleted:Int(handle:Byte Ptr)
	Function bmx_cegui_treeitem_getownerwindow:Object(handle:Byte Ptr)
	Function bmx_cegui_treeitem_getselectioncolours:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_treeitem_getselectionbrushimage:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_treeitem_settext(handle:Byte Ptr, text:Byte Ptr)
	Function bmx_cegui_treeitem_settooltiptext(handle:Byte Ptr, text:Byte Ptr)
	Function bmx_cegui_treeitem_setid(handle:Byte Ptr, itemId:Int)
	Function bmx_cegui_treeitem_setuserdata(handle:Byte Ptr, itemData:Object)
	Function bmx_cegui_treeitem_setselected(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_treeitem_setdisabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_treeitem_setautodeleted(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_treeitem_setownerwindow(handle:Byte Ptr, owner:Byte Ptr)
	Function bmx_cegui_treeitem_setselectioncolours(handle:Byte Ptr, col:Byte Ptr)
	Function bmx_cegui_treeitem_setselectioncoloursforcorners(handle:Byte Ptr, topLeftColour:Byte Ptr, topRightColour:Byte Ptr, bottomLeftColour:Byte Ptr, bottomRightColour:Byte Ptr)
	Function bmx_cegui_treeitem_setselectioncoloursforcolour(handle:Byte Ptr, col:Byte Ptr)
	Function bmx_cegui_treeitem_setselectionbrushimage(handle:Byte Ptr, image:Byte Ptr)
	Function bmx_cegui_treeitem_setselectionbrushimagebyname(handle:Byte Ptr, imageset:Byte Ptr, image:Byte Ptr)
	Function bmx_cegui_treeitem_setbuttonlocation(handle:Byte Ptr, x:Float, y:Float, w:Float, h:Float)
	Function bmx_cegui_treeitem_getbuttonlocation(handle:Byte Ptr, x:Float Ptr, y:Float Ptr, w:Float Ptr, h:Float Ptr)
	Function bmx_cegui_treeitem_getisopen:Int(handle:Byte Ptr)
	Function bmx_cegui_treeitem_toggleisopen(handle:Byte Ptr)
	Function bmx_cegui_treeitem_gettreeitemfromindex:Object(handle:Byte Ptr, itemIndex:Int)
	Function bmx_cegui_treeitem_getitemcount:Int(handle:Byte Ptr)
	Function bmx_cegui_treeitem_additem(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_treeitem_seticon(handle:Byte Ptr, theIcon:Byte Ptr)
	Function bmx_cegui_treeitem_getpixelsize(handle:Byte Ptr, width:Float Ptr, height:Float Ptr)

	Function bmx_cegui_listboxtextitem_getfont:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_listboxtextitem_gettextcolours:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_listboxtextitem_setfont(handle:Byte Ptr, font:Byte Ptr)
	Function bmx_cegui_listboxtextitem_setfontbyname(handle:Byte Ptr, FontName:Byte Ptr)
	Function bmx_cegui_listboxtextitem_settextcolours(handle:Byte Ptr, cols:Byte Ptr)
	Function bmx_cegui_listboxtextitem_settextcoloursforcorner(handle:Byte Ptr, topLeftColour:Byte Ptr, topRightColour:Byte Ptr, bottomLeftColour:Byte Ptr, bottomRightColour:Byte Ptr)
	Function bmx_cegui_listboxtextitem_settextcoloursforcolour(handle:Byte Ptr, col:Byte Ptr)
	Function bmx_cegui_listboxtextitem_getpixelsize(handle:Byte Ptr, width:Float Ptr, height:Float Ptr)
	Function bmx_cegui_listboxtextitem_new:Byte Ptr(text:Byte Ptr, itemId:Int, disabled:Int, autoDelete:Int)
	Function bmx_cegui_listboxtextitem_delete(handle:Byte Ptr)

	Function bmx_cegui_scrollablepane_getcontentpane:Object(handle:Byte Ptr)
	Function bmx_cegui_scrollablepane_isvertscrollbaralwaysshown:Int(handle:Byte Ptr)
	Function bmx_cegui_scrollablepane_setshowvertscrollbar(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_scrollablepane_ishorzscrollbaralwaysshown:Int(handle:Byte Ptr)
	Function bmx_cegui_scrollablepane_setshowhorzscrollbar(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_scrollablepane_iscontentpaneautosized:Int(handle:Byte Ptr)
	Function bmx_cegui_scrollablepane_setcontentpaneautosized(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_scrollablepane_getcontentpanearea(handle:Byte Ptr, x:Float Ptr, y:Float Ptr, w:Float Ptr, h:Float Ptr)
	Function bmx_cegui_scrollablepane_setcontentpanearea(handle:Byte Ptr, x:Float, y:Float, w:Float, h:Float)
	Function bmx_cegui_scrollablepane_gethorizontalstepsize:Float(handle:Byte Ptr)
	Function bmx_cegui_scrollablepane_sethorizontalstepsize(handle:Byte Ptr, stepSize:Float)
	Function bmx_cegui_scrollablepane_gethorizontaloverlapsize:Float(handle:Byte Ptr)
	Function bmx_cegui_scrollablepane_sethorizontaloverlapsize(handle:Byte Ptr, overlap:Float)
	Function bmx_cegui_scrollablepane_gethorizontalscrollposition:Float(handle:Byte Ptr)
	Function bmx_cegui_scrollablepane_sethorizontalscrollposition(handle:Byte Ptr, position:Float)
	Function bmx_cegui_scrollablepane_getverticalstepsize:Float(handle:Byte Ptr)
	Function bmx_cegui_scrollablepane_setverticalstepsize(handle:Byte Ptr, stepSize:Float)
	Function bmx_cegui_scrollablepane_getverticaloverlapsize:Float(handle:Byte Ptr)
	Function bmx_cegui_scrollablepane_setverticaloverlapsize(handle:Byte Ptr, overlap:Float)
	Function bmx_cegui_scrollablepane_getverticalscrollposition:Float(handle:Byte Ptr)
	Function bmx_cegui_scrollablepane_setverticalscrollposition(handle:Byte Ptr, position:Float)
	Function bmx_cegui_scrollablepane_getviewablearea(handle:Byte Ptr, x:Float Ptr, y:Float Ptr, w:Float Ptr, h:Float Ptr)
	Function bmx_cegui_scrollablepane_getvertscrollbar:Object(handle:Byte Ptr)
	Function bmx_cegui_scrollablepane_gethorzscrollbar:Object(handle:Byte Ptr)

	Function bmx_cegui_tree_getitemcount:Int(handle:Byte Ptr)
	Function bmx_cegui_tree_getselectedcount:Int(handle:Byte Ptr)
	Function bmx_cegui_tree_getfirstselecteditem:Object(handle:Byte Ptr)
	Function bmx_cegui_tree_getlastselecteditem:Object(handle:Byte Ptr)
	Function bmx_cegui_tree_getnextselected:Object(handle:Byte Ptr, startItem:Byte Ptr)
	Function bmx_cegui_tree_issortenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_tree_setitemrenderarea(handle:Byte Ptr, x:Float, y:Float, w:Float, h:Float)
	Function bmx_cegui_tree_getvertscrollbar:Object(handle:Byte Ptr)
	Function bmx_cegui_tree_gethorzscrollbar:Object(handle:Byte Ptr)
	Function bmx_cegui_tree_ismultiselectenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_tree_isitemtooltipsenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_tree_findfirstitemwithtext:Object(handle:Byte Ptr, text:Byte Ptr)
	Function bmx_cegui_tree_findnextitemwithtext:Object(handle:Byte Ptr, text:Byte Ptr, startItem:Byte Ptr)
	Function bmx_cegui_tree_findfirstitemwithid:Object(handle:Byte Ptr, searchID:Int)
	Function bmx_cegui_tree_findnextitemwithid:Object(handle:Byte Ptr, searchID:Int, startItem:Byte Ptr)
	Function bmx_cegui_tree_istreeiteminlist:Int(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_tree_isvertscrollbaralwaysshown:Int(handle:Byte Ptr)
	Function bmx_cegui_tree_ishorzscrollbaralwaysshown:Int(handle:Byte Ptr)
	Function bmx_cegui_tree_resetlist(handle:Byte Ptr)
	Function bmx_cegui_tree_additem(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_tree_insertitem(handle:Byte Ptr, item:Byte Ptr, position:Byte Ptr)
	Function bmx_cegui_tree_removeitem(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_tree_clearallselections(handle:Byte Ptr)
	Function bmx_cegui_tree_setsortingenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_tree_setmultiselectenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_tree_setshowvertscrollbar(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_tree_setshowhorzscrollbar(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_tree_setitemtooltipsenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_tree_setitemselectstate(handle:Byte Ptr, item:Byte Ptr, state:Int)
	Function bmx_cegui_tree_setitemselectstateindex(handle:Byte Ptr, itemIndex:Int, state:Int)
	Function bmx_cegui_tree_setlooknfeel(handle:Byte Ptr, look:Byte Ptr)
	Function bmx_cegui_tree_handleupdateditemdata(handle:Byte Ptr)
	Function bmx_cegui_tree_ensureitemisvisible(handle:Byte Ptr, item:Byte Ptr)

	Function bmx_cegui_tabbutton_setselected(handle:Byte Ptr, selected:Int)
	Function bmx_cegui_tabbutton_isselected:Int(handle:Byte Ptr)
	Function bmx_cegui_tabbutton_settargetwindow(handle:Byte Ptr, wnd:Byte Ptr)
	Function bmx_cegui_tabbutton_gettargetwindow:Object(handle:Byte Ptr)

	Function bmx_cegui_resourceprovider_create:Byte Ptr(handle:Object)
	Function bmx_cegui_resourceprovider_delete(handle:Byte Ptr)
	Function bmx_cegui_rawdatacontainer_delete(handle:Byte Ptr)
	Function bmx_cegui_resourceprovider_getdefaultresourcegroup:String(handle:Byte Ptr)
	Function bmx_cegui_resourceprovider_setdefaultresourcegroup(handle:Byte Ptr, group:Byte Ptr)

	Function bmx_cegui_defaultresourceprovider_setresourcegroupdirectory(handle:Byte Ptr, resourceGroup:Byte Ptr, directory:Byte Ptr)
	Function bmx_cegui_defaultresourceprovider_getresourcegroupdirectory:String(handle:Byte Ptr, resourceGroup:Byte Ptr)
	Function bmx_cegui_defaultresourceprovider_clearresourcegroupdirectory(handle:Byte Ptr, resourceGroup:Byte Ptr)

	Function bmx_cegui_rawdatacontainer_setdata(handle:Byte Ptr, data:Byte Ptr)
	Function bmx_cegui_rawdatacontainer_getdataptr:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_rawdatacontainer_setsize(handle:Byte Ptr, size:Int)
	Function bmx_cegui_rawdatacontainer_getsize:Int(handle:Byte Ptr)

	Function bmx_cegui_listbox_getitemcount:Int(handle:Byte Ptr)
	Function bmx_cegui_listbox_getselectedcount:Int(handle:Byte Ptr)
	Function bmx_cegui_listbox_getfirstselecteditem:Object(handle:Byte Ptr)
	Function bmx_cegui_listbox_getnextselected:Object(handle:Byte Ptr, startItem:Byte Ptr)
	Function bmx_cegui_listbox_getlistboxitemfromindex:Object(handle:Byte Ptr, index:Int)
	Function bmx_cegui_listbox_getitemindex:Int(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_listbox_issortenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_listbox_ismultiselectenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_listbox_isitemtooltipsenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_listbox_isitemselected:Int(handle:Byte Ptr, index:Int)
	Function bmx_cegui_listbox_finditemwithtext:Object(handle:Byte Ptr, text:Byte Ptr, startItem:Byte Ptr)
	Function bmx_cegui_listbox_islistboxiteminlist:Int(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_listbox_isvertscrollbaralwaysshown:Int(handle:Byte Ptr)
	Function bmx_cegui_listbox_ishorzscrollbaralwaysshown:Int(handle:Byte Ptr)
	Function bmx_cegui_listbox_resetlist(handle:Byte Ptr)
	Function bmx_cegui_listbox_additem(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_listbox_insertitem(handle:Byte Ptr, item:Byte Ptr, position:Byte Ptr)
	Function bmx_cegui_listbox_removeitem(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_listbox_clearallselections(handle:Byte Ptr)
	Function bmx_cegui_listbox_setsortingenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_listbox_setmultiselectenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_listbox_setshowvertscrollbar(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_listbox_setshowhorzscrollbar(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_listbox_setitemtooltipsenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_listbox_setitemselectstate(handle:Byte Ptr, item:Byte Ptr, state:Int)
	Function bmx_cegui_listbox_setitemselectstateforindex(handle:Byte Ptr, itemIndex:Int, state:Int)
	Function bmx_cegui_listbox_handleupdateditemdata(handle:Byte Ptr)
	Function bmx_cegui_listbox_ensureitemisvisibleforindex(handle:Byte Ptr, itemIndex:Int)
	Function bmx_cegui_listbox_ensureitemisvisible(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_listbox_getlistrenderarea(handle:Byte Ptr, x:Float Ptr, y:Float Ptr, w:Float Ptr, h:Float Ptr)
	Function bmx_cegui_listbox_getvertscrollbar:Object(handle:Byte Ptr)
	Function bmx_cegui_listbox_gethorzscrollbar:Object(handle:Byte Ptr)
	Function bmx_cegui_listbox_gettotalitemsheight:Float(handle:Byte Ptr)
	Function bmx_cegui_listbox_getwidestitemwidth:Float(handle:Byte Ptr)

	Function bmx_cegui_itemlistbase_getitemcount:Int(handle:Byte Ptr)
	Function bmx_cegui_itemlistbase_getitemfromindex:Object(handle:Byte Ptr, index:Int)
	Function bmx_cegui_itemlistbase_getitemindex:Int(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_itemlistbase_finditemwithtext:Object(handle:Byte Ptr, text:Byte Ptr, startItem:Byte Ptr)
	Function bmx_cegui_itemlistbase_isiteminlist:Int(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_itemlistbase_isautoresizeenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_itemlistbase_issortenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_itemlistbase_getsortmode:Int(handle:Byte Ptr)
	Function bmx_cegui_itemlistbase_resetlist(handle:Byte Ptr)
	Function bmx_cegui_itemlistbase_additem(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_itemlistbase_insertitem(handle:Byte Ptr, item:Byte Ptr, position:Byte Ptr)
	Function bmx_cegui_itemlistbase_removeitem(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_itemlistbase_handleupdateditemdata(handle:Byte Ptr, resort:Int)
	Function bmx_cegui_itemlistbase_setautoresizeenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_itemlistbase_sizetocontent(handle:Byte Ptr)
	Function bmx_cegui_itemlistbase_endinitialisation(handle:Byte Ptr)
	Function bmx_cegui_itemlistbase_performchildwindowlayout(handle:Byte Ptr)
	Function bmx_cegui_itemlistbase_getitemrenderarea(handle:Byte Ptr, x:Float Ptr, y:Float Ptr, w:Float Ptr, h:Float Ptr)
	Function bmx_cegui_itemlistbase_getcontentpane:Object(handle:Byte Ptr)
	Function bmx_cegui_itemlistbase_setsortenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_itemlistbase_setsortmode(handle:Byte Ptr, Mode:Int)
	Function bmx_cegui_itemlistbase_sortlist(handle:Byte Ptr, relayout:Int)

	Function bmx_cegui_globaleventset_getinstance:Byte Ptr()
	Function bmx_cegui_globaleventset_subscribeevent:Byte Ptr(handle:Byte Ptr, name:Byte Ptr, cb:Byte Ptr)

	Function bmx_cegui_windowfactory_create:Byte Ptr(handle:Object, typeName:Byte Ptr)

	Function bmx_cegui_scrolledcontainer_iscontentpaneautosized:Int(handle:Byte Ptr)
	Function bmx_cegui_scrolledcontainer_setcontentpaneautosized(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_scrolledcontainer_getcontentarea(handle:Byte Ptr, x:Float Ptr, y:Float Ptr, w:Float Ptr, h:Float Ptr)
	Function bmx_cegui_scrolledcontainer_setcontentarea(handle:Byte Ptr, x:Float, y:Float, w:Float, h:Float)
	Function bmx_cegui_scrolledcontainer_getchildextentsarea(handle:Byte Ptr, x:Float Ptr, y:Float Ptr, w:Float Ptr, h:Float Ptr)

	Function bmx_cegui_clippedcontainer_getcliparea(handle:Byte Ptr, x:Float Ptr, y:Float Ptr, w:Float Ptr, h:Float Ptr)
	Function bmx_cegui_clippedcontainer_getclipperwindow:Object(handle:Byte Ptr)
	Function bmx_cegui_clippedcontainer_setcliparea(handle:Byte Ptr, x:Float, y:Float, w:Float, h:Float)
	Function bmx_cegui_clippedcontainer_setclipperwindow(handle:Byte Ptr, w:Byte Ptr)

	Function bmx_cegui_mousecursor_getsingleton:Byte Ptr()
	Function bmx_cegui_mousecursor_setimagewithname(handle:Byte Ptr, imageset:Byte Ptr, imageName:Byte Ptr)
	Function bmx_cegui_mousecursor_setimage(handle:Byte Ptr, image:Byte Ptr)
	Function bmx_cegui_mousecursor_getimage:Object(handle:Byte Ptr)
	Function bmx_cegui_mousecursor_draw(handle:Byte Ptr)
	Function bmx_cegui_mousecursor_setposition(handle:Byte Ptr, x:Float, y:Float)
	Function bmx_cegui_mousecursor_offsetPosition(handle:Byte Ptr, x:Float, y:Float)
	Function bmx_cegui_mousecursor_setconstraintarea(handle:Byte Ptr, x:Float, y:Float, w:Float, h:Float)
	Function bmx_cegui_mousecursor_hide(handle:Byte Ptr)
	Function bmx_cegui_mousecursor_show(handle:Byte Ptr)
	Function bmx_cegui_mousecursor_setvisible(handle:Byte Ptr, visible:Int)
	Function bmx_cegui_mousecursor_isvisible:Int(handle:Byte Ptr)
	Function bmx_cegui_mousecursor_getposition(handle:Byte Ptr, x:Float Ptr, y:Float Ptr)
	Function bmx_cegui_mousecursor_getconstraintarea(handle:Byte Ptr, x:Float Ptr, y:Float Ptr, w:Float Ptr, h:Float Ptr)
	Function bmx_cegui_mousecursor_getdisplayindependentposition(handle:Byte Ptr, x:Float Ptr, y:Float Ptr)

	Function bmx_cegui_coordconverter_windowtoscreenx:Float(window:Byte Ptr, x:Float)
	Function bmx_cegui_coordconverter_windowtoscreeny:Float(window:Byte Ptr, y:Float)
	Function bmx_cegui_coordconverter_windowtoscreen(window:Byte Ptr, x:Float, y:Float, toX:Float Ptr, toY:Float Ptr)
	Function bmx_cegui_coordconverter_windowtoscreenrect(window:Byte Ptr, x:Float, y:Float, w:Float, h:Float, toX:Float Ptr, toY:Float Ptr, toW:Float Ptr, toH:Float Ptr)
	Function bmx_cegui_coordconverter_screentowindowx:Float(window:Byte Ptr, x:Float)
	Function bmx_cegui_coordconverter_screentowindowy:Float(window:Byte Ptr, y:Float)
	Function bmx_cegui_coordconverter_screentowindow(window:Byte Ptr, x:Float, y:Float, toX:Float Ptr, toY:Float Ptr)
	Function bmx_cegui_coordconverter_screentowindowrect(window:Byte Ptr, x:Float, y:Float, w:Float, h:Float, toX:Float Ptr, toY:Float Ptr, toW:Float Ptr, toH:Float Ptr)

	Function bmx_cegui_combobox_ishit:Int(handle:Byte Ptr, x:Float, y:Float, allowDisabled:Int)
	Function bmx_cegui_combobox_getsingleclickenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_combobox_isdropdownlistvisible:Int(handle:Byte Ptr)
	Function bmx_cegui_combobox_geteditbox:Object(handle:Byte Ptr)
	Function bmx_cegui_combobox_getpushbutton:Object(handle:Byte Ptr)
	Function bmx_cegui_combobox_getdroplist:Object(handle:Byte Ptr)
	Function bmx_cegui_combobox_hasinputfocus:Int(handle:Byte Ptr)
	Function bmx_cegui_combobox_isreadonly:Int(handle:Byte Ptr)
	Function bmx_cegui_combobox_istextvalid:Int(handle:Byte Ptr)
	Function bmx_cegui_combobox_getvalidationstring:String(handle:Byte Ptr)
	Function bmx_cegui_combobox_getcaratindex:Int(handle:Byte Ptr)
	Function bmx_cegui_combobox_getselectionstartindex:Int(handle:Byte Ptr)
	Function bmx_cegui_combobox_getselectionendindex:Int(handle:Byte Ptr)
	Function bmx_cegui_combobox_getselectionlength:Int(handle:Byte Ptr)
	Function bmx_cegui_combobox_getmaxtextlength:Int(handle:Byte Ptr)
	Function bmx_cegui_combobox_getitemcount:Int(handle:Byte Ptr)
	Function bmx_cegui_combobox_getselecteditem:Object(handle:Byte Ptr)
	Function bmx_cegui_combobox_getlistboxitemfromindex:Object(handle:Byte Ptr, index:Int)
	Function bmx_cegui_combobox_getitemindex(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_combobox_issortenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_combobox_isitemselected:Int(handle:Byte Ptr, index:Int)
	Function bmx_cegui_combobox_finditemwithtext:Object(handle:Byte Ptr, text:Byte Ptr, startItem:Byte Ptr)
	Function bmx_cegui_combobox_islistboxiteminlist:Int(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_combobox_isvertscrollbaralwaysshown:Int(handle:Byte Ptr)
	Function bmx_cegui_combobox_ishorzscrollbaralwaysshown:Int(handle:Byte Ptr)
	Function bmx_cegui_combobox_showdroplist(handle:Byte Ptr)
	Function bmx_cegui_combobox_hidedroplist(handle:Byte Ptr)
	Function bmx_cegui_combobox_setsingleclickenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_combobox_setreadonly(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_combobox_setvalidationstring(handle:Byte Ptr, validationString:Byte Ptr)
	Function bmx_cegui_combobox_setcaratindex(handle:Byte Ptr, caratPos:Int)
	Function bmx_cegui_combobox_setselection(handle:Byte Ptr, startPos:Int, endPos:Int)
	Function bmx_cegui_combobox_setmaxtextlength(handle:Byte Ptr, maxLen:Int)
	Function bmx_cegui_combobox_activateeditbox(handle:Byte Ptr)
	Function bmx_cegui_combobox_resetlist(handle:Byte Ptr)
	Function bmx_cegui_combobox_additem(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_combobox_insertitem(handle:Byte Ptr, item:Byte Ptr, position:Byte Ptr)
	Function bmx_cegui_combobox_removeitem(handle:Byte Ptr, item:Byte Ptr)
	Function bmx_cegui_combobox_clearallselections(handle:Byte Ptr)
	Function bmx_cegui_combobox_setsortingenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_combobox_setshowvertscrollbar(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_combobox_setshowhorzscrollbar(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_combobox_setitemselectstate(handle:Byte Ptr, item:Byte Ptr, state:Int)
	Function bmx_cegui_combobox_setitemselectstateindex(handle:Byte Ptr, itemIndex:Int, state:Int)
	Function bmx_cegui_combobox_handleupdatedlistitemdata(handle:Byte Ptr)

	Function bmx_cegui_udim_create:Byte Ptr(scale:Float, offset:Float)
	Function bmx_cegui_udim_delete(handle:Byte Ptr)

	Function bmx_cegui_image_getsize(handle:Byte Ptr, width:Float Ptr, height:Float Ptr)
	Function bmx_cegui_image_getwidth:Float(handle:Byte Ptr)
	Function bmx_cegui_image_getheight:Float(handle:Byte Ptr)
	Function bmx_cegui_image_getoffsets(handle:Byte Ptr, x:Float Ptr, y:Float Ptr)
	Function bmx_cegui_image_getoffsetx:Float(handle:Byte Ptr)
	Function bmx_cegui_image_getoffsety:Float(handle:Byte Ptr)
	Function bmx_cegui_image_getname:String(handle:Byte Ptr)
	Function bmx_cegui_image_getimagesetname:String(handle:Byte Ptr)
	Function bmx_cegui_image_getimageset:Byte Ptr(handle:Byte Ptr)
	Function bmx_cegui_image_getsourcetexturearea(handle:Byte Ptr, x:Float Ptr, y:Float Ptr, w:Float Ptr, h:Float Ptr)

	Function bmx_cegui_font_iscodepointavailable:Int(handle:Byte Ptr, cp:Int)
	Function bmx_cegui_font_getlinespacing:Float(handle:Byte Ptr, yScale:Float)
	Function bmx_cegui_font_getfontheight:Float(handle:Byte Ptr, yScale:Float)
	Function bmx_cegui_font_getbaseline:Float(handle:Byte Ptr, yScale:Float)
	Function bmx_cegui_font_removeproperty(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_font_clearproperties(handle:Byte Ptr)
	Function bmx_cegui_font_ispropertypresent:Int(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_font_getpropertyhelp:String(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_font_getproperty:String(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_font_setproperty(handle:Byte Ptr, name:Byte Ptr, value:Byte Ptr)
	Function bmx_cegui_font_ispropertydefault:Int(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_font_getpropertydefault:String(handle:Byte Ptr, name:Byte Ptr)

	Function bmx_cegui_font_setdefaultresourcegroup(resourceGroup:Byte Ptr)
	Function bmx_cegui_font_getdefaultresourcegroup:String()

	Function bmx_cegui_dragdropeventargs_getdragdropitem:Object(handle:Byte Ptr)
	Function bmx_cegui_mousecursoreventargs_getimage:Object(handle:Byte Ptr)
	Function bmx_cegui_keyeventargs_getcodepoint:Int(handle:Byte Ptr)
	Function bmx_cegui_keyeventargs_getscancode:Int(handle:Byte Ptr)
	Function bmx_cegui_keyeventargs_getsyskeys:Int(handle:Byte Ptr)
	Function bmx_cegui_mouseeventargs_getposition(handle:Byte Ptr, x:Float Ptr, y:Float Ptr)
	Function bmx_cegui_mouseeventargs_getmovedelta(handle:Byte Ptr, x:Float Ptr, y:Float Ptr)
	Function bmx_cegui_mouseeventargs_getbutton:Int(handle:Byte Ptr)
	Function bmx_cegui_mouseeventargs_getsyskeys:Int(handle:Byte Ptr)
	Function bmx_cegui_mouseeventargs_getwheelchange:Float(handle:Byte Ptr)
	Function bmx_cegui_mouseeventargs_getclickcount:Int(handle:Byte Ptr)
	Function bmx_cegui_headersequenceeventargs_getoldindex:Int(handle:Byte Ptr)
	Function bmx_cegui_headersequenceeventargs_getnewindex:Int(handle:Byte Ptr)
	Function bmx_cegui_treeeventargs_gettreeitem:Object(handle:Byte Ptr)

	Function bmx_cegui_imageset_getname:String(handle:Byte Ptr)
	Function bmx_cegui_imageset_getimagecount:Int(handle:Byte Ptr)
	Function bmx_cegui_imageset_isimagedefined:Int(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_imageset_undefineimage(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_imageset_undefineallimages(handle:Byte Ptr)
	Function bmx_cegui_imageset_getimagesize(handle:Byte Ptr, name:Byte Ptr, width:Float Ptr, height:Float Ptr)
	Function bmx_cegui_imageset_getimagewidth:Float(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_imageset_getimageheight:Float(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_imageset_getimageoffset(handle:Byte Ptr, name:Byte Ptr, x:Float Ptr, y:Float Ptr)
	Function bmx_cegui_imageset_getimageoffsetx:Float(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_imageset_getimageoffsety:Float(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_cegui_imageset_defineimage(handle:Byte Ptr, name:Byte Ptr, x:Float, y:Float, width:Float, height:Float, renderOffsetX:Float, renderOffsetY:Float)
	Function bmx_cegui_imageset_isautoscaled:Int(handle:Byte Ptr)
	Function bmx_cegui_imageset_getnativeresoultion(handle:Byte Ptr, width:Float Ptr, height:Float Ptr)
	Function bmx_cegui_imageset_setautoscalingenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_imageset_setnativeresolution(handle:Byte Ptr, width:Float, height:Float)
	Function bmx_cegui_imageset_notifydisplaysizechanged(handle:Byte Ptr, width:Float, height:Float)
	Function bmx_cegui_imageset_setdefaultresourcegroup(resourceGroup:Byte Ptr)
	Function bmx_cegui_imageset_getdefaultresourcegroup:String()

	Function bmx_cegui_groupbox_getcontentpane:Object(handle:Byte Ptr)

	Function bmx_cegui_listheadersegment_issizingenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_listheadersegment_getsortdirection:Int(handle:Byte Ptr)
	Function bmx_cegui_listheadersegment_isdragmovingenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_listheadersegment_getdragmoveoffset(handle:Byte Ptr, x:Float Ptr, y:Float Ptr)
	Function bmx_cegui_listheadersegment_isclickable:Int(handle:Byte Ptr)
	Function bmx_cegui_listheadersegment_issegmenthovering:Int(handle:Byte Ptr)
	Function bmx_cegui_listheadersegment_issegmentpushed:Int(handle:Byte Ptr)
	Function bmx_cegui_listheadersegment_issplitterhovering:Int(handle:Byte Ptr)
	Function bmx_cegui_listheadersegment_isbeingdragmoved:Int(handle:Byte Ptr)
	Function bmx_cegui_listheadersegment_isbeingdragsized:Int(handle:Byte Ptr)
	Function bmx_cegui_listheadersegment_getsizingcursorimage:Object(handle:Byte Ptr)
	Function bmx_cegui_listheadersegment_getmovingcursorimage:Object(handle:Byte Ptr)
	Function bmx_cegui_listheadersegment_setsizingenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_listheadersegment_setsortdirection(handle:Byte Ptr, sortDir:Int)
	Function bmx_cegui_listheadersegment_setdragmovingenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_listheadersegment_setclickable(handle:Byte Ptr, setting:Int)

	Function bmx_cegui_listheader_getcolumncount:Int(handle:Byte Ptr)
	Function bmx_cegui_listheader_getsegmentfromcolumn:Object(handle:Byte Ptr, column:Int)
	Function bmx_cegui_listheader_getsegmentfromid:Object(handle:Byte Ptr, id:Int)
	Function bmx_cegui_listheader_getsortsegment:Object(handle:Byte Ptr)
	Function bmx_cegui_listheader_getcolumnfromsegment:Int(handle:Byte Ptr, segment:Byte Ptr)
	Function bmx_cegui_listheader_getcolumnfromid:Int(handle:Byte Ptr, id:Int)
	Function bmx_cegui_listheader_getsortcolumn:Int(handle:Byte Ptr)
	Function bmx_cegui_listheader_getcolumnwithtext:Int(handle:Byte Ptr, text:Byte Ptr)
	Function bmx_cegui_listheader_getpixeloffsettosegment:Float(handle:Byte Ptr, segment:Byte Ptr)
	Function bmx_cegui_listheader_getpixeloffsettocolumn:Float(handle:Byte Ptr, column:Int)
	Function bmx_cegui_listheader_gettotalsegmentspixelextent:Float(handle:Byte Ptr)
	Function bmx_cegui_listheader_getcolumnwidth:Float(handle:Byte Ptr, column:Int)
	Function bmx_cegui_listheader_getsortdirection:Int(handle:Byte Ptr)
	Function bmx_cegui_listheader_issortingenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_listheader_iscolumnsizingenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_listheader_iscolumndraggingenabled:Int(handle:Byte Ptr)
	Function bmx_cegui_listheader_getsegmentoffset:Float(handle:Byte Ptr)
	Function bmx_cegui_listheader_setsortingenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_listheader_setsortdirection(handle:Byte Ptr, direction:Int)
	Function bmx_cegui_listheader_setsortsegment(handle:Byte Ptr, segment:Byte Ptr)
	Function bmx_cegui_listheader_setsortcolumn(handle:Byte Ptr, column:Int)
	Function bmx_cegui_listheader_setsortcolumnfromid(handle:Byte Ptr, id:Int)
	Function bmx_cegui_listheader_setcolumnsizingenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_listheader_setcolumndraggingenabled(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_listheader_addcolumn(handle:Byte Ptr, text:Byte Ptr, id:Int, width:Float)
	Function bmx_cegui_listheader_insertcolumn(handle:Byte Ptr, text:Byte Ptr, id:Int, width:Float, position:Int)
	Function bmx_cegui_listheader_insertcolumnatsegment(handle:Byte Ptr, text:Byte Ptr, id:Int, width:Float, position:Byte Ptr)
	Function bmx_cegui_listheader_removecolumn(handle:Byte Ptr, column:Int)
	Function bmx_cegui_listheader_removesegment(handle:Byte Ptr, segment:Byte Ptr)
	Function bmx_cegui_listheader_movecolumn(handle:Byte Ptr, column:Int, position:Int)
	Function bmx_cegui_listheader_movecolumnatsegment(handle:Byte Ptr, column:Int, position:Byte Ptr)
	Function bmx_cegui_listheader_movesegment(handle:Byte Ptr, segment:Byte Ptr, position:Int)
	Function bmx_cegui_listheader_movesegmentatsegment(handle:Byte Ptr, segment:Byte Ptr, position:Byte Ptr)
	Function bmx_cegui_listheader_setsegmentoffset(handle:Byte Ptr, offset:Float)
	Function bmx_cegui_listheader_setcolumnwidth(handle:Byte Ptr, column:Int, width:Float)

	Function bmx_cegui_schememanager_createscheme:Object(filename:Byte Ptr, resourceGroup:Byte Ptr, action:Int)
	Function bmx_cegui_schememanager_destroyname(scheme:Byte Ptr)
	Function bmx_cegui_schememanager_destroyobj(scheme:Byte Ptr)
	Function bmx_cegui_schememanager_destroyall()
	Function bmx_cegui_schememanager_get:Object(name:Byte Ptr)
	Function bmx_cegui_schememanager_isdefined:Int(name:Byte Ptr)
	Function bmx_cegui_schememanager_createall(pattern:Byte Ptr, resourceGroup:Byte Ptr)

	Function bmx_cegui_widgetlookmanager_getdefaultresourcegroup:String()
	Function bmx_cegui_widgetlookmanager_setdefaultresourcegroup(group:Byte Ptr)

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

Rem
bbdoc: 
End Rem
Type TCEException Extends TRuntimeException
	Field message:String
	Field name:String
	Field fileName:String
	Field line:Int
	
	Field fullDetails:Int
	
	Method CreateException:TCEException(message:String, name:String, fileName:String, line:Int)
		Self.message = message
		Self.name = name
		Self.fileName = fileName
		Self.line = line
		Return Self
	End Method

	Function _create:TCEException(message:String, name:String, fileName:String, line:Int)
		Local e:TCEException = New TCEException.CreateException(message, name + " (Warning: Unhandled Exception. Inform Brucey!)", ..
			fileName, line)
		e.fullDetails = True
		Return e
	End Function
	
	Method ToString:String()
		Local s:String = name + " : " + message
		If fullDetails Then
			s:+ " : " + fileName + " (" + line + ")"
		End If
		Return s
	End Method
End Type

Rem
bbdoc: 
End Rem
Type TCEAlreadyExistsException Extends TCEException
	Function _create:TCEException(message:String, name:String, fileName:String, line:Int)
		Return New TCEAlreadyExistsException.CreateException(message, name, fileName, line)
	End Function
End Type

Rem
bbdoc: Exception used when a file handling problem occurs. 
End Rem
Type TCEFileIOException Extends TCEException
	Function _create:TCEException(message:String, name:String, fileName:String, line:Int)
		Return New TCEFileIOException.CreateException(message, name, fileName, line)
	End Function
End Type

Rem
bbdoc: 
End Rem
Type TCEGenericException Extends TCEException
	Function _create:TCEException(message:String, name:String, fileName:String, line:Int)
		Return New TCEGenericException.CreateException(message, name, fileName, line)
	End Function
End Type

Rem
bbdoc: 
End Rem
Type TCEInvalidRequestException Extends TCEException
	Function _create:TCEException(message:String, name:String, fileName:String, line:Int)
		Return New TCEInvalidRequestException.CreateException(message, name, fileName, line)
	End Function
End Type

Rem
bbdoc: 
End Rem
Type TCEMemoryException Extends TCEException
	Function _create:TCEException(message:String, name:String, fileName:String, line:Int)
		Return New TCEMemoryException.CreateException(message, name, fileName, line)
	End Function
End Type

Rem
bbdoc: 
End Rem
Type TCENullObjectException Extends TCEException
	Function _create:TCEException(message:String, name:String, fileName:String, line:Int)
		Return New TCENullObjectException.CreateException(message, name, fileName, line)
	End Function
End Type

Rem
bbdoc: 
End Rem
Type TCEObjectInUseException Extends TCEException
	Function _create:TCEException(message:String, name:String, fileName:String, line:Int)
		Return New TCEObjectInUseException.CreateException(message, name, fileName, line)
	End Function
End Type

Rem
bbdoc: 
End Rem
Type TCERendererException Extends TCEException
	Function _create:TCEException(message:String, name:String, fileName:String, line:Int)
		Return New TCERendererException.CreateException(message, name, fileName, line)
	End Function
End Type

Rem
bbdoc: 
End Rem
Type TCEScriptException Extends TCEException
	Function _create:TCEException(message:String, name:String, fileName:String, line:Int)
		Return New TCEScriptException.CreateException(message, name, fileName, line)
	End Function
End Type

Rem
bbdoc: 
End Rem
Type TCEUnknownObjectException Extends TCEException
	Function _create:TCEException(message:String, name:String, fileName:String, line:Int)
		Return New TCEUnknownObjectException.CreateException(message, name, fileName, line)
	End Function
End Type


Rem
bbdoc: Window's position specifies an offset of its top edge from the top edge of its parent.
end rem
Const VA_TOP:Int = 0
Rem
about: Window's position specifies an offset of its vertical centre from the vertical centre of its parent.
End Rem
Const VA_CENTRE:Int = 1
Rem
about: Window's position specifies an offset of its bottom edge from the bottom edge of its parent.
end rem
Const VA_BOTTOM:Int = 2

Rem
bbdoc: Window's position specifies an offset of its left edge from the left edge of its parent.
end rem
Const HA_LEFT:Int = 0
Rem
bbdoc: Window's position specifies an offset of its horizontal centre from the horizontal centre of its parent.
end rem
Const HA_CENTRE:Int = 1
Rem
bbdoc: Window's position specifies an offset of its right edge from the right edge of its parent.
end rem
Const HA_RIGHT:Int = 2

Rem
bbdoc: Top of Image should be aligned with the top of the destination area.
end rem
Const VF_TOP_ALIGNED:Int = 0
Rem
bbdoc: Image should be vertically centred within the destination area.
end rem
Const VF_CENTRE_ALIGNED:Int = 1
Rem
bbdoc: Bottom of Image should be aligned with the bottom of the destination area.
end rem
Const VF_BOTTOM_ALIGNED:Int = 2
Rem
bbdoc: Image should be stretched vertically to fill the destination area.
end rem
Const VF_STRETCHED:Int = 3
Rem
bbdoc: Image should be tiled vertically to fill the destination area (bottom-most tile may be clipped).
end rem
Const VF_TILED:Int = 4

Rem
bbdoc: Left of Image should be aligned with the left of the destination area.
end rem
Const HF_LEFT_ALIGNED:Int = 0
Rem
bbdoc: Image should be horizontally centred within the destination area.
end rem
Const HF_CENTRE_ALIGNED:Int = 1
Rem
bbdoc: Right of Image should be aligned with the right of the destination area.
end rem
Const HF_RIGHT_ALIGNED:Int = 2
Rem
bbdoc: Image should be stretched horizontally to fill the destination area.
end rem
Const HF_STRETCHED:Int = 3
Rem
bbdoc: Image should be tiled horizontally to fill the destination area (right-most tile may be clipped).
end rem
Const HF_TILED:Int = 4

Rem
bbdoc: Top of text should be aligned with the top of the destination area.
end rem
Const VTF_TOP_ALIGNED:Int = 0
Rem
bbdoc: Text should be vertically centred within the destination area.
end rem
Const VTF_CENTRE_ALIGNED:Int = 1
Rem
bbdoc: Bottom of text should be aligned with the bottom of the destination area.
end rem
Const VTF_BOTTOM_ALIGNED:Int = 2

Rem
bbdoc: Left of text should be aligned with the left of the destination area (single line of text only).
end rem
Const HTF_LEFT_ALIGNED:Int = 0
Rem
bbdoc: Right of text should be aligned with the right of the destination area  (single line of text only).
end rem
Const HTF_RIGHT_ALIGNED:Int = 1
Rem
bbdoc: text should be horizontally centred within the destination area  (single line of text only).
end rem
Const HTF_CENTRE_ALIGNED:Int = 2
Rem
bbdoc: text should be spaced so that it takes the full width of the destination area (single line of text only).
end rem
Const HTF_JUSTIFIED:Int = 3
Rem
bbdoc: Left of text should be aligned with the left of the destination area (word wrapped to multiple lines as needed).
end rem
Const HTF_WORDWRAP_LEFT_ALIGNED:Int = 4
Rem
bbdoc: Right of text should be aligned with the right of the destination area  (word wrapped to multiple lines as needed).
end rem
Const HTF_WORDWRAP_RIGHT_ALIGNED:Int = 5
Rem
bbdoc: text should be horizontally centred within the destination area  (word wrapped to multiple lines as needed).
end rem
Const HTF_WORDWRAP_CENTRE_ALIGNED:Int = 6
Rem
bbdoc: text should be spaced so that it takes the full width of the destination area (word wrapped to multiple lines as needed).
end rem
Const HTF_WORDWRAP_JUSTIFIED:Int = 7

Rem
bbdoc: Vertical line spacing value for font.
end rem
Const FMT_LINE_SPACING:Int = 0
Rem
bbdoc: Vertical baseline value for font.
end rem
Const FMT_BASELINE:Int = 1
Rem
bbdoc: Horizontal extent of a string.
end rem
Const FMT_HORZ_EXTENT:Int = 2

Rem
bbdoc: Do nothing operator.
end rem
Const DOP_NOOP:Int = 0
Rem
bbdoc: Dims should be added.
end rem
Const DOP_ADD:Int = 1
Rem
bbdoc: Dims should be subtracted.
end rem
Const DOP_SUBTRACT:Int = 2
Rem
bbdoc: Dims should be multiplied.
end rem
Const DOP_MULTIPLY:Int = 3
Rem
bbdoc: Dims should be divided.
end rem
Const DOP_DIVIDE:Int = 4

Rem
bbdoc: References image used for the background.
end rem
Const FIC_BACKGROUND:Int = 0
Rem
bbdoc: References image used for the top-left corner.
end rem
Const FIC_TOP_LEFT_CORNER:Int = 1
Rem
bbdoc: References image used for the top-right corner.
end rem
Const FIC_TOP_RIGHT_CORNER:Int = 2
Rem
bbdoc: References image used for the bottom-left corner.
end rem
Const FIC_BOTTOM_LEFT_CORNER:Int = 3
Rem
bbdoc: References image used for the bottom-right corner.
end rem
Const FIC_BOTTOM_RIGHT_CORNER:Int = 4
Rem
bbdoc: References image used for the left edge.
end rem
Const FIC_LEFT_EDGE:Int = 5
Rem
bbdoc: References image used for the right edge.
end rem
Const FIC_RIGHT_EDGE:Int = 6
Rem
bbdoc: References image used for the top edge.
end rem
Const FIC_TOP_EDGE:Int = 7
Rem
bbdoc: References image used for the bottom edge.
end rem
Const FIC_BOTTOM_EDGE:Int = 8
Rem
bbdoc: Max number of images for a frame.
end rem
Const FIC_FRAME_IMAGE_COUNT:Int = 9

Rem
bbdoc: Do not load the resource, return the existing instance.
End Rem
Const XREA_RETURN:Int = 0
Rem
bbdoc: Destroy the existing instance and replace with the newly loaded one.
End Rem
Const XREA_REPLACE:Int = 1
Rem
bbdoc: Throw an AlreadyExistsException.
End Rem
Const XREA_THROW:Int = 2



