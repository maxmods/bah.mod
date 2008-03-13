' create a new XML document, and populate it with some stuff

SuperStrict

Framework BaH.Libxml

' Create a new document
Local doc:TxmlDoc = TxmlDoc.newDoc("1.0")

' Create a new node, initially not attached to any document
Local rootNode:TxmlNode = TxmlNode.newNode("myrootnode")

' Make the node our document root node
doc.setRootElement(rootNode)

' Add a child node to the root node
Local child:TxmlNode = rootNode.addChild("achild", Null, "Text for the child!")

' Add an attribute to the child node
child.addAttribute("name", "bob")

' Output the document to stdout
doc.saveFormatFile("-", True)
