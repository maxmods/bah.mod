SuperStrict

Import BaH.Libxml

Local docname:String = "bookstore.xml"
Local doc:TxmlDoc

Local xpath:String = "//title"
Local nodeset:TxmlNodeSet
Local result:TxmlXPathObject

doc = getdoc(docname)
If doc Then
	result = getNodeSet(doc, xpath)
	If result
		nodeset = result.getNodeSet()
		
		Local list:TList = nodeset.getNodeList()
		
		If list <> Null Then
			Print "count = " + list.count()

			For Local node:TxmlNode = EachIn list
				Print "name   : " + node.getText()
				
			Next
		End If
		result.free()
	End If
	
	doc.free()
End If

xmlCleanupParser()
End


Function getdoc:TxmlDoc(docname:String)
	Local doc:TxmlDoc = TxmlDoc.parseFile(docname)
	
	If doc = Null Then
		Print "Document not parsed successfully."
		Return Null
	End If

	Return doc
End Function


Function getnodeset:TxmlXPathObject(doc:TxmlDoc, xpath:String)
	
	Local context:TxmlXPathContext
 	Local result:TxmlXPathObject

	context = doc.newXPathContext()
	If context = Null Then
		Print "Error in newXPathContext"
		Return Null
	End If
	result = context.evalExpression(xpath)
	context.free()
	
	If result = Null Then
		Print "Error in xmlXPathEvalExpression"
		Return Null
	End If
	
	If result.nodeSetIsEmpty() Then
		result.free()
         Print "No result"
		Return Null
	End If
	Return result
End Function

