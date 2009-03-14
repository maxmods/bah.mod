SuperStrict

Framework BaH.DBPostgreSQL
Import BRL.filesystem
Import BRL.RamStream
Import BRL.JPGLoader
Import BRL.StandardIO

Local db:TDBConnection = LoadDatabase("POSTGRESQL", "maxtest", "192.168.2.31", 0, "brucey", "brucey")

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
	db.executeQuery("DROP SEQUENCE person_id")
	
	' create the auto-incrementing field
	db.executeQuery("CREATE SEQUENCE person_id INCREMENT 1 START 1")
	
	Local s:String = "CREATE TABLE person (id integer primary key DEFAULT NEXTVAL('person_id'), " + ..
	  " forename varchar(30)," + ..
	  " surname varchar(30), image bytea )"

	db.executeQuery(s)

	If db.hasError() Then
		errorAndClose(db)
	End If

	' get a new query object 
	Local query:TDatabaseQuery = TDatabaseQuery.Create(db)

	' prepare the insert statement
	' by preparing it once, the database can reuse it on succesive inserts which is more efficient.
	query.prepare("INSERT INTO person (forename, surname, image) values ($1, $2, $3)")
	
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

