SuperStrict

Framework BaH.Libxml

Local doc:TxmlDoc = TxmlDoc.parseFile("catalog.xml")

If doc Then

	' output the document to stdout
	doc.saveFile("-")

End If
