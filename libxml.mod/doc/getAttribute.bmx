SuperStrict

Import BaH.Libxml

Local docname:String = "sampleuri.xml"
parseDoc(docname)

Function getReference(doc:TxmlDoc, node:TxmlNode)

	Local uri:String
	
	Local list:TList = node.getChildren()
	For node = EachIn list
		If node.getName() = "reference" Then
			uri = node.getAttribute("uri")
			Print "uri: " + uri
		End If
	Next
End Function

Function parseDoc(docname:String)

        Local doc:TxmlDoc
        Local node:TxmlNode

        doc = TxmlDoc.parseFile(docname)
        
        If doc = Null Then
                Print "Document not parsed successfully."
                Return
        End If
        
        node = doc.getRootElement()
        
        If node = Null Then
                Print "empty document"
                doc.free()
                Return
        End If
        
        If node.getName() <> "story" Then
                Print "document of the wrong type, root node <> story"
                doc.free()
                Return
        End If
        
        getReference(doc, node)
        doc.free()
End Function

