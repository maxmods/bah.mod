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

?win32
Import "-lole32"
?

Import "source.bmx"

Extern

	Function bmx_xls_basicexcel_CreateFromFile:Byte Ptr(file:String)
	Function bmx_xls_basicexcel_Create:Byte Ptr()

	Function bmx_xls_basicexcel_New(handle:Byte Ptr, sheets:Int)
	Function bmx_xls_basicexcel_Save:Int(handle:Byte Ptr)
	Function bmx_xls_basicexcel_SaveAs:Int(handle:Byte Ptr, filename:String)
	Function bmx_xls_basicexcel_Close(handle:Byte Ptr)

	Function bmx_xls_basicexcel_GetTotalWorkSheets:Int(handle:Byte Ptr)
	Function bmx_xls_basicexcel_GetWorksheet:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_xls_basicexcel_GetWorksheetByName:Byte Ptr(handle:Byte Ptr, name:String)
	Function bmx_xls_basicexcel_AddWorksheet:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_xls_basicexcel_AddWorksheetByName:Byte Ptr(handle:Byte Ptr, name:String, index:Int)
 
	Function bmx_xls_basicexcelworksheet_GetTotalRows:Int(handle:Byte Ptr)
	Function bmx_xls_basicexcelworksheet_GetTotalCols:Int(handle:Byte Ptr)
	Function bmx_xls_basicexcelworksheet_Cell:Byte Ptr(handle:Byte Ptr, row:Int, col:Int)
	Function bmx_xls_basicexcelworksheet_EraseCell:Int(handle:Byte Ptr, row:Int, col:Int)

	Function bmx_xls_basicexcelcell_Set(handle:Byte Ptr, value:Int)
	Function bmx_xls_basicexcelcell_SetDouble(handle:Byte Ptr, value:Double)
	Function bmx_xls_basicexcelcell_SetText(handle:Byte Ptr, value:String)
	Function bmx_xls_basicexcelcell_EraseContents(handle:Byte Ptr)
	Function bmx_xls_basicexcelcell_Get:Int(handle:Byte Ptr)
	Function bmx_xls_basicexcelcell_GetDouble:Double(handle:Byte Ptr)
	Function bmx_xls_basicexcelcell_GetText:String(handle:Byte Ptr)
	Function bmx_xls_basicexcelcell_Type:Int(handle:Byte Ptr)
	Function bmx_xls_basicexcelcell_GetMergedRows:Int(handle:Byte Ptr)
	Function bmx_xls_basicexcelcell_GetMergedColumns:Int(handle:Byte Ptr)
	Function bmx_xls_basicexcelcell_SetMergedRows(handle:Byte Ptr, rows:Int)
	Function bmx_xls_basicexcelcell_SetMergedColumns(handle:Byte Ptr, cols:Int)
	Function bmx_xls_basicexcelcell_SetFormat(handle:Byte Ptr, format:Byte Ptr)

	Function bmx_xls_excelfont_create:Byte Ptr()
	Function bmx_xls_excelfont_free(handle:Byte Ptr)
	Function bmx_xls_excelfont_SetWeight(handle:Byte Ptr, weight:Int)
	Function bmx_xls_excelfont_SetHeight(handle:Byte Ptr, height:Int)
	Function bmx_xls_excelfont_SetItalic(handle:Byte Ptr, italic:Int)
	Function bmx_xls_excelfont_SetColorIndex(handle:Byte Ptr, index:Int)
	Function bmx_xls_excelfont_SetUnderlineType(handle:Byte Ptr, underline:Int)
	Function bmx_xls_excelfont_SetEscapement(handle:Byte Ptr, escapement:Int)
	Function bmx_xls_excelfont_SetFontName(handle:Byte Ptr, name:String)
	Function bmx_xls_excelfont_SetOptions(handle:Byte Ptr, options:Int)
	Function bmx_xls_excelfont_GetOptions:Int(handle:Byte Ptr)

	Function bmx_xls_xlsformatmanager_create:Byte Ptr(xls:Byte Ptr)
	Function bmx_xls_xlsformatmanager_free(handle:Byte Ptr)

	Function bmx_xls_cellformat_create:Byte Ptr(mgr:Byte Ptr)
	Function bmx_xls_cellformat_SetFont(handle:Byte Ptr, font:Byte Ptr)
	Function bmx_xls_cellformat_free(handle:Byte Ptr)
	Function bmx_xls_cellformat_GetFont:Byte Ptr(handle:Byte Ptr)
	Function bmx_xls_cellformat_SetFormatString(handle:Byte Ptr, text:String)
	Function bmx_xls_cellformat_GetFormatString:String(handle:Byte Ptr)
	Function bmx_xls_cellformat_SetRotation(handle:Byte Ptr, rotation:Int)
	Function bmx_xls_cellformat_GetRotation:Int(handle:Byte Ptr)

End Extern


Const FW_NORMAL:Int = 400
Const FW_BOLD:Int = 700

Const CELL_UNDEFINED:Int = 0
Const CELL_INT:Int = 1
Const CELL_DOUBLE:Int = 2
Const CELL_STRING:Int = 3
'Const CELL_WSTRING:Int = 4 ' not used for module. GetType() always returns CELL_STRING
Const CELL_FORMULA:Int = 5


Const EXCEL_UNDERLINE_NONE:Int = $00
Const EXCEL_UNDERLINE_SINGLE:Int = $01
Const EXCEL_UNDERLINE_DOUBLE:Int = $02
Const EXCEL_UNDERLINE_SINGLE_ACCOUNTING:Int = $21
Const EXCEL_UNDERLINE_DOUBLE_ACCOUNTING:Int = $22

Const EXCEL_FONT_FAMILY_DONTCARE:Int = 0
Const EXCEL_FONT_FAMILY_ROMAN:Int = 1
Const EXCEL_FONT_FAMILY_SWISS:Int = 2
Const EXCEL_FONT_FAMILY_MODERN:Int = 3
Const EXCEL_FONT_FAMILY_SCRIPT:Int = 4
Const EXCEL_FONT_FAMILY_DECORATIVE:Int = 5

Const EXCEL_ESCAPEMENT_NONE:Int = 0
Const EXCEL_ESCAPEMENT_SUPERSCRIPT:Int = 1
Const EXCEL_ESCAPEMENT_SUBSCRIPT:Int = 2

Rem
bbdoc: Excel colour index, black.
End Rem
Const EGA_BLACK:Int = 0 ' 000000
Rem
bbdoc: Excel colour index, white.
End Rem
Const EGA_WHITE:Int = 1 ' FFFFFF
Rem
bbdoc: Excel colour index, red.
End Rem
Const EGA_RED:Int = 2 ' FF0000
Rem
bbdoc: Excel colour index, green.
End Rem
Const EGA_GREEN:Int = 3 ' 00FF00
Rem
bbdoc: Excel colour index, blue.
End Rem
Const EGA_BLUE:Int = 4 ' 0000FF
Rem
bbdoc: Excel colour index, yellow.
End Rem
Const EGA_YELLOW:Int = 5 ' FFFF00
Rem
bbdoc: Excel colour index, magenta.
End Rem
Const EGA_MAGENTA:Int = 6 ' FF00FF
Rem
bbdoc: Excel colour index, cyan.
End Rem
Const EGA_CYAN:Int = 7 ' 00FFFF

Rem
bbdoc: Italic font option.
End Rem
Const EXCEL_FONT_ITALIC:Int = $02
Rem
bbdoc: Strike-out font option.
End Rem
Const EXCEL_FONT_STRUCK_OUT:Int = $08
Rem
bbdoc: Outline font option.
End Rem
Const EXCEL_FONT_OUTLINED:Int = $10
Rem
bbdoc: Shadow font option.
End Rem
Const EXCEL_FONT_SHADOWED:Int = $20
Rem
bbdoc: Condensed font option.
End Rem
Const EXCEL_FONT_CONDENSED:Int = $40
Rem
bbdoc: Extended style font option.
End Rem
Const EXCEL_FONT_EXTENDED:Int = $80
