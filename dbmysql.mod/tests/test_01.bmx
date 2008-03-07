SuperStrict

Framework BaH.DBMySQL
Import BRL.filesystem

Local db:TDBConnection = LoadDatabase("MYSQL", "maxtest", Null, 0, "brucey", "brucey")

If Not db Then
	DebugLog("Didn't work...")
	End
End If

If db.hasError() Then
	errorAndClose(db)
End If


Local names:String[][] = [ ..
	[ "Alfred", "Aho" ],   ..
	[ "Brian", "Kernighan" ], ..
	[ "Peter", "Weinberger" ] ]

If db.isOpen() Then

	db.executeQuery("DROP TABLE if exists person")
	
	Local s:String = "CREATE TABLE if not exists person (id integer primary key AUTO_INCREMENT, " + ..
	  " forename varchar(30)," + ..
	  " surname varchar(30) )"

	db.executeQuery(s)

	If db.hasError() Then
		errorAndClose(db)
	End If
'DebugStop
	For Local i:Int = 0 Until names.length
		Local query:TDatabaseQuery = db.executeQuery("INSERT INTO person values (NULL, '" + names[i][0] + "', '" + names[i][1] + "')")

		DebugLog "LastInserted id = " + query.lastInsertedId()
		
		If db.hasError() Then
			errorAndClose(db)
		End If
	Next
'DebugStop		
	Local query:TDatabaseQuery = db.executeQuery("SELECT * from person")
	If db.hasError() Then
		errorAndClose(db)
	End If
'DebugStop
	While query.nextRow()
		Local record:TQueryRecord = query.rowRecord()
		
		DebugLog(TDBInt(record.value(0)).value + ". Name = " + TDBString(record.value(1)).value + " " + TDBString(record.value(2)).value)
	Wend
	
			
	db.close()
	
End If

Function errorAndClose(db:TDBConnection)
	DebugLog(db.error().toString())
	db.close()
	End
End Function
