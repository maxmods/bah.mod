
'
' Example showing directory traversal using the samba client with multiple contexts
'
'
SuperStrict

Framework BaH.LibSMBClient
Import BRL.StandardIO

Local server:String = "10.211.55.6"
Local share:String = "Share"

Local client:TSMBC = New TSMBClient.Create()

Local path:String = "smb://" + server + "/" + share + "/"

' do the directory traversal
Traverse(client, path)


Type TSMBClient Extends TSMBC

	Method GetAuthorization(auth:TSMBCAuth)
		auth.workgroup = "MSHOME"
		auth.username = "brucey"
		auth.password = ""
	End Method

End Type


Function Traverse(client:TSMBC, path:String, group:String = "", level:Int = 0)
	Local list:TSMBDirent[] = client.LoadDir(path)
	
	For Local file:TSMBDirEnt = EachIn list

		Select file.ftype
			Case SMBC_WORKGROUP, SMBC_SERVER
			
				If file.ftype = SMBC_WORKGROUP Then
					group = file.name
					PrintPath("", group, level + 1)
				Else
					PrintPath(file.name, group, level + 1)
				End If
				
				path = "smb://" + file.name
				
				Local client2:TSMBC = New TSMBClient.Create()
				
				If client2 Then
					Traverse(client2, path, group, level + 2)
					client2.Free()
				Else
					Traverse(client, path, group, level + 1)
					client.PurgeCachedServers()
				End If
				
			
			Case SMBC_FILE_SHARE, SMBC_DIR, SMBC_FILE
			
				Local fpath:String = path
				
				If Not fpath.EndsWith("/") Then
					fpath :+ "/"
				End If
				
				fpath :+ file.name
			
				PrintPath(fpath, group, level)
				
				If file.ftype <> SMBC_FILE Then
				
					Traverse(client, fpath, group, level + 1)
					
					If file.ftype = SMBC_FILE_SHARE Then
						client.PurgeCachedServers()
					End If
				
				End If
				
	
		End Select
	Next
	
End Function

Function PrintPath(path:String, group:String, level:Int)

	Local indent:String = ""

	For Local i:Int = 0 Until level
		indent:+ "   "
	Next
	
	indent :+ "/"
	
	If Not path And Not group Then
		Print indent
	End If

	If Not path Then
		Print group
	End If
	
	If Not group Then
		group = "(unknown_group)"
	End If
	
	If path.StartsWith("smb://") Then
		Print indent + group + "/" + path[6..]
	Else
		Print indent + group + "/" + path
	End If

End Function

