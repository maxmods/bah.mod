SuperStrict

Framework wx.wxApp
Import BaH.XLWriter

New MyApp.run()

Type MyApp Extends wxApp

	Method OnInit:Int()
	
		Local items:String[] = ["Item A", "Item B", "Item C", "Item D", "Item E"]
		Local amounts:Int[] = [9, 8, 7, 6, 5]
		Local prices:Float[] = [6.99, 5.50, 10.0, 25.99, 15.0]
	
		Local doc:TXLDocument = New TXLDocument.Create()
		
		Local sheet:TXLWorksheet = doc.Workbook().AddWorksheet("July 06")
		
		' headings
		sheet.Cell(2, 1).SetValue("Item")
		sheet.Cell(2, 4).SetValue("Price")
		sheet.Cell(2, 5).SetValue("Amount")
		sheet.Cell(2, 6).SetValue("Total")
		
		sheet.Row(2).Style().Alignment().SetHorizontal(XLSTYLE_HALIGN_CENTER)
		sheet.Row(2).Style().Font().SetBold()
		
		' items
		For Local i:Int = 0 Until items.length
			Local row:Int = 3 + i
			sheet.Cell(row, 1).SetValue(items[i])   ' item description
			sheet.Cell(row, 1).Style().Font().Color().SetRGB(0, 0, 255) ' items are blue :-)
			sheet.Cell(row, 4).SetValue(prices[i])  ' price per item
			sheet.Cell(row, 5).SetValue(amounts[i]) ' no. of items
			
			sheet.Cell(row, 6).SetFormula(XLCellAddress(row,4) + "*" + XLCellAddress(row,5))
		Next
		
		Local totalCell:TXLCell = sheet.Cell(9, 5)
		totalCell.SetValue("Total")
		
		Local font:TXLFont = totalCell.Style().Font()
		font.SetBold()
		font.SetName("Arial")
		font.SetSize(12)
		
		' calculate the total
		sheet.Cell(9, 6).SetFormula("SUM(F3:F7)")
		
		' add a top and bottom thin border to the total amount
		sheet.Cell(9, 6).Style().Border().Top().SetStyle()
		sheet.Cell(9, 6).Style().Border().Bottom().SetStyle()
		
		' expand the first column width
		sheet.Column(1).SetWidth(15)
		
		doc.Save("example1")
	
		OpenURL("example1.xlsx")
		
		Return False
	
	End Method


End Type
