' Copyright (c) 2009-2010 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the auther nor the names of its contributors may be used to 
'       endorse or promote products derived from this software without specific
'       prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

?linux
Import "-ldl"
Import "-lidn"
Import "-lssl"
Import "-lcrypto"
?win32
Import "-ladvapi32"
Import "-leay32"
Import "-lssleay32"
?macos
Import "-lssl"
Import "-lcrypto"
?

Import "source.bmx"

Extern

	Function bmx_libssh2_session_create:Byte Ptr(handle:Object)
	Function bmx_libssh2_session_startup:Int(handle:Byte Ptr, socket:Int)
	Function bmx_libssh2_session_free:Int(handle:Byte Ptr)
	Function bmx_libssh2_session_disconnect:Int(handle:Byte Ptr, description:String)

	Function bmx_libssh2_userauth_keyboard_interactive:Int(handle:Byte Ptr, username:String)
	Function bmx_libssh2_userauth_password:Int(handle:Byte Ptr, username:String, password:String)
	Function bmx_libssh2_userauth_authenticated:Int(handle:Byte Ptr)

	Function bmx_libssh2_kbdint_prompt_gettext:String(handle:Byte Ptr)
	Function bmx_libssh2_kbdint_prompt_echo:Int(handle:Byte Ptr)
	
	Function bmx_libssh2_kbdint_response_settext(handle:Byte Ptr, text:String)

	Function bmx_libssh2_channel_open:Byte Ptr(handle:Byte Ptr)
	Function bmx_libssh2_channel_setenv:Int(handle:Byte Ptr, name:String, value:String)
	Function bmx_libssh2_channel_requestpty:Int(handle:Byte Ptr, term:String)
	Function bmx_libssh2_channel_shell:Int(handle:Byte Ptr)
	Function bmx_libssh2_channel_exec:Int(handle:Byte Ptr, message:String)
	Function bmx_libssh2_channel_subsystem:Int(handle:Byte Ptr, message:String)
	Function bmx_libssh2_channel_processstartup:Int(handle:Byte Ptr, request:String, message:String)
	Function bmx_libssh2_channel_free(handle:Byte Ptr)
	Function bmx_libssh2_channel_read:Int(handle:Byte Ptr, buffer:Byte Ptr, size:Int)
	Function bmx_libssh2_channel_read_stderr:Int(handle:Byte Ptr, buffer:Byte Ptr, size:Int)
	Function bmx_libssh2_channel_write:Int(handle:Byte Ptr, buffer:Byte Ptr, size:Int)
	Function bmx_libssh2_channel_write_stderr:Int(handle:Byte Ptr, buffer:Byte Ptr, size:Int)
	Function bmx_libssh2_channel_eof:Int(handle:Byte Ptr)
	Function bmx_libssh2_channel_set_blocking(handle:Byte Ptr, blocking:Int)
	Function bmx_libssh2_channel_pollread:Int(handle:Byte Ptr, extended:Int)
	Function bmx_libssh2_channel_sendeof:Int(handle:Byte Ptr)
	Function bmx_libssh2_channel_waitclosed:Int(handle:Byte Ptr)
	Function bmx_libssh2_channel_waiteof:Int(handle:Byte Ptr)
	Function bmx_libssh2_channel_getexitstatus:Int(handle:Byte Ptr)
	Function bmx_libssh2_channel_directtcpip:Byte Ptr(handle:Byte Ptr, host:String, port:Int, shost:String, sport:Int)

End Extern


Const LIBSSH2_POLLFD_SOCKET:Int = 1
Const LIBSSH2_POLLFD_CHANNEL:Int = 2
Const LIBSSH2_POLLFD_LISTENER:Int = 3

Const LIBSSH2_POLLFD_POLLIN:Int = $0001      ' Data available To be read Or connection available -- All
Const LIBSSH2_POLLFD_POLLPRI:Int = $0002      ' Priority data available To be read -- Socket only
Const LIBSSH2_POLLFD_POLLEXT:Int = $0002      ' Extended data available To be read -- Channel only
Const LIBSSH2_POLLFD_POLLOUT:Int = $0004      ' Can May be written -- Socket/Channel
' revents only
Const LIBSSH2_POLLFD_POLLERR:Int = $0008      ' Error Condition -- Socket
Const LIBSSH2_POLLFD_POLLHUP:Int = $0010      ' HangUp/Eof -- Socket
Const LIBSSH2_POLLFD_SESSION_CLOSED:Int = $0010      ' Session Disconnect
Const LIBSSH2_POLLFD_POLLNVAL:Int = $0020      ' Invalid request -- Socket Only
Const LIBSSH2_POLLFD_POLLEX:Int = $0040      ' Exception Condition -- Socket/Win32
Const LIBSSH2_POLLFD_CHANNEL_CLOSED:Int = $0080      ' Channel Disconnect
Const LIBSSH2_POLLFD_LISTENER_CLOSED:Int = $0080      ' Listener Disconnect


' Block Direction Types
Const LIBSSH2_SESSION_BLOCK_INBOUND:Int = $0001
Const LIBSSH2_SESSION_BLOCK_OUTBOUND:Int = $0002

' Hash Types
Const LIBSSH2_HOSTKEY_HASH_MD5:Int = 1
Const LIBSSH2_HOSTKEY_HASH_SHA1:Int = 2

' Disconnect Codes (defined by SSH protocol)
Const SSH_DISCONNECT_HOST_NOT_ALLOWED_TO_CONNECT:Int = 1
Const SSH_DISCONNECT_PROTOCOL_ERROR:Int = 2
Const SSH_DISCONNECT_KEY_EXCHANGE_FAILED:Int = 3
Const SSH_DISCONNECT_RESERVED:Int = 4
Const SSH_DISCONNECT_MAC_ERROR:Int = 5
Const SSH_DISCONNECT_COMPRESSION_ERROR:Int = 6
Const SSH_DISCONNECT_SERVICE_NOT_AVAILABLE:Int = 7
Const SSH_DISCONNECT_PROTOCOL_VERSION_NOT_SUPPORTED:Int = 8
Const SSH_DISCONNECT_HOST_KEY_NOT_VERIFIABLE:Int = 9
Const SSH_DISCONNECT_CONNECTION_LOST:Int = 10
Const SSH_DISCONNECT_BY_APPLICATION:Int = 11
Const SSH_DISCONNECT_TOO_MANY_CONNECTIONS:Int = 12
Const SSH_DISCONNECT_AUTH_CANCELLED_BY_USER:Int = 13
Const SSH_DISCONNECT_NO_MORE_AUTH_METHODS_AVAILABLE:Int = 14
Const SSH_DISCONNECT_ILLEGAL_USER_NAME:Int = 15

' Error Codes (defined by libssh2)
Const LIBSSH2_ERROR_NONE:Int = 0
Const LIBSSH2_ERROR_SOCKET_NONE:Int = -1
Const LIBSSH2_ERROR_BANNER_NONE:Int = -2
Const LIBSSH2_ERROR_BANNER_SEND:Int = -3
Const LIBSSH2_ERROR_INVALID_MAC:Int = -4
Const LIBSSH2_ERROR_KEX_FAILURE:Int = -5
Const LIBSSH2_ERROR_ALLOC:Int = -6
Const LIBSSH2_ERROR_SOCKET_SEND:Int = -7
Const LIBSSH2_ERROR_KEY_EXCHANGE_FAILURE:Int = -8
Const LIBSSH2_ERROR_TIMEOUT:Int = -9
Const LIBSSH2_ERROR_HOSTKEY_INIT:Int = -10
Const LIBSSH2_ERROR_HOSTKEY_SIGN:Int = -11
Const LIBSSH2_ERROR_DECRYPT:Int = -12
Const LIBSSH2_ERROR_SOCKET_DISCONNECT:Int = -13
Const LIBSSH2_ERROR_PROTO:Int = -14
Const LIBSSH2_ERROR_PASSWORD_EXPIRED:Int = -15
Const LIBSSH2_ERROR_FILE:Int = -16
Const LIBSSH2_ERROR_METHOD_NONE:Int = -17
Const LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED:Int = -18
Const LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:Int = -19
Const LIBSSH2_ERROR_CHANNEL_OUTOFORDER:Int = -20
Const LIBSSH2_ERROR_CHANNEL_FAILURE:Int = -21
Const LIBSSH2_ERROR_CHANNEL_REQUEST_DENIED:Int = -22
Const LIBSSH2_ERROR_CHANNEL_UNKNOWN:Int = -23
Const LIBSSH2_ERROR_CHANNEL_WINDOW_EXCEEDED:Int = -24
Const LIBSSH2_ERROR_CHANNEL_PACKET_EXCEEDED:Int = -25
Const LIBSSH2_ERROR_CHANNEL_CLOSED:Int = -26
Const LIBSSH2_ERROR_CHANNEL_EOF_SENT:Int = -27
Const LIBSSH2_ERROR_SCP_PROTOCOL:Int = -28
Const LIBSSH2_ERROR_ZLIB:Int = -29
Const LIBSSH2_ERROR_SOCKET_TIMEOUT:Int = -30
Const LIBSSH2_ERROR_SFTP_PROTOCOL:Int = -31
Const LIBSSH2_ERROR_REQUEST_DENIED:Int = -32
Const LIBSSH2_ERROR_METHOD_NOT_SUPPORTED:Int = -33
Const LIBSSH2_ERROR_INVAL:Int = -34
Const LIBSSH2_ERROR_INVALID_POLL_TYPE:Int = -35
Const LIBSSH2_ERROR_PUBLICKEY_PROTOCOL:Int = -36
Const LIBSSH2_ERROR_EAGAIN:Int = -37


