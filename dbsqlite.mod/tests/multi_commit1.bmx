SuperStrict

Framework bah.DBSQLite

Local db:TDBConnection = LoadDatabase("SQLITE", "test.db") 

If db.isOpen() Then
  db.executeQuery("Create TABLE tbl (id integer primary key AUTOINCREMENT, name varchar(30))") 
	
  For Local i:Int = 1 To 20
    Repeat
      db.executeQuery("INSERT INTO tbl values (NULL, 'Name " + i + "')") 
      If db.hasError() Then
        DebugLog db.error().ToString()
        Delay(2000) 
      EndIf
    Until Not db.hasError() 
    Delay(2000) 
  Next
  db.Close() 
  DebugLog "Done"
End If
