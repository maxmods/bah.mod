' Copyright (c) 2006-2009 Bruce A Henderson
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

Import BRL.LinkedList

' Enhanced List
Type TEList Extends TList
	
	Method insertElementAt:TLink( value:Object, index:Int )
		Local link:TLink=_head._succ
		
		' if there aren't any entries yet, insert it...
		If link = _head Then
			Return InsertBeforeLink( value, _head )
		End If

		While link<>_head
			' insert it at the specified position...
			If Not index Then
				Return InsertBeforeLink(value, link)
			End If
			link=link._succ
			index:- 1
		Wend

		' come to the end of the list... insert it...
		Return InsertBeforeLink(value, link)

	End Method
	
	Method setElementAt( value:Object, index:Int )
		Local link:TLink=_head._succ
		Local l:Int = index
		
		While link<>_head
			If Not index Then
				link._value = value
				Return
			End If
			link=link._succ
			index:- 1
		Wend

		If Not index Then
			link._value = value
			Return
		End If

	End Method

	Method removeElementAt( index:Int )
		Local link:TLink=_head._succ
		
		While link<>_head
			If Not index Then
				link.Remove
				Return
			End If
			link=link._succ
			index:- 1
		Wend
	End Method

	Method addAll(list:TList)
		For Local obj:Object = EachIn list
			addLast(obj)
		Next
	End Method
End Type


