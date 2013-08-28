SuperStrict

Framework BaH.xls

Local xls:TBasicExcel = New TBasicExcel.Create()

' create sheet 1 and get the associated TBasicExcelWorksheet object
xls.NewWorkbook(1)

Local sheet:TBasicExcelWorksheet = xls.GetWorksheet(0)


'Local formatManager:XLSFormatManager = New TXLSFormatManager.Create(xls)


' Create a table containing an header row in bold and four rows below.

'Local fontBold:TExcelFont = New ExcelFont

'fontBold.SetWeight(FW_BOLD)

'Local formatBold:TCellFormat = New CellFormat.Create(fmt_mgr)
'formatBold.SetFont(fontBold)

Local col:Int
Local row:Int

For col = 0 Until 10
	Local cell:TBasicExcelCell = sheet.Cell(row, col)

	cell.SetText("TITLE")
	'cell.SetFormat(formatBold)
Next

row = 1

While row < 5

	For col = 0 Until 10
		sheet.Cell(row, col).SetText("text")
	Next
	
	row:+ 1
Wend



xls.SaveAs("example_01.xls")
