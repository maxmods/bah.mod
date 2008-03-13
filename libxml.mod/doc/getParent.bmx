SuperStrict

Import BaH.Libxml

Local docname:String = "../examples/file1.xml"
Local doc:TxmlDoc

doc = TxmlDoc.parseFile(docname)
If doc Then
	Local root:TxmlNode = doc.getRootElement()
	
	Print root.getName() + " has " + root.getChildren().count() + " children...~n"
	
	For Local node:TxmlNode = EachIn root.getChildren()
		Print "  " + node.getName() + " has parent '" + node.getParent().getName() + "'"
	Next
End If
