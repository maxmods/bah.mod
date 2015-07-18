' Copyright (c) 2015 Bruce A Henderson
'
' This software is provided 'as-is', without any express or implied
' warranty. In no event will the authors be held liable for any damages
' arising from the use of this software.
'
' Permission is granted to anyone to use this software for any purpose,
' including commercial applications, and to alter it and redistribute it
' freely, subject to the following restrictions:
'
'    1. The origin of this software must not be misrepresented; you must not
'    claim that you wrote the original software. If you use this software
'    in a product, an acknowledgment in the product documentation would be
'    appreciated but is not required.
'
'    2. Altered source versions must be plainly marked as such, and must not be
'    misrepresented as being the original software.
'
'    3. This notice may not be removed or altered from any source
'    distribution.
'
SuperStrict

Import "-lxcb"
Import "glue.c"

Extern

	Function bmx_xcb_system_startup:Byte Ptr(screenCount:Int Ptr)
	Function bmx_xcb_system_poll(conn:Byte Ptr)
	Function bmx_xcb_system_wait(conn:Byte Ptr)
	Function bmx_xcb_system_desktopwidth:Int(conn:Byte Ptr)
	Function bmx_xcb_system_desktopheight:Int(conn:Byte Ptr)
	Function bmx_xcb_system_desktopdepth:Int(conn:Byte Ptr)
	
	Function bmx_xcb_window_create:Int(obj:Object, conn:Byte Ptr, title:String, parent:Int, x:Int, y:Int, w:Int, h:Int, flags:Int)
	Function bmx_xcb_window_show(conn:Byte Ptr, winId:Int)
	Function bmx_xcb_window_hide(conn:Byte Ptr, winId:Int)
	
	Function bmx_xcb_connection_flush(conn:Byte Ptr)
	
End Extern

