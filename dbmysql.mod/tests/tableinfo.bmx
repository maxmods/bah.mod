SuperStrict

Framework BaH.DBMySQL
Import BRL.filesystem

Local db:TDBConnection = LoadDatabase("MYSQL", "maxtest", "192.168.2.31", 0, "brucey", "brucey")

If Not db Then
	DebugLog("Didn't work...")
	End
End If

If db.hasError() Then
	errorAndClose(db)
End If


If db.isOpen() Then

	Local table:TDBTable = db.getTableInfo("person", True)
	
	DebugLog "name = " + table.name
	For Local i:Int = 0 Until table.columns.length
		DebugLog i + " : " + table.columns[i].name
	Next
	DebugLog "DDL : ~n" + table.ddl

	db.close()
	
End If

Function errorAndClose(db:TDBConnection)
	DebugLog(db.error().toString())
	db.close()
	End
End Function


