SuperStrict

Import BaH.Libxml

Local docname:String = "../examples/file1.xml"
Local doc:TxmlDoc

doc = TxmlDoc.parseFile(docname)
If doc Then
	Local root:TxmlNode = doc.getRootElement()
	
	For Local node:TxmlNode = EachIn root.getChildren()
		Print node.getName() + " : "
		
		For Local attribute:TxmlAttribute = EachIn node.getAttributeList()
			Print "    " + attribute.getName() + " : " + attribute.getValue()
		Next
		
		Print ""
	Next
End If