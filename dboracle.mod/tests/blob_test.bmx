SuperStrict

Framework BaH.DBOracle
Import BRL.filesystem
Import BRL.RamStream
Import BRL.JPGLoader
Import BRL.StandardIO

'putenv_("ORACLE_HOME=/Users/brucey/Documents/programming/c/ohome")

'DebugStop
Local db:TDBConnection = LoadDatabase("ORACLE", "xe", "192.168.2.31", Null, "brucey", "brucey")

If Not db Then
	Print "Didn't work..."
	End
End If

If db.hasError() Then
	errorAndClose(db)
End If


Local names:String[][] = [ ..
	[ "Alfred", "Aho", "alfred-aho.jpg" ],   ..
	[ "Brian", "Kernighan", "brian-kernighan.jpg" ], ..
	[ "Peter", "Weinberger", "pjw.jpg" ] ]

If db.isOpen() Then

	db.executeQuery("DROP TABLE person")
	db.executeQuery("DROP TRIGGER person_trigger")
	db.executeQuery("DROP SEQUENCE person_seq")

	Local s:String = "CREATE TABLE person (id integer primary key, " + ..
	  " forename varchar(30)," + ..
	  " surname varchar(30), image blob )"

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
	query.prepare("INSERT INTO person values (0, :1, :2, :3)")
	
	If db.hasError() Then
		errorAndClose(db)
	End If

	' iterate round the array inserting new entries
	For Local i:Int = 0 Until 3
		query.bindValue(0, TDBString.Set(names[i][0]))
		query.bindValue(1, TDBString.Set(names[i][1]))

		' load some data (raw jpg data)		
		Local b:Byte[] = LoadByteArray("images/" + names[i][2])
		' bind it (copying the data)
		query.bindValue(2, TDBBlob.Set(b, b.length))

		query.execute()
		
		If db.hasError() Then
			errorAndClose(db)
		End If
	Next
		
	query = db.executeQuery("SELECT * from person")
	If db.hasError() Then
		errorAndClose(db)
	End If

	While query.nextRow()
		Local record:TQueryRecord = query.rowRecord()
		
		Print (record.GetInt(0) + ". Name = " + record.GetString(1) + " " + record.GetString(2))
		
		' get the blob data
		Local b:TDBBlob = TDBBlob(record.value(3))
		' create a pixmap (from the raw jpg data)
		Local pix:TPixmap = LoadPixmap(CreateRamStream(b.getBlob(), b.size(), True, False))
		If pix Then
			Print "Dimension = " + pix.width + ", " + pix.height
		End If
	Wend
	
			
	db.close()
	
End If

Function errorAndClose(db:TDBConnection)
	Print db.error().toString()
	db.close()
	End
End Function

