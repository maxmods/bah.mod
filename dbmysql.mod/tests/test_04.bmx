SuperStrict

Framework BaH.DBMySQL
Import BRL.Random

Type TPersonStuff
	Field forename:String
	Field surname:String
	Field dataInt:Int
	Field dataFloat:Float
	Field dataDouble:Double
	Field dataLong:Long
End Type

Local db:TDBConnection = LoadDatabase("MYSQL", "maxtest", Null, 0, "brucey", "brucey")

If Not db Then
	DebugLog("Didn't work...")
	End
End If

If db.hasError() Then
	errorAndClose(db)
End If

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
	

If db.isOpen() Then

	db.executeQuery("DROP TABLE if exists person")
'DebugStop
	' Create a new table
	Local s:String = "CREATE TABLE if not exists person (id integer primary key AUTO_INCREMENT, " + ..
	  " forename varchar(30), surname varchar(30), dataint integer, datafloat float, datadouble double, datalong bigint )"

	db.executeQuery(s)

	If db.hasError() Then
		errorAndClose(db)
	End If

	' get a new query object 
	Local query:TDatabaseQuery = TDatabaseQuery.Create(db)

	' prepare the insert statement
	' by preparing it once, the database can reuse it on succesive inserts which is more efficient.
	query.prepare("INSERT INTO person values (NULL, ?, ?, ?, ?, ?, ?)")

	If db.hasError() Then
		errorAndClose(db)
	End If

	' iterate around the array inserting new entries
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

	' prepare select
	query.prepare("SELECT * FROM person WHERE surname LIKE ?")

	If db.hasError() Then
		errorAndClose(db)
	End If

	query.bindValue(0, TDBString.Set("%n%"))
'DebugStop
	query.execute()

	If db.hasError() Then
		errorAndClose(db)
	End If

	While query.nextRow()
		Local record:TQueryRecord = query.rowRecord()
		
		' auto_increment starts at 1...
		Local i:Int = record.value(0).getInt() - 1
		
		' compare what went in, to what went out.
		DebugLog(" IN  - " + pstuff[i].forename + " : " + pstuff[i].surname + " : " + pstuff[i].dataInt + ..
			" : " + pstuff[i].dataFloat + " : " + pstuff[i].dataDouble + " : " + pstuff[i].dataLong)
		
		DebugLog(" OUT - " + record.getString(1) + " : " + record.getString(2) + ..
			" : " + record.getInt(3) + " : " + record.getFloat(4) + ..
			" : " + record.getDouble(5) + " : " + record.getLong(6) )
	Wend
			
	db.close()

End If


Function errorAndClose(db:TDBConnection)
	DebugLog(db.error().toString())
	db.close()
	End
End Function




