' Copyright (c) 2008-2011 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
'
SuperStrict

Rem
bbdoc: Database Driver - Oracle
about: An Oracle database driver for #bah.database.Database
End Rem
Module BaH.DBOracle

ModuleInfo "Version: 1.00"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2008-2011 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release (ocilib 3.8.1)"


ModuleInfo "CC_OPTS: -fexceptions -DOCI_CHARSET_MIXED"

?macos
ModuleInfo "LD_OPTS: -L%PWD%/lib/macos/"
?win32
ModuleInfo "LD_OPTS: -L%PWD%/lib/win32/"
ModuleInfo "CC_OPTS: -DOCI_API=__cdecl"
?linux
ModuleInfo "LD_OPTS: -L%PWD%/lib/linux/"
?

Import BaH.Database

Import "common.bmx"


Type TDBOracle Extends TDBConnection

	Global envHandle:Byte Ptr

	Field autoCommit:Int = True

	Function Create:TDBConnection(dbname:String = Null, host:String = Null, ..
		port:Int = Null, user:String = Null, password:String = Null, ..
		server:String = Null, options:String = Null)
		
		Local this:TDBOracle = New TDBOracle
		
		this.init(dbname, host, port, user, password, server, options)
		
		If this._dbname Then
			this.open(user, password)
		End If
		
		Return this
		
	End Function

	Method close()
		_isOpen = False
		
		If handle Then
			Try
				bmx_ora_environment_terminateConnection(envHandle, handle)
			Catch err:TOracleSQLException
				setError("Error closing connection", err.message, TDatabaseError.ERROR_CONNECTION, err.errorCode)
			End Try 

			handle = Null
		End If

	End Method

	Method commit:Int()
		autoCommit = True
		
		If Not _isOpen Or Not handle Then
			Return False
		End If
		
		Try
			bmx_ora_connection_commit(handle)
		Catch err:TOracleSQLException
			setError("Error committing transaction", err.message, TDatabaseError.ERROR_TRANSACTION, err.errorCode)
			Return False
		End Try
	
		Return True
	End Method
	
	Method getTables:String[]()
		Local list:String[]

		If Not _isOpen Then
			Return list
		End If

		Local tables:TList = New TList
		
		Local query:TDatabaseQuery = TDatabaseQuery.Create(Self)
		
		Local sql:String = "SELECT table_name FROM user_tables"
			
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
	
	Method getTableInfo:TDBTable(tableName:String, withDDL:Int = False)
		tableName = tableName.ToUpper()

		Local query:TDatabaseQuery = TDatabaseQuery.Create(Self)
		
		Local sql:String = "SELECT column_name, data_type, data_length, data_precision, data_scale, nullable, data_default FROM user_tab_columns " + ..
			" WHERE table_name = '" + tableName + "' ORDER BY column_id"
			
		Local table:TDBTable

		If query.execute(sql) Then
			table = New TDBTable
			table.name = tableName
			
			Local cols:TList = New TList

			For Local rec:TQueryRecord = EachIn query
				Local name:String = rec.GetString(0)
				Local _type:String = rec.GetString(1)
				Local dbType:Int
				Select _type
					Case "NUMBER"
						If rec.GetInt(4) > 0 Then
							dbType = DBTYPE_FLOAT
						Else If rec.GetInt(3) > 10 Or rec.GetInt(3) = 0 Then
							dbType = DBTYPE_LONG
						Else
							dbType = DBTYPE_INT
						End If
					Case "DATE"
						dbType = DBTYPE_DATE
					Case "timestamp", "datetime"
						dbType = DBTYPE_DATETIME
					Case "time"
						dbType = DBTYPE_TIME
					Case "tinyblob", "blob", "mediumblob", "longblob"
						dbType = DBTYPE_BLOB
					Default
						dbType = DBTYPE_STRING
				End Select
				
				Local nullable:Int
				If rec.GetString(5) = "Y" Then
					nullable = True
				End If
				
				Local defaultValue:TDBType = rec.value(6)
				
				cols.AddLast(TDBColumn.Create(name, dbType, nullable, defaultValue))
			Next

			table.SetCountColumns(cols.count())
			Local i:Int
			For Local col:TDBColumn = EachIn cols
				table.SetColumn(i, col)
				i:+ 1
			Next
			
			cols.Clear()
			
		End If

		Return table
	End Method
	
	Method open:Int(user:String = Null, pass:String = Null)
	
		If Not envHandle Then
			Try
				envHandle = bmx_ora_environment_createEnvironment()
			Catch err:TOracleSQLException
				setError("Error creating environment", err.message, TDatabaseError.ERROR_CONNECTION, err.errorCode)
				Return False
			End Try
		End If
	
		If _isOpen Then
			close()
		End If
		
		Local connect:String = "//"
		
		' connection stuff
		' //host:[port][/service name]
		If _host Then
			connect :+ _host + ":"
		End If

		If _port Then
			connect :+ _port
		End If

		If _dbname Then
			connect :+ "/" + _dbname
		End If
		
		Try
			handle = bmx_ora_environment_createConnection(envHandle, user, pass, connect)
		Catch err:TOracleSQLException
			setError("Error connecting to database", err.message, TDatabaseError.ERROR_CONNECTION, err.errorCode)
			Return False
		End Try
		
		
		_isOpen = True
		Return True
	End Method

	Method rollback:Int()
		autoCommit = True

		If Not _isOpen Or Not handle Then
			Return False
		End If
	
		Try
			bmx_ora_connection_rollback(handle)
		Catch err:TOracleSQLException
			setError("Error rolling back transaction", err.message, TDatabaseError.ERROR_TRANSACTION, err.errorCode)
			Return False
		End Try

		Return True
	End Method
	
	Method startTransaction:Int()
		autoCommit = False
		Return True
	End Method

	Method databaseHandle:Byte Ptr()
		Return handle
	End Method
	
	Method createResultSet:TQueryResultSet()
		Return TOracleResultSet.Create(Self)
	End Method
	
	Method nativeErrorMessage:String(err:Int)
	End Method
	
	Method hasPrepareSupport:Int()
		Return True
	End Method

	Method hasTransactionSupport:Int()
		Return True
	End Method

	Method Free()
		Super.Free()
		
		If envHandle Then
			Try
				bmx_ora_environment_terminateEnvironment(envHandle)
			Catch err:TOracleSQLException
				setError("Error terminating environment", err.message, TDatabaseError.ERROR_CONNECTION, err.errorCode)
			End Try 
			envHandle = Null
		End If
		
	End Method
	
End Type


Type TOracleResultSet Extends TQueryResultSet

	Field resultSetHandle:Byte Ptr
	Field _rowsAffected:Int

	Function Create:TQueryResultSet(db:TDBConnection, sql:String = Null)
		Local this:TOracleResultSet = New TOracleResultSet
		
		this.init(db, sql)
		this.rec = TQueryRecord.Create()
		
		Return this
	End Function

	Method executeQuery:Int(statement:String)

		If Not conn.isOpen() Then
			Return False
		End If
		
		_isActive = False
		index = SQL_BeforeFirstRow
		
		rec.clear()

		If stmtHandle Then
		
			If resultSetHandle Then
			
				' close any existing result set
				Try
					bmx_ora_statement_closeResultSet(stmtHandle, resultSetHandle)
				Catch err:TOracleSQLException
					resultSetHandle = Null
					conn.setError("Error closing result set", err.message, TDatabaseError.ERROR_STATEMENT, err.errorCode)
					Return False
				End Try 
			
				resultSetHandle = Null
					
			End If
			
		Else
		
			' create a new statement
			Try
				stmtHandle = bmx_ora_connection_createStatement(conn.handle)
			Catch err:TOracleSQLException
				conn.setError("Error creating statement", err.message, TDatabaseError.ERROR_STATEMENT, err.errorCode)
				Return False
			End Try 
			
		End If
		
		' set autocommit mode, if required
'		Try
'			If TDBOracle(conn).autoCommit <> bmx_ora_statement_getAutoCommit(conn) Then
'				bmx_ora_statement_setAutoCommit(conn, TDBOracle(conn).autoCommit)
'			End If
'		Catch err:TOracleSQLException
'			conn.setError("Error setting commit mode", err.message, TDatabaseError.ERROR_STATEMENT, err.errorCode)
'			Return False
'		End Try 
		
		Local status:Int
		
		' execute the statement
		Try
			status = bmx_ora_statement_execute(stmtHandle, statement)
		Catch err:TOracleSQLException
			conn.setError("Error executing statement", err.message, TDatabaseError.ERROR_STATEMENT, err.errorCode)
			Return False
		End Try
		
		Select status
			Case STATEMENT_RESULT_SET_AVAILABLE
	
				Try
					resultSetHandle = bmx_ora_statement_getResultSet(stmtHandle)
					
				Catch err:TOracleSQLException
					conn.setError("Error getting result set", err.message, TDatabaseError.ERROR_STATEMENT, err.errorCode)
					Return False
				End Try


				Local fieldCount:Int
				
				Try
					fieldCount = bmx_ora_resultset_getColCount(resultSetHandle)
				Catch err:TOracleSQLException
					conn.setError("Error getting column count", err.message, TDatabaseError.ERROR_STATEMENT, err.errorCode)
					Return False
				End Try

				initRecord(fieldCount)

				' we can now populate the fields with information (column name, size, etc)
				If fieldCount Then
				
					Local columnName:String
					Local nameLength:Int
					Local dataType:Int
					Local columnSize:Int
					Local precision:Int
					Local scale:Int
					Local nullable:Int
				
					For Local i:Int = 0 Until fieldCount

						' get the column/field description
						columnName = bmx_ora_resultset_getColInfo(resultSetHandle, i + 1, Varptr dataType, Varptr columnSize, ..
							Varptr precision, Varptr scale, Varptr nullable)

						Local qf:TQueryField = TQueryField.Create(columnName, dbTypeFromNativeORA(Null, dataType, precision, scale))
						If columnSize = 0 Then
							qf.length = -1 ' not specified
						Else
							qf.length = columnSize
						End If
						If precision = 0 Then
							qf.precision = -1 ' not specified
						Else
							qf.precision = precision
						End If
						qf.nullable = nullable
							
						rec.setField(i, qf)
							
					Next
				End If

				_isActive = True

			Case STATEMENT_UPDATE_COUNT_AVAILABLE
				' not active, as there are no rows to process
				_isActive = False
	
				Try			
					_rowsAffected = bmx_ora_statement_getUpdateCount(stmtHandle)
				Catch err:TOracleSQLException
					conn.setError("Error getting update count", err.message, TDatabaseError.ERROR_STATEMENT, err.errorCode)
					Return False
				End Try
				
				If TDBOracle(conn).autoCommit Then
					' auto commit..
					conn.commit()
				End If
				
		End Select

		Return True
	End Method
	
	Method prepare:Int(statement:String)
		_isActive = False
		index = SQL_BeforeFirstRow
		
		rec.clear()
		
		If stmtHandle Then
		
			Try
				bmx_ora_statement_free(stmtHandle)
			Catch err:TOracleSQLException
				conn.setError("Error freeing statement handle", err.message, TDatabaseError.ERROR_STATEMENT, err.errorCode)
				Return False
			End Try 
			
			stmtHandle = Null

		End If
		
		' allocate a new statement handle
		Try
			stmtHandle = bmx_ora_connection_createStatement(conn.handle)
		Catch err:TOracleSQLException
			conn.setError("Error creating statement", err.message, TDatabaseError.ERROR_STATEMENT, err.errorCode)
			Return False
		End Try 
		
		' prepare the query
		Try
			bmx_ora_statement_prepare(stmtHandle, statement)
		Catch err:TOracleSQLException
			conn.setError("Error preparing statement", err.message, TDatabaseError.ERROR_STATEMENT, err.errorCode)
			Return False
		End Try 
		
		Return True
	End Method

	Method execute:Int()

		_isActive = False
		index = SQL_BeforeFirstRow
		
		If Not stmtHandle Then
			Return False
		End If
		
		Local result:Int = 0


		' BIND stuff
		Local values:TDBType[] = boundValues

		Local params:Byte Ptr[]
		Local strings:Short Ptr[]
		Local blobs:Byte Ptr[]
		Local dates:Byte Ptr[]

		If values Then
			Local paramCount:Int = bindCount

			params = New Byte Ptr[paramCount]
			strings = New Short Ptr[paramCount]
			blobs = New Byte Ptr[paramCount]
			dates = New Byte Ptr[paramCount]
			
			Try
				For Local i:Int = 0 Until paramCount
	
					If Not values[i] Or values[i].isNull() Then
						bmx_ora_bind_setnull(stmtHandle, i + 1)
						Continue
					End If
					
					params[i] = String(":" + (i + 1)).toCString()
	
					Select values[i].kind()
						Case DBTYPE_INT
							result = bmx_ora_bind_int(stmtHandle, params[i], Varptr TDBInt(values[i]).value)
						Case DBTYPE_FLOAT
							' convert float to a Double
							If TDBFloat(values[i]) Then
								Local d:TDBDouble = New TDBDouble
								d.setDouble(Double(TDBFloat(values[i]).value))
								values[i].clear()
								values[i] = d
							End If
							result = bmx_ora_bind_double(stmtHandle, params[i], Varptr TDBDouble(values[i]).value)
						Case DBTYPE_DOUBLE
							result = bmx_ora_bind_double(stmtHandle, params[i], Varptr TDBDouble(values[i]).value)
						Case DBTYPE_LONG
							result = bmx_ora_bind_long(stmtHandle, params[i], Varptr TDBLong(values[i]).value)
						Case DBTYPE_STRING
	
							Local s:String = values[i].getString()
							strings[i] = s.toWString()
	
							result = bmx_ora_bind_string(stmtHandle, params[i], strings[i], s.length)
							
						Case DBTYPE_BLOB
						
							blobs[i] = bmx_ora_blob_create(conn.handle)
						
							result = bmx_ora_bind_blob(stmtHandle, params[i], blobs[i], values[i].getBlob(), values[i].size())
							
						Case DBTYPE_DATE

							Local d:TDBDate = TDBDate(values[i])

							dates[i] = bmx_ora_date_create(conn.handle)
						
							result = bmx_ora_bind_date(stmtHandle, params[i], dates[i], d._year, d._month, d._day)

						Case DBTYPE_DATETIME

							Local d:TDBDateTime = TDBDateTime(values[i])

							dates[i] = bmx_ora_date_create(conn.handle)
						
							result = bmx_ora_bind_datetime(stmtHandle, params[i], dates[i], d._year, d._month, d._day, d._hour, d._minute, d._second)

						Case DBTYPE_TIME

							Local d:TDBTime = TDBTime(values[i])

							dates[i] = bmx_ora_date_create(conn.handle)
						
							result = bmx_ora_bind_time(stmtHandle, params[i], dates[i], d._hour, d._minute, d._second)

					End Select
	
					If Not result Then
						conn.setError("Error binding parameters", "", TDatabaseError.ERROR_STATEMENT, -1)

						' free up the memory
						freeMem2(strings)
						freeMem(params)
						For Local i:Int = 0 Until paramCount
							If blobs[i] Then
								bmx_ora_blob_free(blobs[i])
							End If
							If dates[i] Then
								bmx_ora_date_free(dates[i])
							End If
						Next
			
						Return False
					End If
	
				Next

			Catch err:TOracleSQLException
				conn.setError("Error binding parameters", err.message, TDatabaseError.ERROR_STATEMENT, err.errorCode)

				' free up the memory
				freeMem2(strings)
				freeMem(params)
				For Local i:Int = 0 Until paramCount
					If blobs[i] Then
						bmx_ora_blob_free(blobs[i])
					End If
					If dates[i] Then
						bmx_ora_date_free(dates[i])
					End If
				Next

				Return False
			End Try 
			
		End If
		
		' execute the query
		Try
			result = bmx_ora_statement_executeprepared(stmtHandle)

		Catch err:TOracleSQLException
			conn.setError("Error executing statement", err.message, TDatabaseError.ERROR_STATEMENT, err.errorCode)


			If strings Then
				' free up the memory
				freeMem2(strings)
				freeMem(params)
				For Local i:Int = 0 Until blobs.length
					If blobs[i] Then
						bmx_ora_blob_free(blobs[i])
					End If
					If dates[i] Then
						bmx_ora_date_free(dates[i])
					End If
				Next
			End If

			Return False
		End Try

		' free up the memory
		If strings Then
			freeMem2(strings)
			freeMem(params)
			For Local i:Int = 0 Until blobs.length
				If blobs[i] Then
					bmx_ora_blob_free(blobs[i])
				End If
				If dates[i] Then
					bmx_ora_date_free(dates[i])
				End If
			Next
		End If

		Select result
			Case STATEMENT_RESULT_SET_AVAILABLE
	
				Try
					resultSetHandle = bmx_ora_statement_getResultSet(stmtHandle)
					
				Catch err:TOracleSQLException
					conn.setError("Error getting result set", err.message, TDatabaseError.ERROR_STATEMENT, err.errorCode)
					Return False
				End Try


				Local fieldCount:Int
				
				Try
					fieldCount = bmx_ora_resultset_getColCount(resultSetHandle)
				Catch err:TOracleSQLException
					conn.setError("Error getting column count", err.message, TDatabaseError.ERROR_STATEMENT, err.errorCode)
					Return False
				End Try

				initRecord(fieldCount)

				' we can now populate the fields with information (column name, size, etc)
				If fieldCount Then
				
					Local columnName:String
					Local nameLength:Int
					Local dataType:Int
					Local columnSize:Int
					Local precision:Int
					Local scale:Int
					Local nullable:Int
				
					For Local i:Int = 0 Until fieldCount

						' get the column/field description
						columnName = bmx_ora_resultset_getColInfo(resultSetHandle, i + 1, Varptr dataType, Varptr columnSize, ..
							Varptr precision, Varptr scale, Varptr nullable)

						Local qf:TQueryField = TQueryField.Create(columnName, dbTypeFromNativeORA(Null, dataType, precision, scale))
						If columnSize = 0 Then
							qf.length = -1 ' not specified
						Else
							qf.length = columnSize
						End If
						If precision = 0 Then
							qf.precision = -1 ' not specified
						Else
							qf.precision = precision
						End If
						qf.nullable = nullable
							
						rec.setField(i, qf)
							
					Next
				End If

				_isActive = True

			Case STATEMENT_UPDATE_COUNT_AVAILABLE
				' not active, as there are no rows to process
				_isActive = False
	
				Try			
					_rowsAffected = bmx_ora_statement_getUpdateCount(stmtHandle)
				Catch err:TOracleSQLException
					conn.setError("Error getting update count", err.message, TDatabaseError.ERROR_STATEMENT, err.errorCode)
					Return False
				End Try
				
				If TDBOracle(conn).autoCommit Then
					' auto commit..
					conn.commit()
				End If
				
		End Select


		Return True
	End Method
	
	Method freeMem(data:Byte Ptr[])
		For Local i:Int = 0 Until data.length
			If data[i] Then
				MemFree(data[i])
			End If
		Next
	End Method

	Method freeMem2(data:Short Ptr[])
		For Local i:Int = 0 Until data.length
			If data[i] Then
				MemFree(data[i])
			End If
		Next
	End Method
	
	Method firstRow:Int()
		If index = SQL_BeforeFirstRow Then
			Return nextRow()
		End If
		
		Return False
	End Method

	Method nextRow:Int()
	
		If Not resultSetHandle Then
			Return False
		End If
		
		Local status:Int
		
		Try
			status = bmx_ora_resultset_next(resultSetHandle)
			
		Catch err:TOracleSQLException
			conn.setError("Error fetching row", err.message, TDatabaseError.ERROR_STATEMENT, err.errorCode)
			Return False
		End Try

		' end of fetch
		If Not status Then
			Return False
		End If
		
		For Local i:Int = 0 Until rec.count()
		
			If values[i] Then
				values[i].clear()
			End If

			Try

				If bmx_ora_resultset_isNull(resultSetHandle, i + 1) Then
					Continue
				End If

				Select rec.fields[i].fType
					Case DBTYPE_INT
					
						Local value:Int = bmx_ora_resultset_getInt(resultSetHandle, i + 1)
						
						values[i] = New TDBInt
						values[i].setInt(value)

					Case DBTYPE_LONG
					
						Local value:Long
						bmx_ora_resultset_getLong(resultSetHandle, i + 1, Varptr value)
						
						values[i] = New TDBLong
						values[i].setLong(value)

					Case DBTYPE_FLOAT
					
						Local value:Float = Float(bmx_ora_resultset_getDouble(resultSetHandle, i + 1))
						
						values[i] = New TDBFloat
						values[i].setFloat(value)

					Case DBTYPE_DOUBLE
					
						Local value:Double = bmx_ora_resultset_getDouble(resultSetHandle, i + 1)
						
						values[i] = New TDBDouble
						values[i].setDouble(value)

					Case DBTYPE_BLOB
					
						Local value:Byte Ptr = bmx_ora_resultset_getBlob(resultSetHandle, i + 1)
						
						Local blob:TDBBlob = New TDBBlob
						
						blob._owner = True
						blob._isNull = False
						blob._size = bmx_ora_blob_length(value)
						blob.value = MemAlloc(blob._size)
						
						bmx_ora_blob_getdata(value, blob.value)

						values[i] = blob

					Case DBTYPE_DATETIME
					
						Local d:Int, m:Int, y:Int, hh:Int, mm:Int, ss:Int
					
						bmx_ora_resultset_getDatetime(resultSetHandle, i + 1, Varptr y, Varptr m, Varptr d, Varptr hh, Varptr mm, Varptr ss)
						
						values[i] = TDBDateTime.Set(y, m, d, hh, mm, ss)

					Case DBTYPE_STRING
					
						Local value:String = bmx_ora_resultset_getString(resultSetHandle, i + 1)
						
						values[i] = New TDBString
						values[i].setString(value)
				End Select
	
			Catch err:TOracleSQLException
				conn.setError("Error fetching row data", err.message, TDatabaseError.ERROR_STATEMENT, err.errorCode)
				Return False
			End Try

		Next
	
		
		index:+ 1
		
		Return True		
	End Method
	
	Method lastInsertedId:Long()
	End Method

	Method rowsAffected:Int()
		Return _rowsAffected
	End Method

	Function dbTypeFromNative:Int(name:String, _type:Int = 0, _flags:Int = 0)
	End Function

	Function dbTypeFromNativeORA:Int(name:String, _type:Int = 0, precision:Int = 0, scale:Int = 0)
		Local dbType:Int
		
		Select _type
			Case OCI_CDT_NUMERIC
				If scale > 0 Then
					dbType = DBTYPE_DOUBLE
				Else
					If precision <= 10 Then ' the number precision... > 10 is a long
						dbType = DBTYPE_INT
					Else
						dbType = DBTYPE_LONG
					End If
				End If
			'Case OCCIIBFLOAT
			'	dbType = DBTYPE_FLOAT
			Case OCI_CDT_DATETIME
				dbType = DBTYPE_DATETIME
			Case OCI_CDT_LOB
				dbType = DBTYPE_BLOB
			Default
				dbType = DBTYPE_STRING
		End Select
		
		Return dbType
	End Function

	Method initRecord(size:Int)

		rec.clear()

		If size > 0 Then		
			rec.init(size)
		End If
		
		resetValues(size)
	End Method

	Method cleanup()

		If stmtHandle Then
			bmx_ora_statement_free(stmtHandle)
			stmtHandle = Null
		End If

	End Method
	
End Type


Type TOracleSQLException Extends TRuntimeException

	Field errorCode:Int
	Field message:String
	
	Function _create:TOracleSQLException(errorCode:Int, message:String)
		Local this:TOracleSQLException = New TOracleSQLException
		this.errorCode = errorCode
		this.message = message
		Return this
	End Function

End Type




Type TOracleDatabaseLoader Extends TDatabaseLoader

	Method New()
		_type = "ORACLE"
	End Method

	Method LoadDatabase:TDBConnection( dbname:String = Null, host:String = Null, ..
		port:Int = Null, user:String = Null, password:String = Null, ..
		server:String = Null, options:String = Null )
	
		Return TDBOracle.Create(dbName, host, port, user, password, server, options)
		
	End Method

End Type

AddDatabaseLoader New TOracleDatabaseLoader
