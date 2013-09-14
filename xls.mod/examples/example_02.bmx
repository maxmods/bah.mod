SuperStrict

Framework BaH.xls

Local xls:TBasicExcel = New TBasicExcel.Create()

' create sheet 1 and get the associated TBasicExcelWorksheet object
xls.NewWorkbook(1)

Local sheet:TBasicExcelWorksheet = xls.GetWorksheet(0)


Local formatManager:TXLSFormatManager = New TXLSFormatManager.Create(xls)


Local font:TExcelFont = New TExcelFont.Create()

font.SetWeight(FW_BOLD)
font.SetUnderlineType(EXCEL_UNDERLINE_SINGLE)
font.SetFontName("Times New Roman")
font.SetColorIndex(EGA_BLUE)
font.SetOptions(EXCEL_FONT_STRUCK_OUT)

Local format:TCellFormat = New TCellFormat.Create(formatManager)
format.SetFont(font)
format.SetRotation(30) ' rotate the header cell text 30 degrees to the left

Local col:Int
Local row:Int

For col = 0 Until 10
	Local cell:TBasicExcelCell = sheet.Cell(row, col)

	cell.SetString("TITLE")
	cell.SetFormat(format)
Next

row = 1

While row < 10

	For col = 0 Until 10
		sheet.Cell(row, col).SetString("text " + row + "/" + col)
	Next
	
	row:+ 1
Wend



xls.SaveAs("example_02.xls")

