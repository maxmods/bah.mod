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
Type TCursesLabel Extends TCursesWidget

	Field messagePtr:Byte Ptr
	Field messageCount:Int
	
	Rem
	bbdoc: 
	End Rem
	Function Create:TCursesLabel(x:Int, y:Int, text:String[], box:Int = True, shadow:Int = False)
		Local this:TCursesLabel = New TCursesLabel
		
		this.init(x, y, text, box, shadow)
		
		If Not this.widgetPtr Then
			this.free()
			Return Null
		End If

		TCDKDriver.currentScreen.addWidget(this)
	
		Return this
	End Function
	
	Method Init(x:Int, y:Int, text:String[], box:Int, shadow:Int)

		messageCount = text.length
		messagePtr = arrayToCStrings(text)
	
		widgetPtr = newCDKLabel(TCDKDriver.currentScreen.screenPtr, x, y, messagePtr, text.length, box, shadow)
		
	End Method
	
	Method show()
		bmx_curses_drawCDKLabel(widgetPtr)
	End Method
	
	Method position()
		bmx_curses_positionCDKLabel(widgetPtr)
	End Method

	Method hide()
		bmx_curses_eraseCDKLabel(widgetPtr)
	End Method

	Method move(x:Int, y:Int, relative:Int, refresh:Int)
	' TODO
	End Method

	Method injectCharacter(char:Int)
	End Method

	Method getWindow:Byte Ptr()
		Return bmx_curses_CDKLabel_window(widgetPtr)
	End Method

	Rem
	bbdoc: Wait for the user to press the specified key.
	End Rem
	Method wait(char:String = Null)
	
		If char Then
			waitCDKLabel(widgetPtr, char[0])
		Else
			waitCDKLabel(widgetPtr, Null)
		End If
	
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetText(text:String[])
		clearMessage()
		
		messageCount = text.length
		messagePtr = arrayToCStrings(text)
		
		setCDKLabelMessage(widgetPtr, messagePtr, messageCount)
	End Method
	
	Method clearMessage()
		If messagePtr Then
			freeCStringArray(messagePtr, messageCount)
			messagePtr = Null
		End If
	End Method

	Method free()
	
		clearMessage()
	
		Super.free()
		
	End Method

	Method getType:Int()
		Return vLABEL
	End Method

End Type

