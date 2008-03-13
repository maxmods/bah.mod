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

	' change the node name
	node.setName("branch")
	
	Print node.toString()
	
End If




