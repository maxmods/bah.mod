SuperStrict

Framework BaH.Libxml
Import BRL.StandardIO

' Create a new document
Local doc:TxmlDoc = TxmlDoc.newDoc("1.0")

If doc Then

	' create a test stream
	Local stream:TTestStream = New TTestStream.Create()


	' create a new node, initially not attached to the document
	Local root:TxmlNode = TxmlNode.newNode("root")
	
	' set the node as the document root node
	doc.setRootElement(root)

	root.addChild("things", Null, "some stuff")


	' output the document to a file
	doc.saveFile("testfile.xml")

	' output the document to a stream
	doc.saveFile(stream)
	

End If



Type TTestStream Extends TStream

	Function Create:TTestStream( )
		Return New TTestStream
	End Function

	Method Write:Int( buf:Byte Ptr, count:Int )
		
		Print "outputing..."
		Print String.FromBytes( buf, count )
		
		Return count
	End Method

	
End Type
