SuperStrict

Framework BaH.DBPostgreSQL
Import BRL.StandardIO

Local db:TDBConnection = LoadDatabase("POSTGRESQL", "maxtest", "192.168.2.31", 0, "brucey", "brucey")

If db.hasError() Then
	errorAndClose(db)
End If


Local names:String[][] = [ ..
	[ "Alfred", "Aho" ],   ..
	[ "Brian", "Kernighan" ], ..
	[ "Peter", "Weinberger" ] ]

If db.isOpen() Then

	db.executeQuery("DROP TABLE person")
	db.executeQuery("DROP SEQUENCE person_id")
	
	' create the auto-incrementing field
	db.executeQuery("CREATE SEQUENCE person_id INCREMENT 1 START 1")
	
	' id field assigned to sequence
	Local s:String = "CREATE TABLE person (id integer primary key DEFAULT NEXTVAL('person_id'), " + ..
	  " forename varchar(30)," + ..
	  " surname varchar(30), bday date, thetime time, exact timestamp )"

	db.executeQuery(s)

	If db.hasError() Then
		errorAndClose(db)
	End If

	For Local i:Int = 0 Until names.length
		db.executeQuery("INSERT INTO person (forename, surname, bday, thetime, exact) values ('" + names[i][0] + "', '" + ..
			names[i][1] + "', '200" + i + "-01-01', '04:01:45', '2007-03-04 13:20:13')")
		If db.hasError() Then
			errorAndClose(db)
		End If
	Next

	Local query:TDatabaseQuery = TDatabaseQuery.Create(db)
	
	' prepare select
	query.prepare("SELECT * FROM person WHERE bday = $1 And thetime > $2 And exact < $3")
	
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
			TDBDate(record.value(3)).getString() + " - " + ..
			TDBTime(record.value(4)).getString() + " - " + ..
			TDBDateTime(record.value(5)).getString())
	Wend
	
	
	db.close()
	
End If

Function errorAndClose(db:TDBConnection)
	DebugLog(db.error().toString())
	db.close()
	End
End Function



