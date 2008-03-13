SuperStrict

'Import "-llibxml"

Import BaH.Libxml

'DebugStop
Local doc:TxmlDoc = TxmlDoc.parseFile("aaa.xml")



Print doc.getURL()

Local n:TxmlNode = doc.addProcessingInstruction("xml", "version=~q1.0~q")

Local root:TxmlNode = doc.getRootElement()
root.addPreviousSibling(n)

Print root.getName()

'root.addComment("This is a comment!")

Local children:TList = root.getChildren()

Print children.count()

For Local node:TxmlNode = EachIn children
	Print node.getName()
Next

'doc.setCompressMode(0)
doc.saveFile("aaa.xml")