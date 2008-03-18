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
bbdoc: 
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
	bbdoc: 
	End Rem
	Method Create:TXLDocument()
		_workbook = New TXLWorkbook.Create(Self)
		styles = New TXLStyleManager.Create(Self)
		
		Return Self
	End Method

	Function newXmlDoc:TxmlDoc()
		Local doc:TxmlDoc = TxmlDoc.newDoc("1.0")
		doc.SetEncoding("UTF-8")
		doc.setStandalone(True)
		
		Return doc
	End Function

	Rem
	bbdoc: 
	End Rem
	Method Workbook:TXLWorkbook()
		Return _workbook
	End Method

	Rem
	bbdoc: 
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


	Method SetSharedString(value:String)
		sharedStrings.Insert(value, New TXLint)
		stringsModified = True
	End Method
	
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
		
		Return TXLint(sharedStrings.ValueForKey(value)).value
	End Method
	
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
	
	Method SaveDocProps(path:String, zip:wxZipOutputStream, stream:wxTextOutputStream)

		path:+ "docProps/"

		SaveDocPropsApp(path, zip, stream)
		SaveDocPropsCore(path, zip, stream)
		
	End Method
	
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
	
	Method SaveApplication(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("Application")
		node.SetContent("Microsoft Excel")
	End Method

	Method SaveDocSecurity(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("DocSecurity")
		node.SetContent(0)
	End Method

	Method SaveScaleCrop(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("ScaleCrop")
		node.SetContent("false")
	End Method
	
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

	Method SaveLinksUpToDate(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("LinksUpToDate")
		node.SetContent("false")
	End Method

	Method SaveSharedDoc(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("SharedDoc")
		node.SetContent("false")
	End Method
	
	Method SaveAppVersion(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("AppVersion")
		node.SetContent("12.0000")
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TXLWorkbook

	Field doc:TXLDocument

	Field sheets:TList = New TList

	Method Create:TXLWorkbook(doc:TXLDocument)
		Self.doc = doc
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddWorksheet:TXLWorksheet(name:String)
		Local sheet:TXLWorksheet = New TXLWorksheet.Create(name, doc)
		sheets.AddLast(sheet)
		Return sheet
	End Method

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
	bbdoc: 
	End Rem
	Method Cell:TXLCell(row:Int, col:Int)
		Return cells.GetCell(row, col, Self)
	End Method
	
	Rem
	bbdoc: Get a cell by its address (eg. B7)
	End Rem
	Method CellA:TXLCell(cell:String)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Style:TXLStyle(row:Int, col:Int)
		Return cells.GetStyle(row, col)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method StyleA:TXLStyle(cell:String)
	End Method
	
	Method HasStyle:Int(row:Int, col:Int)
		Return cells.HasStyle(row, col)
	End Method
	
	Method GetStyleId:Int(row:Int, col:Int)
		Return cells.GetStyleId(row, col)
	End Method
		
	Rem
	bbdoc: 
	End Rem
	Method Column:TXLColumn(col:Int)
		If Not columns Then
			columns = New TMap
		End If
		
		Local colInd:TXLInt = TXLInt.Set(col)
		Local c:TXLColumn = TXLColumn(columns.ValueForKey(colInd))
		
		If Not c Then
			c = New TXLColumn.Create(col)
			columns.Insert(colInd, c)
		End If
		
		Return c
	End Method

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
		If Not minRow Then
			minRow = row
			maxRow = row
			minCol = col
			maxCol = col
		End If
		
		' get row
		Local tmpRow:TXLRow = TXLRow.Create(row)
		Local xlRow:TXLRow = TXLRow(rows.ValueForKey(tmpRow))

		If Not xlRow Then
			minRow = Min(minRow, row)
			maxRow = Max(maxRow, row)

			rows.Insert(tmpRow, tmpRow)
			xlRow = tmpRow
		End If
		
		
		minCol = Min(minCol, col)
		maxCol = Max(maxCol, col)

		Return xlRow.GetCell(col, sheet)
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

	Method HasStyle:Int(row:Int, col:Int)
		
	End Method
	
	Method GetStyleId:Int(row:Int, col:Int)
		
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

Type TXLStyleManager
	
	Field _doc:TXLDocument

	Method Create:TXLStyleManager(doc:TXLDocument)
		_doc = doc
		Return Self
	End Method
	
	Method BuildMappings()
		' process all sheets' cell managers to compile a map of all styles used
		' Notes :
		' fonts have unique ids
		' borders have unique ids
		' fills have unique ids
		'
		' a cell xf has (ie. one cell fx per unique set of ids)
		Rem
		3 <sequence>
		4 <element name="alignment" type="CT_CellAlignment" minOccurs="0" maxOccurs="1"/>
		5 <element name="protection" type="CT_CellProtection" minOccurs="0" maxOccurs="1"/>
		6 <element name="extLst" type="CT_ExtensionList" minOccurs="0" maxOccurs="1"/>
		7 </sequence>
		8 <attribute name="numFmtId" type="ST_NumFmtId" use="optional"/>
		9 <attribute name="fontId" type="ST_FontId" use="optional"/>
		10 <attribute name="fillId" type="ST_FillId" use="optional"/>
		11 <attribute name="borderId" type="ST_BorderId" use="optional"/>
		12 <attribute name="xfId" type="ST_CellStyleXfId" use="optional"/>
		13 <attribute name="quotePrefix" type="xsd:boolean" use="optional" default="false"/>
		14 <attribute name="pivotButton" type="xsd:boolean" use="optional" default="false"/>
		15 <attribute name="applyNumberFormat" type="xsd:boolean" use="optional"/>
		16 <attribute name="applyFont" type="xsd:boolean" use="optional"/>
		17 <attribute name="applyFill" type="xsd:boolean" use="optional"/>
		18 <attribute name="applyBorder" type="xsd:boolean" use="optional"/>
		19 <attribute name="applyAlignment" type="xsd:boolean" use="optional"/>
		20 <attribute name="applyProtection" type="xsd:boolean" use="optional"/>
		End Rem
	End Method

	
	Method Save(path:String, zip:wxZipOutputStream, stream:wxTextOutputStream)
		
		BuildMappings()
	

		Local doc:TxmlDoc = TXLDocument.newXmlDoc()
		Local root:TxmlNode = TxmlNode.newNode("styleSheet")
		doc.SetRootElement(root)

		root.SetAttribute("xmlns", "http://schemas.openxmlformats.org/spreadsheetml/2006/main")

		'SaveNumFormats(root)
		
		' fonts
		Local fontsNode:TxmlNode = root.AddChild("fonts")
		fontsNode.SetAttribute("count", 1)
		'SaveFont(fontsNode.AddChild("font"))
		fontsNode.AddChild("font")
		
		' fills
		SaveFills(root)
		
		' borders
		SaveBorders(root)
		
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
	
	Method SaveFont(parent:TxmlNode)
		Local node:TxmlNode = parent.AddChild("name")
		node.SetAttribute("val", "Arial")
		
		node = parent.AddChild("sz")
		node.SetAttribute("val", 12)
Rem
		node = parent.AddChild("b")
		node.SetAttribute("val", "none")

		node = parent.AddChild("i")
		node.SetAttribute("val", "none")

		node = parent.AddChild("u")
		node.SetAttribute("val", "none")

		node = parent.AddChild("color")
		node.SetAttribute("rgb", "FF000000")
End Rem
	End Method
	
	Method SaveFills(parent:TxmlNode)
		Local fillsNode:TxmlNode = parent.AddChild("fills")
		fillsNode.SetAttribute("count", 1)
		Local node:TxmlNode = fillsNode.AddChild("fill")
		
		node = node.AddChild("patternFill")
		node.SetAttribute("patternType", "none")

	End Method
	
	Method SaveBorders(parent:TxmlNode)
		Local bordersNode:TxmlNode = parent.AddChild("borders")
		bordersNode.SetAttribute("count", 1)
		
		Local node:TxmlNode = bordersNode.AddChild("border")
		node.AddChild("left")
		node.AddChild("right")
		node.AddChild("top")
		node.AddChild("bottom")
		node.AddChild("diagonal")
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
		xfsNode.SetAttribute("count", 1)
		
		'For Local i:Int = 0 Until 2
			Local node:TxmlNode = xfsNode.AddChild("xf")
			node.SetAttribute("xfId", "0")
			node.SetAttribute("fontId", "0")
			node.SetAttribute("numFmtId", "0")
			node.SetAttribute("fillId", "0")
			node.SetAttribute("borderId", "0")
			Rem
			node = node.AddChild("alignment")
			node.SetAttribute("horizontal", "general")
			node.SetAttribute("vertical", "bottom")
			node.SetAttribute("textRotation", "0")
			node.SetAttribute("wrapText", "false")
			End Rem
		'Next
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

Rem
bbdoc: 
End Rem
Type TXLStyle

	Field _font:TXLFont = New TXLFont
	Field _border:TXLBorder = New TXLBorder
	
	Rem
	bbdoc: 
	End Rem
	Method Font:TXLFont()
		Return _font
	End Method

		
End Type

Type TXLRow

	Field row:Int
	Field cells:TMap
	
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

	Method Save(parent:TxmlNode)
		
		Local rowNode:TxmlNode = parent.AddChild("row")
		rowNode.SetAttribute("r", row)
		
		For Local cell:TXLCell = EachIn cells.Values()
		
			cell.Save(rowNode)
		
		Next
		
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TXLColumn

	Field index:Int
	Field width:Float = -1
	Field bestFit:Int = False
	Field visible:Int = True
	Field collapsed:Int = False

	Method Create:TXLColumn(index:Int)
		Self.index = index
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
	bbdoc: 
	End Rem
	Method GetStyle:TXLStyle()
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
bbdoc: 
End Rem
Type TXLFont

	Field name:String = "Calibri"
	Field size:Int = 10
	Field bold:Int = False
	Field italic:Int = False
	Field underline:Int = 0
	Field strikethrough:Int = False
	Field color:String
	
	Field _key:String

	Rem
	bbdoc: 
	End Rem
	Method SetName(name:String)
		Self.name = name
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetSize(size:Int)
		Self.size = size
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetBold(bold:Int = True)
		Self.bold = bold
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetItalic(italic:Int = True)
		Self.italic = italic
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetUnderline(underline:Int = True)
		Self.underline = underline
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetStrikethrough(strikethrough:Int = True)
		Self.strikethrough = strikethrough
	End Method

	Method GetKey:String()
		If Not _key Then
			_key = name + "_" + size + "_" + bold + "_" + italic + "_" + underline + "_" + strikethrough
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
	
End Type

Rem
bbdoc: 
End Rem
Type TXLBorder

	Field bLeft:TXLBorderStyle
	Field bRight:TXLBorderStyle
	Field bTop:TXLBorderStyle
	Field bBottom:TXLBorderStyle
	Field bDiagonal:TXLBorderStyle
	Field bHorizontal:TXLBorderStyle
	Field bVertical:TXLBorderStyle
	
	Field diagonalDown:Int
	Field diagonalUp:Int
	Field outline:Int
	
	Field _key:String
	
	Rem
	bbdoc: 
	End Rem
	Method GetLeft:TXLBorderStyle()
		If Not bLeft Then
			bLeft = New TXLBorderStyle
		End If
		Return bLeft
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

			_key:+ diagonalDown + "_" + diagonalUp + "_" + outline
		End If
		Return _key
	End Method	

End Type

Type TXLBorderStyle

	Field color:String
	Field style:String = XLSTYLE_BORDER_NONE

	Field _key:String

	Method GetKey:String()
		If Not _key Then
			_key = color + "_" + style
		End If
		Return _key
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
Const XLSTYLE_BORDER_THING:String = "thin"

Rem
bbdoc: Converts row,col to the standard spreadsheet address of a cell.
about: For example (4,5) converts to E4.
End Rem
Function XLCellAddress:String(row:Int, col:Int)
	Return TXLCell.ColToLetter(col) + row
End Function



