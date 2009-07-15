SuperStrict

Import "xml.bmx"
Import "state.bmx"
Import "common.bmx"

Type TDdbgpReponse

	Method Create:TDdbgpReponse()
		Return Self
	End Method
	
	Function fileToURI:String(file:String)
		Return file
	End Function
	
	Method xml:StringBuffer()
		Local buf:StringBuffer = New StringBuffer
		buf.append("<?xml version=~q1.0~q encoding=~qUTF-8~q?>")
		
		Return buf
	End Method
	
	Method newResponse:TNode(command:String, id:String)
		Local node:TNode = New TNode.Create("response")
		node.addAttribute("command", command)
		node.addAttribute("transaction_id", id)
		Return node
	End Method
	
	
	Method init:String(file:String, session:String)
		Local buf:StringBuffer = xml()
		
		Local node:TNode = New TNode.Create("init")
		node.addAttribute("fileuri", fileToURI(file))
		node.addAttribute("idekey", session)
		node.addAttribute("language", "BlitzMax")
		node.addAttribute("protocol_version", "1.0")
		node.addAttribute("appid", "blitzmax_debugger")
		
		node.render(buf)
		Return buf.ToString()
	End Method
	
	Method status:String(id:String, state:Int, reason:String)
		Local buf:StringBuffer = xml()
		
		Local response:TNode = newResponse("status", id)
		
		response.addAttribute("status", DebugState.map(state))
		response.addAttribute("reason", reason)
		
		response.render(buf)
		Return buf.ToString()
	End Method
	
	Method stream:String(_type:String, data:Byte[])
		Local buf:StringBuffer = xml()
		
		Local s:TNode = New TNode.Create("stream")
		s.addAttribute("type", _type)
		s.addAttribute("encoding", "base64")
		
		s.addNode(New TDataNode.Create(data))
		
		s.render(buf)
		Return buf.ToString()
	End Method

End Type

