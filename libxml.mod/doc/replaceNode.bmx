SuperStrict

Framework BaH.Libxml

Local doc:TxmlDoc = TxmlDoc.parseFile("attributes.xml")

If doc Then

	Local node:TxmlNode = TxmlNode(doc.getRootElement().getFirstChild())

	' a new node for the document
	Local newNode:TxmlNode = TxmlNode.newnode("cd")
	newNode.addAttribute("title", "High on the Happy Side")
	newNode.addAttribute("artist", "Wet Wet Wet")
	newNode.addChild("country", Null, "UK")
	
	' replace node with the new node
	node.replaceNode(newNode)

	' output the document to stdout
	doc.saveFile("-")
End If

