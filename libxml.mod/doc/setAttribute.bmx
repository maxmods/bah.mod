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

	' set an attribute
	node.setAttribute("attr1", "a value")
	
	Print node.toString()
	
	' change the attribute value
	node.setAttribute("attr1", "a new value")
	
	Print node.toString()
	
End If


