' Copyright (c) 2009 Bruce A Henderson
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


Rem
bbdoc: Libssh2
End Rem
Module BaH.libssh2

ModuleInfo "Version: 1.00"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: Wrapper - 2009 Bruce A Henderson"

?win32
ModuleInfo "LD_OPTS: -L%PWD%/ssl/lib"
?

Import BRL.Socket
Import "common.bmx"

Rem
bbdoc: An SSH session object.
End Rem
Type TSSHSession

	Field sessionPtr:Byte Ptr
	
	Field interactiveCallback(name:String, instruction:String, prompts:TSSHKeyboardPrompt[], responses:TSSHKeyboardResponse[])
	
	Method New()
		sessionPtr = bmx_libssh2_session_create(Self)
	End Method
	
	Rem
	bbdoc: Indicates whether or not the named session has been successfully authenticated.
	returns: True if authenticated and False if not.
	End Rem
	Method Authenticated:Int()
		Return bmx_libssh2_userauth_authenticated(sessionPtr)
	End Method
	
	Rem
	bbdoc: Tunnels a TCP/IP connection through the SSH transport via the remote host to a third party.
	returns: A newly allocated LIBSSH2_CHANNEL instance, or NULL on errors.
	about: Communication from the client to the SSH server remains encrypted, communication from the server to the 3rd party host
	travels in cleartext.
	End Rem
	Method DirectTCPIP:TSSHChannel(host:String, port:Int, shost:String = Null, sport:Int = 0)
		Return TSSHChannel._create(bmx_libssh2_channel_directtcpip(sessionPtr, host, port, shost, sport))
	End Method
	
	Rem
	bbdoc: Begins transport layer protocol negotiation with the connected host. 
	returns: 0 on success, negative on failure. 
	End Rem
	Method Startup:Int(socket:TSocket)
		Return bmx_libssh2_session_startup(sessionPtr, socket._socket)
	End Method

	Rem
	bbdoc: Returns the computed digest of the remote system's hostkey.
	returns: Computed hostkey hash value, or NULL if the session has not yet been started up. (The hash consists of raw binary bytes, not hex digits, so is not directly printable.)
	about: The length of the returned string is hashType specific (e.g. 16 bytes for MD5, 20 bytes for SHA1). 
	End Rem
	Method HostkeyHash:String(hashType:Int)
	' TODO
	End Method
	
	Rem
	bbdoc: Sends a disconnect message to the remote host associated with session, along with a verbose description. 
	returns: 0 on success or negative on failure. 
	End Rem
	Method Disconnect:Int(description:String)
		Return bmx_libssh2_session_disconnect(sessionPtr, description)
	End Method
	
	Rem
	bbdoc: Allocates a new channel for exchanging data with the server.
	End Rem
	Method OpenChannel:TSSHChannel()
		Return TSSHChannel._create(bmx_libssh2_channel_open(sessionPtr))
	End Method

	Rem
	bbdoc: Attempts basic password authentication.
	returns: 0 on success or negative on failure. It returns LIBSSH2_ERROR_EAGAIN when it would otherwise block. While LIBSSH2_ERROR_EAGAIN is a negative number, it isn't really a failure per se.
	about: Note that many SSH servers which appear to support ordinary password authentication actually have it disabled and use
	Keyboard Interactive authentication (routed via PAM or another authentication backed) instead.
	End Rem
	Method UserAuthPassword:Int(username:String, password:String)
		Return bmx_libssh2_userauth_password(sessionPtr, username, password)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method UserAuthKeyboardInteractive:Int(username:String, callback:Int(name:String, instruction:String, ..
			prompts:TSSHKeyboardPrompt[], responses:TSSHKeyboardResponse[]))
		
		interactiveCallback = callback
		Return bmx_libssh2_userauth_keyboard_interactive(sessionPtr, username)
	End Method
	
	Function _kbdCallback(session:TSSHSession, name:String, instruction:String, ..
			prompts:TSSHKeyboardPrompt[], responses:TSSHKeyboardResponse[])
		session.interactiveCallback(name, instruction, prompts, responses)
	End Function
	
	Function _newPrompts:TSSHKeyboardPrompt[](size:Int)
		Return New TSSHKeyboardPrompt[size]
	End Function
	
	Function _setPrompt(prompts:TSSHKeyboardPrompt[], index:Int, promptPtr:Byte Ptr)
		prompts[index] = TSSHKeyboardPrompt._create(promptPtr)
	End Function

	Function _newResponses:TSSHKeyboardResponse[](size:Int)
		Return New TSSHKeyboardResponse[size]
	End Function

	Function _setResponse(responses:TSSHKeyboardResponse[], index:Int, responsePtr:Byte Ptr)
		responses[index] = TSSHKeyboardResponse._create(responsePtr)
	End Function
	
	Rem
	bbdoc: Frees resources associated with the session instance.
	about: Typically called after Disconnect().
	End Rem
	Method Free()
		If sessionPtr Then
			bmx_libssh2_session_free(sessionPtr)
			sessionPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TSSHChannel

	Field channelPtr:Byte Ptr
	
	Function _create:TSSHChannel(channelPtr:Byte Ptr)
		If channelPtr Then
			Local this:TSSHChannel = New TSSHChannel
			this.channelPtr = channelPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Sets an environment variable in the remote channel's process space.
	about: Note that this does not make sense for all channel types and may be ignored by the server despite returning success.
	End Rem
	Method SetEnv:Int(name:String, value:String)
		Return bmx_libssh2_channel_setenv(channelPtr, name, value)
	End Method
	
	Rem
	bbdoc: Requests a PTY on an established channel.
	about: Note that this does not make sense for all channel types and may be ignored by the server despite returning success.
	End Rem
	Method RequestPty:Int(term:String)
		Return bmx_libssh2_channel_requestpty(channelPtr, term)
	End Method
	
	Rem
	bbdoc: Initiates a shell request.
	End Rem
	Method Shell:Int()
		Return bmx_libssh2_channel_shell(channelPtr)
	End Method
	
	Rem
	bbdoc: Checks if the remote host has sent an EOF status for the selected stream.
	End Rem
	Method Eof:Int()
		Return bmx_libssh2_channel_eof(channelPtr)
	End Method
	
	Rem
	bbdoc: Initiates an exec request.
	End Rem
	Method Exec:Int(message:String)
		Return bmx_libssh2_channel_exec(channelPtr, message)
	End Method
	
	Rem
	bbdoc: Returns the exit code raised by the process running on the remote host at the other end of the named channel.
	returns: 0 on failure, otherwise the Exit Status reported by remote host.
	about: Note that the exit status may not be available if the remote end has not yet set its status to closed.
	End Rem
	Method GetExitStatus:Int()
		Return bmx_libssh2_channel_getexitstatus(channelPtr)
	End Method
	
	Rem
	bbdoc: Initiates a subsystem request.
	End Rem
	Method Subsystem:Int(message:String)
		Return bmx_libssh2_channel_subsystem(channelPtr, message)
	End Method
	
	Rem
	bbdoc: Checks to see if data is available in the channel's read buffer.
	returns: True when data is available and False otherwise.
	about: No attempt is made with this method to see if packets are available to be processed.
	End Rem
	Method PollRead:Int(extended:Int)
		Return bmx_libssh2_channel_pollread(channelPtr, extended)
	End Method
	
	Rem
	bbdoc: Initiates a request on the channel.
	about: The SSH2 protocol currently defines shell, exec, and subsystem as standard process services.
	End Rem
	Method ProcessStartup:String(request:String, message:String)
		Return bmx_libssh2_channel_processstartup(channelPtr, request, message)
	End Method
	
	Rem
	bbdoc: Attempts to read data from an active channel stream.
	returns: Actual number of bytes read or negative on failure. It returns LIBSSH2_ERROR_EAGAIN when it would otherwise block. While LIBSSH2_ERROR_EAGAIN is a negative number, it isn't really a failure per se.
	End Rem
	Method Read:Int(buffer:Byte Ptr, size:Int)
		Return bmx_libssh2_channel_read(channelPtr, buffer, size)
	End Method
	
	Rem
	bbdoc: Attempts to read data from the stderr substream.
	returns: Actual number of bytes read or negative on failure. It returns LIBSSH2_ERROR_EAGAIN when it would otherwise block. While LIBSSH2_ERROR_EAGAIN is a negative number, it isn't really a failure per se.
	End Rem
	Method ReadStderr:Int(buffer:Byte Ptr, size:Int)
		Return bmx_libssh2_channel_read_stderr(channelPtr, buffer, size)
	End Method
	
	Rem
	bbdoc: Tells the remote host that no further data will be sent on the specified channel.
	returns: 0 on success or negative on failure. It returns LIBSSH2_ERROR_EAGAIN when it would otherwise block. While LIBSSH2_ERROR_EAGAIN is a negative number, it isn't really a failure per se.
	about: Processes typically interpret this as a closed stdin descriptor.
	End Rem
	Method SendEof:Int()
		Return bmx_libssh2_channel_sendeof(channelPtr)
	End Method
	
	Rem
	bbdoc: Sets or clears blocking mode on channel.
	about: Currently this is just a short cut call to TSSHSession::SetBlocking() and therefore will affect the session and all channels.
	End Rem
	Method SetBlocking(blocking:Int)
		bmx_libssh2_channel_set_blocking(channelPtr, blocking)
	End Method
	
	Rem
	bbdoc: Enters a temporary blocking state until the remote host closes the named channel.
	returns: 0 on success or negative on failure. It returns LIBSSH2_ERROR_EAGAIN when it would otherwise block. While LIBSSH2_ERROR_EAGAIN is a negative number, it isn't really a failure per se.
	about: Typically sent after TSSHChannel.Close() in order to examine the exit status.
	End Rem
	Method WaitClosed:Int()
		Return bmx_libssh2_channel_waitclosed(channelPtr)
	End Method
	
	Rem
	bbdoc: Waits for the remote end to acknowledge an EOF request.
	returns: 0 on success or negative on failure. It returns LIBSSH2_ERROR_EAGAIN when it would otherwise block. While LIBSSH2_ERROR_EAGAIN is a negative number, it isn't really a failure per se.
	End Rem
	Method WaitEof:Int()
		Return bmx_libssh2_channel_waiteof(channelPtr)
	End Method
	
	Rem
	bbdoc: Writes data to a channel stream.
	returns: Actual number of bytes written or negative on failure. LIBSSH2_ERROR_EAGAIN when it would otherwise block. While LIBSSH2_ERROR_EAGAIN is a negative number, it isn't really a failure per se.
	End Rem
	Method Write:Int(buffer:Byte Ptr, size:Int)
		Return bmx_libssh2_channel_write(channelPtr, buffer, size)
	End Method
	
	Rem
	bbdoc: Writes data to the stderr substream.
	returns: Actual number of bytes written or negative on failure. LIBSSH2_ERROR_EAGAIN when it would otherwise block. While LIBSSH2_ERROR_EAGAIN is a negative number, it isn't really a failure per se.
	End Rem
	Method WriteStderr:Int(buffer:Byte Ptr, size:Int)
		Return bmx_libssh2_channel_write_stderr(channelPtr, buffer, size)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Free()
		If channelPtr Then
			bmx_libssh2_channel_free(channelPtr)
			channelPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TSSHKeyboardResponse

	Field responsePtr:Byte Ptr
	
	Function _create:TSSHKeyboardResponse(responsePtr:Byte Ptr)
		If responsePtr Then
			Local this:TSSHKeyboardResponse = New TSSHKeyboardResponse
			this.responsePtr = responsePtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method SetText(text:String)
		bmx_libssh2_kbdint_response_settext(responsePtr, text)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TSSHKeyboardPrompt

	Field promptPtr:Byte Ptr
	
	Function _create:TSSHKeyboardPrompt(promptPtr:Byte Ptr)
		If promptPtr Then
			Local this:TSSHKeyboardPrompt = New TSSHKeyboardPrompt
			this.promptPtr = promptPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method GetText:String()
		Return bmx_libssh2_kbdint_prompt_gettext(promptPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Echo:Int()
		Return bmx_libssh2_kbdint_prompt_echo(promptPtr)
	End Method
	
End Type


