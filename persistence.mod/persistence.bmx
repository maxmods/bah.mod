' Copyright (c) 2008 Bruce A Henderson
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
bbdoc: Persistence
about: An object-persistence framework. 
End Rem
Module BaH.Persistence

ModuleInfo "Version: 1.00"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2008 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release"

Import BaH.libxml
Import BRL.Reflection
Import BRL.Map

Rem
bbdoc: Object Persistence.
End Rem
Type TPersist

	Field doc:TxmlDoc
	Field objectMap:TMap = New TMap
	
	Field lastNode:TxmlNode
	
	Field format:Int = True

	Rem
	bbdoc: Serializes the specified Object into a String.
	End Rem
	Function Serialize:String(obj:Object)
		Local ser:TPersist = New TPersist
		
		Local s:String = ser.SerializeToString(obj)
		ser.Free()
		
		Return s
	End Function
	
	Method Free()
		If doc Then
			doc.Free()
			doc = Null
			objectMap.Clear()
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SerializeToString:String(obj:Object)
		Free()
		SerializeObject(obj)
		
		Return ToString()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SerializeToFile(obj:Object, filename:String)
		Free()
		SerializeObject(obj)
		
		If doc Then
			doc.saveFormatFile(filename, format)
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method ToString:String()
		If doc Then
			Return doc.ToStringFormat(format)
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SerializeObject(obj:Object, parent:TxmlNode = Null)
	
		If Not doc Then
			doc = TxmlDoc.newDoc("1.0")
			parent = TxmlNode.newNode("bmo") ' BlitzMax Object
			doc.setRootElement(parent)
		Else
			If Not parent Then
				parent = doc.GetRootElement()
			End If
		End If
		
		If obj Then
			Local tid:TTypeId = TTypeId.ForObject(obj)
	
			Local node:TxmlNode = parent.addChild(tid.Name())
			
			Local objRef:String = GetObjRef(obj)
			node.setAttribute("ref", objRef)
			objectMap.Insert(objRef, objRef)
			
			' special case for String object
			If tid = StringTypeId Then
				node.SetContent(String(obj))
			End If
	
			For Local f:TField = EachIn tid.EnumFields()
			
				Local fieldType:TTypeId = f.TypeId()
				
				Local fieldNode:TxmlNode = node.addChild("field")
				fieldNode.setAttribute("name", f.Name())
				
				Local t:String
				Select fieldType
					Case ByteTypeId
						t = "byte"
						fieldNode.setContent(f.GetInt(obj))
					Case ShortTypeId
						t = "short"
						fieldNode.setContent(f.GetInt(obj))
					Case IntTypeId
						t = "int"
						fieldNode.setContent(f.GetInt(obj))
					Case LongTypeId
						t = "long"
						fieldNode.setContent(f.GetLong(obj))
					Case FloatTypeId
						t = "float"
						fieldNode.setContent(f.GetFloat(obj))
					Case DoubleTypeId
						t = "double"
						fieldNode.setContent(f.GetDouble(obj))
					Case StringTypeId
						t = "string"
						fieldNode.setContent(f.GetString(obj))
					Default
						t = fieldType.Name()
						If fieldType.ExtendsType( ArrayTypeId ) Then
						
							' prefix and strip brackets
							t = "array:" + t.Replace("[]", "")
						
							Local elementType:TTypeId = fieldType.ElementType()
							
							For Local i:Int = 0 Until fieldType.ArrayLength(f.Get(obj))
							
								Local elementNode:TxmlNode = fieldNode.addChild("val")
								
								Local aObj:Object = fieldType.GetArrayElement(f.Get(obj), i)
							
								Select elementType
									Case ByteTypeId
										elementNode.setContent(String(aObj))
									Case ShortTypeId
										elementNode.setContent(String(aObj))
									Case IntTypeId
										elementNode.setContent(String(aObj))
									Case LongTypeId
										elementNode.setContent(String(aObj))
									Case FloatTypeId
										elementNode.setContent(String(aObj))
									Case DoubleTypeId
										elementNode.setContent(String(aObj))
									Case StringTypeId
										elementNode.setContent(String(aObj))
									Default
										SerializeObject(aObj, elementNode)
								End Select
							Next
							
						Else
							Local fieldObject:Object = f.Get(obj)
							Local fieldRef:String = GetObjRef(fieldObject)
							If fieldObject Then
								If Not objectMap.Contains(fieldRef) Then
									SerializeObject(fieldObject, fieldNode)
								Else
									fieldNode.setAttribute("ref", fieldRef)
								End If
							End If
						End If
				End Select
				
				fieldNode.setAttribute("type", t)
			
			Next

		End If
		
	End Method

	Method Delete()
		Free()
	End Method
	
	Rem
	bbdoc: De-serializes @text into an Object structure.
	End Rem
	Function DeSerialize:Object(text:String)
		Local ser:TPersist = New TPersist
		
		Return ser.DeSerializeObject(text)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method DeSerializeObject:Object(text:String, parent:TxmlNode = Null)

		Local node:TxmlNode
		
		If Not doc Then
			doc = TxmlDoc.parseDoc(text)
			parent = doc.GetRootElement()
			node = TxmlNode(parent.GetFirstChild())
			lastNode = node
		Else
			If Not parent Then
				node = TxmlNode(lastNode.NextSibling())
				lastNode = node
			Else
				node = TxmlNode(parent.GetFirstChild())
			End If
		End If
		
		Local obj:Object 
		
		
		If node Then
			
			Local objType:TTypeId = TTypeId.ForName(node.GetName())

			' special case for String object
			If objType = StringTypeId Then
				obj = node.GetContent()
				Return obj
			End If
			
			' create the object
			obj = objType.NewObject()
			objectMap.Insert(node.getAttribute("ref"), obj)
			
			For Local fieldNode:TxmlNode = EachIn node.getChildren()
			
				' this should be a field
				If fieldNode.GetName() = "field" Then
				
					Local fieldObj:TField = objType.FindField(fieldNode.getAttribute("name"))
					
					Local fieldType:String = fieldNode.getAttribute("type")
					Select fieldType
						Case "byte", "short", "int"
							fieldObj.SetInt(obj, fieldNode.GetContent().toInt())
						Case "long"
							fieldObj.SetLong(obj, fieldNode.GetContent().toLong())
						Case "float"
							fieldObj.SetFloat(obj, fieldNode.GetContent().toFloat())
						Case "double"
							fieldObj.SetDouble(obj, fieldNode.GetContent().toDouble())
						Case "string"
							fieldObj.SetString(obj, fieldNode.GetContent())
						Default
							If fieldType.StartsWith("array:") Then

								Local arrayType:TTypeId = fieldObj.TypeId()
								Local arrayElementType:TTypeId = arrayType.ElementType()
							
								Local arrayList:TList = fieldNode.getChildren()
								
								Local arrayObj:Object = arrayType.NewArray(arrayList.Count())
								fieldObj.Set(obj, arrayObj)
								
								Local i:Int
								For Local arrayNode:TxmlNode = EachIn arrayList

									Select arrayElementType
										Case ByteTypeId
											arrayType.SetArrayElement(arrayObj, i, arrayNode.GetContent())
										Case ShortTypeId
											arrayType.SetArrayElement(arrayObj, i, arrayNode.GetContent())
										Case IntTypeId
											arrayType.SetArrayElement(arrayObj, i, arrayNode.GetContent())
										Case LongTypeId
											arrayType.SetArrayElement(arrayObj, i, arrayNode.GetContent())
										Case FloatTypeId
											arrayType.SetArrayElement(arrayObj, i, arrayNode.GetContent())
										Case DoubleTypeId
											arrayType.SetArrayElement(arrayObj, i, arrayNode.GetContent())
										Case StringTypeId
											arrayType.SetArrayElement(arrayObj, i, arrayNode.GetContent())
										Default
											arrayType.SetArrayElement(arrayObj, i, DeSerializeObject("", arrayNode))
									End Select

									i:+ 1
								Next
							Else
								' is this a reference?
								Local ref:String = fieldNode.getAttribute("ref")
								If ref Then
									Local objRef:Object = objectMap.ValueForKey(ref)
									If objRef Then
										fieldObj.Set(obj, objRef)
									Else
										Throw New "Reference not mapped yet : " + ref
									End If
								Else
									fieldObj.Set(obj, DeSerializeObject("", fieldNode))
								End If
							End If
					End Select
					
				End If
			
			Next
			
		End If
	
		Return obj

	End Method


	Function GetObjRef:String(obj:Object)
		Return Base36(Int(Byte Ptr(obj)))
	End Function

	Function Base36:String( val:Int )
		Local buf:Short[6]
		For Local k:Int=5 To 0 Step -1
			Local n:Int=(val Mod 36) + 48
			If n > 57 n:+ 7
			buf[k]=n
			val = val / 36
		Next
		Return String.FromShorts( buf,6 )
	End Function

End Type

