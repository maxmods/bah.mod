' Copyright (c) 2008 Bruce A Henderson
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
bbdoc: XLSX writer.
End Rem
Module BaH.XLWriter

ModuleInfo "Version: 1.00"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2008 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release"

Import BaH.libxml
Import BaH.RegEx
Import BRL.LinkedList
Import BRL.Map
Import BRL.RamStream
Import BRL.TextStream
Import wx.wxZipOutputStream
Import wx.wxZipEntry

Incbin "data/theme1.xml"

Rem
bbdoc: An XLSX document.
End Rem
Type TXLDocument

	Field _workbook:TXLWorkbook
	
	Field sharedStrings:TMap = New TMap
	Field styles:TXLStyleManager
	Field stringsModified:Int = False
	
	' properties
	Field creator:String = "Unknown"
	Field lastModifiedBy:String
	Field createDate:String
	Field modifyDate:String
	Field title:String
	Field description:String
	Field subject:String
	Field keywords:String
	Field category:String

	Rem
	bbdoc: Creates a new document.
	End Rem
	Function CreateDocument:TXLDocument()
		Return New TXLDocument.Create()
	End Function

	Rem
	bbdoc: Creates a new document.
	End Rem
	Method Create:TXLDocument()
		_workbook = New TXLWorkbook.Create(Self)
		styles = New TXLStyleManager.Create(Self)
		
		Return Self
	End Method

	Function newXmlDoc:TxmlDoc()
		Local doc:TxmlDoc = TxmlDoc.newDoc("1.0")
		doc.setStandalone(True)
		
		Return doc
	End Function

	Rem
	bbdoc: Returns the TXLWorkbook for this document.
	End Rem
	Method Workbook:TXLWorkbook()
		Return _workbook
	End Method

	Rem
	bbdoc: Saves the document to @filename.
	End Rem
	Method Save(filename:String)
	
		Local out:wxOutputStream = New wxFileOutputStream.Create(filename + ".xlsx")
		Local zip:wxZipOutputStream = New wxZipOutputStream.Create(out)
		Local stream:wxTextOutputStream = New wxTextOutputStream.Create(zip) 

		Local path:String = ""

		' do content_types
		SaveContentTypes(path, zip, stream)
		
		SaveSharedStrings(path, zip, stream)
		
		SaveDocRels(path, zip, stream)
		
		SaveDocProps(path, zip, stream)
		
		styles.Save(path, zip, stream)
		
		' save the workbook etc
		_workbook.Save(path, zip, stream)
		
		zip.Close()
		out.Close()
		
		stream.Free()
		zip.Free()
		out.Free()
	End Method

	' internal
	Method SetSharedString(value:String)
		sharedStrings.Insert(value, New TXLint)
		stringsModified = True
	End Method
	
	' internal
	Method GetSharedStringId:Int(value:String)
		If stringsModified Then
			Local index:Int
			For Local key:String = EachIn sharedStrings.Keys()
				Local num:TXLint = TXLint(sharedStrings.ValueForKey(key))
				num.value = index
				index:+1
			Next
			stringsModified = False
		End If
		
		Local i:TXLInt = TXLint(sharedStrings.ValueForKey(value))
		If i Then
			Return i.value
		End If
		Return 0
	End Method
	
	' internal
	Method SaveContentTypes(path:String, zip:wxZipOutputStream, stream:wxTextOutputStream)

		Local doc:TxmlDoc = newXmlDoc()
		Local node:TxmlNode = TxmlNode(Doc.GetFirstChild())

		Local root:TxmlNode = TxmlNode.newNode("Types")
		doc.SetRootElement(root)
		
		root.SetAttribute("xmlns", "http://schemas.openxmlformats.org/package/2006/content-types")
		
		Local extNode:TxmlNode = root.AddChild("Default")
		extNode.SetAttribute("Extension", "rels")
		extNode.SetAttribute("ContentType", "application/vnd.openxmlformats-package.relationships+xml")
		
		extNode = root.AddChild("Default")
		extNode.SetAttribute("Extension", "xml")
		extNode.SetAttribute("ContentType", "application/xml")

		extNode = root.AddChild("Default")
		extNode.SetAttribute("Extension", "vml")
		extNode.SetAttribute("ContentType", "application/vnd.openxmlformats-officedocument.vmlDrawing")
		
		' workbook
		extNode = root.AddChild("Override")
		extNode.SetAttribute("PartName", "/xl/workbook.xml")
		extNode.SetAttribute("ContentType", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml")

		' shared strings
		extNode = root.AddChild("Override")
		extNode.SetAttribute("PartName", "/xl/sharedStrings.xml")
		extNode.SetAttribute("ContentType", "application/vnd.openxmlformats-officedocument.spreadsheetml.sharedStrings+xml")

		' app.xml
		extNode = root.AddChild("Override")
		extNode.SetAttribute("PartName", "/docProps/app.xml")
		extNode.SetAttribute("ContentType", "application/vnd.openxmlformats-officedocument.extended-properties+xml")

		' core.xml
		extNode = root.AddChild("Override")
		extNode.SetAttribute("PartName", "/docProps/core.xml")
		extNode.SetAttribute("ContentType", "application/vnd.openxmlformats-package.core-properties+xml")

		' styles.xml
		extNode = root.AddChild("Override")
		extNode.SetAttribute("PartName", "/xl/styles.xml")
		extNode.SetAttribute("ContentType", "application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml")

		' theme1.xml
		extNode = root.AddChild("Override")
		extNode.SetAttribute("PartName", "/xl/theme/theme1.xml")
		extNode.SetAttribute("ContentType", "application/vnd.openxmlformats-officedocument.theme+xml")

		' sheets...
		For Local i:Int = 0 Until _workbook.sheets.count()
			extNode:TxmlNode = root.AddChild("Override")
			extNode.SetAttribute("PartName", "/xl/worksheets/sheet" + (i + 1) + ".xml")
			extNode.SetAttribute("ContentType", "application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml")
		Next


		zip.PutNextEntry(path + "[Content_Types].xml")
		stream.WriteString(doc.ToString())
		
		doc.Free()
	End Method
	
	' internal
	Method SaveSharedStrings(path:String, zip:wxZipOutputStream, stream:wxTextOutputStream)
	
		path:+ "xl/"

		Local doc:TxmlDoc = newXmlDoc()
		Local root:TxmlNode = TxmlNode.newNode("sst")
		doc.SetRootElement(root)
		
		root.SetAttribute("xmlns", "http://schemas.openxmlformats.org/spreadsheetml/2006/main")

		Local count:Int

		For Local text:String = EachIn sharedStrings.Keys()
			Local siNode:TxmlNode = root.AddChild("si")
			Local tNode:TxmlNode = siNode.AddChild("t")
			tNode.SetContent(text)
			count:+ 1
		Next

		root.SetAttribute("uniqueCount", count)

		zip.PutNextEntry(path + "sharedStrings.xml")
		stream.WriteString(doc.ToString())
		
		doc.Free()

	End Method
	
	' internal
	Method SaveDocRels(path:String, zip:wxZipOutputStream, stream:wxTextOutputStream)
		path:+ "_rels/"
		
		Local doc:TxmlDoc = newXmlDoc()
		Local root:TxmlNode = TxmlNode.newNode("Relationships")
		doc.SetRootElement(root)
		
		root.SetAttribute("xmlns", "http://schemas.openxmlformats.org/package/2006/relationships")
		
		Local relNode:TxmlNode = root.AddChild("Relationship")
		relNode.SetAttribute("Id", "rId3")
		relNode.SetAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties")
		relNode.SetAttribute("Target", "docProps/app.xml")

		relNode = root.AddChild("Relationship")
		relNode.SetAttribute("Id", "rId2")
		relNode.SetAttribute("Type", "http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties")
		relNode.SetAttribute("Target", "docProps/core.xml")

		relNode = root.AddChild("Relationship")
		relNode.SetAttribute("Id", "rId1")
		relNode.SetAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument")
		relNode.SetAttribute("Target", "xl/workbook.xml")

		zip.PutNextEntry(path + ".rels")
		stream.WriteString(doc.ToString())
		
		doc.Free()
	End Method
	
	' internal
	Method SaveDocProps(path:String, zip:wxZipOutputStream, stream:wxTextOutputStream)

		path:+ "docProps/"

		SaveDocPropsApp(path, zip, stream)
		SaveDocPropsCore(path, zip, stream)
		
	End Method
	
	' internal
	Method SaveDocPropsApp(path:String, zip:wxZipOutputStream, stream:wxTextOutputStream)
		Local doc:TxmlDoc = newXmlDoc()
		Local root:TxmlNode = TxmlNode.newNode("Properties")
		doc.SetRootElement(root)
		
		root.SetAttribute("xmlns", "http://schemas.openxmlformats.org/officeDocument/2006/extended-properties")
		root.SetAttribute("xmlns:vt", "http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes")
		
		SaveApplication(root)
		SaveDocSecurity(root)
		SaveScaleCrop(root)
		SaveHeadingPairs(root)
		SaveTitlesOfParts(root)
		SaveLinksUpToDate(root)
		SaveSharedDoc(root)
		SaveAppVersion(root)

		zip.PutNextEntry(path + "app.xml")
		stream.WriteString(doc.ToString())
		
		doc.Free()
	End Method

	' internal
	Method SaveDocPropsCore(path:String, zip:wxZipOutputStream, stream:wxTextOutputStream)
		Local doc:TxmlDoc = newXmlDoc()
		Local root:TxmlNode = TxmlNode.newNode("cp:coreProperties")
		doc.SetRootElement(root)
		
		root.SetAttribute("xmlns:cp", "http://schemas.openxmlformats.org/package/2006/metadata/core-properties")
		root.SetAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/")
		root.SetAttribute("xmlns:dcterms", "http://purl.org/dc/terms/")
		root.SetAttribute("xmlns:dcmitype", "http://purl.org/dc/dcmitype/")
		root.SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance")
		
		Local node:TxmlNode = root.AddChild("dc:creator")
		node.SetContent(creator)

		If lastModifiedBy Then
			node = root.AddChild("cp:lastModifiedBy")
			node.SetContent(lastModifiedBy)
		End If

		If createDate Then
			node = root.AddChild("dcterms:created")
			'node.SetContent(lastModifiedBy)
		End If

		If modifyDate Then
			node = root.AddChild("dcterms:modified")
			'node.SetContent(lastModifiedBy)
		End If

		If title Then
			node = root.AddChild("dc:title")
			node.SetContent(title)
		End If

		If description Then
			node = root.AddChild("dc:description")
			node.SetContent(description)
		End If

		If subject Then
			node = root.AddChild("dc:subject")
			node.SetContent(subject)
		End If

		If keywords Then
			node = root.AddChild("cp:keywords")
			node.SetContent(keywords)
		End If

		If category Then
			node = root.AddChild("cp:category")
			node.SetContent(category)
		End If

		zip.PutNextEntry(path + "core.xml")
		stream.WriteString(doc.ToString())
		
		doc.Free()
	End Method
	
	' internal
	Method SaveApplication(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("Application")
		node.SetContent("Microsoft Excel")
	End Method

	' internal
	Method SaveDocSecurity(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("DocSecurity")
		node.SetContent(0)
	End Method

	' internal
	Method SaveScaleCrop(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("ScaleCrop")
		node.SetContent("false")
	End Method
	
	' internal
	Method SaveHeadingPairs(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("HeadingPairs")
		
		Local vecNode:TxmlNode = node.AddChild("vt:vector")
		vecNode.SetAttribute("size", 2)
		vecNode.SetAttribute("baseType", "variant")
		
		Local varNode:TxmlNode = vecNode.AddChild("vt:variant")
		Local vtNode:TxmlNode = varNode.AddChild("vt:lpstr")
		vtNode.SetContent("Worksheets")

		varNode = vecNode.AddChild("vt:variant")
		vtNode = varNode.AddChild("vt:i4")
		vtNode.SetContent(_workbook.sheets.count())
	End Method

	' internal
	Method SaveTitlesOfParts(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("TitlesOfParts")
		
		Local vecNode:TxmlNode = node.AddChild("vt:vector")
		vecNode.SetAttribute("size", _workbook.sheets.count())
		vecNode.SetAttribute("baseType", "lpstr")
		
		For Local sheet:TXLWorksheet = EachIn _workbook.sheets
			Local vtNode:TxmlNode = vecNode.AddChild("vt:lpstr")
			vtNode.SetContent(sheet.name)
		Next
		
	End Method

	' internal
	Method SaveLinksUpToDate(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("LinksUpToDate")
		node.SetContent("false")
	End Method

	' internal
	Method SaveSharedDoc(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("SharedDoc")
		node.SetContent("false")
	End Method
	
	' internal
	Method SaveAppVersion(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("AppVersion")
		node.SetContent("12.0000")
	End Method
	
End Type

Rem
bbdoc: A document workbook.
about: A workbook is composed of book-level properties and a collection of 1 or more sheets.
The sheets are the central working surface for a spreadsheet application. The workbook part and
corresponding properties comprise data used to set application and workbook-level operational
state. The workbook also serves to bind all the sheets and child objects into an organized
single file. The workbook properties include information about what application last saved
the file, where and how the windows of the workbook were positioned, and an enumeration of
the worksheets in the workbook. 
End Rem
Type TXLWorkbook

	Field doc:TXLDocument

	Field sheets:TList = New TList
	Field _activeSheet:TXLWorksheet

	Method Create:TXLWorkbook(doc:TXLDocument)
		Self.doc = doc
		Return Self
	End Method

	Rem
	bbdoc: Creates and returns the new worksheet.
	about: Sets this worksheet as the active sheet.
	End Rem
	Method AddWorksheet:TXLWorksheet(name:String)
		Local sheet:TXLWorksheet = New TXLWorksheet.Create(name, doc)
		sheets.AddLast(sheet)
		_activeSheet = sheet
		Return sheet
	End Method
	
	Rem
	bbdoc: Returns the active worksheet.
	End Rem
	Method ActiveSheet:TXLWorksheet()
		Return _activeSheet
	End Method
	
	Rem
	bbdoc: Sets the active worksheet to that indexed by @index.
	End Rem
	Method SetActiveSheet:TXLWorksheet(index:Int)
		_activeSheet = TXLWorksheet(sheets.ValueAtIndex(index))
		Return ActiveSheet()
	End Method
	
	Rem
	bbdoc: Sets the active worksheet to that named by @name.
	End Rem
	Method SetActiveSheetByName:TXLWorksheet(name:String)
		For Local sheet:TXLWorksheet = EachIn sheets
			If sheet.name = name Then
				_activeSheet = sheet
				Return ActiveSheet()
			End If
		Next
		Return Null
	End Method

	' internal
	Method Save(dir:String, zip:wxZipOutputStream, stream:wxTextOutputStream)
	
		dir:+ "xl/"
		
		SaveWorkbookRels(dir, zip, stream)

		SaveTheme(dir, zip, stream)
	
		Local doc:TxmlDoc = TXLDocument.newXmlDoc()
		Local root:TxmlNode = TxmlNode.newNode("workbook")
		doc.SetRootElement(root)
		
		root.setAttribute("xml:space", "preserve")
		root.setAttribute("xmlns", "http://schemas.openxmlformats.org/spreadsheetml/2006/main")
		root.setAttribute("xmlns:r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships")
		
		' set the file version
		Local verNode:TxmlNode = root.AddChild("fileVersion")
		verNode.SetAttribute("appName", "xl")
		verNode.SetAttribute("lastEdited", "4")
		verNode.SetAttribute("lowestEdited", "4")
		verNode.SetAttribute("rupBuild", "4505")
		
		' add the sheets
		Local node:TxmlNode = root.addChild("sheets")
		
		For Local sheet:TXLWorksheet = EachIn sheets
			sheet.Save(dir, node, zip, stream)
		Next


		zip.PutNextEntry(dir + "workbook.xml")
		stream.WriteString(doc.ToString())
		
		doc.Free()
	End Method

	' internal
	Method SaveWorkbookRels(path:String, zip:wxZipOutputStream, stream:wxTextOutputStream)
		path:+ "_rels/"

		
		Local doc:TxmlDoc = TXLDocument.newXmlDoc()
		Local root:TxmlNode = TxmlNode.newNode("Relationships")
		doc.SetRootElement(root)
		
		root.SetAttribute("xmlns", "http://schemas.openxmlformats.org/package/2006/relationships")
		
		Local relNode:TxmlNode = root.AddChild("Relationship")
		relNode.SetAttribute("Id", "rId1")
		relNode.SetAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/sharedStrings")
		relNode.SetAttribute("Target", "sharedStrings.xml")

		' styles
		relNode = root.AddChild("Relationship")
		relNode.SetAttribute("Id", "rId2")
		relNode.SetAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles")
		relNode.SetAttribute("Target", "styles.xml")

		' themes
		relNode = root.AddChild("Relationship")
		relNode.SetAttribute("Id", "rId3")
		relNode.SetAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/theme")
		relNode.SetAttribute("Target", "theme/theme1.xml")

		For Local sheet:TXLWorksheet = EachIn sheets
			relNode = root.AddChild("Relationship")
			relNode.SetAttribute("Id", "rId" + sheet.relId)
			relNode.SetAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet")
			relNode.SetAttribute("Target", "worksheets/sheet" + sheet.id + ".xml")
		Next
		
		'doc.SaveFormatFile(path + "workbook.xml.rels", True)
		zip.PutNextEntry(path + "workbook.xml.rels")
		stream.WriteString(doc.ToString())
		
		doc.Free()
	End Method

	' internal
	Method SaveTheme(path:String, zip:wxZipOutputStream, stream:wxTextOutputStream)

		path:+ "theme/"

		'Local doc:TxmlDoc = TXLDocument.newXmlDoc()
		'Local root:TxmlNode = TxmlNode.newNode("a:theme")
		'doc.SetRootElement(root)
		
		'root.SetAttribute("xmlns:a", "http://schemas.openxmlformats.org/drawingml/2006/main")
		'root.SetAttribute("name", "Office Theme")

		'doc.SaveFormatFile(path + "theme1.xml", True)
		zip.PutNextEntry(path + "theme1.xml")
		stream.WriteString(LoadText("incbin::data/theme1.xml"))
		
		'doc.Free()
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TXLWorksheet

	Field doc:TXLDocument

	Global ids:Int = 0

	Field name:String
	Field id:Int
	Field relId:Int
	
	Field selected:Int
	
	Field cells:TXLCellManager = New TXLCellManager
	Field columns:TMap

	' internal
	Method Create:TXLWorksheet(name:String, doc:TXLDocument)
		id = ids + 1
		relId = id + 3
		ids:+ 1
		Self.name = name
		Self.doc = doc
		
		If id = 1 Then
			selected = True
		End If
		
		Return Self
	End Method
	
	Rem
	bbdoc: Get a cell by @row and @col.
	End Rem
	Method Cell:TXLCell(row:Int, col:Int)
		Return cells.GetCell(row, col, Self)
	End Method
	
	Rem
	bbdoc: Get a cell by its @address (eg. B7)
	End Rem
	Method CellA:TXLCell(address:String)
	End Method
	
	Rem
	bbdoc: Returns the style for the cell at @row, @col.
	End Rem
	Method Style:TXLStyle(row:Int, col:Int)
		Return cells.GetStyle(row, col)
	End Method
	
	Rem
	bbdoc: Returns the style for the cell at @address
	End Rem
	Method StyleA:TXLStyle(address:String)
	End Method
	
	Rem
	bbddoc: Returns the row at position @r.
	End Rem
	Method Row:TXLRow(r:Int)
		Return cells.GetRow(r, Self)
	End Method
	
	Method HasStyle:Int(row:Int, col:Int)
		If GetStyleId(row, col) Then
			Return True
		End If
	End Method
	
	Method GetStyleId:Int(row:Int, col:Int)
		Local tmp:TXLTmp = TXLTmp(doc.styles.styles.ValueForKey(cells.GetStyle(row, col).GetKey()))
		If tmp Then
			Return tmp.id
		End If
	End Method
		
	Rem
	bbdoc: Returns the column at position @col.
	End Rem
	Method Column:TXLColumn(col:Int)
		If Not columns Then
			columns = New TMap
		End If
		
		Local colInd:TXLInt = TXLInt.Set(col)
		Local c:TXLColumn = TXLColumn(columns.ValueForKey(colInd))
		
		If Not c Then
			c = New TXLColumn.Create(col, Self)
			columns.Insert(colInd, c)
		End If
		
		Return c
	End Method

	' internal
	Method Save(dir:String, wbNode:TxmlNode, zip:wxZipOutputStream, stream:wxTextOutputStream)
	
		dir:+ "worksheets/"
	
		Local node:TxmlNode = wbNode.AddChild("sheet")
		
		node.SetAttribute("name", name)
		node.SetAttribute("sheetId", id)
		node.SetAttribute("r:id", "rId" + relId)
		
		' the worksheet document ...
		Local doc:TxmlDoc = TxmlDoc.newDoc("1.0")
		Local root:TxmlNode = TxmlNode.newNode("worksheet")
		doc.SetRootElement(root)
		
		root.setAttribute("xmlns", "http://schemas.openxmlformats.org/spreadsheetml/2006/main")
		
		cells.SaveDimension(root)
		
		Local viewNode:TxmlNode = root.AddChild("sheetViews")
		node = viewNode.AddChild("sheetView")
		If selected Then
			node.SetAttribute("tabSelected", "1")
		End If
		node.SetAttribute("workbookViewId", "0")
		
		' do cols ?
		If columns Then
			Local colNode:TxmlNode = root.AddChild("cols")
			For Local c:TXLColumn = EachIn columns.Values()
				c.Save(colNode)
			Next
		End If
		
		
		Local dataNode:TxmlNode = root.addChild("sheetData")
		
		cells.Save(dataNode)


		zip.PutNextEntry(dir + "sheet" + id + ".xml")
		stream.WriteString(doc.ToString())
		
		doc.Free()
	End Method
	
End Type

Type TXLCellManager

	Field rows:TMap = New TMap
	Field styles:TMap = New TMap
	
	Field minRow:Int = 0
	Field minCol:Int = 0
	Field maxRow:Int = 0
	Field maxCol:Int = 0

	Method GetCell:TXLCell(row:Int, col:Int, sheet:TXLWorksheet)
		Local xlRow:TXLRow = GetRow(row, sheet)
		
		If Not minCol Then
			minCol = col
			maxCol = col
		End If

		minCol = Min(minCol, col)
		maxCol = Max(maxCol, col)

		Return xlRow.GetCell(col, sheet)
	End Method
	
	Method GetRow:TXLRow(row:Int, sheet:TXLWorksheet)
	
		If Not minRow Then
			minRow = row
			maxRow = row
		End If
		
		' get row
		Local tmpRow:TXLRow = TXLRow.Create(row)
		Local xlRow:TXLRow = TXLRow(rows.ValueForKey(tmpRow))

		If Not xlRow Then
			tmpRow.sheet = sheet
			minRow = Min(minRow, row)
			maxRow = Max(maxRow, row)

			rows.Insert(tmpRow, tmpRow)
			xlRow = tmpRow
		End If
		
		Return xlRow
	End Method

	Method GetStyle:TXLStyle(row:Int, col:Int)
		' get style
		Local addr:String = XLCellAddress(row, col)
		Local style:TXLStyle = TXLStyle(styles.ValueForKey(addr))

		If Not style Then
			style = New TXLStyle
			styles.Insert(addr, style)
		End If
		
		Return style
	End Method

	Method Save(parent:TxmlNode)
		
		For Local row:TXLRow = EachIn rows.keys()
		
			row.Save(parent)
		
		Next
		
	End Method
	
	Method SaveDimension(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("dimension")
		If minRow Then
			node.SetAttribute("ref", XLCellAddress(minRow, minCol) + ":" + XLCellAddress(maxRow, maxCol))
		Else
			node.SetAttribute("ref", "A1:A1")
		End If
	End Method
	
End Type

Type TXLTmp
	Field id:Int
	Field obj:Object
	Function Create:TXLTmp(id:Int, obj:Object)
		Local this:TXLTmp = New TXLTmp
		this.id = id
		this.obj = obj
		Return this
	End Function
End Type

Type TXLStyleManager
	
	Field _doc:TXLDocument
	
	Field fonts:TMap
	Field fontsList:TList
	
	Field borders:TMap
	Field bordersList:TList
	
	Field fills:TMap
	Field fillsList:TList

	Field styles:TMap
	Field stylesList:TList

	Method Create:TXLStyleManager(doc:TXLDocument)
		_doc = doc
		Return Self
	End Method
	
	Method BuildMappings()
	
		fonts = New TMap
		fontsList = New TList

		borders = New TMap
		bordersList = New TList

		fills = New TMap
		fillsList = New TList
		
		styles = New TMap
		stylesList = New TList
		
		BuildDefaults()
		
		For Local sheet:TXLWorksheet = EachIn _doc._workbook.sheets
		
			For Local style:TXLStyle = EachIn sheet.cells.styles.Values()
			
				Local styleKey:String = style.GetKey()
				If Not styles.Contains(styleKey) Then
					styles.Insert(styleKey, TXLTmp.Create(stylesList.Count(), style)) ' key and example
					stylesList.AddLast(styleKey)
				End If

				Local fontKey:String = style.Font().GetKey()
				If Not fonts.Contains(fontKey)
					fonts.Insert(fontKey, TXLTmp.Create(fontsList.Count(), style.Font())) ' key and example
					fontsList.AddLast(fontKey)
				End If
			
				Local borderKey:String = style.Border().GetKey()
				If Not borders.Contains(borderKey) Then
					borders.Insert(borderKey, TXLTmp.Create(bordersList.Count(), style.Border())) ' key and example
					bordersList.AddLast(borderKey)
				End If

				Local fillKey:String = style.Fill().GetKey()
				If Not fills.Contains(fillKey) Then
					fills.Insert(fillKey, TXLTmp.Create(fillsList.Count(), style.Fill())) ' key and example
					fillsList.AddLast(fillKey)
				End If

			Next
		
		Next

	End Method
	
	Method BuildDefaults()
		Local font:TXLFont = New TXLFont
		fonts.Insert(font.GetKey(), TXLTmp.Create(0, font))
		fontsList.AddLast(font.GetKey())
		
		Local border:TXLBorder = New TXLBorder
		borders.Insert(border.GetKey(), TXLTmp.Create(0, border))
		bordersList.AddLast(border.GetKey())

		Local fill:TXLFill = New TXLFill
		fills.Insert(fill.GetKey(), TXLTmp.Create(0, fill))
		fillsList.AddLast(fill.GetKey())
		
		Local style:TXLStyle = New TXLStyle
		styles.Insert(style.GetKey(), TXLTmp.Create(0, style))
		stylesList.AddLast(style.GetKey())
		
	End Method

	
	Method Save(path:String, zip:wxZipOutputStream, stream:wxTextOutputStream)
		
		path:+ "xl/"
		
		BuildMappings()
	

		Local doc:TxmlDoc = TXLDocument.newXmlDoc()
		Local root:TxmlNode = TxmlNode.newNode("styleSheet")
		doc.SetRootElement(root)

		root.SetAttribute("xmlns", "http://schemas.openxmlformats.org/spreadsheetml/2006/main")

		' TODO
		'SaveNumFormats(root)
		
		' fonts
		Local fontsNode:TxmlNode = root.AddChild("fonts")
		fontsNode.SetAttribute("count", fontsList.Count())
		For Local key:String = EachIn fontsList
			SaveFont(fontsNode, key)
		Next
		
		' fills
		SaveFills(root)
		
		' borders
		Local bordersNode:TxmlNode = root.AddChild("borders")
		bordersNode.SetAttribute("count", bordersList.Count())
		For Local key:String = EachIn bordersList
			SaveBorder(bordersNode, key)
		Next
		
		SaveCellStyleXfs(root)
		SaveCellXfs(root)
		SaveCellStyles(root)
		
		
		Local node:TxmlNode = root.AddChild("tableStyles")
		node.SetAttribute("defaultTableStyle", "TableStyleMedium9")
		node.SetAttribute("defaultPivotStyle", "PivotTableStyle1")

		zip.PutNextEntry(path + "styles.xml")
		stream.WriteString(doc.ToString())
		
		doc.Free()
	End Method
	
	Method SaveNumFormats(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("numFmts")
		node.SetAttribute("count", 1)
		
		node = node.AddChild("numFmt")
		node.SetAttribute("numFmtId", 164)
		node.SetAttribute("formatCode", "General")
	End Method
	
	Method SaveFont(parent:TxmlNode, key:String)
		Local font:TXLFont = TXLFont(TXLTmp(fonts.ValueForKey(key)).obj)
		
		font.Save(parent)

	End Method
	
	Method SaveFills(parent:TxmlNode)
		Local fillsNode:TxmlNode = parent.AddChild("fills")
		fillsNode.SetAttribute("count", 1)
		Local node:TxmlNode = fillsNode.AddChild("fill")
		
		node = node.AddChild("patternFill")
		node.SetAttribute("patternType", "none")

	End Method
	
	Method SaveBorder(parent:TxmlNode, key:String)
		Local borderNode:TxmlNode = parent.AddChild("border")
		Local border:TXLBorder = TXLBorder(TXLTmp(borders.ValueForKey(key)).obj)

		border.Save(borderNode)		
	End Method

	Method SaveCellStyleXfs(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("cellStyleXfs")
		node.SetAttribute("count", 1)

		node = node.AddChild("xf")
		node.SetAttribute("numFmtId", "0")
		node.SetAttribute("fontId", "0")
		node.SetAttribute("fillId", "0")
		node.SetAttribute("borderId", "0")
	End Method
	
	Method SaveCellXfs(parent:TxmlNode)
		Local xfsNode:TxmlNode = parent.AddChild("cellXfs")
		xfsNode.SetAttribute("count", stylesList.Count())

		For Local key:String = EachIn stylesList
			Local style:TXLStyle = TXLStyle(TXLTmp(styles.ValueForKey(key)).obj)
		
			Local node:TxmlNode = xfsNode.AddChild("xf")
			node.SetAttribute("xfId", "0")
			
			node.SetAttribute("fontId", TXLTmp(fonts.ValueForKey(style.Font().GetKey())).id)
			If style._numberFormat And style._numberFormat.formatId Then
				node.SetAttribute("numFmtId", style._numberFormat.formatId)
			Else
				node.SetAttribute("numFmtId", "0")
			End If
			node.SetAttribute("fillId", TXLTmp(fills.ValueForKey(style.Fill().GetKey())).id)
			node.SetAttribute("borderId", TXLTmp(borders.ValueForKey(style.Border().GetKey())).id)
			
			If style._alignment Then
				style._alignment.Save(node)
			End If
		Next
	End Method
	
	Method SaveCellStyles(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("cellStyles")
		node.SetAttribute("count", 1)

		node = node.AddChild("cellStyle")
		node.SetAttribute("name", "Normal")
		node.SetAttribute("xfId", "0")
		node.SetAttribute("builtinId", "0")
	End Method

End Type

Type TXLStyleBase

	Field _key:String
	
	Method GetKey:String() Abstract
	
End Type

Rem
bbdoc: Contains formatting information for the contents of the cells on a sheet.
End Rem
Type TXLStyle Extends TXLStyleBase

	Field _font:TXLFont = New TXLFont
	Field _border:TXLBorder = New TXLBorder
	Field _fill:TXLFill = New TXLFill
	Field _alignment:TXLAlignment
	Field _numberFormat:TXLNumberFormat
	
	Rem
	bbdoc: Returns the font for this style.
	End Rem
	Method Font:TXLFont()
		Return _font
	End Method
	
	Rem
	bbdoc: Returns the border for this style.
	End Rem
	Method Border:TXLBorder()
		Return _border
	End Method
	
	Rem
	bbdoc: Returns the fill settings for this style.
	End Rem
	Method Fill:TXLFill()
		Return _fill
	End Method
	
	Rem
	bbdoc: Returns the alignment for this style.
	End Rem
	Method Alignment:TXLAlignment()
		If Not _alignment Then
			_alignment = New TXLAlignment
		End If
		Return _alignment
	End Method

	Rem
	bbdoc: Returns the number formatting for this style.
	End Rem
	Method Format:TXLNumberFormat(formatId:Int = 0, formatCode:String = Null)
		If Not _numberFormat Then
			_numberFormat = New TXLNumberFormat
			If formatId Then
				_numberFormat.formatId = formatId
			End If
			If formatCode Then
				_numberFormat.formatCode = formatCode
			End If
		End If
		Return _numberFormat
	End Method

	Method GetKey:String()
		If Not _key Then
			_key =_font.GetKey() + "_" + _border.GetKey() + "_" + _fill.GetKey()
			_key:+ "_"
			If _alignment Then
				_key:+ _alignment.GetKey()
			End If
			_key:+ "_"
			If _numberFormat Then
				_key:+ _numberFormat.GetKey()
			End If
		End If
		Return _key
	End Method
	
End Type

Rem
bbdoc: A worksheet row.
End Rem
Type TXLRow

	Field row:Int
	Field cells:TMap
	
	Field collapsed:Int
	Field hidden:Int
	Field height:Double
	Field outlineLevel:Int
	Field thickBot:Int
	Field thickTop:Int
	
	Field sheet:TXLWorksheet
	
	Method GetCell:TXLCell(col:Int, sheet:TXLWorksheet)
		If Not cells Then
			cells = New TMap
		End If
		
		Local tmpCol:TXLInt = TXLInt.Set(col)
		Local cell:TXLCell = TXLCell(cells.ValueForKey(tmpCol))
		
		If Not cell Then
			cell = New TXLCell.Create(row, col, sheet)
			cells.Insert(tmpCol, cell)
		End If
		
		Return cell
	End Method
	
	Function Create:TXLRow(row:Int)
		Local this:TXLRow = New TXLRow
		this.row = row
		Return this
	End Function

	Method Compare:Int(obj:Object)
		If TXLRow(obj) Then
			If row < TXLRow(obj).row Then
				Return -1
			Else If row > TXLRow(obj).row Then
				Return 1
			End If
		End If
		Return 0
	End Method
	
	Rem
	bbdoc: Sets the row height, in points.
	End Rem
	Method SetHeight(height:Double)
		Self.height = height
	End Method
	
	Rem
	bbdoc: Returns and enables the TXLStyle for this row
	End Rem
	Method Style:TXLStyle()
		Return sheet.Style(row, 0)
	End Method

	' internal
	Method Save(parent:TxmlNode)
		
		Local rowNode:TxmlNode = parent.AddChild("row")
		rowNode.SetAttribute("r", row)

		If sheet.HasStyle(row, 0) Then
			rowNode.SetAttribute("customFormat", "1")
			rowNode.SetAttribute("s", sheet.GetStyleId(row, 0))
		End If
		
		If height Then
			rowNode.SetAttribute("customHeight", 1)
			rowNode.SetAttribute("ht", height)
		End If
		
		For Local cell:TXLCell = EachIn cells.Values()
		
			cell.Save(rowNode)
		
		Next
		
	End Method
	
End Type

Rem
bbdoc: Defines column width and column formatting for a column of the worksheet. 
End Rem
Type TXLColumn

	Field index:Int
	Field width:Float = -1
	Field bestFit:Int = False
	Field visible:Int = True
	Field collapsed:Int = False
	
	Field sheet:TXLWorksheet

	Method Create:TXLColumn(index:Int, sheet:TXLWorksheet)
		Self.index = index
		Self.sheet = sheet
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetWidth(width:Float)
		Self.width = width
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetBestFit(bestFit:Int = True)
		Self.bestFit = bestFit
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetVisible(visible:Int = True)
		Self.visible = visible
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetCollapsed(collapsed:Int = True)
		Self.collapsed = collapsed
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Style:TXLStyle()
		Return sheet.Style(0, index)
	End Method
	
	
	Method Save(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("col")
		node.SetAttribute("min", index)
		node.SetAttribute("max", index)
		
		If width >= 0 Then
			node.SetAttribute("width", width)
			node.SetAttribute("customWidth", True)
		End If
		
		If bestFit Then
			node.SetAttribute("bestFit", True)
		End If
		
		If Not visible Then
			node.SetAttribute("hidden", True)
		End If

		If collapsed Then
			node.SetAttribute("collapsed", True)
		End If

		If sheet.HasStyle(0, index) Then
			node.SetAttribute("style", sheet.GetStyleId(0, index))
		End If

	End Method
	
End Type

Type TXLint

	Field value:Int
	
	Function Set:TXLInt(value:Int)
		Local this:TXLInt = New TXLInt
		this.value = value
		Return this
	End Function

	Method Compare:Int(obj:Object)
		If TXLInt(obj) Then
			If value < TXLInt(obj).value Then
				Return -1
			Else If value > TXLInt(obj).value Then
				Return 1
			End If
		End If
		Return 0
	End Method

End Type

Rem
bbdoc: A spreadsheet cell
End Rem
Type TXLCell

	Global notNumExp:TRegEx = TRegEx.Create("[^0-9-,.]")
	Global isNumExp:TRegEx = TRegEx.Create("^-[0-9,.]+$|^[0-9,.]+$")

	Field sheet:TXLWorksheet

	Field row:Int
	Field col:Int
	Field value:String
	Field formula:String
	
	Method Create:TXLCell(row:Int, col:Int, sheet:TXLWorksheet)
		Self.row = row
		Self.col = col
		Self.sheet = sheet
		Return Self
	End Method

	Rem
	bbdoc: Sets the value for this cell
	End Rem
	Method SetValue(value:String)
		Self.value = value
		If Not ValueIsNumber() Then
			sheet.doc.SetSharedString(value)
		End If
	End Method
	
	Rem
	bbdoc: Sets the formula for this cell
	End Rem
	Method SetFormula(formula:String)
		Self.formula = formula
	End Method
	
	Rem
	bbdoc: Returns the cell Style.
	End Rem
	Method Style:TXLStyle()
		Return sheet.Style(row, col)
	End Method
	
	Method Save(parent:TxmlNode)

		Local cellNode:TxmlNode = parent.AddChild("c")
		cellNode.SetAttribute("r", ColToLetter(col) + row)
		
		If formula Then
			AddFormula(cellNode)
			
			If value Then
				AddValue(cellNode)
			End If
		Else
			If ValueIsNumber() Then
				AddValue(cellNode)
			Else
				cellNode.SetAttribute("t", "s")
				AddSharedString(cellNode)
			End If
		End If
		
		If sheet.HasStyle(row, col) Then
			cellNode.SetAttribute("s", sheet.GetStyleId(row, col))
		End If
		
	End Method
	
	Method AddInlineString(parent:TxmlNode)
		Local tNode:TxmlNode = parent.AddChild("is")
		Local textNode:TxmlNode = tNode.AddChild("t")
		textNode.SetContent(value)
	End Method
	
	Method AddSharedString(parent:TxmlNode)
		Local valueNode:TxmlNode = parent.AddChild("v")
		valueNode.SetContent(sheet.doc.GetSharedStringId(value))
	End Method

	Method AddValue(parent:TxmlNode)
		Local valueNode:TxmlNode = parent.AddChild("v")
		valueNode.SetContent(value)
	End Method

	Method AddFormula(parent:TxmlNode)
		Local formNode:TxmlNode = parent.AddChild("f")
		formNode.SetContent(formula)
	End Method
	
	Method ValueIsNumber:Int()
		Return (Not notNumExp.Find(value) And isNumExp.Find(value))
	End Method

	Rem
	bbdoc: Converts a column index into the standard spreadsheet letter representation.
	about: For example, 10 converts to J, and 50 converts to AX.
	End Rem
	Function ColToLetter:String(col:Int)
		Local s:String
		Repeat
			col:- 1
			Local n:Int=(col Mod 26) + 65
			s = Chr(n) + s
			col = col / 26
		Until Not col
		Return s
	End Function
	
	Rem
	bbdoc: Converts a column letter representation to an index value.
	about: For example, J converts to 10, and AX converts to 50.
	End Rem
	Function LetterToCol:Int(letter:String)
		Local n:Int
		Local level:Int = 1
		While letter
			n:+ (letter[letter.length-1] - 64) * level
			level:* 26
			letter = letter[..letter.length-1]
		Wend
		Return n
	End Function
	
End Type

Rem
bbdoc: Defines the properties for one of the fonts used in the workbook.
End Rem
Type TXLFont Extends TXLStyleBase

	Field name:String = "Calibri"
	Field size:Int = 10
	Field bold:Int = False
	Field italic:Int = False
	Field underline:Int = 0
	Field strikethrough:Int = False
	Field _color:TXLColor
	Field family:Int
	Field vertAlign:String
	
	Rem
	bbdoc: Sets the face name for the font.
	about: If the font doesn't exist (because it isn't installed on the system), or the charset is invalid for
	that font, then another font should be substituted.
	<p>
	The length for the name should be a maximum of 31 characters.
	</p>
	End Rem
	Method SetName(name:String)
		Self.name = name
	End Method
	
	Rem
	bbdoc: Sets the point size (1/72 of an inch) of the Latin and East Asian text.
	End Rem
	Method SetSize(size:Int)
		Self.size = size
	End Method
	
	Rem
	bbdoc: Sets the bold face font style.
	End Rem
	Method SetBold(bold:Int = True)
		Self.bold = bold
	End Method
	
	Rem
	bbdoc: Displays characters in italic font style.
	about: The italic style is defined by the font at a system level.
	End Rem
	Method SetItalic(italic:Int = True)
		Self.italic = italic
	End Method
	
	Rem
	bbdoc: Sets character underlining.
	End Rem
	Method SetUnderline(underline:Int = True)
		Self.underline = underline
	End Method
	
	Rem
	bbdoc: Draws a strikethrough line through the horizontal middle of the text.
	End Rem
	Method SetStrikethrough(strikethrough:Int = True)
		Self.strikethrough = strikethrough
	End Method
	
	Rem
	bbdoc: Returns the TXLColor object for this font style.
	End Rem
	Method Color:TXLColor()
		If Not _color Then
			_color = New TXLColor
		End If
		Return _color
	End Method
	
	Rem
	bbdoc: Sets the font family this font belongs to.
	about: A font family is a set of fonts having common stroke width and serif characteristics. This is system
	level font information. The font name overrides when there are conflicting values.
	<p>
	Possible values are :
	<ul>
	<li>XLSTYLE_FONTFAM_NONE</li>
	<li>XLSTYLE_FONTFAM_ROMAN</li>
	<li>XLSTYLE_FONTFAM_SWISS</li>
	<li>XLSTYLE_FONTFAM_MODERN</li>
	<li>XLSTYLE_FONTFAM_SCRIPT</li>
	<li>XLSTYLE_FONTFAM_DECORATIVE</li>
	</ul>
	</p>
	End Rem
	Method SetFamily(family:Int)
		Self.family = family
	End Method
	
	Rem
	bbdoc: Adjusts the vertical position of the text relative to the text's default appearance for this run.
	about: It is used to get 'superscript' or 'subscript' texts, and shall reduce the font size (if a smaller
	size is available) accordingly.
	<p>Possible values are :
	<table align="center" wdith="90%">
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>XLSTYLE_FONTVALIGN_BASELINE</td><td>Returns the text in this run to the baseline, default, 
	alignment, and returns it to the original font size.</td></tr>
	<tr><td>XLSTYLE_FONTVALIGN_SUBSCRIPT</td><td>Specifies that this text should be subscript.
		<p>Lowers the text in this run below the baseline and changes it to a smaller size, if a smaller size is available.</p></td></tr>
	<tr><td>XLSTYLE_FONTVALIGN_SUPERSCRIPT</td><td>Specifies that this text should be superscript.
		<p>Raises the text in this run above the baseline and changes it to a smaller size, if a smaller size is available.</p></td></tr>
	</table>
	</p>
	End Rem
	Method SetVertAlign(vertAlign:String)
		Self.vertAlign = vertAlign
	End Method

	Method GetKey:String()
		If Not _key Then
			_key = name + "_" + size + "_" + bold + "_" + italic + "_" + underline + "_" + strikethrough + "_" + ..
				family + "_" + vertAlign + "_"
			If _color Then
				_key:+ _color.GetKey()
			End If
		End If
		Return _key
	End Method	
	
	Method Compare:Int(obj:Object)
		If TXLFont(obj) Then
			If GetKey() < TXLFont(obj).GetKey() Then
				Return -1
			Else If GetKey() > TXLFont(obj).GetKey() Then
				Return 1
			End If
		End If
	End Method
	
	Method Save(parent:TxmlNode)
		Local fontNode:TxmlNode = parent.AddChild("font")

		Local node:TxmlNode = fontNode.AddChild("name")
		node.SetAttribute("val", name)
		
		node = fontNode.AddChild("sz")
		node.SetAttribute("val", size)

		If bold Then
			node = fontNode.AddChild("b")
		End If

		If italic Then
			node = fontNode.AddChild("i")
		End If

		If underline Then
			node = fontNode.AddChild("u")
		End If

		If strikethrough Then
			node = fontNode.AddChild("strike")
		End If
		
		If _color Then
			_color.Save(fontNode)
		End If
		
		If family Then
			node = fontNode.AddChild("family")
			node.SetAttribute("val", family)
		End If

	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TXLBorder Extends TXLStyleBase

	Field bLeft:TXLBorderStyle
	Field bRight:TXLBorderStyle
	Field bTop:TXLBorderStyle
	Field bBottom:TXLBorderStyle
	Field bDiagonal:TXLBorderStyle
	Field bHorizontal:TXLBorderStyle
	Field bVertical:TXLBorderStyle
	
	Field bOutline:TXLBorderOutline
	
	Field diagonalDown:Int
	Field diagonalUp:Int
	Field _outline:Int
	
	Rem
	bbdoc: 
	End Rem
	Method Left:TXLBorderStyle()
		If Not bLeft Then
			bLeft = New TXLBorderStyle
		End If
		Return bLeft
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Right:TXLBorderStyle()
		If Not bRight Then
			bRight = New TXLBorderStyle
		End If
		Return bRight
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Top:TXLBorderStyle()
		If Not bTop Then
			bTop = New TXLBorderStyle
		End If
		Return bTop
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Bottom:TXLBorderStyle()
		If Not bBottom Then
			bBottom = New TXLBorderStyle
		End If
		Return bBottom
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Outline:TXLBorderStyle()
		If Not bOutline Then
			bOutline = New TXLBorderOutline
			bOutline.bLeft = Left()
			bOutline.bRight = Right()
			bOutline.bTop = Top()
			bOutline.bBottom = Bottom()
		End If
		Return bOutline
	End Method


	Method GetKey:String()
		If Not _key Then
			If bLeft Then
				_key:+ bLeft.GetKey() + "_"
			Else
				_key:+ "_"
			End If

			If bRight Then
				_key:+ bRight.GetKey() + "_"
			Else
				_key:+ "_"
			End If

			If bTop Then
				_key:+ bTop.GetKey() + "_"
			Else
				_key:+ "_"
			End If

			If bBottom Then
				_key:+ bBottom.GetKey() + "_"
			Else
				_key:+ "_"
			End If

			If bDiagonal Then
				_key:+ bDiagonal.GetKey() + "_"
			Else
				_key:+ "_"
			End If

			If bHorizontal Then
				_key:+ bHorizontal.GetKey() + "_"
			Else
				_key:+ "_"
			End If

			If bVertical Then
				_key:+ bVertical.GetKey() + "_"
			Else
				_key:+ "_"
			End If

			_key:+ diagonalDown + "_" + diagonalUp + "_" + _outline
		End If
		Return _key
	End Method	

	Method Save(parent:TxmlNode)
		
		Local node:TxmlNode = parent.AddChild("left")
		If bLeft Then
			bLeft.Save(node)
		End If
		
		node = parent.AddChild("right")
		If bRight Then
			bRight.Save(node)
		End If

		node = parent.AddChild("top")
		If bTop Then
			bTop.Save(node)
		End If

		node = parent.AddChild("bottom")
		If bBottom Then
			bBottom.Save(node)
		End If

		node = parent.AddChild("diagonal")
		If bDiagonal Then
			bDiagonal.Save(node)
		End If

	End Method
	
End Type

Rem
bbdoc: Convenience Type for working with all borders of a cell together.
End Rem
Type TXLBorderOutline Extends TXLBorderStyle

	Field bLeft:TXLBorderStyle
	Field bRight:TXLBorderStyle
	Field bTop:TXLBorderStyle
	Field bBottom:TXLBorderStyle
	
	Rem
	bbdoc: 
	End Rem
	Method SetStyle(style:String = XLSTYLE_BORDER_THIN)
		Super.SetStyle(style)
		bLeft.SetStyle(style)
		bRight.SetStyle(style)
		bTop.SetStyle(style)
		bBottom.SetStyle(style)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Color:TXLColor()
		If Not _color Then
			_color = New TXLColor
		End If
		bLeft._color = _color
		bRight._color = _color
		bTop._color = _color
		bBottom._color = _color
		Return _color
	End Method

End Type

Rem
bbdoc: The style and colouring of one edge of a cell border
End Rem
Type TXLBorderStyle Extends TXLStyleBase

	Field _color: TXLColor
	Field _style:String = XLSTYLE_BORDER_NONE

	Method GetKey:String()
		If Not _key Then
			If _color Then
				_key:+ _color.GetKey()
			End If
			_key:+ "_" + _style
		End If
		Return _key
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetStyle(style:String = XLSTYLE_BORDER_THIN)
		_style = style
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Color:TXLColor()
		If Not _color Then
			_color = New TXLColor
		End If
		Return _color
	End Method
	
	Method Save(parent:TxmlNode)
		parent.SetAttribute("style", _style)
		If _color Then
			_color.Save(parent)
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TXLFill Extends TXLStyleBase

	Field _gradientFill:TXLGradientFill
	Field _patternFill:TXLPatternFill
	
	Method GetKey:String()
		If Not _key Then
			If _gradientFill Then
				_key:+ _gradientFill.GetKey()
			End If
			_key:+ "_"
			If _patternFill Then
				_key:+ _patternFill.GetKey()
			End If
		End If
		Return _key
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GradientFill:TXLGradientFill()
		If Not _gradientFill Then
			_gradientFill = New TXLGradientFill
		End If
		Return _gradientFill
	End Method

	Rem
	bbdoc: 
	End Rem
	Method PatternFill:TXLPatternFill()
		If Not _patternFill Then
			_patternFill = New TXLPatternFill
		End If
		Return _patternFill
	End Method
	
End Type

Rem
bbdoc: This element defines a gradient-style cell fill.
about: Gradient cell fills can use one or two colors as the end points of color interpolation.
End Rem
Type TXLGradientFill Extends TXLStyleBase

	Field stops:TList = New TList
	Field _bottom:Double
	Field _degree:Double
	Field _left:Double
	Field _right:Double
	Field _top:Double
	Field _type:String

	Method GetKey:String()
		If Not _key Then
			_key:+ _bottom + "_"
			_key:+ _left + "_"
			_key:+ _right + "_"
			_key:+ _top + "_"
			_key:+ _degree + "_"
			_key:+ _type + "_"
		
			For Local stop:TXLGradientStop = EachIn stops
				_key:+ stop.GetKey()
				_key:+ "_"
			Next
		End If
		Return _key
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method AddStop:TXLGradientStop()
		Local stop:TXLGradientStop = New TXLGradientStop
		stops.AddLast(stop)
		Return stop
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetBottom(value:Double)
		_bottom = value
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetLeft(value:Double)
		_left = value
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetRight(value:Double)
		_right = value
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetTop(value:Double)
		_top = value
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetDegree(value:Double)
		_degree = value
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetType(value:String)
	End Method
	
End Type

Rem
bbdoc: One of a sequence of two or more gradient stops, constituting a gradient fill.
End Rem
Type TXLGradientStop Extends TXLStyleBase

	Field _color:TXLColor
	Field position:Double

	Method GetKey:String()
		If Not _key Then
			If _color Then
				_key:+ _color.GetKey()
			End If
			_key:+ "_"
			_key:+ position
		End If
		Return _key
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Color:TXLColor()
		If _color Then
			_color = New TXLColor
		End If
		Return _color
	End Method
	
	Rem
	bbdoc: Position information for this gradient stop.
	about: Interpreted exactly like gradientFill left, right, bottom, top. The position indicated here indicates
	the point where the color is pure. Before and and after this position the color can be in transition (or pure,
	depending on if this is the last stop or not).
	End Rem
	Method SetPosition(position:Double)
		Self.position = position
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TXLPatternFill Extends TXLStyleBase

	Field _bgColor:TXLColor
	Field _fgColor:TXLColor

	Method GetKey:String()
		If Not _key Then
			If _bgColor Then
				_key:+ _bgColor.GetKey()
			End If
			_key:+ "_"
			If _fgColor Then
				_key:+ _fgColor.GetKey()
			End If
		End If
		Return _key
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method BGColor:TXLColor()
		If Not _bgColor Then
			_bgColor = New TXLColor
		End If
		Return _bgColor
	End Method

	Rem
	bbdoc: 
	End Rem
	Method FGColor:TXLColor()
		If Not _fgColor Then
			_fgColor = New TXLColor
		End If
		Return _fgColor
	End Method

End Type

Rem
bbdoc: One of the colors associated with the data bar or color scale.
End Rem
Type TXLColor Extends TXLStyleBase

	Field _alpha:Int = 255
	Field _r:Int = 0
	Field _g:Int = 0
	Field _b:Int = 0
	Field _tint:Double

	Method GetKey:String()
		If Not _key Then
			_key:+ Hex(_alpha) + "_" + Hex(_r) + "_" + Hex(_g) + "_" + Hex(_b) + "_" + _tint
		End If
		Return _key
	End Method

	Rem
	bbdoc: Sets the red value for the color (0-255).
	End Rem
	Method SetRed(_r:Int)
		Self._r = _r
	End Method

	Rem
	bbdoc: Sets the green value for the color (0-255).
	End Rem
	Method SetGreen(_g:Int)
		Self._g = _g
	End Method

	Rem
	bbdoc: Sets the blue value for the color (0-255).
	End Rem
	Method SetBlue(_b:Int)
		Self._b = _b
	End Method
	
	Rem
	bbdoc: Sets the alpha value for the color (0-255).
	about: Where 0 is transparent and 255 is opaque.
	End Rem
	Method SetAlpha(_alpha:Int)
		Self._alpha = _alpha
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetRGB(_r:Int, _g:Int, _b:Int)
		Self._r = _r
		Self._g = _g
		Self._b = _b
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetRGBColour(colour:wxColour)
		colour.GetRGBA(_r, _g, _b, _alpha)
	End Method
	
	Rem
	bbdoc: Returns the color as a hex-string in the format, ARGB.
	End Rem
	Method GetARGB:String()
		Return Hex(_alpha) + Hex(_r) + Hex(_g) + Hex(_b)
	End Method
	
	Rem
	bbdoc: Specifies the tint value applied to the color.
	about: If tint is supplied, then it is applied to the RGB value of the color to determine the final color applied.
	<p>
	The tint value is stored as a double from -1.0 .. 1.0, where -1.0 means 100% darken and 1.0 means 100% lighten.
	Also, 0.0 means no change.
	</p>
	<p>
	In loading the RGB value, it is converted to HLS where HLS values are (0..HLSMAX), where HLSMAX is currently 255.
	</p>
	End Rem
	Method SetTint(tint:Double)
		_tint = tint
	End Method
	
	Method Save(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("color")
		node.SetAttribute("rgb", GetARGB())
		If _tint Then
			node.SetAttribute("tint", _tint)
		End If
	End Method

	Method Hex:String(val:Int, length:Int = 2)
		Local buf:Short[length]
		For Local k:Int = (length - 1) To 0 Step -1
			Local n:Int = (val&15) + 48
			If n > 57 n:+7
			buf[k]=n
			val:Shr 4
		Next
		Return String.FromShorts( buf, length )
	End Method

End Type

Rem
bbdoc: Formatting information pertaining to text alignment in cells.
about: There are a variety of choices for how text is aligned both horizontally and vertically, as well as
indentation settings, and so on.
End Rem
Type TXLAlignment Extends TXLStyleBase

	Field horizontal:String
	Field indent:Int
	Field justifyLastLine:Int
	Field readingOrder:Int
	Field relativeIndent:Int
	Field shrinkToFit:Int
	Field textRotation:Int
	Field vertical:String
	Field wrapText:Int

	Method GetKey:String()
		If Not _key Then
			If horizontal Then
				_key:+ horizontal
			End If
			_key:+ "_"
			If indent Then
				_key:+ indent
			End If
			_key:+ "_"
			If justifyLastLine Then
				_key:+ justifyLastLine
			End If
			_key:+ "_"
			If readingOrder Then
				_key:+ readingOrder
			End If
			_key:+ "_"
			If relativeIndent Then
				_key:+ relativeIndent
			End If
			_key:+ "_"
			If shrinkToFit Then
				_key:+ shrinkToFit
			End If
			_key:+ "_"
			If textRotation Then
				_key:+ textRotation
			End If
			_key:+ "_"
			If vertical Then
				_key:+ vertical
			End If
			_key:+ "_"
			If wrapText Then
				_key:+ wrapText
			End If
		End If
		Return _key
	End Method
	
	Rem
	bbdoc: Specifies the type of horizontal alignment in cells.
	about: Possible values are :
	<table align="center" wdith="90%">
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>XLSTYLE_HALIGN_CENTER</td><td>The horizontal alignment is centered, meaning the text
is centered across the cell.</td></tr>
	<tr><td>XLSTYLE_HALIGN_CENTERCONTINUOUS</td><td>The horizontal alignment is centered across multiple
cells.</td></tr>
	<tr><td>XLSTYLE_HALIGN_DISTRIBUTED</td><td>Indicates that each 'word' in each line of text inside the cell is
	evenly distributed across the width of the cell, with flush right and left margins.
When there is also an indent value to apply, both the left and right side of the cell are padded by the indent value.
A 'word' is a set of characters with no space character in them. Two lines inside a cell are separated by a carriage return.</td></tr>
	<tr><td>XLSTYLE_HALIGN_FILL</td><td>Indicates that the value of the cell should be filled
across the entire width of the cell. If blank cells to the right also have the fill alignment, they are also filled
with the value, using a convention similar to XLSTYLE_HALIGN_CENTERCONTINUOUS.</td></tr>
	<tr><td>XLSTYLE_HALIGN_GENERAL</td><td>The horizontal alignment is general-aligned. Text data
is left-aligned. Numbers, dates, and times are rightaligned.
Boolean types are centered. Changing the alignment does not change the type of data.</td></tr>
	<tr><td>XLSTYLE_HALIGN_JUSTIFY</td><td>The horizontal alignment is justified (flush left and
right). For each line of text, aligns each line of the wrapped text in a cell to the right and left (except the
last line). If no single line of text wraps in the cell, then the text is not justified.</td></tr>
	<tr><td>XLSTYLE_HALIGN_LEFT</td><td>The horizontal alignment is left-aligned, even in Rightto-
Left mode. Aligns contents at the left edge of the
cell. If an indent amount is specified, the contents of
the cell is indented from the left by the specified
number of character spaces. The character spaces are
based on the default font and font size for the
workbook.</td></tr>
	<tr><td>XLSTYLE_HALIGN_RIGHT</td><td>The horizontal alignment is right-aligned, meaning that
cell contents are aligned at the right edge of the cell, even in Right-to-Left mode.</td></tr>
	</table>
	End Rem
	Method SetHorizontal(horizontal:String = XLSTYLE_HALIGN_GENERAL)
		Self.horizontal = horizontal
	End Method

	Rem
	bbdoc: An integer value, where an increment of 1 represents 3 spaces.
	about: Indicates the number of spaces (of the normal style font) of indentation for text in a cell. The number
	of spaces to indent is calculated as following:
	<pre>
	Number of spaces to indent = indent value * 3
	</pre>
	End Rem
	Method SetIndent(indent:Int)
		Self.indent = indent
	End Method

	Rem
	bbdoc: A boolean value indicating if the cells justified or distributed alignment should be used on the last line of text.
	about: This is typical for East Asian alignments but not typical in other contexts.
	End Rem
	Method SetJustifyLastLine(justifyLastLine:Int)
		Self.justifyLastLine = justifyLastLine
	End Method

	Rem
	bbdoc: An integer value indicating whether the reading order (bidirectionality) of the cell is left-to-right, right-to-left, or context dependent.
	about: 
	<pre>
	0 - Context Dependent
	1 - Left-to-Right
	2 - Right-to-Left
	</pre>
	End Rem
	Method SetReadingOrder(readingOrder:Int)
		Self.readingOrder = readingOrder
	End Method

	Rem
	bbdoc: An integer value to indicate the additional number of spaces of indentation to adjust for text in a cell.
	End Rem
	Method SetRelativeIndent(relativeIndent:Int)
		Self.relativeIndent = relativeIndent
	End Method

	Rem
	bbdoc: A boolean value indicating if the displayed text in the cell should be shrunk to fit the cell width.
	about: Not applicable when a cell contains multiple lines of text.
	End Rem
	Method SetShrinkToFit(shrinkToFit:Int)
		Self.shrinkToFit = shrinkToFit
	End Method

	Rem
	bbdoc: Text rotation in cells.
	about: Expressed in degrees. Values range from 0 to 180. The first letter of the text is considered the center-point of the arc.
	<p>
	For 0 - 90, the value represents degrees above horizon. For 91-180 the degrees below the horizon is calculated as:
	<pre>
	[degrees below horizon] = 90 - textRotation.
	</pre>
	<table align="center" width="90%">
	<tr><th>Angle</th><th>Example</th></tr>
	<tr align="center"><td>0</td><td><img src="text_rot_000.png"/></td></tr>
	<tr align="center"><td>45</td><td><img src="text_rot_045.png"/></td></tr>
	<tr align="center"><td>90</td><td><img src="text_rot_090.png"/></td></tr>
	<tr align="center"><td>135</td><td><img src="text_rot_135.png"/></td></tr>
	<tr align="center"><td>180</td><td><img src="text_rot_180.png"/></td></tr>
	</table>
	</p>
	End Rem
	Method SetTextRotation(textRotation:Int)
		Self.textRotation = textRotation
	End Method
	
	Rem
	bbdoc: Vertical alignment in cells.
	about: Possible values are :
	<table align="center" wdith="90%">
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>XLSTYLE_VALIGN_BOTTOM</td><td>The vertical alignment is aligned-to-bottom.</td></tr>
	<tr><td>XLSTYLE_VALIGN_CENTER</td><td>The vertical alignment is centered across the height of the cell.</td></tr>
	<tr><td>XLSTYLE_VALIGN_DISTRIBUTED</td><td>When text direction is horizontal: the vertical
alignment of lines of text is distributed vertically, where each line of text inside the cell is evenly
distributed across the height of the cell, with flush top and bottom margins.
When text direction is vertical: behaves exactly as distributed horizontal alignment. The first words in a
line of text (appearing at the top of the cell) are flush with the top edge of the cell, and the last words of a
line of text are flush with the bottom edge of the cell, and the line of text is distributed evenly from top to bottom.</td></tr>
	<tr><td>XLSTYLE_VALIGN_JUSTIFY</td><td>When text direction is horizontal: the vertical
alignment of lines of text is distributed vertically, where each line of text inside the cell is evenly
distributed across the height of the cell, with flush top and bottom margins.
When text direction is vertical: similar behavior as horizontal justification. The alignment is justified (flush
top and bottom in this case). For each line of text, each line of the wrapped text in a cell is aligned to the top
and bottom (except the last line). If no single line of text wraps in the cell, then the text is not justified.</td></tr>
	<tr><td>XLSTYLE_VALIGN_TOP</td><td>The vertical alignment is aligned-to-top.</td></tr>
	</table>
	End Rem
	Method SetVertical(vertical:String)
		Self.vertical = vertical
	End Method
	
	Rem
	bbdoc: A boolean value indicating if the text in a cell should be line-wrapped within the cell.
	End Rem
	Method SetWrapText(wrapText:Int)
		Self.wrapText = wrapText
	End Method
	
	Method Save(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("alignment")
		If horizontal Then
			node.SetAttribute("horizontal", horizontal)
		End If

		If indent Then
			node.SetAttribute("indent", indent)
		End If

		If justifyLastLine Then
			node.SetAttribute(justifyLastLine, "true")
		End If

		If readingOrder Then
			node.SetAttribute("readingOrder", readingOrder)
		End If

		If relativeIndent Then
			node.SetAttribute("relativeIndent", relativeIndent)
		End If

		If shrinkToFit Then
			node.SetAttribute("shrinkToFit", "true")
		End If

		If textRotation Then
			node.SetAttribute("textRotation", textRotation)
		End If

		If vertical Then
			node.SetAttribute("vertical", vertical)
		End If

		If wrapText Then
			node.SetAttribute("wrapText", "true")
		End If
	End Method
	
End Type

Rem
bbdoc: Specifies the number format properties which indicate how to format and render the numeric value of a cell.
about: Following is a listing of number formats whose formatCode value is implied rather than explicitly saved in
the file. In this case a numFmtId value is written on the xf record, but no corresponding numFmt element is written.
<table align="center" wdith="90%">
<tr><th>Id</th><th>Format Code</th></tr>
<tr><td>0</td><td>General</td></tr>
<tr><td>1</td><td>0</td></tr>
<tr><td>2</td><td>0.00</td></tr>
<tr><td>3</td><td>#,##0</td></tr>
<tr><td>4</td><td>#,##0.00</td></tr>
<tr><td>9</td><td>0%</td></tr>
<tr><td>10</td><td>0.00%</td></tr>
<tr><td>11</td><td>0.00E+00</td></tr>
<tr><td>12</td><td># ?/?</td></tr>
<tr><td>13</td><td># ??/??</td></tr>
<tr><td>14</td><td>mm-dd-yy</td></tr>
<tr><td>15</td><td>d-mmm-yy</td></tr>
<tr><td>16</td><td>d-mmm</td></tr>
<tr><td>17</td><td>mmm-yy</td></tr>
<tr><td>18</td><td>h:mm AM/PM</td></tr>
<tr><td>19</td><td>h:mm:ss AM/PM</td></tr>
<tr><td>20</td><td>h:mm</td></tr>
<tr><td>21</td><td>h:mm:ss</td></tr>
<tr><td>22</td><td>m/d/yy h:mm</td></tr>
<tr><td>37</td><td>#,##0 ;(#,##0)</td></tr>
<tr><td>38</td><td>#,##0 ;[Red](#,##0)</td></tr>
<tr><td>39</td><td>#,##0.00;(#,##0.00)</td></tr>
<tr><td>40</td><td>#,##0.00;[Red](#,##0.00)</td></tr>
<tr><td>45</td><td>mm:ss</td></tr>
<tr><td>46</td><td>[h]:mm:ss</td></tr>
<tr><td>47</td><td>mmss.0</td></tr>
<tr><td>48</td><td>##0.0E+0</td></tr>
<tr><td>49</td><td>@</td></tr>
</table>
<p>
<b>Number Format Codes</b><br>
Up to four sections of format codes can be specified. The format codes, separated by semicolons, define the
formats for positive numbers, negative numbers, zero values, and text, in that order. If only two sections are
specified, the first is used for positive numbers and zeros, and the second is used for negative numbers. If only
one section is specified, it is used for all numbers. To skip a section, the ending semicolon for that section must
be written.
</p>
<p align="center"><img src="formatcode_format.png"/></p>
<p>
The first section, "Format for positive numbers", is the format code that applies to the cell when the cell value
contains a positive number.
</p>
<p>
The second section, "Format for negative numbers", is the format code that applies to the cell when the cell
value contains a negative number.
</p>
<p>
The third section, "Format for zeros", is the format code that applies to the cell when the cell value is zero.
The fourth, and last, section, "Format for text", is the format code that applies to the cell when the cell value is
text.
</p>
<p>
The &amp; (ampersand) text operator is used to join, or concatenate, two values.
</p>
<p>
The following table describes the different symbols that are available for use in custom number formats.
<table align="center" wdith="90%">
<tr><th>Format Symbol</th><th>Description and result</th></tr>
<tr><td>0</td><td>Digit placeholder. For example, if the value 8.9 is to be displayed as 8.90, use the format
#.00</td></tr>
<tr><td>#</td><td>Digit placeholder. This symbol follows the same rules as the 0 symbol. However, the
application shall not display extra zeros when the number typed has fewer digits on
either side of the decimal than there are # symbols in the format. For example, if the
custom format is #.##, and 8.9 is in the cell, the number 8.9 is displayed.</td></tr>
<tr><td>?</td><td>Digit placeholder. This symbol follows the same rules as the 0 symbol. However, the
application shall put a space for insignificant zeros on either side of the decimal point so
that decimal points are aligned in the column. For example, the custom format 0.0? aligns
the decimal points for the numbers 8.9 and 88.99 in a column.</td></tr>
<tr><td>. (period)</td><td>Decimal point.</td></tr>
<tr><td>%</td><td>Percentage. If the cell contains a number between 0 and 1, and the custom format 0% is
used, the application shall multiply the number by 100 and adds the percentage symbol
in the cell.</td></tr>
<tr><td>, (comma)</td><td>Thousands separator. The application shall separate thousands by commas if the format
contains a comma that is enclosed by number signs (#) or by zeros. A comma that follows
a placeholder scales the number by one thousand. For example, if the format is #.0,, and
the cell value is 12,200,000 then the number 12.2 is displayed.</td></tr>
<tr><td>E- E+ e- e+</td><td>Scientific format. The application shall display a number to the right of the "E" symbol
that corresponds to the number of places that the decimal point was moved. For
example, if the format is 0.00E+00, and the value 12,200,000 is in the cell, the number
1.22E+07 is displayed. If the number format is #0.0E+0, then the number 12.2E+6 is
displayed.</td></tr>
<tr><td>$-+/():space</td><td>Displays the symbol. If it is desired to display a character that differs from one of these
symbols, precede the character with a backslash (\). Alternatively, enclose the character
in quotation marks. For example, if the number format is (000), and the value 12 is in the
cell, the number (012) is displayed.</td></tr>
<tr><td>\</td><td>Display the next character in the format. The application shall not display the backslash.
For example, if the number format is 0\!, and the value 3 is in the cell, the value 3! is
displayed.</td></tr>
<tr><td>*</td><td>Repeat the next character in the format enough times to fill the column to its current
width. There shall not be more than one asterisk in one section of the format. If more
than one asterisk appears in one section of the format, all but the last asterisk shall be
ignored. For example, if the number format is 0*x, and the value 3 is in the cell, the value
3xxxxxx is displayed. The number of x characters that are displayed in the cell varies
based on the width of the column.</td></tr>
<tr><td>_ (underline)</td><td>Skip the width of the next character. This is useful for lining up negative and positive
values in different cells of the same column. For example, the number format
<tt>_(0.0_);(0.0)</tt> aligns the numbers 2.3 and -4.5 in the column even though the negative number is enclosed by parentheses.</td></tr>
<tr><td>"text"</td><td>Display whatever text is inside the quotation marks. For example, the format 0.00
"dollars" displays 1.23 dollars when the value 1.23 is in the cell.</td></tr>
<tr><td>@</td><td>Text placeholder. If text is typed in the cell, the text from the cell is placed in the format
where the at symbol (@) appears. For example, if the number format is "Bob "@" Smith"
(including quotation marks), and the value "John" is in the cell, the value Bob John Smith is displayed.</td></tr>
</table>
</p>
End Rem
Type TXLNumberFormat Extends TXLStyleBase

	Field formatId:Int = 0
	Field formatCode:String

	Method GetKey:String()
		If Not _key Then
			_key:+ formatId
			_key:+ "_"
			_key:+ formatCode
		End If
		Return _key
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetFormatId(formatId:Int)
		Self.formatId = formatId
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetFormatCode(formatCode:String)
		Self.formatCode = formatCode
	End Method
	
End Type

Const XLSTYLE_BORDER_DASHDOT:String = "dashdot"
Const XLSTYLE_BORDER_DASHDOTDOT:String = "dashdotdot"
Const XLSTYLE_BORDER_DASHED:String = "dashed"
Const XLSTYLE_BORDER_DOTTED:String = "dotted"
Const XLSTYLE_BORDER_DOUBLE:String = "double"
Const XLSTYLE_BORDER_HAIR:String = "hair"
Const XLSTYLE_BORDER_MEDIUM:String = "medium"
Const XLSTYLE_BORDER_MEDIUMDASHDOT:String = "mediumDashDot"
Const XLSTYLE_BORDER_MEDIUMDASHDOTDOT:String = "mediumDashDotDot"
Const XLSTYLE_BORDER_MEDIUMDASHED:String = "mediumDashed"
Const XLSTYLE_BORDER_NONE:String = "none"
Const XLSTYLE_BORDER_SLANTDASHDOT:String = "slantDashDot"
Const XLSTYLE_BORDER_THICK:String = "thick"
Const XLSTYLE_BORDER_THIN:String = "thin"

Const XLSTYLE_PATTERN_DARKDOWN:String = "darkDown"
Const XLSTYLE_PATTERN_DARKGRAY:String = "darkGray"
Const XLSTYLE_PATTERN_DARKGRID:String = "darkGrid"
Const XLSTYLE_PATTERN_DARKHORIZONTAL:String = "darkHorizontal"
Const XLSTYLE_PATTERN_DARKTRELLIS:String = "darkTrellis"
Const XLSTYLE_PATTERN_DARKUP:String = "darkUp"
Const XLSTYLE_PATTERN_DARKVERTICAL:String = "darkVertical"
Const XLSTYLE_PATTERN_GRAY0625:String = "gray0625"
Const XLSTYLE_PATTERN_GRAY125:String = "gray125"
Const XLSTYLE_PATTERN_LIGHTDOWN:String = "lightDown"
Const XLSTYLE_PATTERN_LIGHTGRAY:String = "lightGray"
Const XLSTYLE_PATTERN_LIGHTGRID:String = "lightGrid"
Const XLSTYLE_PATTERN_LIGHTHORIZONTAL:String = "lightHorizontal"
Const XLSTYLE_PATTERN_LIGHTTRELLIS:String = "lightTrellis"
Const XLSTYLE_PATTERN_LIGHTUP:String = "lightUp"
Const XLSTYLE_PATTERN_LIGHTVERTICAL:String = "lightVertical"
Const XLSTYLE_PATTERN_MEDIUMGRAY:String = "mediumGray"
Const XLSTYLE_PATTERN_NONE:String = "none"
Const XLSTYLE_PATTERN_SOLID:String = "solid"

Const XLSTYLE_GRADIENT_LINEAR:String = "linear"
Const XLSTYLE_GRADIENT_PATH:String = "path"

Const XLSTYLE_HALIGN_CENTER:String = "center"
Const XLSTYLE_HALIGN_CENTERCONTINUOUS:String = "centerContinuous"
Const XLSTYLE_HALIGN_DISTRIBUTED:String = "distributed"
Const XLSTYLE_HALIGN_FILL:String = "fill"
Const XLSTYLE_HALIGN_GENERAL:String = "general"
Const XLSTYLE_HALIGN_JUSTIFY:String = "justify"
Const XLSTYLE_HALIGN_LEFT:String = "left"
Const XLSTYLE_HALIGN_RIGHT:String = "right"

Const XLSTYLE_VALIGN_BOTTOM:String = "bottom"
Const XLSTYLE_VALIGN_CENTER:String = "center"
Const XLSTYLE_VALIGN_DISTRIBUTED:String = "distributed"
Const XLSTYLE_VALIGN_JUSTIFY:String = "justify"
Const XLSTYLE_VALIGN_TOP:String = "top"

Const XLSTYLE_FONTFAM_NONE:Int = 0
Const XLSTYLE_FONTFAM_ROMAN:Int = 1
Const XLSTYLE_FONTFAM_SWISS:Int = 2
Const XLSTYLE_FONTFAM_MODERN:Int = 3
Const XLSTYLE_FONTFAM_SCRIPT:Int = 4
Const XLSTYLE_FONTFAM_DECORATIVE:Int = 5

Const XLSTYLE_FONTVALIGN_BASELINE:String = "baseline"
Const XLSTYLE_FONTVALIGN_SUBSCRIPT:String = "subscript"
Const XLSTYLE_FONTVALIGN_SUPERSCRIPT:String = "superscript"

Rem
bbdoc: Converts row,col to the standard spreadsheet address of a cell.
about: For example (4,5) converts to E4.
End Rem
Function XLCellAddress:String(row:Int, col:Int)
	Return TXLCell.ColToLetter(col) + row
End Function



