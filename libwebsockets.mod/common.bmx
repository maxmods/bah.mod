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

Import BRL.Map
Import Pub.ZLib

Import "source.bmx"


Extern
	Function _strlen:Int(s:Byte Ptr) = "strlen"

	Function bmx_libwebsockets_create_context:Byte Ptr(handle:Object, info:Byte Ptr)
	Function bmx_libwebsockets_service:Int(handle:Byte Ptr, timeout:Int)
	Function bmx_libwebsockets_serve_http_file:Int(handle:Byte Ptr, wsi:Byte Ptr, file:String, contentType:String)
	Function bmx_libwebsocket_context_destroy(handle:Byte Ptr)
	Function bmx_libwebsocket_callback_on_writable:Int(handle:Byte Ptr, wsi:Byte Ptr)

	Function bmx_libwebsockets_contextcreationinfo_new:Byte Ptr()
	Function bmx_libwebsockets_contextcreationinfo_free(handle:Byte Ptr)
	Function bmx_libwebsockets_contextcreationinfo_setport(handle:Byte Ptr, port:Int)
	Function bmx_libwebsockets_contextcreationinfo_buildprotocollist(handle:Byte Ptr, obj:Object, protocols:Int)

	Function bmx_libwebsockets_protocol_create:Byte Ptr(name:String)
	Function bmx_libwebsocket_callback_on_writable_all_protocol:Int(protocol:Byte Ptr)
	Function bmx_libwebsocket_rx_flow_allow_all_protocol(protocol:Byte Ptr)

	Function bmx_libwebsockets_websocket_writetext:Int(handle:Byte Ptr, offset:Byte Ptr, length:Int)
	Function bmx_libwebsockets_websocket_write:Int(handle:Byte Ptr, offset:Byte Ptr, length:Int, protocol:Int)
	Function bmx_libwebsockets_lws_frame_is_binary:Int(handle:Byte Ptr)
	Function bmx_libwebsockets_remaining_packet_payload:Int(handle:Byte Ptr)
	Function bmx_libwebsocket_rx_flow_control:Int(handle:Byte Ptr, enable:Int)
	Function bmx_libwebsockets_lws_send_pipe_choked:Int(handle:Byte Ptr)

End Extern

Rem
bbdoc: After the server completes a handshake with an incoming client.
End Rem
Const LWS_CALLBACK_ESTABLISHED:Int = 0
Rem
bbdoc: The request client connection has been unable to complete a handshake with the remote server.
End Rem
Const LWS_CALLBACK_CLIENT_CONNECTION_ERROR:Int = 1
Rem
bbdoc: This is the last chance for the client user code to examine the http headers and decide to reject the connection.
aboout: If the content in the headers is interesting to the client (url, etc) it needs to copy it out at this point
since it will be destroyed before the CLIENT_ESTABLISHED call.
End Rem
Const LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH:Int = 2
Rem
bbdoc: fter your client connection completed a handshake with the remote server.
End Rem
Const LWS_CALLBACK_CLIENT_ESTABLISHED:Int = 3
Rem
bbdoc: When the websocket session ends.
End Rem
Const LWS_CALLBACK_CLOSED:Int = 4
Rem
bbdoc: 
End Rem
Const LWS_CALLBACK_CLOSED_HTTP:Int = 5
Rem
bbdoc: Data has appeared for this server endpoint from a remote client, it can be found at *in and is len bytes long.
End Rem
Const LWS_CALLBACK_RECEIVE:Int = 6
Rem
bbdoc: Data has appeared from the server for the client connection, it can be found at *in and is len bytes long.
End Rem
Const LWS_CALLBACK_CLIENT_RECEIVE:Int = 7
Rem
bbdoc: Ff you elected to see PONG packets, they appear with this callback reason.
about: PONG packets only exist in 04+ protocol
End Rem
Const LWS_CALLBACK_CLIENT_RECEIVE_PONG:Int = 8
Rem
bbdoc: If you call libwebsocket_callback_on_writable on a connection, you will get one of these callbacks coming when the connection socket is able to accept another write packet without blocking.
about: If it already was able to take another packet without blocking, you'll get this callback at the next call to the service loop function.
End Rem
Const LWS_CALLBACK_CLIENT_WRITEABLE:Int = 9
Rem
bbdoc: If you call libwebsocket_callback_on_writable on a connection, you will get one of these callbacks coming when the connection socket is able to accept another write packet without blocking.
about: If it already was able to take another packet without blocking, you'll get this callback at the next call to the service loop function.
End Rem
Const LWS_CALLBACK_SERVER_WRITEABLE:Int = 10
Rem
bbdoc: An http request has come from a client that is not asking to upgrade the connection to a websocket one.
about: This is a chance to serve http content, for example, to send a script to the client which will then open the
websockets connection. in points to the URI path requested and libwebsockets_serve_http_file makes it very simple to send
back a file to the client. Normally after sending the file you are done with the http connection, since the rest of the
activity will come by websockets from the script that was delivered by http, so you will want to return 1; to close and
free up the connection. That's important because it uses a slot in the total number of client connections allowed set
by MAX_CLIENTS.
End Rem
Const LWS_CALLBACK_HTTP:Int = 11
Rem
bbdoc: A file requested to be send down http link has completed.
End Rem
Const LWS_CALLBACK_HTTP_FILE_COMPLETION:Int = 12
Rem
bbdoc: You can write more down the http protocol link now.
End Rem
Const LWS_CALLBACK_HTTP_WRITEABLE:Int = 13
Rem
bbdoc: 
End Rem
Const LWS_CALLBACK_FILTER_NETWORK_CONNECTION:Int = 14
Const LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:Int = 15
Const LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS:Int = 16
Const LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS:Int = 17
Const LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:Int = 18
Const LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:Int = 19
Const LWS_CALLBACK_CONFIRM_EXTENSION_OKAY:Int = 20
Const LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:Int = 21
Const LWS_CALLBACK_PROTOCOL_INIT:Int = 22
Const LWS_CALLBACK_PROTOCOL_DESTROY:Int = 23
' external poll() management support
Const LWS_CALLBACK_ADD_POLL_FD:Int = 24
Const LWS_CALLBACK_DEL_POLL_FD:Int = 25
Const LWS_CALLBACK_SET_MODE_POLL_FD:Int = 26
Const LWS_CALLBACK_CLEAR_MODE_POLL_FD:Int = 27


Const LWS_EXT_CALLBACK_SERVER_CONTEXT_CONSTRUCT:Int = 0
Const LWS_EXT_CALLBACK_CLIENT_CONTEXT_CONSTRUCT:Int = 1
Const LWS_EXT_CALLBACK_SERVER_CONTEXT_DESTRUCT:Int = 2
Const LWS_EXT_CALLBACK_CLIENT_CONTEXT_DESTRUCT:Int = 3
Const LWS_EXT_CALLBACK_CONSTRUCT:Int = 4
Const LWS_EXT_CALLBACK_CLIENT_CONSTRUCT:Int = 5
Const LWS_EXT_CALLBACK_CHECK_OK_TO_REALLY_CLOSE:Int = 6
Const LWS_EXT_CALLBACK_CHECK_OK_TO_PROPOSE_EXTENSION:Int = 7
Const LWS_EXT_CALLBACK_DESTROY:Int = 8
Const LWS_EXT_CALLBACK_DESTROY_ANY_WSI_CLOSING:Int = 9
Const LWS_EXT_CALLBACK_ANY_WSI_ESTABLISHED:Int = 10
Const LWS_EXT_CALLBACK_PACKET_RX_PREPARSE:Int = 11
Const LWS_EXT_CALLBACK_PACKET_TX_PRESEND:Int = 12
Const LWS_EXT_CALLBACK_PACKET_TX_DO_SEND:Int = 13
Const LWS_EXT_CALLBACK_HANDSHAKE_REPLY_TX:Int = 14
Const LWS_EXT_CALLBACK_FLUSH_PENDING_TX:Int = 15
Const LWS_EXT_CALLBACK_EXTENDED_PAYLOAD_RX:Int = 16
Const LWS_EXT_CALLBACK_CAN_PROXY_CLIENT_CONNECTION:Int = 17
Const LWS_EXT_CALLBACK_1HZ:Int = 18
Const LWS_EXT_CALLBACK_REQUEST_ON_WRITEABLE:Int = 19
Const LWS_EXT_CALLBACK_IS_WRITEABLE:Int = 20
Const LWS_EXT_CALLBACK_PAYLOAD_TX:Int = 21
Const LWS_EXT_CALLBACK_PAYLOAD_RX:Int = 22



Rem
bbdoc: 
End Rem
Const LWS_CLOSE_STATUS_NOSTATUS:Int = 0
Rem
bbdoc: Indicates a normal closure, meaning that the purpose for which the connection was established has been fulfilled.
end rem
Const LWS_CLOSE_STATUS_NORMAL:Int = 1000
Rem
bbdoc: Indicates that an endpoint is "going away", such as a server going down or a browser having navigated away from a page.
End Rem
Const LWS_CLOSE_STATUS_GOINGAWAY:Int = 1001
Rem
bbdoc: Indicates that an endpoint is terminating the connection due to a protocol error.
End Rem
Const LWS_CLOSE_STATUS_PROTOCOL_ERR:Int = 1002
Rem
bbdoc: Indicates that an endpoint is terminating the connection because it has received a type of data it cannot accept (e.g., an endpoint that understands only text data MAY send this if it receives a binary message).
End Rem
Const LWS_CLOSE_STATUS_UNACCEPTABLE_OPCODE:Int = 1003
Rem
bbdoc: Reserved.
about: The specific meaning might be defined in the future.
End Rem
Const LWS_CLOSE_STATUS_RESERVED:Int = 1004
Rem
bbdoc: A reserved value and MUST NOT be set as a status code in a Close control frame by an endpoint.
about: It is designated for use in applications expecting a status code to indicate that no status
      code was actually present.
End Rem
Const LWS_CLOSE_STATUS_NO_STATUS:Int = 1005
Rem
bbdoc: A reserved value and MUST NOT be set as a status code in a Close control frame by an endpoint.
about: It is designated for use in
      applications expecting a status code to indicate that the
      connection was closed abnormally, e.g., without sending or
      receiving a Close control frame.
End Rem
Const LWS_CLOSE_STATUS_ABNORMAL_CLOSE:Int = 1006
Rem
bbdoc: Indicates that an endpoint is terminating the connection because it has received data within a message that was not consistent with the type of the message (e.g., non-UTF-8 [RFC3629] data within a text message).
End Rem
Const LWS_CLOSE_STATUS_INVALID_PAYLOAD:Int = 1007
Rem
bbdoc: Indicates that an endpoint is terminating the connection because it has received a message that violates its policy.
about: This is a generic status code that can be returned when there is no
      other more suitable status code (e.g., 1003 or 1009) or if there
      is a need to hide specific details about the policy.
End Rem
Const LWS_CLOSE_STATUS_POLICY_VIOLATION:Int = 1008
Rem
bbdoc: Indicates that an endpoint is terminating the connection because it has received a message that is too big for it to process.
End Rem
Const LWS_CLOSE_STATUS_MESSAGE_TOO_LARGE:Int = 1009
Rem
bbdoc: Indicates that an endpoint (client) is terminating the connection because it has expected the server to negotiate one or more extension, but the server didn't return them in the response message of the WebSocket handshake.
about: The list of extensions that are needed SHOULD appear in the /reason/ part of the Close frame.
      Note that this status code is not used by the server, because it
      can fail the WebSocket handshake instead.
End Rem
Const LWS_CLOSE_STATUS_EXTENSION_REQUIRED:Int = 1010
Rem
bbdoc: Indicates that a server is terminating the connection because it encountered an unexpected condition that prevented it from fulfilling the request.
End Rem
Const LWS_CLOSE_STATUS_UNEXPECTED_CONDITION:Int = 1011
Rem
bbdoc: A reserved value and MUST NOT be set as a status code in a Close control frame by an endpoint.
about: It is designated for use in applications expecting a status code to indicate that the
      connection was closed due to a failure to perform a TLS handshake
      (e.g., the server certificate can't be verified).
End Rem
Const LWS_CLOSE_STATUS_TLS_FAILURE:Int = 1015

Const LWS_WRITE_TEXT:Int = 0
Const LWS_WRITE_BINARY:Int = 1
Const LWS_WRITE_CONTINUATION:Int = 2
Const LWS_WRITE_HTTP:Int = 3


Const LWS_SEND_BUFFER_PRE_PADDING:Int = (4 + 10 + (2 * 2))
Const LWS_SEND_BUFFER_POST_PADDING:Int = 4

