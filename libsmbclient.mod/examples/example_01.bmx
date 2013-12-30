'
' Example showing basic directory list using the samba client
'
'
SuperStrict

Framework BaH.LibSMBClient
Import BRL.StandardIO

Local server:String = "10.211.55.6"
Local share:String = "Share"

Local client:TSMBC = New TSMBClient.Create()

Local dir:TSMBCDirHandle = client.ReadDir("smb://" + server + "/" + share + "/")

If dir Then
	
	Local file:TSMBCDirent = client.NextFile(dir)
	
	While file
		Print file.name	

		file = client.NextFile(dir)	
	Wend
	
	client.CloseDir(dir)
	
End If

Type TSMBClient Extends TSMBC

	Method GetAuthorization(auth:TSMBCAuth)
		auth.workgroup = "MSHOME"
		auth.username = "brucey"
		auth.password = ""
	End Method

End Type
