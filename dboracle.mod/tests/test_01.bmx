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

Local names:String[][] = [ ..
	[ "Alfred", "Aho" ],   ..
	[ "Brian", "Kernighan" ], ..
	[ "Peter", "Weinberger" ] ]

If db.isOpen() Then

	db.executeQuery("DROP TABLE person")

	Local s:String = "CREATE TABLE person (id integer primary key, " + ..
	  " forename varchar(30)," + ..
	  " surname varchar(30) )"

	db.executeQuery(s)

	If db.hasError() Then
		errorAndClose(db)
	End If
	
	' transaction test :-)
	'db.StartTransaction()

	If db.hasError() Then
		errorAndClose(db)
	End If

	For Local i:Int = 0 Until names.length
		db.executeQuery("INSERT INTO person values (" + i + ", '" + names[i][0] + "', '" + names[i][1] + "')")
		If db.hasError() Then
			errorAndClose(db)
		End If
		
	Next
	
	' commit our changes :-)
	'db.Commit()

	If db.hasError() Then
		errorAndClose(db)
	End If

	Local query:TDatabaseQuery = db.executeQuery("SELECT * from person")
	If db.hasError() Then
		errorAndClose(db)
	End If

	While query.nextRow()
		Local record:TQueryRecord = query.rowRecord()
		
		DebugLog("Name = " + record.GetString(1) + " " + record.GetString(2))
	Wend
			
	db.close()
	
End If

Function errorAndClose(db:TDBConnection)
	DebugLog(db.error().toString())
	db.close()
	End
End Function

