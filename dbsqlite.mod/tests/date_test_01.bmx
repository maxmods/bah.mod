SuperStrict

Framework BaH.DBSQLite
Import BRL.filesystem
Import brl.standardio

DeleteFile("maxtest.db")

Local db:TDBConnection = LoadDatabase("SQLITE", "maxtest.db")

If db.hasError() Then
	errorAndClose(db)
End If


Local names:String[][] = [ ..
	[ "Alfred", "Aho" ],   ..
	[ "Brian", "Kernighan" ], ..
	[ "Peter", "Weinberger" ] ]

If db.isOpen() Then

	Local s:String = "CREATE TABLE person (id integer primary key AUTOINCREMENT, " + ..
	  " forename varchar(30)," + ..
	  " surname varchar(30), bday date, thetime time, exact datetime )"

	db.executeQuery(s)

	If db.hasError() Then
		errorAndClose(db)
	End If

	For Local i:Int = 0 Until names.length
		db.executeQuery("INSERT INTO person values (NULL, '" + names[i][0] + "', '" + ..
			names[i][1] + "', '200" + i + "-01-01', '2007-03-04 13:20:13', '04:01:45')")
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
		
		Print("Name = " + record.getString(1) + " " + record.getString(2) + " - " + ..
			record.getString(3) + " - " + record.getString(4) + " - " + record.getString(5))
	Wend
	
	
	db.close()
	
End If

Function errorAndClose(db:TDBConnection)
	Print(db.error().toString())
	db.close()
	End
End Function

