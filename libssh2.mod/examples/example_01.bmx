SuperStrict


Framework BaH.libssh2
Import BRL.StandardIO


Const ipAddress:String = "127.0.0.1"
Const username:String = "username"
Const password:String = "password"


' Ultra basic "connect to port 22 on localhost"
' Your code is responsible for creating the socket establishing the connection

Local socket:TSocket = TSocket.CreateTCP()

If Not socket.Connect(HostIp(ipAddress), 22) Then

	Print "Failed to connect"
	End

End If


' Create a session instance and start it up
' This will trade welcome banners, exchange keys, and setup crypto, compression, and MAC layers
Local session:TSSHSession = New TSSHSession

If session.Startup(socket) Then
	Print "Failure establishing SSH session"
	End
End If

If session.UserAuthKeyboardInteractive(username, kbdCallback) Then
	Print "Authentication by keyboard-interactive failed!"
Else
	Print "Authentication by keyboard-interactive succeeded."
End If

Local channel:TSSHChannel = session.OpenChannel()

If Not channel Then
	Print "Unable to open a session"
	End
End If

' Some environment variables may be set,
' It's up to the server which ones it'll allow though

channel.SetEnv("FOO", "bar")

' Request a terminal with 'vanilla' terminal emulation
' See /etc/termcap for more options

If channel.RequestPty("vanilla") Then
	Print "Failed requesting pty"
	End
End If

' Open a SHELL on that pty

If channel.Shell() Then
	Print "Unable to request shell on allocated pty"
	End
End If


'
' The following is by no means the correct way to interact with a shell..
'
Local buffer:Byte[2048]
Local count:Int = channel.Read(buffer, 2048)

While count > 0
	WriteStdout String.FromBytes(buffer, count)
	
	Local read:Int = socket._socket
	If select_( 1,Varptr read,0,Null,0,Null,1000 ) <= 0 Then
		Exit
	End If
	
	count = channel.Read(buffer, 2048)
Wend

' get a directory listing...
Local b:Byte Ptr = "ls~n".ToCString()
count = channel.Write(b, 3)
MemFree(b)

' ... read until output is finished.
count = channel.Read(buffer, 2048)
While count > 0
	WriteStdout String.FromBytes(buffer, count)
	
	Local read:Int = socket._socket
	If select_( 1,Varptr read,0,Null,0,Null,1000 ) <= 0 Then
		Exit
	End If
	
	count = channel.Read(buffer, 2048)
Wend


' callback function to handle Keyboard Interactive Auth.
Function kbdCallback(name:String, instruction:String, prompts:TSSHKeyboardPrompt[], responses:TSSHKeyboardResponse[])

	If prompts.length = 1 Then
		responses[0].SetText(password)
	End If

End Function
