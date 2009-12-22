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
Type TCursesEntry Extends TCursesWidget

	Field titlePtr:Byte Ptr
	Field labelPtr:Byte Ptr
	
	Rem
	bbdoc: 
	End Rem
	Function Create:TCursesEntry(x:Int, y:Int, title:String, label:String, attribute:Int, ..
			filler:String, displayType:Int, width:Int, fieldMin:Int, fieldMax:Int, box:Int = True, ..
			shadow:Int = False)
	
		Local this:TCursesEntry = New TCursesEntry
		
		this.init(x, y, title, label, attribute, filler, displayType, width, fieldMin, fieldMax, box, shadow)

		If Not this.widgetPtr Then
			this.free()
			Return Null
		End If
		
		TCDKDriver.currentScreen.addWidget(this)

		Return this
	
	End Function
	
	Method Init(x:Int, y:Int, title:String, label:String, attribute:Int, ..
			filler:String = ".", displayType:Int, width:Int, fieldMin:Int, fieldMax:Int, box:Int, ..
			shadow:Int)

		If title Then
			titlePtr = title.toCString()
		End If
		labelPtr = label.toCString()
		
		If title Then
			widgetPtr = newCDKEntry(TCDKDriver.currentScreen.screenPtr, x, y, titlePtr, labelPtr, attribute, ..
				filler[0], displayType, width, fieldMin, fieldMax, box, shadow)
		Else
			widgetPtr = newCDKEntry(TCDKDriver.currentScreen.screenPtr, x, y, Null, labelPtr, attribute, ..
				filler[0], displayType, width, fieldMin, fieldMax, box, shadow)
		End If
		
	
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Activate:String(actions:Int = 0)
	
		Local s:Byte Ptr = activateCDKEntry(widgetPtr, actions)
		
		If s Then
			Return String.fromCString(s)
		End If
		
		Return Null
	End Method
	
	Rem
	bbdoc: Returns the exit type.
	about: Can be one of vEARLY_EXIT, vESCAPE_HIT, vNORMAL or vNEVER_ACTIVATED.
	End Rem
	Method exitType:Int()
		Return bmx_entry_exitType(widgetPtr)
	End Method

	Rem
	bbdoc: Sets various attributes of the entry field.
	End Rem
	Method setEntry(value:String, fieldMin:Int, fieldMax:Int, box:Int = True)
		setCDKEntry(widgetPtr, value, fieldMin, fieldMax, box)
	End Method
	
	Rem
	bbdoc: Sets the value of the entry field.
	End Rem
	Method setValue(value:String)
		setCDKEntryValue(widgetPtr, value)
	End Method
	
	Rem
	bbdoc: Gets the current value of the entry field.
	End Rem
	Method getValue:String()
		Local s:Byte Ptr = getCDKEntryValue(widgetPtr)
		If s Then
			Return String.fromCString(s)
		End If
	End Method
	
	Rem
	bbdoc: This sets the maximum length of the string allowable.
	End Rem
	Method setMax(fieldMax:Int)
		setCDKEntryMax(widgetPtr, fieldMax)
	End Method
	
	Rem
	bbdoc: This sets the minimum length of the string allowable.
	End Rem
	Method setMin(fieldMin:Int)
		setCDKEntryMin(widgetPtr, fieldMin)
	End Method
	
	Method show()
		bmx_curses_drawCDKEntry(widgetPtr)
	End Method

	Rem
	bbdoc: Enables positionin
	End Rem
	Method position()
		bmx_curses_positionCDKEntry(widgetPtr)
	End Method
	
	Method hide()
		bmx_curses_eraseCDKEntry(widgetPtr)
	End Method

	Method move(x:Int, y:Int, relative:Int, refresh:Int)
	End Method

	Method injectCharacter(char:Int)
		bmx_curses_injectCDKEntry(widgetPtr, char)
	End Method

	Method getWindow:Byte Ptr()
		Return bmx_curses_CDKEntry_window(widgetPtr)
	End Method

	Method free()
	
		If titlePtr Then
			MemFree(titlePtr)
			titlePtr = Null
		End If
		
		If labelPtr Then
			MemFree(labelPtr)
			labelPtr = Null
		End If
	
		Super.free()
		
	End Method

	Method getType:Int()
		Return vENTRY
	End Method


End Type


