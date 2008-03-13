SuperStrict

Framework BaH.Libxml
Import BRL.StandardIO

Local doc:TxmlDoc = TxmlDoc.parseFile("catalog.xml")

If doc Then

	Local node:TxmlNode = TxmlNode(doc.getRootElement().getLastChild())

	node.addComment("This CD rocks!")

	Print node.toString()

End If

