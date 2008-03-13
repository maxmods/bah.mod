SuperStrict

Framework BaH.Libxml
Import BRL.StandardIO

Local doc:TxmlDoc = TxmlDoc.parseFile("attributes.xml")

If doc Then

	Local node:TxmlNode = TxmlNode(doc.getRootElement().getFirstChild())
	
	Local attribute:TxmlAttribute = node.hasAttribute("artist")
	
	If attribute Then
	
		Print "Artist = " + attribute.getValue()
	
	End If

End If