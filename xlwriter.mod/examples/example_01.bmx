SuperStrict

Framework wx.wxApp
Import BaH.XLWriter

New MyApp.run()

Type MyApp Extends wxApp

	Method OnInit:Int()
	
		Local doc:TXLDocument = New TXLDocument.Create()
		
		Local sheet:TXLWorksheet = doc.Workbook().AddWorksheet("July 06")
		
		sheet.Cell(7, 1).SetValue("Alpha Hydroxy Toner Mild")
		sheet.Cell(8, 1).SetValue("Alpha Hydroxy Toner Mild")
		sheet.Cell(9, 1).SetValue("Aquatrol")
		
		sheet.Cell(7, 4).SetValue(10.25)
		sheet.Cell(7, 5).SetValue(5)
		' dynamic cell addresses
		sheet.Cell(7, 6).SetFormula(XLCellAddress(7,4) + "*" + XLCellAddress(7,5))

		sheet.Cell(8, 4).SetValue(6.99)
		sheet.Cell(8, 5).SetValue(7)
		' hardcoded addresses
		sheet.Cell(8, 6).SetFormula("D8*E8")
		
		doc.Save("example1")
	
		OpenURL("example1.xlsx")
		
		Return False
	
	End Method


End Type
