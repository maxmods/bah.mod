SuperStrict

Import "xml.bmx"
Import "stack.bmx"
Import "state.bmx"
Import "common.bmx"

Type TDdbgpReponse

	Method Create:TDdbgpReponse()
		Return Self
	End Method
	
	Function fileToURI:String(file:String)
		Global Hex:Int[] = [48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70]

		file = fixSlashes(file)
		
		Local s:String
		
		For Local i:Int = 0 Until file.length
			Local c:Int = file[i]
			If c >= 128 Or ";?:@&=+$,[] ".Find(Chr(c)) >= 0 Then
				s:+ "%" + Chr(Hex[(c Shr 4) & $0f]) + Chr(Hex[(c Shr 0) & $0f])
			Else
				s:+ Chr(c)
			End If
		Next

		Return "file://" + s
	End Function
	
	Function fixSlashes:String(path:String)
?win32
		path = path.Replace("\", "/")
?
		If Not path.StartsWith("/") Then
			path = "/" + path
		End If
		
		Return path
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
	
	Method featureGet:String(id:String, name:String, supported:Int, value:String)
		Local buf:StringBuffer = xml()

		Local response:TNode = newResponse("feature_get", id)
		response.addAttribute("feature_name", name)
		If supported Then
			response.addAttribute("supported", "1")
		Else
			response.addAttribute("supported", "0")
		End If
		
		response.addText(value)
		
		response.render(buf)
		Return buf.ToString()
	End Method

	Method featureSet:String(id:String, name:String, supported:Int)
		Local buf:StringBuffer = xml()

		Local response:TNode = newResponse("feature_set", id)
		response.addAttribute("feature_name", name)
		If supported Then
			response.addAttribute("supported", "1")
		Else
			response.addAttribute("supported", "0")
		End If
		
		response.render(buf)
		Return buf.ToString()
	End Method
	
	Method simpleResponseWithSuccess:String(name:String, id:String, success:Int)
		Local buf:StringBuffer = xml()
		
		Local response:TNode = newResponse("name", id)
		If success Then
			response.addAttribute("success", "1")
		Else
			response.addAttribute("success", "0")
		End If
		
		response.render(buf)
		Return buf.ToString()
	End Method
	
	Method stdin:String(id:String, success:Int)
		Return simpleResponseWithSuccess("stdin", id, success)
	End Method

	Method stderr:String(id:String, success:Int)
		Return simpleResponseWithSuccess("stderr", id, success)
	End Method

	Method stdout:String(id:String, success:Int)
		Return simpleResponseWithSuccess("stdout", id, success)
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

	Method run:String(id:String, state:Int, success:Int)
		Local buf:StringBuffer = xml()
		
		Local response:TNode = newResponse("run", id)
		response.addAttribute("status", DebugState.map(state))
		
		If success Then
			response.addAttribute("reason", "ok")
		Else
			response.addAttribute("reason", "error")
		End If

		response.render(buf)
		Return buf.ToString()
	End Method

	Method breakpoint:String(id:String)
		Local buf:StringBuffer = xml()
		
		Local response:TNode = newResponse("break", id)
		response.addAttribute("status", DebugState.map(DebugState.BREAK))
		response.addAttribute("reason", "ok")

		response.render(buf)
		Return buf.ToString()
	End Method

	Method stackGet:String(id:String, stack:TList)
		Local buf:StringBuffer = xml()
		
		Local response:TNode = newResponse("stack_get", id)
		
		For Local scope:TBlitzMaxStackScope = EachIn stack
			addToStack(response, scope, scope.getLevel())
		Next
		
		response.render(buf)
		Return buf.ToString()
	End Method
	
	Method addToStack(parent:TNode, scope:TBlitzMaxStackScope, level:Int)
		Local stack:TNode = parent.addNodeName("stack")
		
		stack.addAttribute("level", level)
		stack.addAttribute("type", "file")
		stack.addAttribute("filename", fileToURI(scope.getSource()))
		stack.addAttribute("lineno", scope.getLine())
		
		' include the function/method name?
		If level > 0 Then
			If scope.getWhere() Then
				stack.addAttribute("where", scope.getWhere())
			End If
		End If
		
	End Method
	
	Method contextNames:String(id:String, stack:TList)
		' FIXME : make this work for different contexts. I'm hard-coding this to Local for now.

		Local buf:StringBuffer = xml()
		
		Local response:TNode = newResponse("context_names", id)
		
		Local context:TNode = response.addNodeName("context")
		context.addAttribute("name", "Local")
		context.addAttribute("id", "0")

		response.render(buf)
		Return buf.ToString()
	End Method

	Method stop:String(id:String, state:Int)
		Local buf:StringBuffer = xml()
		
		Local response:TNode = newResponse("stop", id)
		response.addAttribute("status", DebugState.map(DebugState.BREAK))
		response.addAttribute("reason", "ok")

		response.render(buf)
		Return buf.ToString()
	End Method
	
	Method contextGet:String(id:String, stack:TList, depth:Int, context:Int)
		Local buf:StringBuffer = xml()
		
		Local response:TNode = newResponse("context_get", id)
		response.addAttribute("context", context)
		
		For Local scope:TBlitzMaxStackScope = EachIn stack
			If scope.getVariables() Then
				For Local variable:TBlitzMaxScopeVariable = EachIn scope.getVariables()
					addProperty(response, variable)
				Next
			End If
		Next
	
		response.render(buf)
		Return buf.ToString()
	End Method
	
	Method addProperty(response:TNode, variable:TBlitzMaxScopeVariable)
		Local prop:TNode = response.addNodeName("property")
		
		prop.addAttribute("name", variable.getName())
		prop.addAttribute("fullname", variable.getName())
		prop.addAttribute("type", variable.getType())
		
		prop.addAttribute("constant", "1") ' ie. Not modifiable in the variable editor!

		If variable.hasChildren() Then
			prop.addAttribute("children", "1")
		Else
			prop.addAttribute("children", "0")
		End If

		If variable.getKey() Then
			prop.addAttribute("key", variable.getKey())
			prop.addAttribute("address", variable.getKey())
		Else
			If BlitzMaxType.isPrimitive(variable.getBaseType()) Then
				prop.addText(variable.getValue())
			Else
				If BlitzMaxType._STRING = variable.getBaseType() Then
					prop.addAttribute("encoding", "base64")
					prop.addNode(New TDataNode.Create(StringToByteArray(variable.getValue())))
				End If
			End If
		End If
		
	End Method

End Type

