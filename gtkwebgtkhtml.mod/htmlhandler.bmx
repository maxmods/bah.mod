' Copyright (c) 2006-2008 Bruce A Henderson
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

'Import brl.standardio ' for debug - Print

Type THtmlHandler

	Const REMOVE_NONE:Int = 0
	Const REMOVE_PROTOCOL:Int = 1
	Const REMOVE_USERNAME:Int = 2
	Const REMOVE_PASSWORD:Int = 4
	Const REMOVE_HOSTNAME:Int = 8
	Const REMOVE_PORT:Int = 16
	Const REMOVE_PATH:Int = 32
	Const REMOVE_REFERENCE:Int = 64
	Const REMOVE_CGIARGS:Int = 128

	Field _protocol:String
	Field _username:String
	Field _password:String
	Field _hostname:String
	Field _port:Int
	Field _path:String
	Field _reference:String


	Function newURL:THtmlHandler(url:String)
		Local this:THtmlHandler = New THtmlHandler
		Local p:Int
		Local path_start:Int
		Local s_len:Int
	
		If url = Null Or url.length = 0 Then
			' The Path can't be NULL.
			If this._path = Null Then
				this._path = "/"
			End If
			Return this
		End If
	
		s_len = url.length
	
		While p < s_len
			Local t:String = url[p..p+1]
			If Not ((t >= "a" And t <= "z") ..
		       Or (t >= "A" And t <= "Z") ..
		       Or (t >= "0" And t <= "9")) Then
				Exit
			End If
			p:+1
		Wend
	
		If url[p..p+1] <> ":" Then
			' No protocol.
			path_start = 0
		Else
			this._protocol = url[0..p]
	
			' Check For a host name And a port.
			If url[p+1..p+2] = "/" And url[p+2..p+3] = "/" Then
				path_start = p + 3 + this.scan_host_info(url[p+3..])
			Else
				path_start = p + 1
			End If
		End If
	
		' Look For a reference.
	
		Local reference_ptr:Int = -1
		p = s_len
		While p <> path_start
			p:-1
			If url[p..p+1] = "#" Then
				reference_ptr = p + 1
			End If
		Wend
	
		If reference_ptr <> -1 Then
			this._reference = url[reference_ptr..]
			If url[path_start..path_start+1] <> "/" Then
				this._path = "/" +  url[path_start..]
			Else
				this._path = url[path_start..path_start + ((reference_ptr - 1) - path_start)]
			End If
		Else
			this._path = url[path_start..]
		End If
	
		' The Path can't be NULL.
		If this._path = Null Then
			this._path = "/"
		End If
	
		Return this
	End Function

	Method duplicate:THtmlHandler(flags:Int)

		Local _new:THtmlHandler = New THtmlHandler
	
		If flags & REMOVE_PROTOCOL Then
			_new._protocol = Null
		Else
			_new._protocol = _protocol
		End If
	
		If flags & REMOVE_USERNAME Then
			_new._username = Null
		Else
			_new._username = _username
		End If
	
		If flags & REMOVE_PASSWORD Then
			_new._password = Null
		Else
			_new._password = _password
		End If
	
		If flags & REMOVE_HOSTNAME Then
			_new._HostName = Null
		Else
			_new._HostName = _hostName
		End If
	
		If flags & REMOVE_PORT Then
			_new._port = 0;
		Else
			_new._port = _port
		End If
	
		If flags & REMOVE_PATH Then
			_new._path = Null
		Else
			_new._path = _path
		End If
	
		If (flags & REMOVE_CGIARGS) And _new._path Then
			' Cut the path after the first '?'
			Local p:Int = _new._path.find("?")
			If p Then
				_new._path = _new._path[..p]
			End If
		End If
	
		If flags & REMOVE_REFERENCE Then
			_new._reference = Null
		Else
			_new._reference = _reference
		End If
	
		Return _new
	End Method
	
	Method scan_host_info:Int(url:String)
		Local slash_ptr:Int
		Local at_ptr:Int
		Local colon_ptr:Int
		Local host_ptr:Int
		Local rest_ptr:Int
	
		slash_ptr = url.find("/")
		If slash_ptr < 0 Then
			host_ptr = 0
			rest_ptr = url.length
		Else
			at_ptr = url.find("@", slash_ptr)
	
			If at_ptr < 0 Then
				' No `@', so it's going to be a hostname only.
				host_ptr = 0
			Else
				host_ptr = at_ptr + 1
	
	
				' Check If we have a password.
	
				colon_ptr = url.find(":", host_ptr)
				If colon_ptr < 0 Then
					_username  = url[at_ptr..]
				Else
					_username = url[..colon_ptr]
					_password = url[colon_ptr + 1..colon_ptr + (slash_ptr - (colon_ptr + 1))]
				End If
			End If
	
			rest_ptr = url.find("/", host_ptr)
			If rest_ptr < 0 Then
				rest_ptr = url.length
			End If
		End If
	
		' Look For a port number And set the host name.
	
		colon_ptr = url.find(":", host_ptr)
		If colon_ptr < 0 Then
			_hostName = url[host_ptr..rest_ptr]
		Else
			Local port:String
	
			'If (sscanf (colon_ptr + 1, "%d", &port) == 1) Then
			'	_port = (guint16) port;
			'End If
	
			_hostName = url[host_ptr..host_ptr +(colon_ptr - host_ptr)]
		End If
	
		Return rest_ptr
	
	End Method

	Method setReference(ref:String)
		_reference = ref
	End Method
	
	Method toString:String()

		Local url:String

		Local reference_length:Int
		Local protocol_length:Int
		Local hostname_length:Int
		Local username_length:Int
		Local password_length:Int
		Local path_length:Int
		Local port_length:Int
		Local port_string:String = Null;

		Local length:Int
	
		If _protocol <> Null And _protocol.length > 0 Then
			protocol_length = _protocol.length
			If protocol_length > 0 Then
				length:+ protocol_length + 1
			End If
		End If

		If _HostName <> Null And _HostName.length > 0 Then
			hostname_length = _hostName.length
			length:+ hostname_length + 2
	
			If _username <> Null And _username.length > 0 Then
				username_length = _username.length
				length:+ username_length
	
				If _password <> Null And _password.length > 0 Then
					password_length = _password.length
					length:+ password_length + 1
				End If
				length:+ 1
			End If
	
			If _port <> 0 Then
				port_string = String.fromInt(_port)
				port_length = port_string.length + 1
			End If
		End If
	
		If _path <> Null And _path.length > 0 Then
			path_length = _path.length
			length:+ path_length
	
			If _reference <> Null And _reference.length > 0 Then
	
				reference_length = _reference.length
				length:+ reference_length + 1
			End If
		End If
	
		length:+ port_length
	
		If length = 0 Then
			Return ""
		End If
	
		If protocol_length <> 0 Then
			url:+ _protocol + ":"
		End If
	
		If hostname_length <> 0 Then
			url:+ "//"
	
			If username_length <> 0 Then
				url:+ _username
				If password_length <> 0 Then
					url:+ ":" + _password
				End If
				url:+ "@"
			End If
	
			url:+ _hostName
	
			If port_length <> 0 Then
				url:+ ":" + port_string
			End If
		End If
	
		' Notice that the `path' part is always supposed to start with a
	     '      slash, so we don't need to append the slash here.  */
	
		If path_length <> 0 Then
			url:+ _path
		End If
	
		If reference_length <> 0 Then
			url:+ "#" + _reference
		End If
	
		Return url
	End Method
	
	Method setPath(path:String)
		_path = path
	End Method
	
	Method getProtocol:String()
		Return _protocol
	End Method
	
	Method AppendPath:THtmlHandler(path:String)
'Print "AppendPath()"
	End Method
End Type

