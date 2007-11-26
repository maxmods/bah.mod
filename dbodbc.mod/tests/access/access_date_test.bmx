SuperStrict

Framework BaH.DBODBC
Import BRL.Random
Import BRL.FileSystem

Type TPersonStuff
	Field forename:String
	Field surname:String
	Field dataInt:Int
	Field dataFloat:Float
	Field dataDouble:Double
	Field dataLong:Long
	Field dataDate:String
End Type


'                                             database                                   dsn name - seems to need full path!
Local db:TDBConnection = LoadDatabase("ODBC", "maxtest", Null, Null, "brucey", "brucey", CurrentDir() + "/maxtest.dsn")

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
	pstuff[i].dataLong = Rnd(1, 1000000000:Long) ' this isn't really a long long... access can't do more than 4 bytes!
	pstuff[i].dataDate = "200" + i + "-04-03 10:03:04"
Next
	

If db.isOpen() Then

	Local t:String[]= db.getTables() 
	DebugLog t.Length
	For Local i:Int = 0 To t.Length - 1
		DebugLog(t[i]) 
	Next

	' we don't care if the drop table fails, since it might not exist yet...
	db.executeQuery("DROP TABLE person")

	' Create a new table
	Local s:String = "CREATE TABLE person (id counter constraint pk primary key, " + ..
	  " forename text(30), surname text(30), dataint integer, datafloat float, datadouble double, datalong long, datadate date )"

	db.executeQuery(s)

	If db.hasError() Then
		errorAndClose(db)
	End If

	' we need to wait for access to catch up... (or table 'person' does not exist)
	Delay 100


	For Local i:Int = 0 Until names.length
		Local sql:String = "INSERT INTO person (forename, surname, dataint, datafloat, datadouble, datalong, datadate) " + ..
			" values ('" + pstuff[i].forename + "', '" + ..
			pstuff[i].surname + "', " + pstuff[i].dataInt + ", " + ..
			pstuff[i].dataFloat + ", " + pstuff[i].datadouble + ", " + ..
			pstuff[i].dataLong + ", '" + pstuff[i].dataDate + "')"

		db.executeQuery(sql)
			
		If db.hasError() Then
			errorAndClose(db)
		End If
	Next
	
	If db.hasError() Then
		errorAndClose(db)
	End If

	' prepare select
	Local query:TDatabaseQuery = db.executeQuery("SELECT * FROM person WHERE surname LIKE '%n%'")
	
	If db.hasError() Then
		errorAndClose(db)
	End If

	While query.nextRow()
		Local record:TQueryRecord = query.rowRecord()
		
		' auto_increment starts at 1...
		Local i:Int = record.value(0).getInt() - 1
		
		' compare what went in, to what went out.
		DebugLog(" IN  - " + pstuff[i].forename + " : " + pstuff[i].surname + " : " + pstuff[i].dataInt + ..
			" : " + pstuff[i].dataFloat + " : " + pstuff[i].dataDouble + " : " + pstuff[i].dataLong + " : " + pstuff[i].dataDate) 
		
		DebugLog(" OUT - " + record.value(1).getString() + " : " + record.value(2).getString() + ..
			" : " + record.value(3).getInt() + " : " + record.value(4).getFloat() + ..
			" : " + record.value(5).getDouble() + " : " + record.value(6).getLong() + " : " + record.value(7).getString()) 
	Wend


	db.close()
End If



Function errorAndClose(db:TDBConnection)
	DebugLog(db.error().toString())
	db.close()
	End
End Function
