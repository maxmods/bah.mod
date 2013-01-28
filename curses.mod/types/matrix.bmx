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
Type TCursesMatrix Extends TCursesWidget

	Field titlePtr:Byte Ptr
	Field _rowTitles:Byte Ptr
	Field _rowsCount:Int
	Field _colTitles:Byte Ptr
	Field _colsCount:Int

	Function Create:TCursesMatrix(x:Int, y:Int, rows:Int, cols:Int, vrows:Int, vcols:Int, ..
			title:String, rowtitles:String[] = Null, coltitles:String[] = Null, numbers:Int = False, highlight:Int = A_REVERSE, box:Int = True, shadow:Int = False)

		Local this:TCursesMatrix = New TCursesMatrix

		this.init(x, y, rows, cols, title, rows, cols, buttons, highlight, box, shadow)
		
		If Not this.widgetPtr Then
			this.free()
			Return Null
		End If

		TCDKDriver.currentScreen.addWidget(this)

		Return this
	
	End Function

	Method Init(x:Int, y:Int, rows:Int, cols:Int, vrows:Int, vcols:Int, ..
			title:String, rowtitles:String[], coltitles:String[], numbers:Int, highlight:Int, box:Int, shadow:Int)
	
		rowsCount = rowTitles.length
		_rowTitles = arrayToCStrings(rowTitles)
		colsCount = colTitles.length
		_colTitles = arrayToCStrings(colTitles)

		If title Then
			titlePtr = title.toCString()
		End If
		
		If title Then
			widgetPtr = newCDKMatrix(TCDKDriver.currentScreen.screenPtr, x, y, rows, cols, vrows, vcols, titlePtr, ..
				_rowTitles, buttonCount, highlight, box, shadow)
		Else
			widgetPtr = newCDKMatrix(TCDKDriver.currentScreen.screenPtr, x, y, height, width, Null, rows, ..
				cols, _buttons, buttonCount, highlight, box, shadow)
		End If
			
	
	End Method

	Method show()
		bmx_curses_drawCDKMatrix(widgetPtr)
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
		bmx_curses_injectCDKMatrix(widgetPtr, char)
	End Method

	Method getWindow:Byte Ptr()
		Return bmx_curses_CDKMatrix_window(widgetPtr)
	End Method
	
	Rem
	bbdoc: Returns the row of the matrix.
	End Rem
	Method getRow:Int()
		Return bmx_matrix_getCDKMatrixRow(widgetPtr)
	End Method
	
	Rem
	bbdoc: Returns the col of the matrix.
	End Rem
	Method getCol:Int()
		Return bmx_matrix_getCDKMatrixCol(widgetPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method activate:Int(row:Int, col:Int)
		Return activateCDKMatrix(widgetPtr, actions)
	End Method

	Rem
	bbdoc: Cleans out all the cells from the matrix.
	End Rem
	Method clean()
		bmx_matrix_cleanMatrix(widgetPtr)
	End Method

	Rem
	bbdoc: Cleans one cell in the matrix.
	End Rem
	Method cleanCell(row:Int, col:Int)
		bmx_matrix_cleanMatrixCell(widgetPtr, row, col)
	End Method

	Rem
	bbdoc: Moves the matrix to the given cell.
	End Rem
	Method moveToCell:Int(row:Int, col:Int)
		Return bmx_matrix_moveToCell(widgetPtr, row, col)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setBox(box:Int)
		bmx_matrix_setBox(widgetPtr, box)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method getBox:Int()
		Return bmx_matrix_getBox(widgetPtr)
	End Method

	Method getType:Int()
		Return vMATRIX
	End Method

End Type

