' Copyright (c) 2013 Bruce A Henderson
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

Rem
bbdoc: Excel .xls Reader/Writer
End Rem
Module BaH.xls

ModuleInfo "Version: 1.08"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: ExcelFormat - Code Project Open License (CPOL)"
ModuleInfo "Copyright: ExcelFormat - 2009, 2010, 2011 Martin Fuchs"
ModuleInfo "Copyright: Wrapper - 2013 Bruce A Henderson"


ModuleInfo "CC_OPTS: -fexceptions"

Import "common.bmx"

Rem
bbdoc: 
End Rem
Type TBasicExcel

	Field xlsPtr:Byte Ptr

	Rem
	bbdoc: Create or open an excel workbook.
	End Rem
	Method Create:TBasicExcel(xls:Object = Null)
		If Not xls Then
			xlsPtr = bmx_xls_basicexcel_Create()
		Else If String(xls) Then
			xlsPtr = bmx_xls_basicexcel_CreateFromFile(String(xls))
		End If

		Return Self
	End Method

	Rem
	bbdoc: Create a new Excel workbook with a given number of spreadsheets (Minimum 1).
	End Rem
	Method NewWorkbook(sheets:Int = 3)
		bmx_xls_basicexcel_New(xlsPtr, sheets)
	End Method

	Rem
	bbdoc: Save current Excel workbook to opened file.
	End Rem
	Method Save:Int()
		Return bmx_xls_basicexcel_Save(xlsPtr)
	End Method

	Rem
	bbdoc: Save current Excel workbook to a file.
	End Rem
	Method SaveAs:Int(filename:String)
		Return bmx_xls_basicexcel_SaveAs(xlsPtr, filename)
	End Method

	Rem
	bbdoc: Closes the current workbook.
	End Rem
	Method Close()
		bmx_xls_basicexcel_Close(xlsPtr)
	End Method

	Rem
	bbdoc: Returns total number of Excel worksheets in current Excel workbook.
	End Rem
	Method GetTotalWorksheets:Int()
		Return bmx_xls_basicexcel_GetTotalWorkSheets(xlsPtr)
	End Method

	Rem
	bbdoc: Returns an Excel worksheet at the given index.
	about: @index starts from 0. Returns Null if @index is invalid.
	End Rem
	Method GetWorksheet:TBasicExcelWorksheet(index:Int)
		Return TBasicExcelWorksheet._create(bmx_xls_basicexcel_GetWorksheet(xlsPtr, index))
	End Method

	Method GetWorksheetByName:TBasicExcelWorksheet(name:String)
		Return TBasicExcelWorksheet._create(bmx_xls_basicexcel_GetWorksheetByName(xlsPtr, name))
	End Method

	Method AddWorksheet:TBasicExcelWorksheet(index:Int = -1)
		Return TBasicExcelWorksheet._create(bmx_xls_basicexcel_AddWorksheet(xlsPtr, index))
	End Method

	Method AddWorksheetByName:TBasicExcelWorksheet(name:String, index:Int = -1)
		Return TBasicExcelWorksheet._create(bmx_xls_basicexcel_AddWorksheetByName(xlsPtr, name, index))
	End Method

	Method GetSheetName:String(index:Int)
	End Method

	Method RenameWorksheet:Int(index:Int, toName:String)
	End Method

	Method RenameWorksheetByName:Int(fromName:String, toName:String)
	End Method

End Type

Type TBasicExcelWorksheet

	Field sheetPtr:Byte Ptr

	Function _create:TBasicExcelWorksheet(sheetPtr:Byte Ptr)
		If sheetPtr Then
			Local this:TBasicExcelWorksheet = New TBasicExcelWorksheet
			this.sheetPtr = sheetPtr
			Return this
		End If

		Return Null
	End Function

	Rem
	bbdoc: 
	End Rem
	Method GetSheetName:String()
	End Method

	Rem
	bbdoc: Total number of rows in current Excel worksheet.
	End Rem
	Method GetTotalRows:Int()
		Return bmx_xls_basicexcelworksheet_GetTotalRows(sheetPtr)
	End Method

	Rem
	bbdoc: Total number of columns in current Excel worksheet.
	End Rem
	Method GetTotalCols:Int()
		Return bmx_xls_basicexcelworksheet_GetTotalCols(sheetPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Cell:TBasicExcelCell(row:Int, col:Int)
		Return TBasicExcelCell._create(bmx_xls_basicexcelworksheet_Cell(sheetPtr, row, col))
	End Method

	Rem
	bbdoc: 
	End Rem
	Method EraseCell:Int(row:Int, col:Int)
		Return bmx_xls_basicexcelworksheet_EraseCell(sheetPtr, row, col)
	End Method

	Method SetColWidth(col:Int, colWidth:Int)
	End Method

	Method GetColWidth:Int(col:Int)
	End Method

	Method MergeCells(row:Int, col:Int, rowRange:Int, colRange:Int)
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TBasicExcelCell

	Field cellPtr:Byte Ptr

	Function _create:TBasicExcelCell(cellPtr:Byte Ptr)
		If cellPtr Then
			Local this:TBasicExcelCell = New TBasicExcelCell
			this.cellPtr = cellPtr
			Return this
		End If

		Return Null
	End Function

	Rem
	bbdoc: Sets content of current Excel cell to an Int.
	End Rem
	Method Set(value:Int)
		bmx_xls_basicexcelcell_Set(cellPtr, value)
	End Method

	Rem
	bbdoc: Sets content of current Excel cell to a Double.
	End Rem
	Method SetDouble(value:Double)
		bmx_xls_basicexcelcell_SetDouble(cellPtr, value)
	End Method

	Rem
	bbdoc: Sets content of current Excel cell to a String.
	End Rem
	Method SetText(value:String)
		bmx_xls_basicexcelcell_SetText(cellPtr, value)
	End Method

	Rem
	bbdoc: Gets an integer value.
	returns: 0 if cell does not contain an integer.
	End Rem
	Method Get:Int()
		Return bmx_xls_basicexcelcell_Get(cellPtr)
	End Method

	Rem
	bbdoc: Gets a double value.
	returns: 0 if cell does not contain a double.
	End Rem
	Method GetDouble:Double()
		Return bmx_xls_basicexcelcell_GetDouble(cellPtr)
	End Method

	Rem
	bbdoc: Gets a String value.
	returns: Null if cell does not contain a string.
	End Rem
	Method GetText:String()
		Return bmx_xls_basicexcelcell_GetText(cellPtr)
	End Method

	Rem
	bbdoc: Erases the content of current Excel cell.
	End Rem
	Method EraseContents()
		bmx_xls_basicexcelcell_EraseContents(cellPtr)
	End Method

	Method GetMergedRows:Int()
	End Method

	Method GetMergedColumns:Int()
	End Method

	Method SetMergedRows(rows:Int)
	End Method

	Method SetMergedColumns(cols:Int)
	End Method

	Method SetFormat(format:TCellFormat)
	End Method

End Type

Type TExcelFont

	Field fontPtr:Byte Ptr

	Method Create:TExcelFont()
		fontPtr = bmx_xls_excelfont_create()
		return Self
	End Method


	Method Delete()
		If fontPtr Then
			bmx_xls_excelfont_free(fontPtr)
			fontPtr = Null
		End If
	End Method

End Type

Type TXLSFormatManager

	Field mgrPtr:Byte Ptr

End Type

Type TCellFormat

	Field formatPtr:Byte Ptr

End Type



