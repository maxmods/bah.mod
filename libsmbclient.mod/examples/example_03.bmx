'
' Example showing stream reading using the samba client
' Use the smb:: protocol to create the right stream
'
'
SuperStrict

Framework BaH.LibSMBClient
Import BRL.StandardIO
Import BRL.TextStream

Local server:String = "10.211.55.6"
Local share:String = "Share"
Local file:String = "smb::" + server + "/" + share + "/example.txt"

Local client:TSMBC = New TSMBClient.Create()

' Load a string with TextStream interface
Local text:String = LoadText(file)
Print "Text length = " + text.Length

' load by line
' NOTE : this method of line reading is *extremely* slow, mainly because it reads the file a byte at a time,
'    rather than as a buffered block of bytes.
Local stream:TStream = ReadStream(file)
Local lineCount:Int

If stream Then
	While Not stream.Eof()
		Local line:String = stream.ReadLine()	
		
		lineCount :+ 1
	Wend
	
	Print "File contains " + lineCount + " lines."

	stream.Close()
End If


Type TSMBClient Extends TSMBC

	Method GetAuthorization(auth:TSMBCAuth)
		auth.workgroup = "MSHOME"
		auth.username = "brucey"
		auth.password = ""
	End Method

End Type
