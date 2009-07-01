SuperStrict

Framework BaH.DBOracle
Import BRL.filesystem
Import PUB.Stdc

'putenv_("ORACLE_HOME=/Users/brucey/Documents/programming/c/ohome")

Local db:TDBConnection = LoadDatabase("ORACLE", "xe", "192.168.2.31", Null, "brucey", "brucey")

If db.hasError() Then
	errorAndClose(db)
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
	  " surname varchar(30), bday date, exact date, thetime date)"

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


	For Local i:Int = 0 Until names.length
		db.executeQuery("INSERT INTO person values (0, '" + names[i][0] + "', '" + ..
			names[i][1] + "', to_date('200" + i + "-01-01', 'yyyy-mm-dd'), to_date('2007-03-04 13:20:13', 'yyyy-mm-dd hh24:mi:ss'), to_date('04:01:45', 'hh24:mi:ss'))")
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
		
		DebugLog("Name = " + record.getString(1) + " " + record.getString(2) + " - " + ..
			record.getString(3) + " - " + record.getString(4) + " - " + record.getString(5))
	Wend
	
	
	db.close()
	
End If

Function errorAndClose(db:TDBConnection)
	DebugLog(db.error().toString())
	db.close()
	End
End Function


