SuperStrict

Import BRL.LinkedList
Import BaH.Base64

Import "common.bmx"

Type TNode

	Field name:String
	Field attributes:TList
	Field nodes:TList
	Field text:String
	
	Method Create:TNode(name:Object)
		Self.name = String(name)
		Return Self
	End Method
	
	Method render(buf:StringBuffer)
		Local empty:Int = False
		
		buf.append("<").append(name)
		
		' attributes
		If attributes Then
			For Local att:TAttribute = EachIn attributes
				att.render(buf)
			Next
		End If
		
		' no children?, we can close this node now
		If (Not nodes Or nodes.IsEmpty()) And Not text Then
			buf.append("/")
			empty = True
		End If
		
		buf.append(">")
		
		' children
		If nodes Then
			For Local node:TNode = EachIn nodes
				node.render(buf)
			Next
		End If
		
		If text Then
			buf.append(text)
		End If
		
		' if we have content, then we need to close the node
		If Not empty Then
			buf.append("</").append(name).append(">")
		End If
		
	End Method
	
	Method addAttribute:TNode(name:String, value:String)
		If Not attributes Then
			attributes = New TList
		End If
		
		attributes.AddLast(New TAttribute.Create(name, value))
		
		Return Self
	End Method
	
	Method addNodeName:TNode(name:String)
		Local node:TNode = New TNode.Create(name)
		
		addNode(node)
		
		Return node
	End Method
	
	Method addNode:TNode(node:TNode)
		If Not nodes Then
			nodes = New TList
		End If
		
		nodes.AddLast(node)
		
		Return node
	End Method
	
	Method addText(value:String)
		text = value
	End Method
	
End Type

' A base64 encoded CDATA node.
Type TDataNode Extends TNode

	Method Create:TDataNode(data:Object)
		name = TBase64.Encode(Byte[](data), Byte[](data).length)
		Return Self
	End Method

	Method render(buf:StringBuffer)
		buf.append("<![CDATA[").append(name).append("]]>")
	End Method
	
End Type

Type TAttribute

	Field name:String
	Field value:String
	
	Method Create:TAttribute(name:String, value:String)
		Self.name = name
		Self.value = value
		Return Self
	End Method
	
	Method render(buf:StringBuffer)
		buf.append(" ").append(name).append("=~q").append(value).append("~q")
	End Method

End Type
