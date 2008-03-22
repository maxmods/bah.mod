SuperStrict

Framework wx.wxApp
Import BaH.XLWriter

New MyApp.run()

Type MyApp Extends wxApp

	Method OnInit:Int()
	
		Local doc:TXLDocument = New TXLDocument.Create()
		
		Local sheet:TXLWorksheet = doc.Workbook().AddWorksheet("Boxes")
		
		sheet.Cell(1, 1).Style().Border().Outline().SetStyle()
		
		sheet.Cell(2, 2).Style().Border().Outline().SetStyle()
		sheet.Cell(2, 2).Style().Border().Outline().Color().SetRGB(255, 0, 0)

		sheet.Cell(3, 3).Style().Border().Outline().SetStyle(XLSTYLE_BORDER_DASHED)
		sheet.Cell(3, 3).Style().Border().Outline().Color().SetRGB(0, 0, 255)

		sheet.Cell(4, 4).Style().Border().Outline().SetStyle(XLSTYLE_BORDER_MEDIUMDASHDOTDOT)
		sheet.Cell(4, 4).Style().Border().Outline().Color().SetRGB(0, 255, 0)

		sheet.Cell(5, 5).Style().Border().Outline().SetStyle(XLSTYLE_BORDER_DOTTED)
		sheet.Cell(5, 5).Style().Border().Outline().Color().SetRGB(0, 255, 255)
		
		doc.Save("example2")
	
		OpenURL("example2.xlsx")
		
		Return False
	
	End Method


End Type

