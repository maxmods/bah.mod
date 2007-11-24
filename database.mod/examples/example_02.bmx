' Using a prepared statement

SuperStrict

Framework BaH.DBSQLite
Import BRL.filesystem
Import BRL.StandardIO

' delete the db file if it already exists (just to tidy up the examples!)
DeleteFile("maxtest.db")

' load the database driver, creating and opening the database
Local db:TDBConnection = LoadDatabase("SQLITE", "maxtest.db")

If Not db Then
	Print "No valid driver!"
	End
End If

' check for errors
If db.hasError() Then
	errorAndClose(db)
End If


' if the connection is open, do something with it...
If db.isOpen() Then

	Local names:String[][] = [ ..
		[ "Alfred", "Aho" ],   ..
		[ "Brian", "Kernighan" ], ..
		[ "Peter", "Weinberger" ] ]


	' Create a new table
	Local s:String = "CREATE TABLE person (id integer primary key AUTOINCREMENT, " + ..
	  " forename varchar(30)," + ..
	  " surname varchar(30) )"

	' execute the SQL
	db.executeQuery(s)

	If db.hasError() Then
		errorAndClose(db)
	End If

	' get a new query object 
	Local query:TDatabaseQuery = TDatabaseQuery.Create(db)

	' prepare the insert statement
	' by preparing it once, the database can reuse it on succesive inserts which is more efficient.
	query.prepare("INSERT INTO person values (NULL, ?, ?)")
	
	If db.hasError() Then
		errorAndClose(db)
	End If

	' iterate overy the array inserting new entries
	For Local i:Int = 0 Until names.length
		query.bindValue(0, TDBString.Set(names[i][0]))
		query.bindValue(1, TDBString.Set(names[i][1]))

		' execute the prepared statement with the bound values
		query.execute()
		
		If db.hasError() Then
			errorAndClose(db)
		End If
	Next
	
	' retrieve data from the database
	query = db.executeQuery("SELECT * from person")
	
	If db.hasError() Then
		errorAndClose(db)
	End If

	' iterate over the retrieved rows
	While query.nextRow()
		Local record:TQueryRecord = query.rowRecord()
		
		Print "Name = " + record.value(1).getString() + " " + record.value(2).getString()
	Wend
	
	' close the connection!
	db.close()
	
End If

Function errorAndClose(db:TDBConnection)
	Print db.error().toString()
	db.close()
	End
End Function

