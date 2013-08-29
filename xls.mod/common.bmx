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

	Function bmx_xls_excelfont_create:Byte Ptr()
	Function bmx_xls_excelfont_free(handle:Byte Ptr)

End Extern


Const FW_NORMAL:Int = 400
Const FW_BOLD:Int = 700


