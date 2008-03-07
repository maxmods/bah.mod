SuperStrict

Rem
bbdoc: Database Driver - MySQL
about: A MySQL database driver for #bah.database.Database
End Rem
Module BaH.DBMySQL

ModuleInfo "Version: 1.05"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: Wrapper - 2007 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.05"
ModuleInfo "History: Fixed lastInsertId() issue."
ModuleInfo "History: Win32 now uses local static lib. No copying required!"
ModuleInfo "History: 1.04"
ModuleInfo "History: Improved object cleanup."
ModuleInfo "History: 1.03"
ModuleInfo "History: Fixed issue with mis-count of bound parameters."
ModuleInfo "History: 1.02"
ModuleInfo "History: Added TEXT field-type support."
ModuleInfo "History: Added hasPrepareSupport() and hasTransactionSupport() methods."
ModuleInfo "History: 1.01"
ModuleInfo "History: Windows build change. Requires copying of libmysql.a to lib folder!"
ModuleInfo "History: 1.00 Initial Release (MySQL 5)"

?win32
ModuleInfo "LD_OPTS: -L%PWD%/lib/win32"
?

Import BaH.Database

Import "common.bmx"


' The implementation

Type TDBMySQL Extends TDBConnection

	Field clientVersion:Int
	Field serverVersion:Int

	Function Create:TDBConnection(dbname:String = Null, host:String = Null, ..
		port:Int = Null, user:String = Null, password:String = Null, server:String = Null, options:String = Null)
		
		Local this:TDBMySQL = New TDBMySQL
		
		this.init(dbname, host, port, user, password, server, options)
		
		this.clientVersion = mysql_get_client_version()
		
		If this._dbname Then
			this.open(user, password)
		End If
		
		Return this
		
	End Function

	Method close()
		If _isOpen Then

			free() ' tidy up queries and stuff
			
			mysql_close(handle)
			handle = Null
			_isOpen = False
		End If
	End Method
	
	Method commit:Int()
		If Not _isOpen Then
			Return False
		End If
		
		If mysql_query(handle, "COMMIT") Then
			setError("Error committing transaction", Null, TDatabaseError.ERROR_TRANSACTION, mysql_errno(handle))
			Return False
		End If
		
		Return True
	End Method
	
	Method getTables:String[]()
		Local list:String[]
		
		If Not _isOpen Then
			Return list
		End If
		
		Local tableList:TList = New TList
		Local tablesHandle:Byte Ptr = mysql_list_tables(handle, Null)
		
		If tablesHandle Then
			Local row:Byte Ptr = mysql_fetch_row(tablesHandle)

			While row
				Local s:String = convertUTF8toISO8859(bmx_mysql_rowField_chars(row, 0))
				tableList.addLast(s)
			
				row = mysql_fetch_row(tablesHandle)
			Wend
			
			mysql_free_result(tablesHandle)
		End If
		
		If tableList.count() > 0 Then
			list = New String[tableList.count()]
			Local i:Int = 0
			For Local s:String = EachIn tableList
				list[i] = s
				i:+ 1
			Next
		End If
		
		Return list
	End Method
	
	Method open:Int(user:String = Null, pass:String = Null)
		If _isOpen Then
			close()
		End If
		
		If user Then
			_user = user
		End If
		
		If pass Then
			_password = pass
		End If
		
		' initialize the handle
		handle = mysql_init(0)
		
		If handle Then
		
			Local ret:Int = 0
			
			If _host Then
				If _user Then
					If _password
						ret = mysql_real_connect(handle, _host, _user, _password, _dbname, _port, Null, 0)
					Else
						ret = mysql_real_connect(handle, _host, _user, Null, _dbname, _port, Null, 0)
					End If
				Else
					ret = mysql_real_connect(handle, _host, Null, Null, _dbname, _port, Null, 0)
				End If
			Else
				If _user Then
					If _password
						ret = mysql_real_connect(handle, Null, _user, _password, _dbname, _port, Null, 0)
					Else
						ret = mysql_real_connect(handle, Null, _user, Null, _dbname, _port, Null, 0)
					End If
				Else
					ret = mysql_real_connect(handle, Null, Null, Null, _dbname, _port, Null, 0)
				End If
			End If
			
			If Not ret Then
				setError("Error connecting to database", convertUTF8toISO8859(mysql_error(handle)), TDatabaseError.ERROR_CONNECTION, mysql_errno(handle))
				Return False
			End If
			
			If mysql_select_db(handle, _dbname) Then
				setError("Error opening database '" + _dbname + "'", convertUTF8toISO8859(mysql_error(handle)), TDatabaseError.ERROR_CONNECTION, mysql_errno(handle))
				Return False
			End If
		Else
			setError("Error initializing database", Null, TDatabaseError.ERROR_CONNECTION, 0)
			Return False
		End If
		
		If clientVersion >= 50007 Then
			mysql_set_character_set(handle, "utf8")
		End If
		
		serverVersion = mysql_get_server_version(handle)
		
		_isOpen = True
		Return True
	End Method
	
	Method rollback:Int()
		If Not _isOpen Then
			Return False
		End If
		
		If mysql_query(handle, "ROLLBACK") Then
			setError("Error rolling back transaction", Null, TDatabaseError.ERROR_TRANSACTION, mysql_errno(handle))
			Return False
		End If
		
		Return True
	End Method
	
	Method startTransaction:Int()
		If Not _isOpen Then
			Return False
		End If
		
		If mysql_query(handle, "BEGIN WORK") Then
			setError("Error starting transaction", Null, TDatabaseError.ERROR_TRANSACTION, mysql_errno(handle))
			Return False
		End If
		
		Return True
	End Method
	
	Method databaseHandle:Byte Ptr()
		Return handle
	End Method
	
	Method createResultSet:TQueryResultSet()
		Return TMySQLResultSet.Create(Self)
	End Method

	Method nativeErrorMessage:String(err:Int)
	End Method
	
	Method hasPrepareSupport:Int()
		Return True
	End Method

	Method hasTransactionSupport:Int()
		Return True
	End Method

End Type

Type TMySQLField

	Field mySQLField:Byte Ptr
	Field dataValue:Byte Ptr
	Field dataLength:Int
	Field isNull:Int
	Field flag:Int

	Method clear()
		mySQLField = Null
		If dataValue Then
			MemFree(dataValue)
			dataValue = Null
		End If
		dataLength = 0
		isNull = 0
	End Method
	
	Method Delete()
		clear()
	End Method
	
End Type

Type TMySQLResultSet Extends TQueryResultSet

	' a pointer to a mysql result
	Field resultHandle:Byte Ptr
	Field row:Byte Ptr
	
	' a pointer to a mysql prepared statement
	Field stmtHandle:Byte Ptr
	Field metaHandle:Byte Ptr
	
	Field preparedQuery:Int
	Field _rowsAffected:Int

	Field parameterBindings:Byte Ptr
	Field selectBindings:Byte Ptr
	
	Field mySQLFields:TMySQLField[]

	Function Create:TQueryResultSet(db:TDBConnection, sql:String = Null)
		Local this:TMySQLResultSet = New TMySQLResultSet
		
		this.init(db, sql)
		this.rec = TQueryRecord.Create()
		
		Return this
	End Function

	Method executeQuery:Int(statement:String)
	
		If Not conn.isOpen() Then
			Return False
		End If
		
		preparedQuery = False
		
		Local q:String = convertISO8859toUTF8(statement)
		
		If mysql_real_query(conn.handle, q, q.length) Then
			conn.setError("Error executing query", convertUTF8toISO8859(mysql_error(conn.handle)), TDatabaseError.ERROR_STATEMENT, mysql_errno(conn.handle))
			Return False
		End If
		
		resultHandle = mysql_store_result(conn.handle)
		
		If Not resultHandle And mysql_field_count(conn.handle) > 0 Then
			conn.setError("Error storing result set", convertUTF8toISO8859(mysql_error(conn.handle)), TDatabaseError.ERROR_STATEMENT, mysql_errno(conn.handle))
			Return False
		End If
		
		Local fieldCount:Int = mysql_field_count(conn.handle)

		initRecord(fieldCount)

		_rowsAffected = mysql_affected_rows(conn.handle)
		
		If fieldCount <> 0 Then
			
			For Local i:Int = 0 Until fieldCount
				Local _field:Byte Ptr = mysql_fetch_field_direct(resultHandle, i)
				
				Local qf:TQueryField = TQueryField.Create(convertUTF8toISO8859(bmx_mysql_field_name(_field)), dbTypeFromNative(Null, bmx_mysql_field_type(_field), bmx_mysql_field_flags(_field)))
				qf.length = bmx_mysql_field_length(_field)
				qf.precision = bmx_mysql_field_decimals(_field)
				
				rec.setField(i, qf)
				
			Next
		End If
		
		_isActive = True
		Return True
	End Method
	
	Method prepare:Int(statement:String)
	
		cleanup()
		
		If Not statement Or statement.length = 0 Then
			Return False
		End If
		
		' initialize the statement if required
		If Not stmtHandle Then
			stmtHandle = mysql_stmt_init(conn.handle)
		End If
		
		If Not stmtHandle Then
			conn.setError("Error preparing statement", convertUTF8toISO8859(mysql_error(conn.handle)), TDatabaseError.ERROR_STATEMENT, mysql_errno(conn.handle))
			Return False
		End If
		
		' prepare the statement
		Local q:String = convertISO8859toUTF8(statement)
		Local result:Int = mysql_stmt_prepare(stmtHandle, q, q.length)
		
		If result Then
			conn.setError("Error preparing statement", convertUTF8toISO8859(mysql_stmt_error(stmtHandle)), TDatabaseError.ERROR_STATEMENT, mysql_errno(stmtHandle))
			cleanup()
			Return False
		End If
		
		' if the param count > 0 there are "?" in the SQL that need to be bound
		If mysql_stmt_param_count(stmtHandle) > 0 Then
			parameterBindings = bmx_mysql_makeBindings(mysql_stmt_param_count(stmtHandle))
		End If
		
		' **********************************
		' setup bindings for inbound data...
		If Not metaHandle Then
			metaHandle = mysql_stmt_result_metadata(stmtHandle)
		End If
		
		If metaHandle Then

			Local fieldCount:Int = mysql_num_fields(metaHandle)
			initRecord(fieldCount)

			mySQLFields = New TMySQLField[fieldCount]
		
			selectBindings = bmx_mysql_makeBindings(fieldCount)
			
			For Local i:Int = 0 Until fieldCount
			
				Local _field:Byte Ptr = mysql_fetch_field(metaHandle)

				mySQLFields[i] = New TMySQLField
				
				mySQLFields[i].mySQLField = _field
				mySQLFields[i].dataLength = bmx_mysql_field_length(_field) + 1
				' make some space for the data...
				mySQLFields[i].dataValue = MemAlloc(mySQLFields[i].dataLength)
				
				' build result set field information
				Local qf:TQueryField = TQueryField.Create(convertUTF8toISO8859(bmx_mysql_field_name(_field)), dbTypeFromNative(Null, bmx_mysql_field_type(_field), bmx_mysql_field_flags(_field)))
				qf.length = bmx_mysql_field_length(_field)
				qf.precision = bmx_mysql_field_decimals(_field)
				rec.setField(i, qf)

				bmx_mysql_inbind(selectBindings, i, _field, mySQLFields[i].dataValue, Varptr mySQLFields[i].dataLength, Varptr mySQLFields[i].isNull)
			Next

		End If
		
		Return True
	End Method
	
	Method execute:Int()

		If Not preparedQuery Then
			Return False
		End If
		
		If Not stmtHandle Then
			Return False
		End If
		
		index = SQL_BeforeFirstRow
		
		Local result:Int = 0
		
		result = mysql_stmt_reset(stmtHandle)
		If result Then
			conn.setError("Error resetting statement", convertUTF8toISO8859(mysql_stmt_error(stmtHandle)), TDatabaseError.ERROR_STATEMENT, mysql_errno(stmtHandle))
			Return False
		End If

		' BIND stuff
		Local values:TDBType[] = boundValues

		Local paramCount:Int = mysql_stmt_param_count(stmtHandle)

		If paramCount = bindCount Then

			Local strings:Byte Ptr[] = New Byte Ptr[paramCount]
		
			For Local i:Int = 0 Until paramCount

				Local isNull:Int = False
				
				If Not values[i] Or values[i].isNull() Then
					isNull = True
				Else
					Select values[i].kind()
						Case DBTYPE_INT
							bmx_mysql_bind_int(parameterBindings, i, Varptr TDBInt(values[i]).value)
						Case DBTYPE_FLOAT
							bmx_mysql_bind_float(parameterBindings, i, Varptr TDBFloat(values[i]).value)
						Case DBTYPE_DOUBLE
							bmx_mysql_bind_double(parameterBindings, i, Varptr TDBDouble(values[i]).value)
						Case DBTYPE_LONG
							bmx_mysql_bind_long(parameterBindings, i, Varptr TDBLong(values[i]).value)
						Case DBTYPE_STRING
							Local s:String = convertISO8859toUTF8(values[i].getString())
							strings[i] = s.toCString()
							bmx_mysql_bind_string(parameterBindings, i, strings[i], s.length)
							
						Case DBTYPE_BLOB
	
							'result = sqlite3_bind_blob(stmtHandle, i + 1, values[i].getBlob(), values[i].size(), 0)
						Case DBTYPE_DATE
							'result = sqlite3_bind_int(stmtHandle, i + 1, values[i].getDate())
					End Select
				End If

			Next

			' actually bind the parameters
			result = bmx_mysql_stmt_bind_param(stmtHandle, parameterBindings)
			
			' free up the strings
			For Local i:Int = 0 Until paramCount
				If strings[i] Then
					MemFree(strings[i])
				End If
			Next
			
			If result Then
				conn.setError("Error binding values", convertUTF8toISO8859(mysql_stmt_error(stmtHandle)), TDatabaseError.ERROR_STATEMENT, mysql_errno(stmtHandle))
			
				Return False
			End If
			
		End If
	
		' execute the statement
		result = mysql_stmt_execute(stmtHandle)
		
		If result Then
			conn.setError("Error executing statement", convertUTF8toISO8859(mysql_stmt_error(stmtHandle)), TDatabaseError.ERROR_STATEMENT, mysql_errno(stmtHandle))
			Return False
		End If
		
		_rowsAffected = mysql_stmt_affected_rows(stmtHandle)

		' if this is set, then there is data returned from the statement execution
		' in which case we need to bind the results for the result set
		If metaHandle Then

			result = bmx_mysql_stmt_bind_result(stmtHandle, selectBindings)

			If result Then
				conn.setError("Error binding result", convertUTF8toISO8859(mysql_stmt_error(stmtHandle)), TDatabaseError.ERROR_STATEMENT, mysql_errno(stmtHandle))
				Return False
			End If

			result = mysql_stmt_store_result(stmtHandle)
			
			If result Then
				conn.setError("Error storing result", convertUTF8toISO8859(mysql_stmt_error(stmtHandle)), TDatabaseError.ERROR_STATEMENT, mysql_errno(stmtHandle))
				Return False
			End If
		
		End If


		_isActive = True
		Return True
	End Method

	Method initRecord(size:Int)

		rec.clear()

		If size > 0 Then		
			rec.init(size)
		End If
		
		resetValues(size)
	End Method
	
	Method firstRow:Int()
		If index = SQL_BeforeFirstRow Then
			Return nextRow()
		End If
		
		Return False
	End Method
	
	Method nextRow:Int()
	
		If preparedQuery Then
			If Not stmtHandle Then
				Return False
			End If
			
			If bmx_mysql_stmt_fetch(stmtHandle) Then
				Return False
			End If
		Else
			row = mysql_fetch_row(resultHandle)
			If Not row Then
				Return False
			End If
		End If
		
		' now populate the values[] array with the fetched data !
		For Local i:Int = 0 Until rec.count()
		
			If values[i] Then
				values[i].clear()
			End If

			
			If preparedQuery Then
			
				If Not mySQLFields[i].isNull Then
				
					Local fieldLength:Int = mySQLFields[i].dataLength
				
					' it seems that we need to retrieve all values as if they were "strings"...
					' Don't ask... it doesn't work otherwise. (except on Windows... haw)
					Select rec.fields[i].fType
						Case DBTYPE_INT
							values[i] = New TDBInt
							values[i].setInt(String.fromBytes(mySQLFields[i].dataValue, fieldLength).toInt())
						Case DBTYPE_LONG
							values[i] = New TDBLong
							values[i].setLong(String.fromBytes(mySQLFields[i].dataValue, fieldLength).toLong())
						Case DBTYPE_FLOAT
							values[i] = New TDBFloat
							values[i].setFloat(String.fromBytes(mySQLFields[i].dataValue, fieldLength).toFloat())
						Case DBTYPE_DOUBLE
							values[i] = New TDBDouble
							values[i].setDouble(String.fromBytes(mySQLFields[i].dataValue, fieldLength).toDouble())
						Case DBTYPE_DATE
							' TODO
						Case DBTYPE_BLOB
							' TODO
						Default
							values[i] = New TDBString
							values[i].setString(sizedUTF8toISO8859(mySQLFields[i].dataValue, fieldLength))
					End Select
					
				End If
				
			Else
				' a non-prepared query
				
				If Not bmx_mysql_rowField_isNull(row, i) Then
				
					Local fieldLength:Int = bmx_mysql_getLength(mysql_fetch_lengths(resultHandle), i)
				
					Select rec.fields[i].fType
						Case DBTYPE_INT
							values[i] = New TDBInt
							values[i].setInt(String.fromBytes(bmx_mysql_rowField_chars(row, i), fieldLength).toInt())
						Case DBTYPE_LONG
							values[i] = New TDBLong
							values[i].setLong(String.fromBytes(bmx_mysql_rowField_chars(row, i), fieldLength).toLong())
						Case DBTYPE_FLOAT
							values[i] = New TDBFloat
							values[i].setFloat(String.fromBytes(bmx_mysql_rowField_chars(row, i), fieldLength).toFloat())
						Case DBTYPE_DOUBLE
							values[i] = New TDBDouble
							values[i].setDouble(String.fromBytes(bmx_mysql_rowField_chars(row, i), fieldLength).toDouble())
						Case DBTYPE_DATE
							' TODO
						Case DBTYPE_BLOB
							' TODO
						Default
							values[i] = New TDBString
							values[i].setString(sizedUTF8toISO8859(bmx_mysql_rowField_chars(row, i), fieldLength))
					End Select
					
				End If
				
			
			End If
		Next		
		
		
		index:+ 1
		
		Return True
	End Method
	
	Method lastInsertedId:Long()
		If Not isActive()
			Return -1
		End If
		
		Local id:Long = -1
		
		If preparedQuery Then
			bmx_mysql_stmt_insert_id(stmtHandle, Varptr id)
		Else
			bmx_mysql_insert_id(conn.handle, Varptr id)
		End If
		
		Return id
	End Method
	
	Method rowsAffected:Int()
		Return _rowsAffected
	End Method

	Method dbTypeFromNative:Int(name:String, _type:Int = 0, _flags:Int = 0)
	
		Local dbType:Int
		
		Select _type
			Case MYSQL_TYPE_TINY, MYSQL_TYPE_SHORT, MYSQL_TYPE_LONG, MYSQL_TYPE_INT24
				dbType = DBTYPE_INT
			Case MYSQL_TYPE_YEAR
				dbType = DBTYPE_INT
			Case MYSQL_TYPE_LONGLONG
				dbType = DBTYPE_LONG
			Case MYSQL_TYPE_FLOAT
				dbType = DBTYPE_FLOAT
			Case MYSQL_TYPE_DOUBLE, MYSQL_TYPE_DECIMAL
				dbType = DBTYPE_DOUBLE
			Case MYSQL_TYPE_DATE
				dbType = DBTYPE_DATE
			Case MYSQL_TYPE_TIME
				dbType = DBTYPE_DATE ' TODO
			Case MYSQL_TYPE_DATETIME, MYSQL_TYPE_TIMESTAMP
				dbType = DBTYPE_DATE ' TODO
			Case MYSQL_TYPE_BLOB, MYSQL_TYPE_TINY_BLOB, MYSQL_TYPE_MEDIUM_BLOB, MYSQL_TYPE_LONG_BLOB
				If _flags & 128 Then ' binary !
					dbType = DBTYPE_BLOB
				Else ' String!
					dbType = DBTYPE_STRING
				End If
			Default
				dbType = DBTYPE_STRING
		End Select
		
		Return dbType
	End Method

	Method cleanup()

		If resultHandle Then
			mysql_free_result(resultHandle)
			resultHandle = Null
		End If
		
		If stmtHandle Then
			If mysql_stmt_close(stmtHandle) Then
			
			End If
			stmtHandle = Null
		End If
		
		If metaHandle Then
			mysql_free_result(metaHandle)
			metaHandle = Null
		End If
		
		If parameterBindings Then
			bmx_mysql_deleteBindings(parameterBindings)
			parameterBindings = Null
		End If

		If selectBindings Then
			bmx_mysql_deleteBindings(selectBindings)
			selectBindings = Null
		End If
	
		If mySQLFields Then
			For Local i:Int = 0 Until mySQLFields.length
				If mySQLFields[i] Then
					mySQLFields[i].clear()
					mySQLFields[i] = Null
				End If
			Next
		End If
		
		index = SQL_BeforeFirstRow
		rec.clear()
		_isActive = False
		
		preparedQuery = True
	End Method
	
	Method clear()
		cleanup()
	
		Super.clear()
	End Method
	
	Method free()
		clear()
	End Method
	
	Method Delete()
		free()
	End Method
	
End Type




Type TMySQLDatabaseLoader Extends TDatabaseLoader

	Method New()
		_type = "MYSQL"
	End Method

	Method LoadDatabase:TDBConnection( dbname:String = Null, host:String = Null, ..
		port:Int = Null, user:String = Null, password:String = Null, ..
		server:String = Null, options:String = Null )
	
		Return TDBMySQL.Create(dbName, host, port, user, password, server, options)
		
	End Method

End Type

AddDatabaseLoader New TMySQLDatabaseLoader
