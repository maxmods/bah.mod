' Copyright (c) 2007-2009, Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Import "../core.bmx"

Rem
bbdoc: 
End Rem
Type TCursesButtonBox Extends TCursesWidget

	Field titlePtr:Byte Ptr
	Field _buttons:Byte Ptr
	Field buttonCount:Int

	Function Create:TCursesButtonBox(x:Int, y:Int, width:Int, height:Int, ..
			title:String, rows:Int, cols:Int, buttons:String[], highlight:Int, box:Int = True, shadow:Int = False)
			
		Local this:TCursesButtonBox = New TCursesButtonBox

		this.init(x, y, width, height, title, rows, cols, buttons, highlight, box, shadow)
		
		If Not this.widgetPtr Then
			this.free()
			Return Null
		End If

		TCDKDriver.currentScreen.addWidget(this)

		Return this
	End Function
	
	Method Init(x:Int, y:Int, width:Int, height:Int, ..
			title:String, rows:Int, cols:Int, buttons:String[], highlight:Int, box:Int, shadow:Int)
	
		buttonCount = buttons.length
		_buttons = arrayToCStrings(buttons)

		If title Then
			titlePtr = title.toCString()
		End If
		
		If title Then
			widgetPtr = newCDKButtonbox(TCDKDriver.currentScreen.screenPtr, x, y, height, width, titlePtr, rows, ..
				cols, _buttons, buttonCount, highlight, box, shadow)
		Else
			widgetPtr = newCDKButtonbox(TCDKDriver.currentScreen.screenPtr, x, y, height, width, Null, rows, ..
				cols, _buttons, buttonCount, highlight, box, shadow)
		End If
			
	
	End Method

	Method show()
		bmx_curses_drawCDKButtonBox(widgetPtr)
	End Method
	
	Method position()
		'bmx_curses_positionCDKLabel(widgetPtr)
	End Method

	Method hide()
		'bmx_curses_eraseCDKLabel(widgetPtr)
	End Method

	Method move(x:Int, y:Int, relative:Int, refresh:Int)
	End Method

	Method injectCharacter(char:Int)
		bmx_curses_injectCDKButtonBox(widgetPtr, char)
	End Method

	Method getWindow:Byte Ptr()
		Return bmx_curses_CDKButtonbox_window(widgetPtr)
	End Method
	
	Rem
	bbdoc: Activates the button box.
	End Rem
	Method Activate:Int(actions:Int = 0)
		Return activateCDKButtonbox(widgetPtr, actions)
	End Method

	Rem
	bbdoc: Returns the currently selected button.
	End Rem
	Method currentButton:Int()
		Return bmx_buttonbox_currentButton(widgetPtr)
	End Method

	Method getType:Int()
		Return vBUTTONBOX
	End Method

	Method free()
		If titlePtr Then
			MemFree(titlePtr)
			titlePtr = Null
		End If
		
		If _buttons Then
			freeCStringArray(_buttons, buttonCount)
			_buttons = Null
		End If
		
		Super.free()
	End Method
	
End Type

