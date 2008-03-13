SuperStrict

Framework BaH.Libxml

Local xml:String = "<?xml version=~q1.0~q?>"
xml:+ "<rootnode><childnode name=~qbob~q>Hello Bob!</childnode></rootnode>"

Local doc:TxmlDoc = TxmlDoc.parseDoc(xml)

If doc Then

	' output the document to stdout, with formatting
	doc.saveformatFile("-", True)

End If

