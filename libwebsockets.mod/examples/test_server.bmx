SuperStrict

Framework BaH.LibWebsockets


Global serveableWhitelist:TServeable[3]
serveableWhitelist[0] = TServeable.Create("/favicon.ico", "image/x-icon")
serveableWhitelist[1] = TServeable.Create("/libwebsockets.org-logo.png", "image/png")
' last one is the default served if no match
serveableWhitelist[2] = TServeable.Create("/test.html", "text/html")


Local info:TLWSContextCreationInfo = New TLWSContextCreationInfo
info.SetPort(7681)

Local protocol:TLWSProtocol = New TLWSProtocol.Create("http-only", http_callback)
info.addProtocol(protocol)

Local dumbIncrementProtocol:TLWSProtocol = New TLWSProtocol.Create("dumb-increment-protocol", dumb_increment_callback, createIncrememtObject)
info.addProtocol(dumbIncrementProtocol)

Local context:TLWSContext = New TLWSContext.Create(info)

Local n:Int = 0

Local lastTime:Int
While n >= 0 

	Local time:Int = MilliSecs()

	' This provokes the LWS_CALLBACK_SERVER_WRITEABLE for every
	' live websocket connection using the DUMB_INCREMENT protocol,
	' as soon as it can take more packets (usually immediately)

	If (time - lastTime) > 50 Then
		CallbackOnWritableAllProtocol(dumbIncrementProtocol)
		lastTime = time
	End If

	n = context.service(50)

Wend



Function http_callback:Int(context:TLWSContext, wsi:TLWSWebSocket, reason:Int, user:Object, in:Byte Ptr, length:Int)
	Select reason
		Case LWS_CALLBACK_HTTP
		
			Local s:String = String.FromBytes(in, length)
			Local n:Int
			
			For n = 0 Until serveableWhitelist.length
				If serveableWhitelist[n].urlpath = s Then
					Exit
				End If
			Next
			
			If n = serveableWhitelist.length Then
				n :- 1
			End If
		
			If context.serveHttpFile(wsi, "." + serveableWhitelist[n].urlpath, serveableWhitelist[n].mimetype) Then
				' through completion or error, close the socket
				Return -1
			End If
		
		Case LWS_CALLBACK_HTTP_FILE_COMPLETION
			' kill the connection after we sent one file
			Return -1
		
		Case LWS_CALLBACK_HTTP_WRITEABLE
		
		
		Case LWS_CALLBACK_FILTER_NETWORK_CONNECTION
			' if we returned non-zero from here, we kill the connection
	End Select
	
	Return 0
End Function


Function dumb_increment_callback:Int(context:TLWSContext, wsi:TLWSWebSocket, reason:Int, user:Object, in:Byte Ptr, length:Int)
	Local obj:TIncrement = TIncrement(user)

	Select reason
		Case LWS_CALLBACK_ESTABLISHED

			' initialise the number
			obj.number = 0

		Case LWS_CALLBACK_SERVER_WRITEABLE

			' increment the number and update the client
			obj.number :+ 1
			wsi.writeText(obj.number)
		
		Case LWS_CALLBACK_RECEIVE

			' check for a reset command, and reset the count if required
			If length >= 6 Then
				Local s:String = String.FromUTF8String(in)
				If s = "reset~n" Then
					obj.number = 0
				End If
			End If
			
			
		Case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION
	End Select

	Return 0
End Function

Function createIncrememtObject:Object()
	Return New TIncrement
End Function

Type TServeable

	Field urlpath:String
	Field mimetype:String
	
	Function Create:TServeable(urlpath:String, mimetype:String)
		Local this:TServeable = New TServeable
		this.urlpath = urlpath
		this.mimetype = mimetype
		Return this
	End Function

End Type

Type TIncrement

	Field number:Int
	
End Type

