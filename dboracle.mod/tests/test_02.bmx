SuperStrict

Framework BaH.DBOracle
Import BRL.filesystem
Import PUB.Stdc

'putenv_("ORACLE_HOME=/Users/brucey/Documents/programming/c/ohome")

Local db:TDBConnection = LoadDatabase("ORACLE", "xe", "192.168.2.31", Null, "brucey", "brucey")

If Not db Then
	DebugLog("Didn't work...")
	End
End If

Local names:String[][] = [ ..
	[ "Alfred", "Aho" ],   ..
	[ "Brian", "Kernighan" ], ..
	[ "Peter", "Weinberger" ] ]

If db.isOpen() Then

	db.executeQuery("DROP TABLE person")
	db.executeQuery("DROP TRIGGER person_trigger")
	db.executeQuery("DROP SEQUENCE person_seq")

	' Create a new table
	Local s:String = "CREATE TABLE person (id integer primary key, " + ..
	  " forename varchar(30)," + ..
	  " surname varchar(30) )"

	db.executeQuery(s)

	If db.hasError() Then
		errorAndClose(db)
	End If
	
	' set up the auto-incrementing id field
	db.executeQuery("create sequence person_seq start with 1 increment by 1 nomaxvalue")

	If db.hasError() Then
		errorAndClose(db)
	End If

	db.executeQuery("create trigger person_trigger before insert on person For each row begin Select person_seq.nextval into :New.id from dual; End;")
	
	If db.hasError() Then
		errorAndClose(db)
	End If
	

	' get a new query object 
	Local query:TDatabaseQuery = TDatabaseQuery.Create(db)

	' prepare the insert statement
	' by preparing it once, the database can reuse it on succesive inserts which is more efficient.
	query.prepare("INSERT INTO person values (0, :1, :2)")
	
	If db.hasError() Then
		errorAndClose(db)
	End If

	' iterate round the array inserting new entries
	For Local i:Int = 0 Until names.length
		query.bindValue(0, TDBString.Set(names[i][0]))
		query.bindValue(1, TDBString.Set(names[i][1]))

		query.execute()
		
		If db.hasError() Then
			errorAndClose(db)
		End If
	Next
	
	' select
	query = db.executeQuery("SELECT * from person")
	If db.hasError() Then
		errorAndClose(db)
	End If

	While query.nextRow()
		Local record:TQueryRecord = query.rowRecord()
		
		DebugLog(record.getInt(0) + ": Name = " + record.getString(1) + " " + record.getString(2))
	Wend
	
			
	db.close()
	
End If

Function errorAndClose(db:TDBConnection)
	DebugLog(db.error().toString())
	db.close()
	End
End Function


