SuperStrict

Framework BaH.DBMySQL
Import BRL.filesystem

Local db:TDBConnection = LoadDatabase("MYSQL", "maxtest", "192.168.0.31", 0, "brucey", "brucey")

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

