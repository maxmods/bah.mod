SuperStrict

Framework BaH.DBOracle
Import brl.filesystem

'putenv_("ORACLE_HOME=/Users/brucey/Documents/programming/c/ohome")

Local db:TDBConnection = LoadDatabase("ORACLE", "xe", "192.168.2.31", Null, "brucey", "brucey")

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
		DebugLog i + " : " + table.columns[i].name + " : " + table.columns[i].dbType
	Next
	DebugLog "DDL : ~n" + table.ddl

	db.close()
	
End If

Function errorAndClose(db:TDBConnection)
	DebugLog(db.error().toString())
	db.close()
	End
End Function



