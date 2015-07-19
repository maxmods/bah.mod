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

Rem
bbdoc: Linux XCB system driver.
End Rem
Module BaH.xcb


ModuleInfo "Version: 1.00"
ModuleInfo "License: zlib/libpng"
ModuleInfo "Copyright: 2015 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

?linux

Import BRL.System
Import Pub.FreeProcess

Import "common.bmx"

Private
Global _xcbDriver:TXCBSystemDriver
Public

Type TXCBWindow

	Field winId:Int
	
	Field x:Int, y:Int
	Field width:Int, height:Int

	Method Create:TXCBWindow(title:String, parent:TXCBWindow = Null, x:Int = 0, y:Int = 0, width:Int = 100, height:Int = 100, flags:Int = 0)
		If parent Then
			winId = bmx_xcb_window_create(Self, _xcbDriver.xcbPtr, title, parent.winId, x, y, width, height, flags)
		Else
			winId = bmx_xcb_window_create(Self, _xcbDriver.xcbPtr, title, 0, x, y, width, height, flags)
		End If
		
		Self.x = x
		Self.y = y
		Self.width = width
		Self.height = height

		AddHook EmitEventHook,_EventHook,Self,0

		OnInit()
		Return Self
	End Method

	Rem
	bbdoc: Shows the window.
	End Rem
	Method Show()
		bmx_xcb_window_show(_xcbDriver.xcbPtr, winId)
	End Method

	Rem
	bbdoc: Hides the window.
	End Rem
	Method Hide()
		bmx_xcb_window_hide(_xcbDriver.xcbPtr, winId)
	End Method
	
	Rem
	bbdoc: Called when the window needs painted.
	End Rem
	Method OnPaint()
	End Method
	
	Rem
	bbdoc: Called when the window has been resized.
	End Rem
	Method OnSize(w:Int, h:Int)
	End Method
	
	Rem
	bbdoc: Called as part of window creation.
	about: Put initialisation stuff here.
	End Rem
	Method OnInit()
	End Method
	
	Rem
	bbdoc: Flushes changes to the display.
	about: Usually required at the end of an OnPaint().
	End Rem
	Method Flush()
		bmx_xcb_connection_flush(_xcbDriver.xcbPtr)
	End Method

	Function _EventHook:Object( id:Int,data:Object,context:Object )

		Local ev:TEvent=TEvent(data)
		If Not ev Return data
		
		Local win:TXCBWindow = TXCBWindow(context)
		If win And win = ev.source Then
			Select ev.id
				Case EVENT_GADGETPAINT ' window needs a repaint
					win.OnPaint()
				Case EVENT_WINDOWSIZE ' window was resized
					win.width = ev.x
					win.height = ev.y
					win.OnSize(ev.x, ev.y)
			End Select
		End If
	
		Return data
	End Function
	
End Type

Type TXCBSystemDriver Extends TSystemDriver

	Field xcbPtr:Byte Ptr
	Field screenCount:Int
	
	Field _platform:Int
	
	Const _GNOME:Int = 1
	Const _KDE:Int = 2
	Const _UNKNOWN:Int = 3

	Method New()
		xcbPtr = bmx_xcb_system_startup(Varptr screenCount)
	End Method

	Method Poll:Int()
		bmx_xcb_system_poll(xcbPtr)
	End Method
	
	Method Wait:Int()
		bmx_xcb_system_wait(xcbPtr)
	End Method

	Method Emit( osevent:Byte Ptr,source:Object )
		'Throw "simon come here"
	End Method

	Method SetMouseVisible:Int( visible:Int )
		' TODO
	End Method

	Method MoveMouse:Int( x:Int,y:Int )
		' TODO
	End Method

	Method Notify:Int( text$,serious:Int )
		_checkPlatform()
		Select _platform
			Case _GNOME
				Local info:String = "--info"
				If serious Then
					info = "--error"
				End If
				system_("zenity " + info + " --title=~q" + AppTitle + "~q --text=~q" + text + "~q")
		End Select
	End Method
	
	Method Confirm:Int( text$,serious:Int )
		_checkPlatform()
		Select _platform
			Case _GNOME
				Local question:String = "--question"
				If serious Then
					question = "--error"
				End If
				If Not system_("zenity " + question + " --title=~q" + AppTitle + "~q --text=~q" + text + "~q")
					Return True
				Else
					Return False
				End If
		End Select
		Return 0
	End Method
	
	Method Proceed:Int( text$,serious:Int )
		_checkPlatform()
		Select _platform
			Case _GNOME
				Local question:String = "--question"
				If serious Then
					question = "--error"
				End If
				If Not system_("zenity " + question + " --title=~q" + AppTitle + "~q --text=~q" + text + "~q")
					Return True
				Else
					Return False
				End If
		End Select
		Return -1
	End Method

	Method RequestFile$( text$,exts$,save:Int,file$ )
		_checkPlatform()
		Select _platform
			Case _GNOME
				Local cmd:String = "zenity --file-selection --title=~q" + text + "~q "
				If save Then
					cmd :+ "--save "
				End If
				If file Then
					cmd :+ "--filename=~q" + file + "~q "
				End If
				Return _runCommand(cmd)
		End Select
	End Method
	
	Method RequestDir$( text$,path$ )
		_checkPlatform()
		Select _platform
			Case _GNOME
				Local cmd:String = "zenity --file-selection --directory --title=~q" + text + "~q "
				If path Then
					cmd :+ "--filename=~q" + path + "~q "
				End If
				Return _runCommand(cmd)
		End Select
	End Method

	Method OpenURL:Int( url$ )
		'If getenv_("KDE_FULL_DESKTOP")
		'	system_ "kfmclient exec ~q"+url+"~q"
		'ElseIf getenv_("GNOME_DESKTOP_SESSION_ID")
		'	system_ "gnome-open ~q"+url+"~q"
		'EndIf
	End Method

	Method DesktopWidth:Int()
		Return bmx_xcb_system_desktopwidth(xcbPtr)
	End Method
	
	Method DesktopHeight:Int()
		Return bmx_xcb_system_desktopheight(xcbPtr)
	End Method
	
	Method DesktopDepth:Int()
		Return bmx_xcb_system_desktopdepth(xcbPtr)
	End Method
	
	Method DesktopHertz:Int()
		Return 60 ' until we find something better
	End Method

	Method _checkPlatform()
		If _platform Then
			Return
		End If
		
		If Not system_("type zenity 1>/dev/null") Then
			_platform = _GNOME
			Return
		End If
		
		_platform = _UNKNOWN
	End Method
	
	Method _runCommand:String(cmd:String)
		Local process:TProcess = CreateProcess(cmd)
		While process.Status()
			Delay 10
		Wend

		Return process.pipe.ReadLine()
	End Method
	
End Type

_xcbDriver=New TXCBSystemDriver
Driver = _xcbDriver


?
