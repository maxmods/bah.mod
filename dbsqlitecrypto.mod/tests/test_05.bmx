SuperStrict

Framework BaH.DBSQLite
Import BRL.filesystem

Local db:TDBConnection = LoadDatabase("SQLITE_CRYPTO", "maxtest.db")

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


