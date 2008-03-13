SuperStrict

Framework BaH.Libxml

Local doc:TxmlDoc = TxmlDoc.parseFile("attributes.xml")

If doc Then

	Local node:TxmlNode = TxmlNode(doc.getRootElement().getFirstChild())

	node.addChild("ID", Null, "C0122200")

	doc.savefile("-")

End If
