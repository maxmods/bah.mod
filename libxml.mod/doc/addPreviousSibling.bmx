SuperStrict

Framework BaH.Libxml

Local doc:TxmlDoc = TxmlDoc.parseFile("attributes.xml")

If doc Then

	Local node:TxmlNode = TxmlNode(doc.getRootElement().getFirstChild())
	
	' a new node for the document
	Local newNode:TxmlNode = TxmlNode.newnode("cd")
	newNode.addAttribute("title", "This is the Sea")
	newNode.addAttribute("artist", "Waterboys")
	newNode.addChild("country", Null, "UK")
	
	' add new node to document as previous sibling of node.
	node.addPreviousSibling(newNode)

	' output the document to stdout
	doc.saveFile("-")
End If

