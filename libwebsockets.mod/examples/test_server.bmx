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

Local mirrorProtocol:TLWSProtocol = New TLWSProtocol.Create("lws-mirror-protocol", mirror_callback, createMirrorObject)
info.addProtocol(mirrorProtocol)


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

Function mirror_callback:Int(context:TLWSContext, wsi:TLWSWebSocket, reason:Int, user:Object, in:Byte Ptr, length:Int)
	Local mirror:TMirror = TMirror(user)
	
	Select reason
		Case LWS_CALLBACK_ESTABLISHED
			mirror.ringBufferTail = TMirror.ringBufferHead
			mirror.wsi = wsi

		Case LWS_CALLBACK_SERVER_WRITEABLE
		
			While mirror.ringBufferTail <> TMirror.ringBufferHead
			
				Local n:Int = wsi.write(Byte Ptr(TMirror.ringBuffer[mirror.ringBufferTail].payload) + ..
					LWS_SEND_BUFFER_PRE_PADDING, TMirror.ringBuffer[mirror.ringBufferTail].length, LWS_WRITE_TEXT)

				If n < TMirror.ringBuffer[mirror.ringBufferTail].length Then
					'lwsl_err("ERROR %d writing to mirror socket\n", n);
					Return -1
				End If
				
				If mirror.ringBufferTail = TMirror.MAX_MESSAGE_QUEUE - 1 Then
					mirror.ringBufferTail = 0
				Else
					mirror.ringBufferTail :+ 1
				End If
				
				If ((TMirror.ringBufferHead - mirror.ringBufferTail) & (TMirror.MAX_MESSAGE_QUEUE - 1)) = (TMirror.MAX_MESSAGE_QUEUE - 15)
					RxFlowAllowAllProtocol(wsi.protocol)
				End If

				If wsi.sendPipeChoked() Then
					context.callbackOnWritable(wsi)
					Exit
				End If
				
			
			Wend


		Case LWS_CALLBACK_RECEIVE
		
			Local done:Int = False
		
			If ((TMirror.ringBufferHead - mirror.ringBufferTail) & (TMirror.MAX_MESSAGE_QUEUE - 1)) = (TMirror.MAX_MESSAGE_QUEUE - 1) Then
				DebugLog "dropping!"
			Else

				If Not TMirror.ringBuffer[TMirror.ringBufferHead] Then
					TMirror.ringBuffer[TMirror.ringBufferHead] = New TMessage
				End If
				
				Local message:TMessage = TMirror.ringBuffer[TMirror.ringBufferHead]
				message.payload = New Byte[LWS_SEND_BUFFER_PRE_PADDING + length + LWS_SEND_BUFFER_POST_PADDING]
				message.length = length
				
				MemCopy(Byte Ptr(message.payload) + LWS_SEND_BUFFER_PRE_PADDING, in, length)
			
				If TMirror.ringBufferHead = TMirror.MAX_MESSAGE_QUEUE - 1 Then
					TMirror.ringBufferHead = 0
				Else
					TMirror.ringBufferHead :+ 1
				End If
			
			End If
			
			If ((TMirror.ringBufferHead - mirror.ringBufferTail) & (TMirror.MAX_MESSAGE_QUEUE - 1)) <> (TMirror.MAX_MESSAGE_QUEUE - 2) Then
				done = True
			End If
			
			If Not done Then
				wsi.rxFlowControl(False)
			End If
			
			CallbackOnWritableAllProtocol(wsi.protocol)

	End Select
	
	Return 0
End Function

Function createIncrememtObject:Object()
	Return New TIncrement
End Function

Function createMirrorObject:Object()
	Return New TMirror
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

Type TMessage
	Field payload:Byte[]
	Field length:Int
End Type

Type TMirror
	Field wsi:TLWSWebSocket
	Field ringBufferTail:Int
	
	Const MAX_MESSAGE_QUEUE:Int = 32

	Global ringBuffer:TMessage[MAX_MESSAGE_QUEUE]
	Global ringBufferHead:Int
End Type


