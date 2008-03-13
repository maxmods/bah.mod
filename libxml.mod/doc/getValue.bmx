SuperStrict

Framework BaH.Libxml
Import BRL.StandardIO

Local doc:TxmlDoc = TxmlDoc.parseFile("attributes.xml")

If doc Then

	' output the document to stdout
	doc.saveFile("-")

	' print first child attributes	
	For Local attribute:TxmlAttribute = EachIn TxmlNode(doc.getRootElement().getFirstChild()).getAttributeList()

		Print attribute.getName() + " : " + attribute.getValue()

	Next
End If

