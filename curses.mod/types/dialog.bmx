' Copyright (c) 2007-2013 Bruce A Henderson
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
Type TCursesDialog Extends TCursesWidget

	Field message:Byte Ptr
	Field messageCount:Int
	Field _buttons:Byte Ptr
	Field buttonCount:Int
	
	Rem
	bbdoc: 
	End Rem
	Function Create:TCursesDialog(x:Int, y:Int, text:String[], buttons:String[], highlight:Int, separator:Int, box:Int, shadow:Int)
		Local this:TCursesDialog = New TCursesDialog
		
		this.init(x, y, text, buttons, highlight, separator, box, shadow)
		
		If Not this.widgetPtr Then
			this.free()
			Return Null
		End If

		TCDKDriver.currentScreen.addWidget(this)
	
		Return this
	End Function
	
	Method Init(x:Int, y:Int, text:String[], buttons:String[], highlight:Int, separator:Int, box:Int, shadow:Int)

		messageCount = text.length
		message = arrayToCStrings(text)
	
		buttonCount = buttons.length
		_buttons = arrayToCStrings(buttons)
		
		widgetPtr = newCDKDialog(TCDKDriver.currentScreen.screenPtr, x, y, message, text.length, _buttons, buttons.length, highlight, separator, box, shadow)
		
	End Method
	
	Method free()
	
		If message Then
			freeCStringArray(message, messageCount)
		End If
		
		If _buttons Then
			freeCStringArray(_buttons, buttonCount)
		End If
	
		Super.free()
	
	End Method
	
	Method show()
	
	End Method
	
	Method position()
	
	End Method
	
	Method hide()
	
	End Method

	Method move(x:Int, y:Int, relative:Int, refresh:Int)
	End Method
	
	Method injectCharacter(char:Int)
	End Method
	
	Method getWindow:Byte Ptr()
		Return bmx_curses_CDKDialog_window(widgetPtr)
	End Method

	Rem
	bbdoc: Returns the currently selected button.
	End Rem
	Method currentButton:Int()
		Return bmx_dialog_currentButton(widgetPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Activate:Int(actions:Int = 0)
	
		Return activateCDKDialog(widgetPtr, actions)
	
	End Method

	Method getType:Int()
		Return vDIALOG
	End Method

End Type

