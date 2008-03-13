' output list of node paths
SuperStrict

Framework BaH.Libxml
Import BRL.StandardIO

Local doc:TxmlDoc = TxmlDoc.parseFile("attributes.xml")

If doc Then

	Local node:TxmlNode = TxmlNode(doc.getRootElement().getFirstChild())
	
	For Local child:TxmlNode = EachIn node.getChildren()

		' output the child Xpath value
		Print child.getNodePath()
	
	Next
	
End If

