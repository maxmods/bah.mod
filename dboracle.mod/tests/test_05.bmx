SuperStrict

Framework BaH.DBOracle
Import BRL.filesystem
Import PUB.Stdc

'putenv_("ORACLE_HOME=/Users/brucey/Documents/programming/c/ohome")

'DebugStop
Local db:TDBConnection = LoadDatabase("ORACLE", "xe", "192.168.2.31", Null, "brucey", "brucey")


If Not db Then
	DebugLog("Didn't work...")
	End
End If

If db.hasError() Then
	errorAndClose(db)
End If


If db.isOpen() Then

	' get a list of tables in the database
	Local list:String[] = db.getTables()
	
	If list Then
		For Local i:Int = 0 Until list.length
			DebugLog("   " + (i + 1) + ".   " + list[i])
		Next
	End If

	db.close()
	
End If

Function errorAndClose(db:TDBConnection)
	DebugLog(db.error().toString())
	db.close()
	End
End Function



