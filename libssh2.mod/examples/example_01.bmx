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





' callback function to handle Keyboard Interactive Auth.
Function kbdCallback(name:String, instruction:String, prompts:TSSHKeyboardPrompt[], responses:TSSHKeyboardResponse[])

	If prompts.length = 1 Then
		responses[0].SetText(password)
	End If

End Function
