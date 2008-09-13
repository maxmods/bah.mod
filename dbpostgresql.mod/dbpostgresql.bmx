' Copyright (c) 2007-2008, Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ''AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Rem
bbdoc: Database Driver - PostgreSQL
about: A PostgreSQL database driver for #bah.database.Database
End Rem
Module BaH.DBPostgreSQL

ModuleInfo "Version: 1.03"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.03"
ModuleInfo "History: isOpen() now checks the connection status."
ModuleInfo "History: Sets active to false when all rows read."
ModuleInfo "History: Resultset cleanup improvements."
ModuleInfo "History: 1.02"
ModuleInfo "History: Added hasPrepareSupport() and hasTransactionSupport() methods."
ModuleInfo "History: 1.01"
ModuleInfo "History: Fixed open() not closing if already open."
ModuleInfo "History: 1.00 Initial Release"

?macos
ModuleInfo "LD_OPTS: -L%PWD%/lib/macos"
?win32
ModuleInfo "LD_OPTS: -L%PWD%/lib/win32"
?

Import BaH.Database

Import "common.bmx"



Type TDBPostgreSQL Extends TDBConnection

	Function Create:TDBConnection(dbname:String = Null, host:String = Null, ..
		port:Int = Null, user:String = Null, password:String = Null, ..
		server:String = Null, options:String = Null)
		
		Local this:TDBPostgreSQL = New TDBPostgreSQL
		
		this.init(dbname, host, port, user, password, server, options)
		
		If this._dbname Then
			this.open(user, password)
		End If
		
		Return this
		
	End Function

	Method close()
	
		If _isOpen Then
			If handle Then
				bmx_pgsql_PQfinish(handle)
				handle = Null
			End If
			
			_isOpen = False
		End If
	
	End Method

	Method isOpen:Int()
		If _isOpen Then
			' really check that the database is open
			If bmx_pgsql_PQstatus(handle) Then
				_isOpen = False
			End If
		End If
		
		Return _isOpen
	End Method
		
	Method commit:Int()

		If Not _isOpen Or Not handle Then
			Return False
		End If
		
		Local result:Byte Ptr = bmx_pgsql_PQexec(handle, "COMMIT")
		
		If Not result Or bmx_pgsql_PQresultStatus(result) <> PGRES_COMMAND_OK Then
			setError("Error committing transaction", convertUTF8toISO8859(bmx_pgsql_PQerrorMessage(handle)), TDatabaseError.ERROR_TRANSACTION)
			bmx_pgsql_PQclear(result)
			Return False
		End If
		
		bmx_pgsql_PQclear(result)
		
		Return True
	End Method
	
	Method getTables:String[]()
		Local list:String[]

		If Not _isOpen Then
			Return list
		End If

		Local tables:TList = New TList
		
		Local query:TDatabaseQuery = TDatabaseQuery.Create(Self)
		
		Local sql:String = "Select tablename from pg_tables where schemaname Not in ('pg_catalog', 'information_schema')"
			
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
	
		If _isOpen Then
			close()
		End If
		
		If user Then
			_user = user
		End If
		
		If pass Then
			_password = pass
		End If
		
		
		Local connect:String
		
		If _dbname Then
			connect :+ "dbname=" + _dbname + "~n"
		End If
		
		If _host Then
			connect :+ "host=" + _host + "~n"
		End If

		If _port Then
			connect :+ "port=" + _port + "~n"
		End If

		If _user Then
			connect :+ "user=" + _user + "~n"
		End If

		If _password Then
			connect :+ "password=" + _password + "~n"
		End If
		
		
		handle = bmx_pgsql_PQconnectdb(connect)
		
		If bmx_pgsql_PQstatus(handle) Then
			setError("Error connecting to database '" + _dbname + "'", convertUTF8toISO8859(bmx_pgsql_PQerrorMessage(handle)), TDatabaseError.ERROR_CONNECTION)
			Return False
		End If
		
		
		_isOpen = True
		Return True
	End Method

	Method rollback:Int()
	
		If Not _isOpen Or Not handle Then
			Return False
		End If
		
		Local result:Byte Ptr = bmx_pgsql_PQexec(handle, "ROLLBACK")
		
		If Not result Or bmx_pgsql_PQresultStatus(result) <> PGRES_COMMAND_OK Then
			setError("Error rolling back transaction", convertUTF8toISO8859(bmx_pgsql_PQerrorMessage(handle)), TDatabaseError.ERROR_TRANSACTION)
			bmx_pgsql_PQclear(result)
			Return False
		End If
		
		bmx_pgsql_PQclear(result)
		
		Return True
	End Method
	
	Method startTransaction:Int()
	
		If Not _isOpen Or Not handle Then
			Return False
		End If
		
		Local result:Byte Ptr = bmx_pgsql_PQexec(handle, "BEGIN")
		
		If Not result Or bmx_pgsql_PQresultStatus(result) <> PGRES_COMMAND_OK Then
			setError("Error starting transaction", convertUTF8toISO8859(bmx_pgsql_PQerrorMessage(handle)), TDatabaseError.ERROR_TRANSACTION)
			bmx_pgsql_PQclear(result)
			Return False
		End If
		
		bmx_pgsql_PQclear(result)
		
		Return True
	End Method

	Method databaseHandle:Byte Ptr()
		Return handle
	End Method
	
	Method createResultSet:TQueryResultSet()
		Return TPostgreSQLResultSet.Create(Self)
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


Type TPostgreSQLResultSet Extends TQueryResultSet

	' a pointer to a PGResult
	Field pgResult:Byte Ptr
	' number of rows returned in the query
	Field _queryRows:Int

	Field _rowsAffected:Int
	
	Field _preparedStatementName:String

	Function Create:TQueryResultSet(db:TDBConnection, sql:String = Null)
		Local this:TPostgreSQLResultSet = New TPostgreSQLResultSet
		
		this.init(db, sql)
		this.rec = TQueryRecord.Create()
		
		Return this
	End Function
	
	Method Delete()
		If _preparedStatementName Then
			executeQuery("DEALLOCATE " + _preparedStatementName)
			_preparedStatementName = Null
		End If
		cleanup()
	End Method

	Method clearResultSet()
		If pgResult Then
			bmx_pgsql_PQclear(pgResult)
			pgResult = Null
		End If
	End Method
	
	Method cleanup()
		clearResultSet()
		index = SQL_BeforeFirstRow
		_isActive = False
		_queryRows = -1
	End Method
	
	Method executeQuery:Int(statement:String)
		
		If Not conn.isOpen() Then
			Return False
		End If
		
		cleanup()
		
		Local q:String = convertISO8859toUTF8(statement)
		pgResult = bmx_pgsql_PQexec(conn.handle, q)
		
		If Not pgResult Then
			Return False
		End If
		
		Local status:Int = bmx_pgsql_PQresultStatus(pgResult)
		
		Select status
			Case PGRES_TUPLES_OK
				' returned some row data... probably a select!?
				' how many ?
				_queryRows = bmx_pgsql_PQntuples(pgResult)
			Case PGRES_COMMAND_OK
				' success but returned nothing. insert, update, delete etc
				' nothing to see here...
				_queryRows = -1
			Default
				' an error!
				conn.setError("Error executing statement", convertUTF8toISO8859(bmx_pgsql_PQerrorMessage(conn.handle)), TDatabaseError.ERROR_STATEMENT, 0)				
				Return False
		End Select

		Local fieldCount:Int = bmx_pgsql_PQnfields(pgResult)

		initRecord(fieldCount)

		' PQcmdTuples returns an empty string for non-change statements, so we should
		' get a zero in here for selects...
		_rowsAffected = String.fromCString(bmx_pgsql_PQcmdTuples(pgResult)).toInt()

		' get the field descriptions
		If fieldCount <> 0 Then
			
			For Local i:Int = 0 Until fieldCount
				Local dtype:Int = bmx_pgsql_PQftype(pgResult, i)
				Local qf:TQueryField = TQueryField.Create(convertUTF8toISO8859(bmx_pgsql_PQfname(pgResult, i)), dbTypeFromNative(Null, dtype))
				qf.length = bmx_pgsql_PQfsize(pgResult, i)
				qf.precision = bmx_pgsql_PQfmod(pgResult, i)
				qf.dtype = dtype
				' if length is -1, then precision field holds actual length value, and
				' precision should be ignored.
				If qf.length = -1 Then
					qf.length = qf.precision - 4
					qf.precision = -1
				End If
				
				rec.setField(i, qf)
				
			Next
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

	Method prepare:Int(statement:String)

		cleanup()
		
		If Not statement Or statement.length = 0 Then
			Return False
		End If
		
		If Not _preparedStatementName Then
			_preparedStatementName = "prep" + Self.toString()
		Else
			executeQuery("DEALLOCATE " + _preparedStatementName)
			cleanup()
		End If
		
		Local q:String = convertISO8859toUTF8(statement)
		
		pgResult = bmx_pgsql_PQprepare(conn.handle, _preparedStatementName, q)

		If Not pgResult Then
			Return False
		End If
		
		If bmx_pgsql_PQresultStatus(pgResult) <> PGRES_COMMAND_OK Then
			conn.setError("Error preparing statement", convertUTF8toISO8859(bmx_pgsql_PQerrorMessage(conn.handle)), TDatabaseError.ERROR_STATEMENT, 0)				
			Return False
		End If

		Return True
	End Method
	
	Method execute:Int()
	
		cleanup()

		Local params:Byte Ptr
		Local paramCount:Int
		Local strings:Byte Ptr[]
		
		' BIND stuff
		Local values:TDBType[] = boundValues
		
		If values Then
			paramCount = values.length

			' ** NOTE **
			' PQdescribePrepared is only available in more recent additions.
			' It is useful in it lets us check validity of parameter count.
			' Otherwise we hope that the database catches any issues... :-/

			'Local result:Byte Ptr = bmx_pgsql_PQdescribePrepared(conn.handle, _preparedStatementName)
			
			'If bmx_pgsql_PQresultStatus(pgResult) <> PGRES_COMMAND_OK Then
			'	conn.setError("Error getting prepared statement details", convertUTF8toISO8859(bmx_pgsql_PQerrorMessage(conn.handle)), TDatabaseError.ERROR_STATEMENT, 0)				
			'	Return False
			'End If
			
			'If paramCount <> bmx_pgsql_PQnparams(result) Then
			'	conn.setError("Wrong number of bind parameters. Expected " + bmx_pgsql_PQnparams(result) + ..
			'		". Actual " + paramCount, Null, TDatabaseError.ERROR_STATEMENT, 0)				
			'	If result Then
			'		bmx_pgsql_PQclear(result)
			'	End If
			'	Return False
			'End If
			
			strings = New Byte Ptr[paramCount]
			params = bmx_pgsql_createParamValues(paramCount)
			
			For Local i:Int = 0 Until paramCount
			
				If Not values[i] Or values[i].isNull() Then
					bmx_pgsql_setNullParam(params, i)
				Else
					
					Select values[i].kind()
						Case DBTYPE_INT
							strings[i] = String.fromInt(TDBInt(values[i]).value).toCString()
							bmx_pgsql_setParam(params, i, strings[i])
						Case DBTYPE_LONG
							strings[i] = String.fromLong(TDBLong(values[i]).value).toCString()
							bmx_pgsql_setParam(params, i, strings[i])
						Case DBTYPE_FLOAT
							strings[i] = String.fromFloat(TDBFloat(values[i]).value).toCString()
							bmx_pgsql_setParam(params, i, strings[i])
						Case DBTYPE_DOUBLE
							strings[i] = String.fromDouble(TDBDouble(values[i]).value).toCString()
							bmx_pgsql_setParam(params, i, strings[i])
						Case DBTYPE_BLOB
							' TODO
						Case DBTYPE_DATE
							' TODO
						Default
							Local s:String = convertISO8859toUTF8(values[i].getString())
							strings[i] = s.toCString()
							
							bmx_pgsql_setParam(params, i, strings[i])
					End Select
					
					
					
				End If

			Next
			
			'If result Then
			'	bmx_pgsql_PQclear(result)
			'End If
		End If

		If params Then
			pgResult = bmx_pgsql_PQexecPrepared(conn.handle, _preparedStatementName, ..
				paramCount, params)
		Else
			pgResult = bmx_pgsql_PQexecPrepared(conn.handle, _preparedStatementName, ..
				paramCount, Null)
		End If
		
		' free up the strings
		For Local i:Int = 0 Until paramCount
			If strings[i] Then
				MemFree(strings[i])
			End If
		Next
		
		If params Then
			bmx_pgsql_deleteParamValues(params)
		End If
		
		If Not pgResult Then
			conn.setError("Error executing prepared statement", "", TDatabaseError.ERROR_STATEMENT, 0)				
			cleanup()
			Return False
		End If

		Local status:Int = bmx_pgsql_PQresultStatus(pgResult)
		
		Select status
			Case PGRES_TUPLES_OK
				' returned some row data... probably a select!?
				' how many ?
				_queryRows = bmx_pgsql_PQntuples(pgResult)
			Case PGRES_COMMAND_OK
				' success but returned nothing. insert, update, delete etc
				' nothing to see here...
				_queryRows = -1
			Default
				' an error!
				conn.setError("Error executing prepared statement", convertUTF8toISO8859(bmx_pgsql_PQerrorMessage(conn.handle)), TDatabaseError.ERROR_STATEMENT, 0)				
				cleanup()
				Return False
		End Select

		Local fieldCount:Int = bmx_pgsql_PQnfields(pgResult)

		initRecord(fieldCount)

		' PQcmdTuples returns an empty string for non-change statements, so we should
		' get a zero in here for selects...
		_rowsAffected = String.fromCString(bmx_pgsql_PQcmdTuples(pgResult)).toInt()

		' get the field descriptions
		If fieldCount <> 0 Then
			
			For Local i:Int = 0 Until fieldCount
				Local dtype:Int = bmx_pgsql_PQftype(pgResult, i)
				Local qf:TQueryField = TQueryField.Create(convertUTF8toISO8859(bmx_pgsql_PQfname(pgResult, i)), dbTypeFromNative(Null, dtype))
				qf.length = bmx_pgsql_PQfsize(pgResult, i)
				qf.precision = bmx_pgsql_PQfmod(pgResult, i)
				qf.dtype = dtype
				' if length is -1, then precision field holds actual length value, and
				' precision should be ignored.
				If qf.length = -1 Then
					qf.length = qf.precision - 4
					qf.precision = -1
				End If
				
				rec.setField(i, qf)
				
			Next
		End If

		' did we return any data?
		' if we didn't, then we may as well cleanup now
		If _queryRows < 1 Then
			cleanup()
		Else
			_isActive = True
		End If
		
		Return True
	End Method
	
	Method firstRow:Int()
		If index = SQL_BeforeFirstRow Then
			Return nextRow()
		End If
		
		Return False
	End Method
	
	Method nextRow:Int()

		If Not _isActive
			cleanup()
			Return False
		End If
		
		If index >= _queryRows - 1 Then
			cleanup()
			Return False
		End If
		
		' now populate the values[] array with the fetched data !
		
		For Local i:Int = 0 Until rec.count()
		
			If values[i] Then
				values[i].clear()
			End If

			If Not bmx_pgsql_PQgetisnull(pgResult, index + 1, i)
			
				Local fieldLength:Int = bmx_pgsql_PQgetlength(pgResult, index + 1, i)
			
				Select rec.fields[i].fType
					Case DBTYPE_INT
						values[i] = New TDBInt
						values[i].setInt(String.fromBytes(bmx_pgsql_PQgetvalue(pgResult, index + 1, i), fieldLength).toInt())
					Case DBTYPE_LONG
						values[i] = New TDBLong
						values[i].setLong(String.fromBytes(bmx_pgsql_PQgetvalue(pgResult, index + 1, i), fieldLength).toLong())
					Case DBTYPE_FLOAT
						values[i] = New TDBFloat
						values[i].setFloat(String.fromBytes(bmx_pgsql_PQgetvalue(pgResult, index + 1, i), fieldLength).toFloat())
					Case DBTYPE_DOUBLE
						values[i] = New TDBDouble
						values[i].setDouble(String.fromBytes(bmx_pgsql_PQgetvalue(pgResult, index + 1, i), fieldLength).toDouble())
					Case DBTYPE_DATE
						' TODO
					Case DBTYPE_BLOB
						' TODO
					Default
						values[i] = New TDBString
						values[i].setString(sizedUTF8toISO8859(bmx_pgsql_PQgetvalue(pgResult, index + 1, i), fieldLength))
				End Select
			
				
			End If
			
		Next
		
		index:+ 1
		
		If index >= _queryRows - 1 Then
			cleanup()
		End If
		
		Return True
	End Method
	
	Method lastInsertedId:Long()
		If _isActive And pgResult Then
			Return Long(bmx_pgqsl_PQoidValue(pgResult))
		End If
		Return -1
	End Method
	
	Method rowsAffected:Int()
		Return _rowsAffected
	End Method

	Method dbTypeFromNative:Int(name:String, _type:Int = 0, _flags:Int = 0)
	
		Local dbType:Int
		
		Select _type
			Case BOOLOID, INT2OID, INT4OID, VOIDOID, REGPROCOID, XIDOID, CIDOID
				dbType = DBTYPE_INT
			Case INT8OID
				dbType = DBTYPE_LONG
			Case FLOAT4OID
				dbType = DBTYPE_FLOAT
			Case NUMERICOID, FLOAT8OID
				dbType = DBTYPE_DOUBLE
			Case ABSTIMEOID, RELTIMEOID, DATEOID
				dbType = DBTYPE_DATE
			Case TIMEOID, TIMETZOID
				dbType = DBTYPE_DATE ' TODO
			Case TIMESTAMPOID, TIMESTAMPTZOID
				dbType = DBTYPE_DATE ' TODO
			Case BYTEAOID
				dbType = DBTYPE_BLOB
			Default
				dbType = DBTYPE_STRING
		End Select
		
		Return dbType
	End Method

End Type



Type TPostgreSQLDatabaseLoader Extends TDatabaseLoader

	Method New()
		_type = "POSTGRESQL"
	End Method

	Method LoadDatabase:TDBConnection( dbname:String = Null, host:String = Null, ..
		port:Int = Null, user:String = Null, password:String = Null, ..
		server:String = Null, options:String = Null )
	
		Return TDBPostgreSQL.Create(dbName, host, port, user, password, server, options)
		
	End Method

End Type

AddDatabaseLoader New TPostgreSQLDatabaseLoader
