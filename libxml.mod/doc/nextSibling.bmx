SuperStrict

Framework BaH.Libxml
Import BRL.StandardIO

Local doc:TxmlDoc = TxmlDoc.parseFile("catalog.xml")

If doc Then

	Local root:TxmlNode = doc.getRootElement()

	Local cd:TxmlNode = TxmlNode(root.getFirstChild())
	
	While cd
	
		' print element node details
		If cd.getType() = XML_ELEMENT_NODE Then
			Print cd.toString()
		End If
		
		' get the next sibling
		cd = TxmlNode(cd.nextSibling())
	Wend
	
End If

