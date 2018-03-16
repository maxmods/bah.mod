SuperStrict

Framework bah.DBSQLite
Import brl.timer
Import brl.standardio

Local db:TDBConnection = LoadDatabase("SQLITE", "test.db") 

If db.isOpen() Then
  Local timer:Int = MilliSecs() 
  Repeat
	db.executeQuery("UPDATE tbl SET name = 'Updated name' WHERE id = 1") 
	If db.hasError() Then 
	  Print db.error().toString() 
    End If
	Delay(20) 
  Until MilliSecs() - timer > 40000
  
  db.Close() 
End If
