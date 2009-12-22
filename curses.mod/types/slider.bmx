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
Type TCursesSlider Extends TCursesWidget
	
	Field titlePtr:Byte Ptr
	Field labelPtr:Byte Ptr
	
	Rem
	bbdoc: 
	about: numbers - show as a numbered list
	End Rem
	Function Create:TCursesSlider(x:Int, y:Int, title:String, label:String, fieldAttribute:Int, ..
			fieldWidth:Int, start:Int, low:Int, high:Int, stepSize:Int, pageSize:Int, ..
			box:Int = True, shadow:Int = False)
			
		Local this:TCursesSlider = New TCursesSlider

		this.init(x, y, title, label, fieldAttribute, fieldWidth, start, low, high, stepSize, pageSize, box, shadow)
		
		If Not this.widgetPtr Then
			this.free()
			Return Null
		End If

		TCDKDriver.currentScreen.addWidget(this)

		Return this
	End Function
	
	Method Init(x:Int, y:Int, title:String, label:String, fieldAttribute:Int, ..
			fieldWidth:Int, start:Int, low:Int, high:Int, stepSize:Int, pageSize:Int, box:Int, shadow:Int)
	
		If title Then
			titlePtr = title.toCString()
		End If

		If label Then
			labelPtr = label.toCString()
		End If
		
		If title Then
		
			If label Then
				widgetPtr = newCDKSlider(TCDKDriver.currentScreen.screenPtr, x, y, titlePtr, labelPtr, fieldAttribute, ..
					fieldWidth, start, low, high, stepSize, pageSize, box, shadow)
			Else
				widgetPtr = newCDKSlider(TCDKDriver.currentScreen.screenPtr, x, y, titlePtr, Null, fieldAttribute, ..
					fieldWidth, start, low, high, stepSize, pageSize, box, shadow)
			End If
		Else
			If label Then
				widgetPtr = newCDKSlider(TCDKDriver.currentScreen.screenPtr, x, y, Null, labelPtr, fieldAttribute, ..
					fieldWidth, start, low, high, stepSize, pageSize, box, shadow)
			Else
				widgetPtr = newCDKSlider(TCDKDriver.currentScreen.screenPtr, x, y, Null, Null, fieldAttribute, ..
					fieldWidth, start, low, high, stepSize, pageSize, box, shadow)
			End If	
		End If
	
	End Method

	Method show()
		'bmx_curses_drawCDKLabel(widgetPtr)
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
	End Method

	Method getWindow:Byte Ptr()
		Return bmx_curses_CDKSlider_window(widgetPtr)
	End Method

	Rem
	bbdoc: Activates the slider.
	End Rem
	Method Activate:Int(actions:Int = 0)
		Return activateCDKSlider(widgetPtr, actions)
	End Method
	
	Rem
	bbdoc: Enables or disables the surrounding box.
	End Rem
	Method setBox(state:Int)
		setCDKSliderBox(widgetPtr, state)
	End Method
	
	Rem
	bbdoc: Returns True if box is enabled.
	End Rem
	Method getBox:Int()
		Return getCDKSliderBox(widgetPtr)
	End Method
	
	Rem
	bbdoc: Sets the current value.
	End Rem
	Method setValue(value:Int)
		setCDKSliderValue(widgetPtr, value)
	End Method
	
	Rem
	bbdoc: Gets the current value.
	End Rem
	Method getValue:Int()
		Return getCDKSliderValue(widgetPtr)
	End Method
	
	Rem
	bbdoc: Sets the low and high values.
	End Rem
	Method setLowHigh(low:Int, high:Int)
		setCDKSliderLowHigh(widgetPtr, low, high)
	End Method
	
	Rem
	bbdoc: Gets the slider low value.
	End Rem
	Method getLow:Int()
		Return getCDKSliderLowValue(widgetPtr)
	End Method
	
	Rem
	bbdoc: Gets the slider high value.
	End Rem
	Method getHigh:Int()
		Return getCDKSliderHighValue(widgetPtr)
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

	Rem
	bbdoc: Returns the exit type.
	about: Can be one of vEARLY_EXIT, vESCAPE_HIT, vNORMAL or vNEVER_ACTIVATED.
	End Rem
	Method exitType:Int()
		Return bmx_slider_exitType(widgetPtr)
	End Method

	Method getType:Int()
		Return vSLIDER
	End Method

End Type
