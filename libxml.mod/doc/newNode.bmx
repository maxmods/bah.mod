SuperStrict

Framework BaH.Libxml

Local doc:TxmlDoc = TxmlDoc.newDoc("1.0")

If doc Then
	
	' create a new node, initially not attached to the document
	Local root:TxmlNode = TxmlNode.newNode("root")
	
	' set the node as the document root node
	doc.setRootElement(root)
	
	' output the document to stdout
	doc.saveFile("-")
	
End If




