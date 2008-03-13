SuperStrict

Framework BaH.Libxml

Local doc:TxmlDoc = TxmlDoc.parseFile("attributes.xml")

If doc Then

	Local node:TxmlNode = TxmlNode(doc.getRootElement().getFirstChild())

	Local desc:TxmlNode = node.addChild("description", Null, Null)

	desc.addContent("Some of the songs on this CD are awesome.~n")
	desc.addContent("Tracks 5 & 6 put this CD up there...")

	doc.savefile("-")

End If
