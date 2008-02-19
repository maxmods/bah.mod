' Copyright (c) 2007, Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the auther nor the names of its contributors may be used to 
'       endorse or promote products derived from this software without specific
'       prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Rem
bbdoc: Database Driver - SQLite
about: An SQLite database driver for #bah.database.Database
End Rem
Module BaH.DBSQLite

ModuleInfo "Version: 1.10"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: Wrapper - 2007 Bruce A Henderson"
ModuleInfo "Copyright: SQLite - The original author of SQLite has dedicated the code to the public domain. Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original SQLite code, either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means."
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.10"
ModuleInfo "History: Update to SQLite 3.5.6."
ModuleInfo "History: Fixed lack of error reporting during query execution."
ModuleInfo "History: Transaction queries are finalized more quickly."
ModuleInfo "History: Statement should generally be reset before acquiring error message."
ModuleInfo "History: 1.09"
ModuleInfo "History: Update to SQLite 3.5.2. Now using the Amalgamated version."
ModuleInfo "History: Implementation of Date, DateTime and Time types."
ModuleInfo "History: 1.08"
ModuleInfo "History: Fixed null column types not being handled."
ModuleInfo "History: 1.07"
ModuleInfo "History: Fixed problem with lastInsertedId() not returning.. the last inserted id."
ModuleInfo "History: 1.06"
ModuleInfo "History: Update to SQLite 3.4.2."
ModuleInfo "History: 1.05"
ModuleInfo "History: Fixed database Close to cleanup non-finalized queries."
ModuleInfo "History: 1.04"
ModuleInfo "History: Improved error message details."
ModuleInfo "History: 1.03"
ModuleInfo "History: Fixed NextRow returning True on empty queries."
ModuleInfo "History: 1.02"
ModuleInfo "History: Fixed issue with mis-count of bound parameters."
ModuleInfo "History: 1.01"
ModuleInfo "History: Added hasPrepareSupport() and hasTransactionSupport() methods."
ModuleInfo "History: 1.00 Initial Release"
ModuleInfo "History: Includes SQLite 3.3.13 source."


Import BaH.Database

Import "common.bmx"


' The implementation

Type TDBSQLite Extends TDBConnection

	Field queries:TList = New TList

	Function Create:TDBConnection(dbname:String = Null, host:String = Null, ..
		port:Int = Null, user:String = Null, password:String = Null, ..
		server:String = Null, options:String = Null)
		
		Local this:TDBSQLite = New TDBSQLite
		
		this.init(dbname, host, port, user, password, server, options)
		
		If this._dbname Then
			this.open(user, password)
		End If
		
		Return this
		
	End Function

	Method close()
	
		clearQueries()
	
		If _isOpen Then
		
			If sqlite3_close(handle) <> SQLITE_OK Then
				setError("Error closing database", Null, TDatabaseError.ERROR_CONNECTION)
			End If
			
			handle = Null
			_isOpen = False
			
		End If
		
	End Method
	
	Method commit:Int()
	
		If Not _isOpen Then
			Return False
		End If
		
		Local query:TDatabaseQuery = executeQuery("COMMIT TRANSACTION")

		If hasError() Then
			setError("Error committing transaction", error().error, TDatabaseError.ERROR_TRANSACTION)
			
			Return False
		End If
		
		query.Free()

		Return True
	End Method

	Method getTables:String[]()
		Local list:String[]

		If Not _isOpen Then
			Return list
		End If

		Local tables:TList = New TList
		
		Local query:TDatabaseQuery = TDatabaseQuery.Create(Self)
		
		Local sql:String = "SELECT name FROM sqlite_master WHERE type = 'table' " + ..
			"UNION ALL SELECT name FROM sqlite_temp_master WHERE type = 'table'"
			
		If query.execute(sql) Then
			While query.nextRow()
				tables.addLast(query.value(0).getString())
			Wend
		End If

		If tables.count() > 0 Then
			list = New String[tables.count()]
			Local i:Int = 0
			For Local s:String = EachIn tables
				list[i] = s
				i:+ 1
			Next
		End If
		
		Return list
	End Method

	Method open:Int(user:String = Null, pass:String = Null)
	
		' close if the connection is already open
		If _isOpen Then
			close()
		End If
		
		If sqlite3_open(convertISO8859toUTF8(_dbname), Varptr handle) = SQLITE_OK Then
			_isOpen = True

			Return True
		Else
			
		End If
		
		Return False
		
	End Method

	Method rollback:Int()
		If Not _isOpen Then
			Return False
		End If
		
		Local query:TDatabaseQuery = executeQuery("ROLLBACK TRANSACTION")
		
		If hasError() Then
		
			setError("Error rolling back transaction", error().error, TDatabaseError.ERROR_TRANSACTION)
			
			Return False
		End If
		
		query.Free()
		
		Return True
	End Method

	Method startTransaction:Int()
	
		If Not _isOpen Then
			Return False
		End If
		
		Local query:TDatabaseQuery = executeQuery("BEGIN TRANSACTION")

		If hasError() Then
			
			setError("Error starting transaction", error().error, TDatabaseError.ERROR_TRANSACTION)
			
			Return False
		End If
		
		query.Free()
		
		Return True

	End Method

	Method databaseHandle:Byte Ptr()
		Return handle
	End Method
	
	Method createResultSet:TQueryResultSet()
		Return TSQLiteResultSet.Create(Self)
	End Method

	Method nativeErrorMessage:String(err:Int)
		Select err
			Case 0 Return "Successful result"
			Case 1 Return "SQL error Or missing database"
			Case 2 Return "An internal logic error in SQLite"
			Case 3 Return "Access permission denied"
			Case 4 Return "Callback routine requested an abort"
			Case 5 Return "The database file is locked"
			Case 6 Return "A table in the database is locked"
			Case 7 Return "A malloc() failed"
			Case 8 Return "Attempt To write a readonly database"
			Case 9 Return "Operation terminated by sqlite_interrupt()"
			Case 10 Return "Some kind of disk I/O error occurred"
			Case 11 Return "The database disk image is malformed"
			Case 12 Return "(Internal Only) Table Or record Not found"
			Case 13 Return "Insertion failed because database is full"
			Case 14 Return "Unable To open the database file"
			Case 15 Return "Database lock protocol error"
			Case 16 Return "(Internal Only) Database table is empty"
			Case 17 Return "The database schema changed"
			Case 18 Return "Too much data For one row of a table"
			Case 19 Return "Abort due To constraint violation"
			Case 20 Return "Data Type mismatch"
			Case 21 Return "Library used incorrectly"
			Case 22 Return "Uses OS features Not supported on host"
			Case 23 Return "Authorization denied"
			Case 100 Return "sqlite_step() has another row ready"
			Case 101 Return "sqlite_step() has finished executing"
			Default Return ""
		End Select
	End Method

	Method hasPrepareSupport:Int()
		Return True
	End Method

	Method hasTransactionSupport:Int()
		Return True
	End Method

	Method clearQueries()
		For Local q:TSQLiteResultSet = EachIn queries
			q.free()
		Next
		queries.clear()
	End Method
	
	Method addQuery(query:TSQLiteResultSet)
		If Not queries.Contains(query) Then
			queries.addLast(query)
		End If
	End Method
	
	Method removeQuery(query:TSQLiteResultSet)
		queries.remove(query)
	End Method
	
End Type


Type TSQLiteResultSet Extends TQueryResultSet

	Field initialFetch:Int = True
	Field fakeFirstRowFetch:Int
	
	Method free()
		TDBSQLite(conn).removeQuery(Self)
	
		If stmtHandle Then
			sqlite3_finalize(stmtHandle)
			stmtHandle = Null
		End If	
	End Method
	
	Method Delete()
		free()
	End Method

	Function Create:TQueryResultSet(db:TDBConnection, sql:String = Null)
		Local this:TSQLiteResultSet = New TSQLiteResultSet
		
		this.init(db, sql)
		this.rec = TQueryRecord.Create()
		TDBSQLite(this.conn).addQuery(this)
		
		Return this
	End Function

	Method executeQuery:Int(statement:String)
		If Not prepare(statement) Then
			Return False
		End If
		
		Return execute()
	End Method
	
	Method cleanup()
		index = SQL_beforeFirstRow
		initialFetch = True
		
		clear()
		free()
		
	End Method

	Method prepare:Int(stmt:String)
		If Not conn  Or Not conn.isOpen() Then
			Return False
		End If
		
		cleanup()
		TDBSQLite(conn).addQuery(Self)

		' set the query if not set already
		If Not query Then
			query = stmt
		End If
		
		Local q:String = convertISO8859toUTF8(query)
		
		Local result:Int = sqlite3_prepare_v2(TDBSQLite(conn).handle, q, q.length , Varptr stmtHandle, 0)
		If result <> SQLITE_OK Then
			conn.setError("Error preparing statement", convertUTF8toISO8859(sqlite3_errmsg(TDBSQLite(conn).handle)), TDatabaseError.ERROR_STATEMENT, result)
		
			free()
			Return False
		End If
		
		Return True
		
	End Method
	
	Method execute:Int()
	
		fakeFirstRowFetch = False
				
		Local result:Int = sqlite3_reset(stmtHandle)
		If result <> SQLITE_OK Then
			conn.setError("Error resetting statement", convertUTF8toISO8859(sqlite3_errmsg(TDBSQLite(conn).handle)), TDatabaseError.ERROR_STATEMENT, result)

			free()
			Return False
		End If
		
		' BIND stuff
		Local values:TDBType[] = boundValues

		Local paramCount:Int = sqlite3_bind_parameter_count(stmtHandle)
		
		If paramCount = bindCount Then
		
			For Local i:Int = 0 Until paramCount
			
				' reset error-state flag
				result = SQLITE_OK
			
				If Not values[i] Or values[i].isNull() Then
					result = sqlite3_bind_null(stmtHandle, i + 1)
				Else
					Select values[i].kind()
						Case DBTYPE_INT
							result = sqlite3_bind_int(stmtHandle, i + 1, values[i].getInt())
						Case DBTYPE_FLOAT
							result = sqlite3_bind_double(stmtHandle, i + 1, values[i].getFloat())
						Case DBTYPE_DOUBLE
							result = sqlite3_bind_double(stmtHandle, i + 1, values[i].getDouble())
						Case DBTYPE_LONG
							result = sqlite3_bind_int64(stmtHandle, i + 1, values[i].getLong())
						Case DBTYPE_STRING
							Local s:String = convertISO8859toUTF8(values[i].getString())
							result = sqlite3_bind_text(stmtHandle, i + 1, s, s.length, -1)
						Case DBTYPE_BLOB
							result = sqlite3_bind_blob(stmtHandle, i + 1, values[i].getBlob(), values[i].size(), 0)
						Case DBTYPE_DATE, DBTYPE_DATETIME, DBTYPE_TIME
							Local s:String = convertISO8859toUTF8(values[i].getString())
							result = sqlite3_bind_text(stmtHandle, i + 1, s, s.length, -1)
					End Select
					
				End If
			
				If result <> SQLITE_OK Then
					sqlite3_reset(stmtHandle)
					conn.setError("Failed to bind parameter (" + i + ")", convertUTF8toISO8859(sqlite3_errmsg(TDBSQLite(conn).handle)), TDatabaseError.ERROR_STATEMENT, result)
					
					free()
					Return False
				End If
			Next
		
		Else
		
			conn.setError("Parameter count mismatch", Null, TDatabaseError.ERROR_STATEMENT, 0)
			
			free()
			Return False		
		End If
		
		' we need to pre-fetch the first row to get the field information
		nextRow()
		
		If conn.error().isSet() Then
			_isActive = False
			Return False
		End If
		
		_isActive = True
		
		Return True
	End Method
	
	Method firstRow:Int()
		If index = SQL_BeforeFirstRow Then
			Return nextRow()
		End If
		
		Return False
	End Method
	
	Method nextRow:Int()

		If fakeFirstRowFetch Then
			fakeFirstRowFetch = False
			Return True
		End If
		
		If initialFetch Then
			fakeFirstRowFetch = True
			initialFetch = False
		End If
		
		Local result:Int = sqlite3_step(stmtHandle)
		
		Select result
			Case SQLITE_ROW
				If rec.isEmpty() Then
					initRecord()
					resetValues(rec.count())
				End If
				
				
				For Local i:Int = 0 Until rec.count()
				
					If values[i] Then
						values[i].clear()
					End If
				
					Select sqlite3_column_type(stmtHandle, i)
						Case SQLITE_INTEGER
							values[i] = New TDBLong
							Local lvalue:Long
							bmx_sqlite3_column_int64(stmtHandle, i, Varptr lvalue)
							values[i].setLong(lvalue)
						Case SQLITE_FLOAT
							values[i] = New TDBDouble
							values[i].setDouble(sqlite3_column_double(stmtHandle, i))
						Case SQLITE_NULL
						Case SQLITE_BLOB
							values[i] = New TDBBlob
							values[i].setBlob(sqlite3_column_blob(stmtHandle, i), sqlite3_column_bytes(stmtHandle, i))
						Default
							values[i] = New TDBString
							values[i].setString(sizedUTF8toISO8859(sqlite3_column_text(stmtHandle, i), sqlite3_column_bytes(stmtHandle, i)))
					End Select
				Next

				index:+ 1
				
				Return True
			Case SQLITE_DONE
				If rec.isEmpty() Then
					initRecord()
					resetValues(rec.count())
					rec.SetIsEmptySet()
				End If
				
				' prevent NextRow() returning True on the first fetch - since there are no rows.
				fakeFirstRowFetch = False
				
				sqlite3_reset(stmtHandle)
				
				Return False
			Default

				sqlite3_reset(stmtHandle)

				' raise an error!
				conn.setError(convertUTF8toISO8859(sqlite3_errmsg(TDBSQLite(conn).handle)), Null, TDatabaseError.ERROR_STATEMENT, result)
				Return False
		End Select
		
		Return False
	End Method

	Method initRecord()
		rec.clear()
		
		Local colCount:Int = sqlite3_column_count(stmtHandle)
		If colCount <= 0 Then
			Return
		End If
		
		rec.init(colCount)
		
		For Local i:Int = 0 Until colCount
		
			Local columnName:String = convertUTF8toISO8859(sqlite3_column_name(stmtHandle, i))
			Local tn:Byte Ptr = sqlite3_column_decltype(stmtHandle, i)
			Local typeName:String
			If tn Then
				typeName = convertUTF8toISO8859(tn)
			End If
			
			Local dotPosition:Int = columnName.findLast(".") + 1
			
			rec.setField(i, TQueryField.Create(columnName[dotPosition..], dbTypeFromNative(typeName)))
		Next
		
		
	End Method
	
	Method dbTypeFromNative:Int(name:String, _type:Int = 0, _flags:Int = 0)
		
		name = name.ToLower()
		
		If name.startsWith("numeric") Then
			Return DBTYPE_DOUBLE
		End If
		
		Select name
			Case "integer"
				Return DBTYPE_LONG
			Case "int"
				Return DBTYPE_LONG
			Case "double"
				Return DBTYPE_DOUBLE
			Case "float"
				Return DBTYPE_DOUBLE
			Case "blob"
				Return DBTYPE_BLOB
			Default
				Return DBTYPE_STRING
		End Select
		
	End Method

	Method lastInsertedId:Long()
		If isActive() Then
			Local id:Long
			bmx_sqlite3_last_insert_rowid(conn.handle, Varptr id)
			Return id
		End If
	End Method
	
	Method rowsAffected:Int()
		If isActive() Then
			Return sqlite3_changes(conn.handle)
		End If
		Return -1
	End Method

End Type



Type TSQLiteDatabaseLoader Extends TDatabaseLoader

	Method New()
		_type = "SQLITE"
	End Method

	Method LoadDatabase:TDBConnection( dbname:String = Null, host:String = Null, ..
		port:Int = Null, user:String = Null, password:String = Null, ..
		server:String = Null, options:String = Null )
	
		Return TDBSQLite.Create(dbName, host, port, user, password, server, options)
		
	End Method

End Type

AddDatabaseLoader New TSQLiteDatabaseLoader
