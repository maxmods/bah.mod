SuperStrict

Import BaH.Libxml

Local docname:String = "sample.xml"
Local keyword:String = "Blitzmax"
Local doc:TxmlDoc

doc = parseDoc(docname, keyword)
If doc <> Null Then
	doc.saveFormatFile("-", False)
	doc.free()
End If

Function parseStory(node:TxmlNode, keyword:String)
	node.addTextChild("keyword", Null, keyword)
End Function

Function parseDoc:TxmlDoc(docname:String, keyword:String)

	Local doc:TxmlDoc
	Local node:TxmlNode
	
	doc = TxmlDoc.parseFile(docname)
	
	If doc = Null Then
		Print "Document not parsed successfully."
		Return Null
	End If
	
	node = doc.getRootElement()
	
	If node = Null Then
		Print "empty document"
		doc.free()
		Return Null
	End If
	
	If node.getName() <> "story" Then
		Print "document of the wrong type, root node <> story"
		doc.free()
		Return Null
	End If
	
	Local nodelist:TList = node.getChildren()
	For node = EachIn nodelist
		If node.getName() = "storyinfo" Then
			parseStory(node, keyword)
		End If
	Next
	
	Return doc
End Function
