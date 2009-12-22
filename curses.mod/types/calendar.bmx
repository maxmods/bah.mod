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
Type TCursesCalendar Extends TCursesWidget

	Field titlePtr:Byte Ptr

	Rem
	bbdoc: 
	End Rem
	Function Create:TCursesCalendar(x:Int, y:Int, title:String, _day:Int, _month:Int, _year:Int, dayAttrib:Int, ..
			monthAttrib:Int, yearAttrib:Int, highlight:Int, box:Int = True, shadow:Int = False)
			
		Local this:TCursesCalendar = New TCursesCalendar
		
		this.init(x, y, title, _day, _month, _year, dayAttrib, monthAttrib, yearAttrib, highlight, box, shadow)
		
		If Not this.widgetPtr Then
			this.free()
			Return Null
		End If

		TCDKDriver.currentScreen.addWidget(this)

		Return this
	End Function
	
	Method Init(x:Int, y:Int, title:String, _day:Int, _month:Int, _year:Int, dayAttrib:Int, ..
			monthAttrib:Int, yearAttrib:Int, highlight:Int, box:Int = True, shadow:Int = False)

		If title Then
			titlePtr = title.toCString()
		End If

		If title Then
			widgetPtr = newCDKCalendar(TCDKDriver.currentScreen.screenPtr, x, y, titlePtr, _day, _month, _year, ..
				dayAttrib, monthAttrib, yearAttrib, highlight, box, shadow)
		Else
			widgetPtr = newCDKCalendar(TCDKDriver.currentScreen.screenPtr, x, y, Null, _day, _month, _year, ..
				dayAttrib, monthAttrib, yearAttrib, highlight, box, shadow)
		End If

	End Method

	Method show()
		bmx_curses_drawCDKCalendar(widgetPtr)
	End Method
	
	Method position()
		bmx_curses_positionCDKCalendar(widgetPtr)
	End Method

	Method hide()
		bmx_curses_eraseCDKCalendar(widgetPtr)
	End Method

	Method move(x:Int, y:Int, relative:Int, refresh:Int)
	' TODO
	End Method

	Method injectCharacter(char:Int)
		bmx_curses_injectCDKCalendar(widgetPtr, char)
	End Method

	Method getType:Int()
		Return vCALENDAR
	End Method

	Method getWindow:Byte Ptr()
		Return bmx_curses_CDKCalendar_window(widgetPtr)
	End Method
	
	Rem
	bbdoc: Sets the current date.
	End Rem
	Method setDate(_day:Int, _month:Int, _year:Int)
		setCDKCalendarDate(widgetPtr, _day, _month, _year)
	End Method
	
	Rem
	bbdoc: Returns the current date.
	End Rem
	Method getDate(_day:Int Var, _month:Int Var, _year:Int Var)
		getCDKCalendarDate(widgetPtr, Varptr _day, Varptr _month, Varptr _year)
	End Method
	
	Rem
	bbdoc: Sets the month names.
	End Rem
	Method setMonthNames(months:String[])
		Local listPtr:Byte Ptr = arrayToCStrings(months)
		setCDKCalendarMonthsNames(widgetPtr, listPtr)
		freeCStringArray(listPtr, months.length)
	End Method
	
	Rem
	bbdoc: Sets the day names.
	End Rem
	Method setDayNames(days:String[])
		Local listPtr:Byte Ptr = arrayToCStrings(days)
		setCDKCalendarDaysNames(widgetPtr, listPtr)
		freeCStringArray(listPtr, days.length)
	End Method
	
	Rem
	bbdoc: Sets a marker on the calendar.
	End Rem
	Method setMarker(_day:Int, _month:Int, _year:Int, marker:Int)
		setCDKCalendarMarker(widgetPtr, _day, _month, _year, marker)
	End Method
	
	Rem
	bbdoc: Returns a marker set on the calendar.
	End Rem
	Method getMarker:Int(_day:Int, _month:Int, _year:Int)
		Return getCDKCalendarMarker(widgetPtr, _day, _month, _year)
	End Method
	
	Rem
	bbdoc: Removes a marker from the calendar.
	End Rem
	Method removeMarker(_day:Int, _month:Int, _year:Int)
		removeCDKCalendarMarker(widgetPtr, _day, _month, _year)
	End Method
	
	Method free()
		If titlePtr Then
			MemFree(titlePtr)
			titlePtr = Null
		End If

		Super.free()
	End Method
	
End Type



