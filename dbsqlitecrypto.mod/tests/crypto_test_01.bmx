SuperStrict

Framework BaH.DBSQLiteCrypto
Import BRL.filesystem
Import BRL.StandardIO

DeleteFile("crypotest.db")

Local db:TDBConnection = LoadDatabase("SQLITE_CRYPTO", "crypotest.db")

If Not db Then
	Print "Problem initialising database environment."
	End
End If

If db.hasError() Then
	errorAndClose(db)
End If

If db.isOpen() Then
	

	' encrypt database
	' cast to SQLite to access SQLite-only functionality
	Local sqliteDB:TDBSQLite = TDBSQLite(db)

	' NOTE : the key will be applied when the database is modified in some way.
	sqliteDB.setkey("BlitzMax Rocks!")

	If db.hasError() Then
		errorAndClose(db)
	End If


	' create a table
	Local s:String = "CREATE TABLE person (id integer primary key AUTOINCREMENT, " + ..
	  " forename varchar(30)," + ..
	  " surname varchar(30) )"

	db.executeQuery(s)

	
	' insert some data
	Local names:String[][] = [ ..
	[ "Alfred", "Aho" ],   ..
	[ "Brian", "Kernighan" ], ..
	[ "Peter", "Weinberger" ] ]

	For Local i:Int = 0 Until names.length
		db.executeQuery("INSERT INTO person values (NULL, '" + names[i][0] + "', '" + names[i][1] + "')")
		If db.hasError() Then
			errorAndClose(db)
		End If
	Next

	' close
	db.close()
	
	' open and attempt to use without authenticating...
	db = LoadDatabase("SQLITE_CRYPTO", "crypotest.db")
	
	Local query:TDatabaseQuery = db.executeQuery("SELECT * from person")

	' we should get an error
	If db.hasError() Then
		Print "EXPECTED : " + db.error().toString()
	End If
	
	db.close()
	
	' connect with wrong password...
	db = LoadDatabase("SQLITE_CRYPTO", "crypotest.db", , , , "password")

	query = db.executeQuery("SELECT * from person")
	
	If db.hasError() Then
		Print "EXPECTED : " + db.error().toString()
	End If
	
	db.close()

	' connect with correct password...
	db = LoadDatabase("SQLITE_CRYPTO", "crypotest.db", , , , "BlitzMax Rocks!")

	If db.hasError() Then
		errorAndClose(db)
	End If

	Print "Trying again!..."
	
	query = db.executeQuery("SELECT * from person")

	' we should get an error
	If db.hasError() Then
		errorAndClose(db)
	End If

	While query.nextRow()
		Local record:TQueryRecord = query.rowRecord()
		
		Print "Name = " + TDBString(record.value(1)).value + " " + TDBString(record.value(2)).value
	Wend
		
End If

Function errorAndClose(db:TDBConnection)
	Print db.error().toString()
	db.close()
	End
End Function



