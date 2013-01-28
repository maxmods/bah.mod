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
Type TCursesScrollList Extends TCursesWidget

	Field itemList:String[] = New String[0]

	Field listPtr:Byte Ptr
	Field listCount:Int
	
	Field titlePtr:Byte Ptr
	
	Rem
	bbdoc: 
	about: numbers - show as a numbered list
	End Rem
	Function Create:TCursesScrollList(x:Int, y:Int, scrollPos:Int, width:Int, height:Int, ..
			title:String, list:String[] = Null, numbers:Int = False, highlight:Int = A_REVERSE, box:Int = True, shadow:Int = False)
			
		Local this:TCursesScrollList = New TCursesScrollList

		this.init(x, y, scrollPos, width, height, title, list, numbers, highlight, box, shadow)
		
		If Not this.widgetPtr Then
			this.free()
			Return Null
		End If

		TCDKDriver.currentScreen.addWidget(this)

		Return this
	End Function
	
	Method Init(x:Int, y:Int, spos:Int, width:Int, height:Int, title:String, list:String[], numbers:Int, highlight:Int, box:Int, shadow:Int)

		If list And list.length > 0 Then
			itemList = list[0..list.length]
			
			listCount = list.length
			listPtr = arrayToCStrings(list)
		End If

		If title Then
			titlePtr = title.toCString()
		End If

		If title Then
			If listPtr Then
				widgetPtr = newCDKScroll(TCDKDriver.currentScreen.screenPtr, x, y, spos, width, height, titlePtr, listPtr, listCount, numbers, highlight, box, shadow)
			Else
				widgetPtr = newCDKScroll(TCDKDriver.currentScreen.screenPtr, x, y, spos, width, height, titlePtr, Null, 0, numbers, highlight, box, shadow)
			End If
		Else
			If listPtr Then
				widgetPtr = newCDKScroll(TCDKDriver.currentScreen.screenPtr, x, y, spos, width, height, Null, listPtr, listCount, numbers, highlight, box, shadow)
			Else
				widgetPtr = newCDKScroll(TCDKDriver.currentScreen.screenPtr, x, y, spos, width, height, Null, Null, 0, numbers, highlight, box, shadow)
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
		Return bmx_curses_CDKScroll_window(widgetPtr)
	End Method

	Method free()
	
		If listPtr Then
			freeCStringArray(listPtr, listCount)
			listPtr = Null
		End If
		
		If titlePtr Then
			MemFree(titlePtr)
			titlePtr = Null
		End If
	
		Super.free()
		
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Activate:Int(actions:Int = 0)
	
		Return activateCDKScroll(widgetPtr, actions)

	End Method

	Rem
	bbdoc: 
	End Rem
	Method getItem:String(index:Int)
		If index >= 0 And index < itemList.length Then
			Return itemList[index]
		End If
		
		Return Null
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getCurrentIndex:Int()
		Return getCDKScrollCurrentItem(widgetPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setCurrentIndex(index:Int)
		setCDKScrollCurrentItem(widgetPtr, index)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method addItem(item:String)
		If item Then
			Local i:Int = itemList.length
			itemList = itemList[0..i + 1]
			itemList[i] = item
			
			Local s:Byte Ptr = item.toCString()
			addCDKScrollItem(widgetPtr, s)
			MemFree(s)
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method insertItem(item:String, index:Int = -1)
		If item Then
			Local oldIndex:Int = -1
			
			If index < 0 Then
				index = getCurrentIndex()
			Else
				oldIndex = getCurrentIndex()
				setCurrentIndex(index)
			End If
				
			
			Local tmpList:String[] = New String[itemList.length + 1]
			For Local i:Int = 0 Until index
				tmpList[i] = itemList[i]
			Next
			
			tmpList[index] = item
			
			For Local i:Int = index Until itemList.length
				tmpList[i + 1] = itemList[i]
			Next
			
			itemList = tmpList[0..tmpList.length]
		
			Local s:Byte Ptr = item.toCString()
			insertCDKScrollItem(widgetPtr, s)
			MemFree(s)
			
			If oldIndex >= 0 Then
				setCurrentIndex(oldIndex)
			End If
		End If
	End Method
	
	Method deleteItem(index:Int)
		' TODO : remove from itemList...
		
		deleteCDKScrollItem(widgetPtr, index)
		
	End Method
	
	Method getType:Int()
		Return vSCROLL
	End Method	
	
End Type

