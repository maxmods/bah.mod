SuperStrict

Import BaH.Libxml

Local docname:String = "../examples/file2.xml"
Local doc:TxmlDoc

doc = TxmlDoc.parseFile(docname)
If doc Then
	Local root:TxmlNode = doc.getRootElement()
	
	Print "Last child is - " + root.getLastChild().getName()

End If


