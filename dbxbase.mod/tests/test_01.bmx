SuperStrict

Framework BaH.DBXbase
Import BRL.filesystem

Local db:TDBConnection = LoadDatabase("XBASE", ".")

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

	db.executeQuery("DROP TABLE person")
	
	Local s:String = "CREATE TABLE person (id int(10), " + ..
	  " forename char(30)," + ..
	  " surname char(30) )"

	db.executeQuery(s)

	If db.hasError() Then
		errorAndClose(db)
	End If

	For Local i:Int = 0 Until names.length
		db.executeQuery("INSERT INTO person values (" + (i + 1) + ", '" + names[i][0] + "', '" + names[i][1] + "')")
		If db.hasError() Then
			errorAndClose(db)
		End If
	Next

	Local query:TDatabaseQuery = db.executeQuery("SELECT * from person")
	If db.hasError() Then
		errorAndClose(db)
	End If

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
