' Copyright (c) 2018 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
' 
SuperStrict

Rem
bbdoc: A collection of Serializers for BaH.Persistence
End Rem
Module BaH.PersistenceHelper

ModuleInfo "Version: 1.00"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2018 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

Import BaH.Persistence

?bmxng

Type TIntMapXMLSerializer Extends TXMLSerializer

	Method TypeName:String()
		Return "TIntMap"
	End Method
	
	Method Serialize(tid:TTypeId, obj:Object, node:TxmlNode)
		Local map:TIntMap = TIntMap(obj)
		
		If map Then
			For Local mapNode:TIntNode = EachIn map
				Local v:TxmlNode = node.addChild("e")
				If mapNode.Value() Then
					SerializeReferencedObject(mapNode.Value(), v)
				End If
				v.setAttribute("index", mapNode.Key())
			Next
		End If
	End Method
	
	Method Deserialize:Object(objType:TTypeId, node:TxmlNode)
		Local map:TIntMap = TIntMap(CreateObjectInstance(objType, node))
		If node.getChildren() Then
			Local ver:Int = GetFileVersion()
			
			For Local mapNode:TxmlNode = EachIn node.getChildren()
				Local index:Int = Int(mapNode.getAttribute("index"))
				Local obj:Object
				If ver < 8 Then
					obj = DeserializeObject(mapNode, True)
				Else
					obj = DeserializeReferencedObject(mapNode)
				End If
				map.Insert(index, obj)
			Next
		End If
		Return map
	End Method

	Method Clone:TXMLSerializer()
		Return New TIntMapXMLSerializer
	End Method

End Type

Type TStringMapXMLSerializer Extends TXMLSerializer

	Method TypeName:String()
		Return "TStringMap"
	End Method
	
	Method Serialize(tid:TTypeId, obj:Object, node:TxmlNode)
		Local map:TStringMap = TStringMap(obj)
		
		If map Then
			For Local mapNode:TStringNode = EachIn map
				Local n:TxmlNode = node.addChild("n")
				SerializeReferencedObject(mapNode.Key(), n.addChild("k"))
				SerializeReferencedObject(mapNode.Value(), n.addChild("v"))
			Next
		End If
	End Method
	
	Method Deserialize:Object(objType:TTypeId, node:TxmlNode)
		Local map:TStringMap = TStringMap(CreateObjectInstance(objType, node))

		If node.getChildren() Then
			Local ver:Int = GetFileVersion()

			For Local mapNode:TxmlNode = EachIn node.getChildren()
				Local keyNode:TxmlNode = TxmlNode(mapNode.getFirstChild())
				Local valueNode:TxmlNode = TxmlNode(mapNode.getLastChild())
				
				Local k:String
				Local v:Object
				
				If ver < 8 Then
					k = String(DeserializeObject(keyNode))
					v = DeserializeObject(valueNode)
				Else
					k = String(DeserializeReferencedObject(keyNode))
					v = DeserializeReferencedObject(valueNode)
				End If
				map.Insert(k, v)
			Next
		End If

		Return map
	End Method

	Method Clone:TXMLSerializer()
		Return New TStringMapXMLSerializer
	End Method

End Type

TXMLPersistenceBuilder.RegisterDefault(New TIntMapXMLSerializer)
TXMLPersistenceBuilder.RegisterDefault(New TStringMapXMLSerializer)

?
