SuperStrict

Framework BaH.DBSQLite
Import BRL.filesystem


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
			names[i][1] + "', '200" + i + "-01-01', '04:01:45', '2007-03-04 13:20:13')")
		If db.hasError() Then
			errorAndClose(db)
		End If
	Next

	Local query:TDatabaseQuery = TDatabaseQuery.Create(db)
	
	' prepare select
	query.prepare("SELECT * FROM person WHERE bday = ? And thetime > ? And exact < ?")
	
	If db.hasError() Then
		errorAndClose(db)
	End If

	query.bindValue(0, TDBDate.Set(2001, 1, 1))
	query.bindValue(1, TDBTime.Set(4, 0 ,0))
	query.bindValue(2, TDBDateTime.Set(2008, 1, 1, 10, 0, 0))

	query.execute()

	If db.hasError() Then
		errorAndClose(db)
	End If


	While query.nextRow()
		Local record:TQueryRecord = query.rowRecord()
		
		DebugLog("Name = " + record.getString(1) + " " + record.getString(2) + " - " + ..
			TDBDate.SetFromString(record.getString(3)).getString() + " - " + ..
			TDBTime.SetFromString(record.getString(4)).getString() + " - " + ..
			TDBDateTime.SetFromString(record.getString(5)).getString())
	Wend
	
	
	db.close()
	
End If

Function errorAndClose(db:TDBConnection)
	DebugLog(db.error().toString())
	db.close()
	End
End Function


