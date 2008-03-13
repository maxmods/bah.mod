SuperStrict

Framework BaH.Libxml

' Create a new document
Local doc:TxmlDoc = TxmlDoc.newDoc("1.0")

If doc Then

	' output the document to stdout
	doc.saveFile("-")

End If
