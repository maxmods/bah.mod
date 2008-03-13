' incbin'd xml test

SuperStrict

Import BaH.libxml

Incbin "test_small.xml"

Local doc:TxmlDoc = TxmlDoc.parseFile("incbin::test_small.xml")

If doc <> Null Then
	Print doc.savefile("-")
End If

