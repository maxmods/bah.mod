SuperStrict

Framework BaH.Libxml
Import BRL.StandardIO

Local doc:TxmlDoc = TxmlDoc.newDoc("1.0")

If doc Then
	
	Local root:TxmlNode = TxmlNode.newNode("root")
	doc.setRootElement(root)
	
	' create a new empty node
	Local node:TxmlNode = root.addChild("node", Null, Null)

	Print node.toString()

	' add some attributes
	node.setAttribute("attr1", "Attribute value")
	node.setAttribute("attr2", "Another value")
	
	Print node.toString()
	
	' remove an attribute
	node.unsetAttribute("attr1")
	
	Print node.toString()
	
End If




