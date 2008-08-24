SuperStrict

Framework BaH.DBPostgreSQL
Import BRL.Random

Type TPersonStuff
	Field forename:String
	Field surname:String
	Field dataInt:Int
	Field dataFloat:Float
	Field dataDouble:Double
	Field dataLong:Long
End Type


Local db:TDBConnection = LoadDatabase("POSTGRESQL", "maxtest", "192.168.2.31", 0, "brucey", "brucey")

If Not db Then
	DebugLog("Didn't work...")
	End
End If

If db.hasError() Then
	errorAndClose(db)
End If

If db.isOpen() Then

	' Load up some data for insertion...

	Local names:String[][] = [ ..
		[ "Alfred", "Aho" ],   ..
		[ "Brian", "Kernighan" ], ..
		[ "Peter", "Weinberger" ] ]
		
	Local pstuff:TPersonStuff[] = New TPersonStuff[names.length]
	For Local i:Int = 0 Until names.length
		pstuff[i] = New TPersonStuff
		pstuff[i].forename = names[i][0]
		pstuff[i].surname = names[i][1]
		pstuff[i].dataInt = Rnd(1, 10)
		pstuff[i].dataFloat = Rnd(1, 10)
		pstuff[i].dataDouble = Rnd(1, 10)
		pstuff[i].dataLong = Rnd(1, 100000000000:Long)
	Next


	db.executeQuery("DROP TABLE person")
	db.executeQuery("DROP SEQUENCE person_id")
	
	' create the auto-incrementing field
	db.executeQuery("CREATE SEQUENCE person_id INCREMENT 1 START 1")


	' Create a new table
	Local s:String = "CREATE TABLE person (id integer primary key DEFAULT NEXTVAL('person_id'), " + ..
	  " forename varchar(30), surname varchar(30), dataint integer, datafloat float, datadouble float8, datalong bigint )"

	db.executeQuery(s)

	If db.hasError() Then
		errorAndClose(db)
	End If

	' get a new query object 
	Local query:TDatabaseQuery = TDatabaseQuery.Create(db)

	' prepare the insert statement
	' by preparing it once, the database can reuse it on succesive inserts which is more efficient.
	query.prepare("INSERT INTO person (forename, surname, dataint, datafloat, datadouble, datalong)" + ..
		" values ($1, $2, $3, $4, $5, $6)")
	
	If db.hasError() Then
		errorAndClose(db)
	End If

	' iterate round the array inserting new entries
	For Local i:Int = 0 Until names.length
		query.bindValue(0, TDBString.Set(pstuff[i].forename))
		query.bindValue(1, TDBString.Set(pstuff[i].surname))
		query.bindValue(2, TDBInt.Set(pstuff[i].dataInt))
		query.bindValue(3, TDBFloat.Set(pstuff[i].dataFloat))
		query.bindValue(4, TDBDouble.Set(pstuff[i].datadouble))
		query.bindValue(5, TDBLong.Set(pstuff[i].dataLong))

		query.execute()
		
		If db.hasError() Then
			errorAndClose(db)
		End If
	Next

	' select
	query = db.executeQuery("SELECT * FROM person")
	If db.hasError() Then
		errorAndClose(db)
	End If


	For Local record:TQueryRecord = EachIn query
		
		Local i:Int = record.value(0).getInt() - 1
		DebugLog(" IN  - " + pstuff[i].forename + " : " + pstuff[i].surname + " : " + pstuff[i].dataInt + ..
			" : " + pstuff[i].dataFloat + " : " + pstuff[i].dataDouble + " : " + pstuff[i].dataLong)
		
		DebugLog(" OUT - " + record.value(1).getString() + " : " + record.value(2).getString() + ..
			" : " + record.value(3).getInt() + " : " + record.value(4).getFloat() + ..
			" : " + record.value(5).getDouble() + " : " + record.value(6).getLong() )

	Next
			
	db.close()
	
End If

Function errorAndClose(db:TDBConnection)
	DebugLog(db.error().toString())
	db.close()
	End
End Function



