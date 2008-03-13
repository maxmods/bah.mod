SuperStrict

Framework BaH.Libxml
Import BRL.StandardIO

Local doc:TxmlDoc = TxmlDoc.parseFile("catalog.xml")

If doc Then

	' get the document root node
	Local root:TxmlNode = doc.getRootElement()

	' output the node structure
	Print root.toString()
	
End If


