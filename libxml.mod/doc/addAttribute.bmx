SuperStrict

Import BaH.Libxml

Local docname:String = "sample.xml"
Local uri:String = "http://blitzmax.com"
Local doc:TxmlDoc

doc = parseDoc(docname, uri)
If doc <> Null Then
	doc.saveFormatFile("-", True)
	doc.free()
End If


Function parseDoc:TxmlDoc(docname:String, uri:String)

        Local doc:TxmlDoc
        Local node:TxmlNode
        Local newnode:TxmlNode
        Local newattr:TxmlAttribute

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
        
        newnode = node.addTextChild("reference", Null, Null)
        newattr = newnode.addAttribute("uri", uri)
        Return doc
End Function
