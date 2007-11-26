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
bbdoc: Database Driver - ODBC
about: An ODBC database driver for #bah.database.Database
End Rem
Module BaH.DBODBC

ModuleInfo "Version: 1.04"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: Wrapper - 2007 Bruce A Henderson"
ModuleInfo "Copyright: iODBC - OpenLink Software"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.04"
ModuleInfo "History: Implemented Date, DateTime and Time types."
ModuleInfo "History: 1.03"
ModuleInfo "History: Fixed issue with mis-count of bound parameters."
ModuleInfo "History: 1.02"
ModuleInfo "History: Added hasPrepareSupport() and hasTransactionSupport() methods."
ModuleInfo "History: 1.01"
ModuleInfo "History: Fixed MacOS SQLSMALLINT/int cast issues."
ModuleInfo "History: 1.00 Initial Release"
ModuleInfo "History: Includes iODBC 3.52.5 source for Linux/MacOS module."


Import BaH.Database

Import "common.bmx"

Type TDBODBC Extends TDBConnection

	Field envHandle:Byte Ptr
	

	Function Create:TDBConnection(dbname:String = Null, host:String = Null, ..
		port:Int = Null, user:String = Null, password:String = Null, ..
		server:String = Null, options:String = Null)
		
		Local this:TDBODBC = New TDBODBC
		
		this.init(dbname, host, port, user, password, server, options)
		
		If this._dbname Then
			this.open(user, password)
		End If
		
		Return this
		
	End Function

	Method close()
		_isOpen = False
		
		If handle Then
			bmx_odbc_disconnectAndFree(handle)
			handle = Null
		End If
		
		If envHandle Then
			bmx_odbc_freeEnvHandle(envHandle)
			envHandle = Null
		End If
		
	End Method
	
	Method commit:Int()
		If Not _isOpen Then
			Return False
		End If
		
		Local result:Int = bmx_odbc_commitTransaction(handle)
		
		If isSQLError(result) Then
			processError(SQL_HANDLE_DBC, "Error committing transaction", TDatabaseError.ERROR_TRANSACTION)
			Return False
		End If

		result = bmx_odbc_toggleTransaction(handle, True) ' enable autocommit - ends transaction
		
		If isSQLError(result) Then
			processError(SQL_HANDLE_DBC, "Error ending transaction", TDatabaseError.ERROR_TRANSACTION)
			Return False
		End If
		
		Return True
	End Method
	
	Method getTables:String[]()
		Local list:String[]
		
		If Not _isOpen Then
			Return list
		End If
		
		Local stmtHandle:Byte Ptr
		
		' allocate a new statement handle
		Local result:Int = bmx_odbc_SQLAllocHandle(SQL_HANDLE_STMT, handle, Varptr stmtHandle)
		
		If isSQLError(result) Then
			processError(SQL_HANDLE_DBC, "Error allocating statement handle", TDatabaseError.ERROR_STATEMENT)
			If stmtHandle Then
				result = bmx_odbc_freeStmtHandle(stmtHandle)
			End If
			Return list
		End If
		
		' set the cursor to forward only
		result = bmx_odbc_setForwardCursor(stmtHandle)
		
		If isSQLError(result) Then
			processError(SQL_HANDLE_STMT, "Error setting cursor type", TDatabaseError.ERROR_STATEMENT, stmtHandle)
			result = bmx_odbc_freeStmtHandle(stmtHandle)
			Return list
		End If
		
		result = bmx_odbc_SQLTables(stmtHandle, "TABLE", 5)

		If isSQLError(result) Then
			processError(SQL_HANDLE_STMT, "Error setting cursor type", TDatabaseError.ERROR_STATEMENT, stmtHandle)
			result = bmx_odbc_freeStmtHandle(stmtHandle)
			Return list
		End If
		
		result = bmx_odbc_SQLFetchScroll(stmtHandle)
		
		Local tables:TList = New TList
		While result = SQL_SUCCESS
		
		
			' This is a copy of the string stuff from executeQuery...
			' It might be better to refactor things so that we can reuse the same code.
		
			Local lenIndicator:Int
			Local buffer:Byte Ptr = MemAlloc(256)
			Local s:String
			
			While True
			
				result = bmx_odbc_SQLGetData_string(stmtHandle, 3, buffer, 256, Varptr lenIndicator)
			
				If isSQLError(result) Or result = SQL_NO_DATA Then
					Exit
				End If
				
				' nothing here...
				If lenIndicator = SQL_NULL_DATA Or lenIndicator = SQL_NO_TOTAL Then
					Exit
				End If
				
				Local actualSize:Int
				If result = SQL_SUCCESS_WITH_INFO Then
					actualSize = 256
				Else
					actualSize = lenIndicator
				End If
				
				If Not s Then
					s = sizedUTF8toISO8859(buffer, actualSize)
				Else
					s:+ sizedUTF8toISO8859(buffer, actualSize)
				End If
				
				If lenIndicator < 256 Then
					Exit
				End If
			Wend
			
			' free up the buffer memory
			MemFree(buffer)

			tables.addLast(s)
			
			result = bmx_odbc_SQLFetchScroll(stmtHandle)
		Wend
		
		If tables.count() > 0 Then
			list = New String[tables.count()]
			Local i:Int = 0
			For Local s:String = EachIn tables
				list[i] = s
				i:+ 1
			Next
		End If

		result = bmx_odbc_freeStmtHandle(stmtHandle)
		
		Return list
	End Method
	
	Method open:Int(user:String = Null, pass:String = Null)
	
		If _isOpen Then
			close()
		End If
		
		Assert _server, "server identifies the Datasource name. It cannot be empty"
		
		If user Then
			_user = user
		End If
		
		If pass Then
			_password = pass
		End If
	
		envHandle = bmx_odbc_newEnv()
		
		' allocate environment
		Local result:Int = bmx_odbc_SQLAllocHandle(SQL_HANDLE_ENV, Null, Varptr envHandle)
			
		If isSQLError(result) Then
			processError(SQL_HANDLE_ENV, "Error allocating environment", TDatabaseError.ERROR_CONNECTION)
			Return False
		End If
		
		' set env to use odbc3
		bmx_odbc_setattr_odbc3(envHandle)
		
		' allocate connection
		result = bmx_odbc_SQLAllocHandle(SQL_HANDLE_DBC, envHandle, Varptr handle)

		If isSQLError(result) Then
			processError(SQL_HANDLE_DBC, "Error allocating connection", TDatabaseError.ERROR_CONNECTION)
			Return False
		End If
		
		' TODO : connection options
		
		' connect to the driver/database
		Local connect:String
		If _server.contains("DRIVER") Or _server.contains("SERVER") Then
			connect = _server
		Else If _server.contains(".dsn") Then
			connect = "FILEDSN=" + _server
		Else
			connect = "DSN=" + _server
		End If
	
		If _dbname And _dbname.length > 0 Then
			connect:+ ";DATABASE=" + _dbname
		End If
		
		If _user Then
			connect:+ ";USER=" + _user
		End If
		
		If _password Then
			connect:+ ";PWD=" + _password
		End If
		
		If _host Then
			connect:+ ";HOST=" + _host
		End If
		
		If _port Then
			connect:+ ";PORT=" + _port
		End If

		Local conv:String = convertISO8859toUTF8(connect)
		result = bmx_odbc_SQLDriverConnect(handle, conv, conv.length)
		
		If isSQLError(result) Then
			processError(SQL_HANDLE_DBC, "Error opening connection", TDatabaseError.ERROR_CONNECTION)
			Return False
		End If
		
		' success!
		_isOpen = True
		Return True
	End Method

	Method rollback:Int()
		If Not _isOpen Then
			Return False
		End If
		
		Local result:Int = bmx_odbc_rollbackTransaction(handle)
		
		If isSQLError(result) Then
			processError(SQL_HANDLE_DBC, "Error rolling back transaction", TDatabaseError.ERROR_TRANSACTION)
			Return False
		End If
		
		result = bmx_odbc_toggleTransaction(handle, True) ' enable autocommit - ends transaction
		
		If isSQLError(result) Then
			processError(SQL_HANDLE_DBC, "Error ending transaction", TDatabaseError.ERROR_TRANSACTION)
			Return False
		End If
		
		Return True
	End Method
	
	Method startTransaction:Int()
	
		If Not _isOpen Then
			Return False
		End If
		
		' ODBC doesn't have an actual "begin work" type of option.
		' Instead, we need to turn off "autocommit"...
		Local result:Int = bmx_odbc_toggleTransaction(handle, False) ' disable autocommit - begins transaction
		
		If isSQLError(result) Then
			processError(SQL_HANDLE_DBC, "Error starting transaction", TDatabaseError.ERROR_TRANSACTION)
			Return False
		End If
		
		Return True
	End Method

	Method databaseHandle:Byte Ptr()
	End Method
	
	Method createResultSet:TQueryResultSet()
		Return TODBCResultSet.Create(Self)
	End Method
	
	Method nativeErrorMessage:String(err:Int)
	End Method

	Method processError(kind:Int, msg:String, errType:Int, h:Byte Ptr = Null)
		Local code:Int
		Local size:Int
		Local err:Byte Ptr
		
		Select kind
			Case SQL_HANDLE_ENV
				err = bmx_odbc_envError(envHandle, Varptr code, Varptr size)
			Case SQL_HANDLE_DBC
				err = bmx_odbc_connError(handle, Varptr code, Varptr size)
			Case SQL_HANDLE_STMT
				err = bmx_odbc_stmtError(h, Varptr code, Varptr size)
		End Select
		
		setError(msg, sizedUTF8toISO8859(err, size), errType, code)
	End Method

	Method hasPrepareSupport:Int()
		Return True
	End Method

	Method hasTransactionSupport:Int()
		Return True
	End Method

End Type


Function isSQLError:Int(result:Int)
	If result = SQL_SUCCESS Or result = SQL_SUCCESS_WITH_INFO Then
		Return False
	End If
	Return True
End Function



Type TODBCResultSet Extends TQueryResultSet

	Function Create:TQueryResultSet(db:TDBConnection, sql:String = Null)
		Local this:TODBCResultSet = New TODBCResultSet
		
		this.init(db, sql)
		this.rec = TQueryRecord.Create()
		
		Return this
	End Function
	
	
	Method executeQuery:Int(statement:String)
	
		_isActive = False
		index = SQL_BeforeFirstRow
		
		rec.clear()
		
		Local result:Int
		
		If stmtHandle Then
		
			result = bmx_odbc_freeStmtHandle(stmtHandle)
			stmtHandle = Null

			If isSQLError(result) Then
				TDBODBC(conn).processError(SQL_HANDLE_STMT, "Error freeing statement handle", TDatabaseError.ERROR_STATEMENT, stmtHandle)
				Return False
			End If
			
		End If
		
		' allocate a new statement handle
		result = bmx_odbc_SQLAllocHandle(SQL_HANDLE_STMT, conn.handle, Varptr stmtHandle)
		
		If isSQLError(result) Then
			TDBODBC(conn).processError(SQL_HANDLE_DBC, "Error allocating statement handle", TDatabaseError.ERROR_STATEMENT)
			Return False
		End If
		
		' set the cursor to forward only
		result = bmx_odbc_setForwardCursor(stmtHandle)
		
		If isSQLError(result) Then
			TDBODBC(conn).processError(SQL_HANDLE_STMT, "Error setting cursor type", TDatabaseError.ERROR_STATEMENT, stmtHandle)
			Return False
		End If

		Local q:String = convertISO8859toUTF8(statement)
		
		' execute the query
		result = bmx_odbc_execute(stmtHandle, q, q.length)
		
		If isSQLError(result) Then
			TDBODBC(conn).processError(SQL_HANDLE_STMT, "Error executing statement", TDatabaseError.ERROR_STATEMENT, stmtHandle)
			Return False
		End If

		Local fieldCount:Int
		bmx_odbc_SQLNumResultCols(stmtHandle, Varptr fieldCount)

		initRecord(fieldCount)
		
		' this was a select... we can populate the fields with information (column name, size, etc)
		If fieldCount <> 0 Then
			Local bufferLength:Int = 256
			Local columnName:Byte Ptr = MemAlloc(bufferLength)
			Local nameLength:Int
			Local dataType:Int
			Local columnSize:Int
			Local decimalDigits:Int
			Local nullable:Int

			For Local i:Int = 0 Until fieldCount

				' get the column/field description
				result = bmx_odbc_SQLDescribeCol(stmtHandle, i + 1, columnName, bufferLength, ..
					Varptr nameLength, Varptr dataType, Varptr columnSize, ..
					Varptr decimalDigits, Varptr nullable)

				If isSQLError(result) Then
					TDBODBC(conn).processError(SQL_HANDLE_STMT, "Error getting column description", TDatabaseError.ERROR_STATEMENT, stmtHandle)

					' ensure the memory is freed!
					If columnName Then
						MemFree(columnName)
					End If
					
					Return False
				End If
				
				Local qf:TQueryField = TQueryField.Create(sizedUTF8toISO8859(columnName, nameLength), dbTypeFromNative(Null, dataType))
				If columnSize = 0 Then
					qf.length = -1 ' not specified
				Else
					qf.length = columnSize
				End If
				If decimalDigits = 0 Then
					qf.precision = -1 ' not specified
				Else
					qf.precision = decimalDigits
				End If
				If nullable = SQL_NULLABLE Then
					qf.nullable = True
				Else If nullable = SQL_NO_NULLS Then
					qf.nullable = False
				End If
					
				rec.setField(i, qf)
				
			Next
			
			' free up the byte ptr memory
			If columnName Then
				MemFree(columnName)
			End If
			
		End If

		_isActive = True
		Return True
	End Method
	
	Method prepare:Int(statement:String)

		_isActive = False
		index = SQL_BeforeFirstRow
		
		rec.clear()
		
		Local result:Int
		
		If stmtHandle Then
		
			result = bmx_odbc_freeStmtHandle(stmtHandle)
			stmtHandle = Null

			If isSQLError(result) Then
				TDBODBC(conn).processError(SQL_HANDLE_STMT, "Error freeing statement handle", TDatabaseError.ERROR_STATEMENT, stmtHandle)
				Return False
			End If
			
		End If
		
		' allocate a new statement handle
		result = bmx_odbc_SQLAllocHandle(SQL_HANDLE_STMT, conn.handle, Varptr stmtHandle)
		
		If isSQLError(result) Then
			TDBODBC(conn).processError(SQL_HANDLE_DBC, "Error allocating statement handle", TDatabaseError.ERROR_STATEMENT)
			Return False
		End If
		
		' set the cursor to forward only
		result = bmx_odbc_setForwardCursor(stmtHandle)
		
		If isSQLError(result) Then
			TDBODBC(conn).processError(SQL_HANDLE_STMT, "Error setting cursor type", TDatabaseError.ERROR_STATEMENT, stmtHandle)
			Return False
		End If

		Local q:String = convertISO8859toUTF8(statement)
		
		' prepare the query
		result = bmx_odbc_prepare(stmtHandle, q, q.length)
		
		If isSQLError(result) Then
			TDBODBC(conn).processError(SQL_HANDLE_STMT, "Error preparing statement", TDatabaseError.ERROR_STATEMENT, stmtHandle)
			Return False
		End If

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

		If values Then
			Local paramCount:Int = bindCount

			Local isNull:Int[] = New Int[paramCount]
			Local strings:Byte Ptr[] = New Byte Ptr[paramCount]
			
			For Local i:Int = 0 Until paramCount

				isNull[i] = False
				
				If Not values[i] Or values[i].isNull() Then
					isNull[i] = SQL_NULL_DATA
				End If

				Select values[i].kind()
					Case DBTYPE_INT
						If Not values[i] Then
							values[i] = New TDBInt
						End If
						result = bmx_odbc_SQLBindParameter_int(stmtHandle, i + 1, Varptr TDBInt(values[i]).value, Varptr isNull[i])
					Case DBTYPE_FLOAT
						If Not values[i] Then
							values[i] = New TDBDouble
						End If
						' since ODBC doesn't do Floats, we convert to a Double, just to be safe
						If TDBFloat(values[i]) Then
							Local d:TDBDouble = New TDBDouble
							d.setDouble(Double(TDBFloat(values[i]).value))
							values[i].clear()
							values[i] = d
						End If
						result = bmx_odbc_SQLBindParameter_double(stmtHandle, i + 1, Varptr TDBDouble(values[i]).value, Varptr isNull[i])
					Case DBTYPE_DOUBLE
						If Not values[i] Then
							values[i] = New TDBDouble
						End If
						result = bmx_odbc_SQLBindParameter_double(stmtHandle, i + 1, Varptr TDBDouble(values[i]).value, Varptr isNull[i])
					Case DBTYPE_LONG
						If Not values[i] Then
							values[i] = New TDBLong
						End If

						result = bmx_odbc_SQLBindParameter_long(stmtHandle, i + 1, Varptr TDBLong(values[i]).value, Varptr isNull[i])
					Case DBTYPE_STRING
						If Not values[i] Then
							values[i] = New TDBString
						End If
						
						Local s:String = convertISO8859toUTF8(values[i].getString())
						strings[i] = s.toCString()

						If Not isNull[i] Then
							isNull[i] = s.length
						End If

						result = bmx_odbc_SQLBindParameter_string(stmtHandle, i + 1, strings[i], s.length, Varptr isNull[i])
						
					Case DBTYPE_BLOB
						'result = sqlite3_bind_blob(stmtHandle, i + 1, values[i].getBlob(), values[i].size(), 0)
					Case DBTYPE_DATE
					
						If Not values[i] Then
							values[i] = New TDBDate
						End If
						
						Local s:String = convertISO8859toUTF8(values[i].getString())
						strings[i] = s.toCString()

						If Not isNull[i] Then
							isNull[i] = s.length
						End If

						result = bmx_odbc_SQLBindParameter_string(stmtHandle, i + 1, strings[i], s.length, Varptr isNull[i])
						
					Case DBTYPE_DATETIME
					
						If Not values[i] Then
							values[i] = New TDBDateTime
						End If
						
						Local s:String = convertISO8859toUTF8(values[i].getString())
						strings[i] = s.toCString()

						If Not isNull[i] Then
							isNull[i] = s.length
						End If

						result = bmx_odbc_SQLBindParameter_string(stmtHandle, i + 1, strings[i], s.length, Varptr isNull[i])
						
					Case DBTYPE_TIME

						If Not values[i] Then
							values[i] = New TDBTime
						End If
						
						Local s:String = convertISO8859toUTF8(values[i].getString())
						strings[i] = s.toCString()

						If Not isNull[i] Then
							isNull[i] = s.length
						End If

						result = bmx_odbc_SQLBindParameter_string(stmtHandle, i + 1, strings[i], s.length, Varptr isNull[i])

				End Select

				If isSQLError(result) Then
					TDBODBC(conn).processError(SQL_HANDLE_STMT, "Error binding parameters", TDatabaseError.ERROR_STATEMENT, stmtHandle)

					' free up the strings
					For Local i:Int = 0 Until paramCount
						If strings[i] Then
							MemFree(strings[i])
						End If
					Next
		
					Return False
				End If

			Next
			
			' free up the strings
			For Local i:Int = 0 Until paramCount
				If strings[i] Then
					MemFree(strings[i])
				End If
			Next
			
		End If
		
		' execute the query
		result = bmx_odbc_executePrepared(stmtHandle)
		
		If isSQLError(result) Then
			TDBODBC(conn).processError(SQL_HANDLE_STMT, "Error executing statement", TDatabaseError.ERROR_STATEMENT, stmtHandle)
			Return False
		End If

		Local fieldCount:Int
		bmx_odbc_SQLNumResultCols(stmtHandle, Varptr fieldCount)

		initRecord(fieldCount)
		
		' this was a select... we can populate the fields with information (column name, size, etc)
		If fieldCount <> 0 Then

			Local bufferLength:Int = 256
			Local columnName:Byte Ptr = MemAlloc(bufferLength)
			Local nameLength:Int
			Local dataType:Int
			Local columnSize:Int
			Local decimalDigits:Int
			Local nullable:Int

			For Local i:Int = 0 Until fieldCount

				' get the column/field description
				result = bmx_odbc_SQLDescribeCol(stmtHandle, i + 1, columnName, bufferLength, ..
					Varptr nameLength, Varptr dataType, Varptr columnSize, ..
					Varptr decimalDigits, Varptr nullable)

				If isSQLError(result) Then
					TDBODBC(conn).processError(SQL_HANDLE_STMT, "Error getting column description", TDatabaseError.ERROR_STATEMENT, stmtHandle)

					' ensure the memory is freed!
					If columnName Then
						MemFree(columnName)
					End If
					
					Return False
				End If

				Local qf:TQueryField = TQueryField.Create(sizedUTF8toISO8859(columnName, nameLength), dbTypeFromNative(Null, dataType))
				If columnSize = 0 Then
					qf.length = -1 ' not specified
				Else
					qf.length = columnSize
				End If
				If decimalDigits = 0 Then
					qf.precision = -1 ' not specified
				Else
					qf.precision = decimalDigits
				End If
				If nullable = SQL_NULLABLE Then
					qf.nullable = True
				Else If nullable = SQL_NO_NULLS Then
					qf.nullable = False
				End If
					
				rec.setField(i, qf)
				
			Next
			
			' free up the byte ptr memory
			If columnName Then
				MemFree(columnName)
			End If
			
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
		
		If Not stmtHandle Then
			Return False
		End If
		
		
		Local result:Int = bmx_odbc_SQLFetchScroll(stmtHandle)

		If result <> SQL_SUCCESS And result <> SQL_SUCCESS_WITH_INFO Then
			If result <> SQL_NO_DATA Then
				TDBODBC(conn).processError(SQL_HANDLE_STMT, "Error fetching row", TDatabaseError.ERROR_STATEMENT, stmtHandle)
			End If
			Return False
		End If
		
		' now populate the values[] array with the fetched data !
		
		For Local i:Int = 0 Until rec.count()
		
			If values[i] Then
				values[i].clear()
			End If
			
			Local lenIndicator:Int
			
			Select rec.fields[i].fType
				Case DBTYPE_INT
				
					Local intValue:Int
					
					result = bmx_odbc_SQLGetData_int(stmtHandle, i + 1, Varptr intValue, Varptr lenIndicator)
				
					If isSQLError(result) Or lenIndicator = SQL_NULL_DATA Then
						Continue
					End If
					
					values[i] = New TDBInt
					values[i].setInt(intValue)
					
				Case DBTYPE_LONG
				
					Local longValue:Long
					
					result = bmx_odbc_SQLGetData_long(stmtHandle, i + 1, Varptr longValue, Varptr lenIndicator)
					
					If isSQLError(result) Or lenIndicator = SQL_NULL_DATA Then
						Continue
					End If
					
					values[i] = New TDBLong
					values[i].setLong(longValue)
					
				Case DBTYPE_DOUBLE
				
					Local doubleValue:Double
					
					result = bmx_odbc_SQLGetData_double(stmtHandle, i + 1, Varptr doubleValue, Varptr lenIndicator)
					
					If isSQLError(result) Or lenIndicator = SQL_NULL_DATA Then
						Continue
					End If
					
					values[i] = New TDBDouble
					values[i].setDouble(doubleValue)
					
				Case DBTYPE_DATE

					Local y:Int, m:Int, d:Int
	
					result = bmx_odbc_SQLGetData_date(stmtHandle, i + 1, Varptr y, Varptr m, ..
							Varptr d, Varptr lenIndicator)
					
					If isSQLError(result) Or lenIndicator = SQL_NULL_DATA Then
						Continue
					End If
					
					Local date:TDBDate = New TDBDate
					values[i] = date
					date.setFromParts(y, m, d)

				Case DBTYPE_DATETIME

					Local y:Int, m:Int, d:Int, hh:Int, mm:Int, ss:Int
	
					result = bmx_odbc_SQLGetData_datetime(stmtHandle, i + 1, Varptr y, Varptr m, ..
							Varptr d, Varptr hh, Varptr mm, Varptr ss, Varptr lenIndicator)
					
					If isSQLError(result) Or lenIndicator = SQL_NULL_DATA Then
						Continue
					End If
					
					Local date:TDBDateTime = New TDBDateTime
					values[i] = date
					date.setFromParts(y, m, d, hh, mm, ss)
	
				Case DBTYPE_TIME

					Local hh:Int, mm:Int, ss:Int
	
					result = bmx_odbc_SQLGetData_time(stmtHandle, i + 1, Varptr hh, Varptr mm, Varptr ss, Varptr lenIndicator)
					
					If isSQLError(result) Or lenIndicator = SQL_NULL_DATA Then
						Continue
					End If
					
					Local date:TDBTime = New TDBTime
					values[i] = date
					date.setFromParts(hh, mm, ss)

				Case DBTYPE_BLOB
					' TODO
				Default
					
					Local s:String = getStringData(rec.fields[i], i, result)
					
					If isSQLError(result) Or result = SQL_NO_DATA Then
						Continue
					End If
					
					If s Then
						values[i] = New TDBString
						values[i].setString(s)
					End If
			End Select

		Next
	
	
		index:+ 1
		
		Return True
	End Method
	
	Method getStringData:String(record:TQueryField, i:Int, result:Int Var)
		' Strings are returned in blocks... so we need to loop thru
		' to get all the possible data.

		Local bufferSize:Int = record.length
					
		If record.length <= 0 Then
			bufferSize = 256
		Else If record.length > 32768 Then
			bufferSize = 32768
		End If
		
		Local lenIndicator:Int
		Local buffer:Byte Ptr = MemAlloc(bufferSize)
		Local s:String
		
		While True
		
			result = bmx_odbc_SQLGetData_string(stmtHandle, i + 1, buffer, bufferSize, Varptr lenIndicator)
		
			If isSQLError(result) Or result = SQL_NO_DATA Then
				Exit
			End If
			
			' nothing here...
			If lenIndicator = SQL_NULL_DATA Or lenIndicator = SQL_NO_TOTAL Then
				Exit
			End If
			
			Local actualSize:Int
			If result = SQL_SUCCESS_WITH_INFO Then
				actualSize = bufferSize
			Else
				actualSize = lenIndicator
			End If
			
			If Not s Then
				s = sizedUTF8toISO8859(buffer, actualSize)
			Else
				s:+ sizedUTF8toISO8859(buffer, actualSize)
			End If
			
			If lenIndicator < bufferSize Then
				Exit
			End If
		Wend
		
		' free up the buffer memory
		MemFree(buffer)
		
		Return s
	End Method
	
	Method lastInsertedId:Long()
		' TODO : with... "SELECT LAST_INSERT_ID()"
	End Method

	Method rowsAffected:Int()
	
		If stmtHandle Then
			Local num:Int
			Local result:Int = bmx_odbc_SQLRowCount(stmtHandle, Varptr num)
			
			If Not isSQLError(result) Then
				Return num
			End If
		End If
	
		Return -1
	End Method

	Method dbTypeFromNative:Int(name:String, _type:Int = 0, _flags:Int = 0)
		Local dbType:Int
		
		Select _type
			Case SQL_SMALLINT, SQL_INTEGER, SQL_BIT, SQL_TINYINT
				dbType = DBTYPE_INT
			Case SQL_BIGINT, 65531
				' 65531 is what it returned from a bigint field against a mysql DB on linux....
				dbType = DBTYPE_LONG
			Case SQL_DECIMAL, SQL_NUMERIC, SQL_REAL, SQL_FLOAT, SQL_DOUBLE
				dbType = DBTYPE_DOUBLE
			Case SQL_DATE, SQL_TYPE_DATE
				dbType = DBTYPE_DATE
			Case SQL_TIME, SQL_TYPE_TIME
				dbType = DBTYPE_TIME
			Case SQL_TIMESTAMP, SQL_TYPE_TIMESTAMP
				dbType = DBTYPE_DATETIME
			Case SQL_BINARY, SQL_VARBINARY, SQL_LONGVARBINARY
				dbType = DBTYPE_BLOB
			Default
				dbType = DBTYPE_STRING
		End Select
		
		Return dbType
	End Method

	Method initRecord(size:Int)

		rec.clear()

		If size > 0 Then		
			rec.init(size)
		End If
		
		resetValues(size)
	End Method

End Type





Type TODBCDatabaseLoader Extends TDatabaseLoader

	Method New()
		_type = "ODBC"
	End Method

	Method LoadDatabase:TDBConnection( dbname:String = Null, host:String = Null, ..
		port:Int = Null, user:String = Null, password:String = Null, ..
		server:String = Null, options:String = Null )
	
		Return TDBODBC.Create(dbName, host, port, user, password, server, options)
		
	End Method

End Type

AddDatabaseLoader New TODBCDatabaseLoader
