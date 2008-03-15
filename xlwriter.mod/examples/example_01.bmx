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
		
		
		doc.Save("example1")
	
		OpenURL("example1.xlsx")
		
		Return False
	
	End Method


End Type
