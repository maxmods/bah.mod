' Copyright (c) 2013 Bruce A Henderson
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
bbdoc: libwebsockets
End Rem
Module BaH.libwebsockets

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: (libwebsockets) 2010-2013 Andy Green. LGPL with static linking exception."
ModuleInfo "Copyright: (wrapper) 2013 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release (libwebsockets 1.3)"


?win32
ModuleInfo "LD_OPTS: -lws2_32"
?

Import "common.bmx"

'
' Notes :
'  libwebsockets callbacks modified to provide an extra parameter of the protocol. This makes it easier
' to connect the lib callback with the BlitzMax user-provided protocol callbacks.
'
'

Rem
bbdoc: 
End Rem
Type TLWSContextCreationInfo

	Field infoPtr:Byte Ptr
	
	Field user:Object
	
	Field protocolsPtr:Byte Ptr
	Field protocols:TLWSProtocol[]
	
	Method New()
		infoPtr = bmx_libwebsockets_contextcreationinfo_new()
	End Method
	
	Rem
	bbdoc: Sets the port to listen on.
	about: You can use 0 to suppress listening on any port, that's what you want if you are not running a websocket
	server at all but just using it as a client.
	End Rem
	Method setPort(port:Int)
		bmx_libwebsockets_contextcreationinfo_setport(infoPtr, port)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method addProtocol(protocol:TLWSProtocol)
		protocols :+ [protocol]
	End Method
	
	' private
	Method buildProtocolList()
		bmx_libwebsockets_contextcreationinfo_buildprotocollist(infoPtr, Self, protocols.length)
	End Method
	
	' private
	Function _initProtocols(info:TLWSContextCreationInfo, protocolsPtr:Byte Ptr)
		info.protocolsPtr = protocolsPtr
	End Function
	
	' private
	Function _getProtocol:Byte Ptr(info:TLWSContextCreationInfo, index:Int, offsetPtr:Byte Ptr)
		info.protocols[index].offsetPtr = offsetPtr
		Return info.protocols[index].protocolPtr
	End Function
	
	Method Delete()
		If infoPtr Then
			bmx_libwebsockets_contextcreationinfo_free(infoPtr)
			infoPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: Request a callback for all connections using the given protocol when it becomes possible to write to each socket without blocking in turn.
End Rem
Function CallbackOnWritableAllProtocol:Int(protocol:TLWSProtocol)
	Return bmx_libwebsocket_callback_on_writable_all_protocol(protocol.offsetPtr)
End Function


Rem
bbdoc: 
End Rem
Type TLWSContext

	Global contexts:TMap = New TMap

	Field contextPtr:Byte Ptr
	
	Field user:Object
	
	Field _info:TLWSContextCreationInfo

	Method Create:TLWSContext(info:TLWSContextCreationInfo)
		info.buildProtocolList()
		contextPtr = bmx_libwebsockets_create_context(Self, info.infoPtr)
		_info = info
		user = info.user
		
		contexts.Insert(String(Int(contextPtr)), Self)
		Return Self
	End Method
	
	Rem
	bbdoc: Deals with any pending websocket traffic, for three kinds of event.
	about: It handles these events on both server and client types of connection the same.
	<ol>
	<li>Accept new connections to our context's server</li>
	<li>Call the receive callback for incoming frame data received by server or client connections.</li>
	</ol>
	<p>
	You need to call this service function periodically to all the above functions to happen; if your application is single-threaded you can just call it in your main event loop.
	</p>
	<p>
	If you are calling it in a single-threaded app, you don't want it to wait around blocking other things in your loop from happening, so you would call it with a timeout of 0,
	so it returns immediately if nothing is pending, or as soon as it services whatever was pending.
	</p>
	End Rem
	Method service:Int(timeout:Int)
		Return bmx_libwebsockets_service(contextPtr, timeout)
	End Method
	
	Rem
	bbdoc: Intended to be called from the callback in response to http requests from the client.
	returns: Less than 0 indicates error and the wsi should be closed. Greateer than 0 indicates the file was completely sent and the wsi should be closed. 0 indicates the file transfer is started and needs more service later, the wsi should be left alone.
	about: It allows the callback to issue local files down the http link in a single step.
	End Rem
	Method serveHttpFile:Int(wsi:TLWSWebSocket, file:String, contentType:String)
		Return bmx_libwebsockets_serve_http_file(contextPtr, wsi.socketPtr, file, contentType)
	End Method
	
	Rem
	bbdoc: Frees the websocket context.
	about: Closes any active connections and then frees the context. After calling this, any further use of the context is undefined.
	End Rem
	Method Free()
		If contextPtr Then
			bmx_libwebsocket_context_destroy(contextPtr)
			contextPtr = Null
		End If
	End Method
	
End Type


Rem
bbdoc: 
End Rem
Type TLWSProtocol

	Global protocolMap:TMap = New TMap

	Field protocolPtr:Byte Ptr
	Field offsetPtr:Byte Ptr

	Field callback:Int(context:TLWSContext, wsi:TLWSWebSocket, reason:Int, user:Object, in:Byte Ptr, length:Int)
	Field objectCB:Object()
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TLWSProtocol(name:String, serviceCallback:Int(context:TLWSContext, wsi:TLWSWebSocket, reason:Int, user:Object, in:Byte Ptr, length:Int), objectCallback:Object() = Null)
		callback = serviceCallback
		objectCB = objectCallback
		protocolMap.Insert(name, Self)
		
		protocolPtr = bmx_libwebsockets_protocol_create(name)
		
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function getProtocol:TLWSProtocol(name:String)
		Return TLWSProtocol(protocolMap.ValueForKey(name))
	End Function

	Function _callback:Int(name:String, contextPtr:Byte Ptr, wsiPtr:Byte Ptr, reason:Int, user:Object, in:Byte Ptr, length:Int)
		Local proto:TLWSProtocol = getProtocol(name)

		Return proto.callback(TLWSContext(TLWSContext.contexts.ValueForKey(String(Int(contextPtr)))), ..
				TLWSWebSocket._create(wsiPtr), reason, user, in, length)
	End Function
	
	Function _objectCallback:Object(name:String)
		Local proto:TLWSProtocol = getProtocol(name)

		If proto.objectCB Then
			Return proto.objectCB()
		Else
			Return Null
		End If
	End Function
	
End Type

Rem
bbdoc: 
End Rem
Type TLWSWebSocket

	Field socketPtr:Byte Ptr
	
	Function _create:TLWSWebSocket(socketPtr:Byte Ptr)
		If socketPtr Then
			Local this:TLWSWebSocket = New TLWSWebSocket
			this.socketPtr = socketPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Write text to the client.
	End Rem
	Method writeText:Int(text:String)
		Local s:Byte Ptr = text.ToUTF8String()
		Local n:Int = _strlen(s)
	
		Local data:Byte[LWS_SEND_BUFFER_PRE_PADDING + n + LWS_SEND_BUFFER_POST_PADDING]
		Local offset:Byte Ptr = Byte Ptr(data) + LWS_SEND_BUFFER_PRE_PADDING
		MemCopy(offset, s, n)

		Local ret:Int = bmx_libwebsockets_websocket_writetext(socketPtr, offset, n)
		
		MemFree(s)
		Return ret
	End Method
	
	Rem
	bbdoc: Apply protocol then write data to client
	about: Provides the way to issue data back to the client for both http and websocket protocols.
	End Rem
	Method write:Int(buffer:Byte Ptr, length:Int, protocol:Int)
		If protocol = LWS_WRITE_BINARY Or protocol = LWS_WRITE_TEXT Then
			Local data:Byte[LWS_SEND_BUFFER_PRE_PADDING + length + LWS_SEND_BUFFER_POST_PADDING]
			Local offset:Byte Ptr = Byte Ptr(data) + LWS_SEND_BUFFER_PRE_PADDING
			MemCopy(offset, buffer, length)
			Return bmx_libwebsockets_websocket_write(socketPtr, offset, length, protocol)
		Else
			Return bmx_libwebsockets_websocket_write(socketPtr, buffer, length, protocol)
		End If
	End Method
	
	Rem
	bbdoc: Determines if the frame is binary.
	about: This is intended to be called from the LWS_CALLBACK_RECEIVE callback if it's interested to see if the frame it's dealing with was sent in binary mode.
	End Rem
	Method frameIsBinary:Int()
		Return bmx_libwebsockets_lws_frame_is_binary(socketPtr)
	End Method
	
	Rem
	bbdoc: Bytes to come before "overall" rx packet is complete.
	about: This is intended to be called from the callback if the user code is interested in "complete packets" from the client.
	about: libwebsockets just passes through payload as it comes and issues a buffer additionally when it hits a built-in limit.
	The LWS_CALLBACK_RECEIVE callback handler can use this API to find out if the buffer it has just been given is the last piece of
	a "complete packet" from the client -- when that is the case remainingPacketPayload will return 0.
	<p>
	Many protocols won't care becuse their packets are always small.
	</p>
	End Rem
	Method remainingPacketPayload:Int()
		Return bmx_libwebsockets_remaining_packet_payload(socketPtr)
	End Method
	
End Type

