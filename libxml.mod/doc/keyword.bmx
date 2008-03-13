SuperStrict

Import BaH.Libxml

Local docname:String = "sample.xml"
parseDoc(docname)


Function parseStory(doc:TxmlDoc, node:TxmlNode)

	Local key:String
	Local children:TList = node.getChildren()
	
	For node = EachIn children
		If node.getName() = "keyword" Then
			key = node.getText()
			Print "keyword: " + key
		End If
	Next
	Return
End Function

Function parseDoc(docname:String)

	Local doc:TxmlDoc
	Local node:TxmlNode
	
	doc = TxmlDoc.parseFile(docname)
	
	If doc = Null Then
		Print "Document not parsed successfully."
		Return
	End If
	
	node = doc.GetRootElement()
	
	If node = Null Then
		Print "empty document"
		doc.free()
		return
	End If
	
	If node.getName() <> "story"
		Print "document of the wrong type, root node <> story"
		doc.free()
		Return
	End If
	
	Local children:TList = node.getChildren()
	
	For node = EachIn children
		If node.getName() = "storyinfo" Then
			parseStory(doc, node)
		End If
	Next
	
	doc.free()
	Return
End Function