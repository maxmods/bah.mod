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

	If db.hasError() Then
		errorAndClose(db)
	End If

	' Create a new table
	Local s:String = "CREATE TABLE person (id int(10) unique index, " + ..
	  " forename char(30)," + ..
	  " surname char(30) )"

	db.executeQuery(s)

	If db.hasError() Then
		errorAndClose(db)
	End If

	' get a new query object 
	Local query:TDatabaseQuery = TDatabaseQuery.Create(db)

	For Local i:Int = 0 Until names.length
		db.executeQuery("INSERT INTO person values (" + (i + 1) + ", '" + names[i][0] + "', '" + names[i][1] + "')")
		If db.hasError() Then
			errorAndClose(db)
		End If
	Next
	
	If db.hasError() Then
		errorAndClose(db)
	End If

	' prepare select
	query.prepare("SELECT * FROM person WHERE surname = ?")
	
	If db.hasError() Then
		errorAndClose(db)
	End If

	query.bindValue(0, TDBString.Set("Kernighan"))

	query.execute()

	If db.hasError() Then
		errorAndClose(db)
	End If

	While query.nextRow()
		Local record:TQueryRecord = query.rowRecord()
		
		DebugLog("Name = " + record.value(1).getString() + " " + record.value(2).getString())
	Wend
	
			
	db.close()
	
End If

Function errorAndClose(db:TDBConnection)
	DebugLog(db.error().toString())
	db.close()
	End
End Function


