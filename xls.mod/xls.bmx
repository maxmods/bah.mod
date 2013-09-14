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

ModuleInfo "Version: 1.00"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: ExcelFormat - Code Project Open License (CPOL)"
ModuleInfo "Copyright: ExcelFormat - 2009, 2010, 2011 Martin Fuchs"
ModuleInfo "Copyright: Wrapper - 2013 Bruce A Henderson"


ModuleInfo "CC_OPTS: -fexceptions"

Import "common.bmx"

Rem
bbdoc: An Excel workbook.
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
		
		If Not xlsPtr Then
			Return Null
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
	Method SetString(value:String)
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
	Method GetString:String()
		Return bmx_xls_basicexcelcell_GetText(cellPtr)
	End Method

	Rem
	bbdoc: Erases the content of current Excel cell.
	End Rem
	Method EraseContents()
		bmx_xls_basicexcelcell_EraseContents(cellPtr)
	End Method

	Rem
	bbdoc: Returns the number of rows merged to this cell.
	about: 1 means only itself.
	End Rem
	Method GetMergedRows:Int()
		Return bmx_xls_basicexcelcell_GetMergedRows(cellPtr)
	End Method

	Rem
	bbdoc: Returns the number of columns merged to this cell.
	about: 1 means only itself.
	End Rem
	Method GetMergedColumns:Int()
		Return bmx_xls_basicexcelcell_GetMergedColumns(cellPtr)
	End Method

	Rem
	bbdoc: Sets the number of rows merged to this cell.
	about: 1 means only itself.
	End Rem
	Method SetMergedRows(rows:Int)
		bmx_xls_basicexcelcell_SetMergedRows(cellPtr, rows)
	End Method

	Rem
	bbdoc: Sets the number of columns merged to this cell.
	about: 1 means only itself.
	End Rem
	Method SetMergedColumns(cols:Int)
		bmx_xls_basicexcelcell_SetMergedColumns(cellPtr, cols)
	End Method

	Rem
	bbdoc: Sets the format (font, colour, border, etc) for the cell.
	End Rem
	Method SetFormat(format:TCellFormat)
		bmx_xls_basicexcelcell_SetFormat(cellPtr, format.formatPtr)
	End Method
	
	Rem
	bbdoc: Returns the type of value stored in current Excel cell.
	about: One of CELL_UNDEFINED, CELL_INT, CELL_DOUBLE, CELL_STRING, CELL_FORMULA
	End Rem
	Method GetType:Int()
		Return bmx_xls_basicexcelcell_Type(cellPtr)
	End Method

End Type

Rem
bbdoc: An excel font definition.
End Rem
Type TExcelFont

	Field fontPtr:Byte Ptr
	
	Function _create:TExcelFont(fontPtr:Byte Ptr)
		If fontPtr Then
			Local this:TExcelFont = New TExcelFont
			this.fontPtr = fontPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Creates a new, default font.
	End Rem
	Method Create:TExcelFont()
		fontPtr = bmx_xls_excelfont_create()
		Return Self
	End Method
	
	Rem
	bbdoc: Sets the font weight.
	about: For example, FW_NORMAL (400), FW_BOLD (700)
	End Rem
	Method SetWeight(weight:Int)
		bmx_xls_excelfont_SetWeight(fontPtr, weight)
	End Method
	
	Rem
	bbdoc: Sets the font height in twips (1/20 of a point).
	End Rem
	Method SetHeight(height:Int)
		bmx_xls_excelfont_SetHeight(fontPtr, height)
	End Method
	
	Rem
	bbdoc: Sets if the font is italic or not.
	End Rem
	Method SetItalic(italic:Int)
		bmx_xls_excelfont_SetItalic(fontPtr, italic)
	End Method

	Rem
	bbdoc: Sets the font colour index.
	End Rem
	Method SetColorIndex(index:Int)
		bmx_xls_excelfont_SetColorIndex(fontPtr, index)
	End Method
	
	Rem
	bbdoc: Sets the font underline type.
	about: One of EXCEL_UNDERLINE_NONE,	EXCEL_UNDERLINE_SINGLE, EXCEL_UNDERLINE_DOUBLE,
	EXCEL_UNDERLINE_SINGLE_ACCOUNTING or EXCEL_UNDERLINE_DOUBLE_ACCOUNTING.
	End Rem
	Method SetUnderlineType(underline:Int)
		bmx_xls_excelfont_SetUnderlineType(fontPtr, underline)
	End Method
	
	Rem
	bbdoc: Sets the font escapement type.
	about: One of EXCEL_ESCAPEMENT_NONE, EXCEL_ESCAPEMENT_SUPERSCRIPT or EXCEL_ESCAPEMENT_SUBSCRIPT.
	End Rem
	Method SetEscapement(escapement:Int)
		bmx_xls_excelfont_SetEscapement(fontPtr, escapement)
	End Method
	
	Rem
	bbdoc: Sets the font name.
	about: Default is "Arial".
	End Rem
	Method SetFontName(name:String)
		bmx_xls_excelfont_SetFontName(fontPtr, name)
	End Method
	
	Rem
	bbdoc: Sets various font option flags.
	about: One or more of EXCEL_FONT_ITALIC, EXCEL_FONT_STRUCK_OUT, EXCEL_FONT_OUTLINED,
		EXCEL_FONT_SHADOWED, EXCEL_FONT_CONDENSED or EXCEL_FONT_EXTENDED, Or'd together.
	End Rem
	Method SetOptions(options:Int)
		bmx_xls_excelfont_SetOptions(fontPtr, options)
	End Method
	
	Rem
	bbdoc: Gets the currently set font option flags.
	about: One or more of EXCEL_FONT_ITALIC, EXCEL_FONT_STRUCK_OUT, EXCEL_FONT_OUTLINED,
		EXCEL_FONT_SHADOWED, EXCEL_FONT_CONDENSED or EXCEL_FONT_EXTENDED, Or'd together.
	End Rem
	Method GetOptions:Int()
		Return bmx_xls_excelfont_GetOptions(fontPtr)
	End Method

	Method Delete()
		If fontPtr Then
			bmx_xls_excelfont_free(fontPtr)
			fontPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: Formatter Manager
about: Manages fonts and formats within the workbook.
End Rem
Type TXLSFormatManager

	Field mgrPtr:Byte Ptr

	Method Create:TXLSFormatManager(xls:TBasicExcel)
		mgrPtr = bmx_xls_xlsformatmanager_create(xls.xlsPtr)
		Return Self
	End Method
	
	Method Free()
		bmx_xls_xlsformatmanager_free(mgrPtr)
	End Method

End Type

Rem
bbdoc: A cell format definition.
End Rem
Type TCellFormat

	Field formatPtr:Byte Ptr

	Rem
	bbdoc: 
	End Rem
	Method Create:TCellFormat(manager:TXLSFormatManager)
		formatPtr = bmx_xls_cellformat_create(manager.mgrPtr)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetFont:TCellFormat(font:TExcelFont)
		bmx_xls_cellformat_SetFont(formatPtr, font.fontPtr)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the font.
	End Rem
	Method GetFont:TExcelFont()
		Return TExcelFont._create(bmx_xls_cellformat_GetFont(formatPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetFormatString:TCellFormat(text:String)
		bmx_xls_cellformat_SetFormatString(formatPtr, text)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the format string.
	End Rem
	Method GetFormatString:String()
		Return bmx_xls_cellformat_GetFormatString(formatPtr)
	End Method
	
	Rem
	bbdoc: Sets the contents rotation angle.
	End Rem
	Method SetRotation:TCellFormat(rotation:Int)
		bmx_xls_cellformat_SetRotation(formatPtr, rotation)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetRotation:Int()
		Return bmx_xls_cellformat_GetRotation(formatPtr)
	End Method
	
	Method Delete()
		If formatPtr Then
			bmx_xls_cellformat_free(formatPtr)
			formatPtr = Null
		End If
	End Method

End Type



