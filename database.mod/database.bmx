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
bbdoc: Database Framework
End Rem
Module BaH.Database

ModuleInfo "Version: 1.06"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.06"
ModuleInfo "History: Implementation of Date, DateTime and Time types."
ModuleInfo "History: 1.05"
ModuleInfo "History: Improved object cleanup."
ModuleInfo "History: 1.04"
ModuleInfo "History: Improved getFieldByName efficiency."
ModuleInfo "History: Added TQueryRecord helper methods for type/name retrieval - getXXXByName()."
ModuleInfo "History: 1.03"
ModuleInfo "History: Fixed clearing of lasterror after successful query prepare/execute."
ModuleInfo "History: 1.02"
ModuleInfo "History: Added TDatabaseQuery helper binding functions for set/add values."
ModuleInfo "History: Docs update."
ModuleInfo "History: 1.01"
ModuleInfo "History: Fixed Null exception on re-prepare."
ModuleInfo "History: Added TDatabaseQuery clearBindValues() method."
ModuleInfo "History: Added getter methods to TQueryRecord for String, Int, Long, Float and Double."
ModuleInfo "History: Added hasPrepareSupport() and hasTransactionSupport() methods."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."


Import BRL.LinkedList
Import "dbtypes.bmx"

Const SQL_BeforeFirstRow:Int = -1
Const SQL_AfterLastRow:Int = -2

Rem
bbdoc: Represents a connection to a database.
about: Usually, creating a #TDBConnection object is done through a call to #LoadDatabase with an
appropriate dbtype parameter.<br>
End Rem
Type TDBConnection Abstract

	' the native handle
	Field handle:Byte Ptr
	
	Field _dbname:String
	Field _host:String
	Field _port:Int
	Field _user:String
	Field _password:String
	Field _options:String
	Field _server:String
	
	Field _isOpen:Int = False
	
	Field _lastError:TDatabaseError
	
	' actual implementation in the driver
	Function Create:TDBConnection(dbname:String = Null, host:String = Null, ..
		port:Int = Null, user:String = Null, password:String = Null, ..
		server:String = Null, options:String = Null) Abstract

	Method Init(dbname:String, host:String, port:Int, user:String, password:String, server:String, options:String)
		_dbname = dbname
		_host = host
		_port = port
		_user = user
		_password = password
		_options = options
		_server = server
	End Method
		
	Rem
	bbdoc: Closes the database connection.
	about: Check #hasError and #error for details of any problems.
	End Rem
	Method close() Abstract
	
	Rem
	bbdoc: Commits a database transaction.
	returns: True if successful.
	about: Calling this method is only valid for a previous call to #startTransaction.
	<p>Check #hasError and #error for details of any problems.</p>
	End Rem
	Method commit:Int() Abstract
	
	Rem
	bbdoc: Returns a list of table names for the current database.
	End Rem
	Method getTables:String[]() Abstract
	
	Rem
	bbdoc: Attempts to open a new database connection.
	returns: True if successful.
	about: Check #hasError and #error for details of any problems.
	End Rem
	Method open:Int(user:String = Null, pass:String = Null) Abstract

	Rem
	bbdoc: Rolls back a database transaction.
	returns: True if successful.
	about: Calling this method is only valid for a previous call to #startTransaction.
	<p>Check #hasError and #error for details of any problems.</p>
	End Rem
	Method rollback:Int() Abstract
	
	Rem
	bbdoc: Starts a database transaction.
	returns: True if successful.
	about: Once a transaction has started, it should be eventually closed with a call to either
	#rollback (if the transaction should be abandoned) or #commit (to save all database changes).
	<p>Check #hasError and #error for details of any problems.</p>
	End Rem
	Method startTransaction:Int() Abstract
	
	Rem
	bbdoc: Executes an sql statement.
	returns: A new #TDatabaseQuery object.
	about: Check #hasError and #error for details of any problems.
	End Rem
	Method executeQuery:TDatabaseQuery(sql:String)
		Local query:TDatabaseQuery = TDatabaseQuery.Create(Self)
		
		If sql And sql.length > 0 Then
			If query.execute(sql) Then
				' reset error...
				If _lasterror Then
					_lasterror.reset()
				End If
			End If
		End If
		
		Return query
	End Method

	Rem
	bbdoc: Determines if the database connection is open.
	returns: True if the connection is open.
	End Rem
	Method isOpen:Int()
		Return _isOpen
	End Method
	
	Rem
	bbdoc: Returns the database name.
	returns: The database name.
	End Rem
	Method getDatabaseName:String()
		Return _dbName
	End Method

	Rem
	bbdoc: Returns the connection host.
	returns: The host, or Null.
	about: Not all drivers require a Host.
	End Rem
	Method getHost:String()
		Return _dbName
	End Method

	Rem
	bbdoc: Returns the connection port number.
	returns: The port number, or 0.
	about: Not all drivers require a Port number.
	End Rem
	Method getPortNumber:Int()
		Return _port
	End Method

	Rem
	bbdoc: Returns the last database error.
	returns: A #TDatabaseError object.
	about: Will always return a valid #TDatabaseError object.
	End Rem
	Method error:TDatabaseError()
		If Not _lastError Then
			_lastError = New TDatabaseError
		End If
		
		Return _lastError
	End Method
	
	Rem
	bbdoc: Determines if there is an outstanding error.
	returns: True if there is an error.<br>
	Use #error to retrieve the #TDatabaseError object.
	End Rem
	Method hasError:Int()
		If _lastError Then
			Return _lastError.isSet()
		End If
		
		Return False
	End Method
	
	Method setError(error:String, append:String = Null, eType:Int, errorValue:Int = 0)
		If Not _lastError Then
			_lastError = TDatabaseError.Create(Self, error, append, eType, errorValue)
		Else
			_lastError.error = error
			
			If append Then
				_lastError.error:+ " : " + append
			End If
			
			_lastError.errorValue = errorValue
			_lastError.errorType = eType
		End If
	End Method

	Method databaseHandle:Byte Ptr() Abstract
	
	Method createResultSet:TQueryResultSet() Abstract
	
	Method nativeErrorMessage:String(err:Int) Abstract
	
	Rem
	bbdoc: Determines if the database has support for Prepare/Execute statements.
	returns: True if the driver supports Prepare/Execute statements.
	End Rem
	Method hasPrepareSupport:Int() Abstract

	Rem
	bbdoc: Determines if the database has transactioning support.
	returns: True if the driver supports transactions.
	End Rem
	Method hasTransactionSupport:Int() Abstract
	
	Method free()
		If _lastError Then
			_lastError = Null
		End If
	End Method
	
	Method Delete()
		free()
	End Method
	
End Type


Rem
bbdoc: A Query object for executing queries and navigating the result sets.
End Rem
Type TDatabaseQuery

	Field conn:TDBConnection
	
	Field resultSet:TQueryResultSet

	
	Rem
	bbdoc: Creates a new #TDatabaseQuery using the supplied @connection.
	End Rem
	Function Create:TDatabaseQuery(connection:TDBConnection)
		Local this:TDatabaseQuery = New TDatabaseQuery
		
		this.conn = connection
		
		Return this
	End Function

	Rem
	bbdoc: Prepares an SQL statement for execution.
	returns: True if the prepare succeeded.
	about: Check connection #hasError and #error for details of any problems.
	End Rem
	Method prepare:Int(statement:String)
		If Not resultSet Then
			resultSet = conn.createResultSet()
		Else
			resultSet.clear()
			resultSet._isActive = False

		End If

		If statement = Null Or statement.length = 0 Then

			conn.setError("Cannot prepare empty statement", Null, TDatabaseError.ERROR_STATEMENT)

			Return False
		End If

		resultSet.query = statement.Trim()
		
		If Not conn.isOpen() Then

			conn.setError("The connection is not open", Null, TDatabaseError.ERROR_CONNECTION)

			Return False
		End If
		
		If resultSet.prepare(statement) Then

			' on success, reset the last error.
			If conn._lasterror Then
				conn._lasterror.reset()
			End If

			Return True
		Else
			Return False
		End If

	End Method
	
	Rem
	bbdoc: Executes an SQL statement.
	returns: True if the execute succeeded.
	about: For a previously prepared statement, pass Null into this method.
	<p>Check connection #hasError and #error for details of any problems.</p>
	End Rem
	Method execute:Int(statement:String = Null)
	
		If statement Then
			If Not resultSet Then
				resultSet = conn.createResultSet()
			Else
				resultSet.clear()
				resultSet._isActive = False
	
			End If
	
			If statement.Trim().length = 0 Then
				
				conn.setError("Cannot execute empty statement", Null, TDatabaseError.ERROR_STATEMENT)

				Return False
			End If	
	
			resultSet.query = statement.Trim()			

			If Not conn.isOpen() Then

				conn.setError("The connection is not open", Null, TDatabaseError.ERROR_CONNECTION)

				Return False
			End If
			
			If resultSet.executeQuery(statement) Then

				' on success, reset the last error.
				If conn._lasterror Then
						conn._lasterror.reset()
				End If

				Return True
			Else
				Return False
			End If
			
		Else

			If Not conn.isOpen() Then

				conn.setError("The connection is not open", Null, TDatabaseError.ERROR_CONNECTION)

				Return False
			End If
			
			If resultSet.execute() Then

				' on success, reset the last error.
				If conn._lasterror Then
					conn._lasterror.reset()
				End If

				Return True
			Else
				Return False
			End If
		
		End If

		
	End Method
	
	Rem
	bbdoc: Returns the value of the field at @index.
	returns: A #TDBType object or Null.
	End Rem
	Method value:TDBType(index:Int)
		If isActive() And index > SQL_BeforeFirstRow Then
			Return resultSet.dataValue(index)
		End If
		
		Return Null
	End Method
	
	Rem
	bbdoc: Retrieves the next row in the result set.
	returns: True if a row was retrieved.
	about: Each call to this method populates a #TQueryRecord which can be
	retrieved via the #record method.
	<p>Check connection #hasError and #error for details of any problems.</p>
	End Rem
	Method nextRow:Int()
		If Not isActive() Then
			Return False
		End If
		
		Local result:Int
		
		Select rowIndex()
			Case SQL_BeforeFirstRow
				result = resultSet.firstRow()	
				Return result
			Case SQL_AfterLastRow
				Return False
			Default
				If Not resultSet.nextRow() Then
					resultSet.setRowIndex(SQL_AfterLastRow)
					' done with the resultset...
					resultSet.free()
					Return False
				End If
		End Select
		
		Return True
	End Method

	Method isActive:Int()
		Return resultSet.isActive()
	End Method

	Method rowIndex:Int()
		Return resultSet.rowIndex()
	End Method
	
	Rem
	bbdoc: Returns the record for the query.
	End Rem
	Method rowRecord:TQueryRecord()
		Local r:TQueryRecord =  resultSet.rowRecord()
		
		' if the resultSet is valid we can fill in the values.
		If resultSet.isValid() Then
			Local c:Int = r.count()
			
			For Local i:Int = 0 Until c
				r.setValue(i, value(i))
			Next
		End If
		
		Return r
	End Method
	
	Rem
	bbdoc: Binds a #TDBType value at the specified position.
	about: If a bind value already exists at the specified @position, it is replaced with the new one.
	End Rem
	Method bindValue(position:Int, value:TDBType)
		If resultSet Then
			resultSet.bindValue(position, value)
		End If
	End Method
	
	Rem
	bbdoc: Adds a new #TDBType bind value.
	about: The value is added to the end of the current list of bind values.
	End Rem
	Method addBindValue(value:TDBType)
		If resultSet Then
			resultSet.addBindValue(value)
		End If
	End Method

	Rem
	bbdoc: Binds the String @value at the specified @position.
	about: If a bind value already exists at the specified @position, it is replaced with the new one.
	End Rem
	Method setString(position:Int, value:String)
		bindValue(position, TDBString.Set(value))
	End Method

	Rem
	bbdoc: Binds the Int @value at the specified @position.
	about: If a bind value already exists at the specified @position, it is replaced with the new one.
	End Rem
	Method setInt(position:Int, value:Int)
		bindValue(position, TDBInt.Set(value))
	End Method

	Rem
	bbdoc: Binds the Long @value at the specified @position.
	about: If a bind value already exists at the specified @position, it is replaced with the new one.
	End Rem
	Method setLong(position:Int, value:Long)
		bindValue(position, TDBLong.Set(value))
	End Method

	Rem
	bbdoc: Binds the Float @value at the specified @position.
	about: If a bind value already exists at the specified @position, it is replaced with the new one.
	End Rem
	Method setFloat(position:Int, value:Float)
		bindValue(position, TDBFloat.Set(value))
	End Method

	Rem
	bbdoc: Binds the Double @value at the specified @position.
	about: If a bind value already exists at the specified @position, it is replaced with the new one.
	End Rem
	Method setDouble(position:Int, value:Double)
		bindValue(position, TDBDouble.Set(value))
	End Method

	Rem
	bbdoc: Adds a new String bind value.
	about: The value is added to the end of the current list of bind values.
	End Rem
	Method addString(value:String)
		addBindValue(TDBString.Set(value))
	End Method

	Rem
	bbdoc: Adds a new Int bind value.
	about: The value is added to the end of the current list of bind values.
	End Rem
	Method addInt(value:Int)
		addBindValue(TDBInt.Set(value))
	End Method

	Rem
	bbdoc: Adds a new Long bind value.
	about: The value is added to the end of the current list of bind values.
	End Rem
	Method addLong(value:Long)
		addBindValue(TDBLong.Set(value))
	End Method

	Rem
	bbdoc: Adds a new Float bind value.
	about: The value is added to the end of the current list of bind values.
	End Rem
	Method addFloat(value:Float)
		addBindValue(TDBFloat.Set(value))
	End Method

	Rem
	bbdoc: Adds a new Double bind value.
	about: The value is added to the end of the current list of bind values.
	End Rem
	Method addDouble(value:Double)
		addBindValue(TDBDouble.Set(value))
	End Method

	Rem
	bbdoc: Clears the query bind values.
	End Rem
	Method clearBindValues()
		If resultSet Then
			resultSet.clearBindValues()
		End If
	End Method
	
	Rem
	bbdoc: Returns the id of the last inserted row.
	about: Results returned from this method on anything other than an insert on a table with
	an auto-incrementing field, are undetermined.
	End Rem
	Method lastInsertedId:Long()
		If resultSet Then
			Return resultSet.lastInsertedId()
		End If
	End Method
	
	Rem
	bbdoc: Returns the number of rows affected by the previously executed statement.
	about: Only really useful for inserts, updates and deletes. That is, results on selects are
	undetermined.
	End Rem
	Method rowsAffected:Int()
		If resultSet Then
			Return resultSet.rowsAffected()
		End If
		Return -1
	End Method

	' "eachin" support
	Method ObjectEnumerator:TRowEnumerator()
		Local enum:TRowEnumerator = New TRowEnumerator
		enum.query = Self

		Return enum
	End Method

	Method free()
		If resultSet Then
			resultSet.free()
			resultSet = Null
		End If
		
		If conn Then
			conn = Null
		End If
	End Method

	Method Delete()
		free()
	End Method
	
End Type

' "eachin" support
Type TRowEnumerator
	Method HasNext:Int()
		Local result:Int = query.nextRow()
		If result Then
			record = query.rowRecord()
			If record._isEmptySet Then
				Return False
			End If
		End If
		Return result
	End Method
	
	Method NextObject:Object()
		Return record
	End Method
	
	Method Delete()
		query = Null
		record = Null
	End Method

	'***** PRIVATE *****
		
	Field query:TDatabaseQuery
	Field record:TQueryRecord
End Type

' Implementation specific result set.
' You probably shouldn't be using any of this type's methods directly...
Type TQueryResultSet

	Field conn:TDBConnection
	
	Field stmtHandle:Byte Ptr
	Field query:String
	
	Field _isActive:Int = False
	Field index:Int = SQL_BeforeFirstRow
	Field values:TDBType[]
	Field bindCount:Int
	Field boundValues:TDBType[]

	Field rec:TQueryRecord

	' actual implementation in the driver
	Function Create:TQueryResultSet(db:TDBConnection, sql:String = Null) Abstract
	
	Method Init(db:TDBConnection, sql:String)
		conn = db
		query = sql
	End Method
	
	Method clearBindValues()
		
		If boundValues Then
			For Local i:Int = 0 Until boundValues.length
				If boundValues[i] Then
					boundValues[i].clear()
					'boundValues[i] = Null
				End If
			Next
			'boundValues = Null
			bindCount = 0
		End If

		If values Then
			For Local i:Int = 0 Until values.length
				If values[i] Then
					values[i].clear()
					'values[i] = Null
				End If
			Next
			'values = Null
		End If
		
	End Method
	
	Method clear()
		clearBindValues()
	End Method
	
	Method executeQuery:Int(statement:String) Abstract
	
	Method prepare:Int(statement:String) Abstract
	Method execute:Int() Abstract
	
	Method firstRow:Int() Abstract
	Method nextRow:Int() Abstract
	
	Method lastInsertedId:Long() Abstract
	Method rowsAffected:Int() Abstract

	Method dbTypeFromNative:Int(name:String, _type:Int = 0, _flags:Int = 0) Abstract

	Method rowRecord:TQueryRecord()
		If Not isActive() Then
			Return TQueryRecord.Create()
		End If
		
		Return rec
	End Method

	Method dataValue:TDBType(index:Int)
		If isActive() And rec And Not rec.isEmpty() Then
			If index >= 0 And index < rec.count() Then
				Return values[index]
			End If
		End If
		
		Return Null
	End Method
	
	Method rowIndex:Int()
		Return index
	End Method
	
	Method setRowIndex(i:Int)
		index = i
	End Method

	Method isActive:Int()
		Return _isActive
	End Method

	Method isValid:Int()
		Return index <> SQL_BeforeFirstRow And index <> SQL_AfterLastRow
	End Method
	
	Method resetValues(size:Int)
		If values Then
			For Local i:Int = 0 Until values.length
				values[i].clear()
			Next
			values = Null
		End If
		
		values = New TDBType[size]
	End Method
	
	Method resetBindCount()
		bindCount = 0
	End Method

	Method addBindValue(value:TDBType)
		bindValue(bindCount, value)
	End Method
	
	Method bindValue(index:Int, value:TDBType)
		If Not boundValues Then
			boundValues = New TDBType[index + 1]
		End If
		
		' extend the array if required
		If boundValues.length <= index Then
			boundValues = boundValues[..index + 1]
		End If
		
		' amend bindCount if required
		If index > bindCount Then
			bindCount = index
		End If
		
		' bindCount represents the length, so if last index matches, need to increment it.
		If index = bindCount Then
			bindCount :+ 1
		End If
		
		' a value already exists here... remove it first.
		If boundValues[index] Then
			boundValues[index].clear()
			boundValues[index] = Null
		End If
		
		boundValues[index] = value
	End Method
	
	Method free()
		clear()
		values = Null
		boundValues = Null
		rec = Null
	End Method
	
	Method Delete()
		free()
	End Method
	
End Type

Rem
bbdoc: A specific record (or row) for a result set.
End Rem
Type TQueryRecord

	Field _empty:Int = True
	Field _isEmptySet:Int = False
	Field fields:TQueryField[]
	Field fieldsMap:TMap

	Function Create:TQueryRecord()
		Local this:TQueryRecord = New TQueryRecord
		
		Return this
	End Function
	
	Rem
	bbdoc: Returns the #TQueryField object at @index.
	End Rem
	Method getField:TQueryField(index:Int)
		Return fields[index]
	End Method
	
	Rem
	bbdoc: Returns the named #TQueryField object.
	End Rem
	Method getFieldByName:TQueryField(name:String)
		Return TQueryField(fieldsMap.valueForKey(name))
	End Method
	
	Rem
	bbdoc: The index (position) of the field @name in the record.
	returns: The field index, or -1 if not found.
	End Rem
	Method indexOf:Int(name:String)
		For Local i:Int = 0 Until fields.length
			If name = fields[i].name Then
				Return i
			End If
		Next
		
		Return -1
	End Method
	
	Rem
	bbdoc: A count of the number of fields in the record.
	returns: The field count.
	End Rem
	Method count:Int()
		Return fields.length
	End Method
	
	Method isEmpty:Int()
		Return _empty
	End Method
	
	Method setValue(index:Int, value:Object)
		If index >= 0 And index < fields.length Then
			fields[index].setValue(value)
		End If
	End Method
	
	Rem
	bbdoc: Returns the value of the field at @index
	returns: a #TDBType value object or Null.
	End Rem
	Method value:TDBType(index:Int)
		If fields Then
			If index >= 0 And index < fields.length Then
				Return fields[index].value
			End If
		End If
		Return Null
	End Method
	
	Rem
	bbdoc: Returns the string value at @index
	about: The result is undetermined if the value at @index is not a string field.
	End Rem
	Method getString:String(index:Int)
		Local v:TDBType = value(index)
		If v Then
			Return v.getString()
		End If
	End Method
	
	Rem
	bbdoc: Returns the string value for the field @name
	about: The result is undetermined if the value at @name is not a string field.
	End Rem
	Method getStringByName:String(name:String)
		Local f:TQueryField = getFieldByName(name)
		If f Then
			Return f.value.getString()
		End If
	End Method

	Rem
	bbdoc: Returns the int value at @index
	about: The result is undetermined if the value at @index is not an int field.
	End Rem
	Method getInt:Int(index:Int)
		Local v:TDBType = value(index)
		If v Then
			Return v.getInt()
		End If
	End Method

	Rem
	bbdoc: Returns the int value for the field @name
	about: The result is undetermined if the value at @name is not an int field.
	End Rem
	Method getIntByName:Int(name:String)
		Local f:TQueryField = getFieldByName(name)
		If f Then
			Return f.value.getInt()
		End If
	End Method

	Rem
	bbdoc: Returns the long value at @index
	about: The result is undetermined if the value at @index is not a long field.
	End Rem
	Method getLong:Long(index:Int)
		Local v:TDBType = value(index)
		If v Then
			Return v.getLong()
		End If
	End Method

	Rem
	bbdoc: Returns the long value for the field @name
	about: The result is undetermined if the value at @name is not a long field.
	End Rem
	Method getLongByName:Long(name:String)
		Local f:TQueryField = getFieldByName(name)
		If f Then
			Return f.value.getLong()
		End If
	End Method

	Rem
	bbdoc: Returns the float value at @index
	about: The result is undetermined if the value at @index is not a float field.
	End Rem
	Method getFloat:Float(index:Int)
		Local v:TDBType = value(index)
		If v Then
			Return v.getFloat()
		End If
	End Method

	Rem
	bbdoc: Returns the float value for the field @name
	about: The result is undetermined if the value at @name is not a float field.
	End Rem
	Method getFloatByName:Float(name:String)
		Local f:TQueryField = getFieldByName(name)
		If f Then
			Return f.value.getFloat()
		End If
	End Method

	Rem
	bbdoc: Returns the double value at @index
	about: The result is undetermined if the value at @index is not a double field.
	End Rem
	Method getDouble:Double(index:Int)
		Local v:TDBType = value(index)
		If v Then
			Return v.getDouble()
		End If
	End Method

	Rem
	bbdoc: Returns the double value for the field @name
	about: The result is undetermined if the value at @name is not a double field.
	End Rem
	Method getDoubleByName:String(name:String)
		Local f:TQueryField = getFieldByName(name)
		If f Then
			Return f.value.getDouble()
		End If
	End Method

	Method clear()
		If fields Then
			For Local i:Int = 0 Until fields.length
				fields[i].clear()
			Next
			fields = Null
			
			fieldsMap.clear()
			fieldsMap = Null
		End If
		_empty = True
		_isEmptySet = True
	End Method
	
	Method Init(size:Int)
		If fields Then
			clear()
		End If
		
		fields = New TQueryField[size]
		fieldsMap = New TMap
		_empty = False
		_isEmptySet = False
	End Method
	
	Method setField(index:Int, theField:TQueryField)
		If fields Then
			fields[index] = theField
			fieldsMap.insert(theField.name, theField)
		End If
	End Method
	
	Method setIsEmptySet()
		_isEmptySet = True
	End Method

	Method Delete()
		Clear()
	End Method
	
End Type

Rem
bbdoc: A field definition, including a value if part of a result set record.
End Rem
Type TQueryField

	Rem
	bbdoc: The field name
	End Rem
	Field name:String
	Rem
	bbdoc: Field type
	End Rem
	Field fType:Int
	Rem
	bbdoc: Field size.
	about: Dependent on the type of field. For a DBString field it would indicate number of characters.<br>
	A value of -1 means that this is undetermined by the database driver.
	End Rem
	Field length:Int = -1
	Rem
	bbdoc: Decimal precision.
	about: Only applicable for DBFloat and DBDouble field types.<br>
	A value of -1 means that this is undetermined by the database driver.
	End Rem
	Field precision:Int = -1
	Field value:TDBType
	Rem
	bbdoc: Whether this field is required or not.
	about: True if field is optional (can be NULL), False if field is required (NOT NULL).<br>
	A value of -1 means that this is undetermined by the database driver.
	End Rem
	Field nullable:Int = -1
	
	' driver field type
	Field dtype:Int
	Field dflags:Int
	
	Function Create:TQueryField(name:String, fType:Int)
		Local this:TQueryField = New TQueryField
		
		this.name = name
		this.fType = fType
		
		Return this
	End Function
	
	Method clear()
		value = Null
	End Method
	
	Method setValue(v:Object)
		If TDBType(v) Then
			value = TDBType(v)
		End If
	End Method
	
	Method Delete()
		clear()
	End Method
	
End Type

Rem
bbdoc: Contains details of the last error from the driver.
End Rem
Type TDatabaseError

	Const ERROR_NONE:Int = 0
	Const ERROR_TRANSACTION:Int = 1
	Const ERROR_CONNECTION:Int = 2
	Const ERROR_STATEMENT:Int = 3
	Const ERROR_UNKOWN:Int = 4

	Field db:TDBConnection
	Rem
	bbdoc: The error text.
	End Rem
	Field error:String
	Rem
	bbdoc: The type of error.
	about: Can be one of ERROR_NONE, ERROR_TRANSACTION, ERROR_CONNECTION, ERROR_STATEMENT or ERROR_UNKOWN.
	End Rem
	Field errorType:Int
	Rem
	bbdoc: The "native" error value.
	about: Refer to the specific database documentation for details.
	End Rem
	Field errorValue:Int

	Function Create:TDatabaseError(db:TDBConnection, error:String, append:String = Null, errorType:Int, errorValue:Int)
		Local this:TDatabaseError = New TDatabaseError
		
		this.db = db
		this.error = error
		
		If append Then
			this.error:+ " : " + append
		End If

		If errorValue Then
			this.error:+ " (" + errorValue + ") "
		End If
		
		this.errorValue = errorValue
		this.errorType = errorType
		
		Return this
	End Function
	
	Method reset()
		error = Null
		errorValue = 0
		errorType = 0
	End Method
	
	Rem
	bbdoc: Determines if there is an outstanding error.
	returns: True if this represents an error.
	End Rem
	Method isSet:Int()
		Return error <> Null And error.length > 0
	End Method
	
	Rem
	bbdoc: Returns the full error details.
	End Rem
	Method toString:String()
		Return "(" + errorType + ") " + error + " : " + nativeError()
	End Method
	
	Method nativeError:String()
		If db Then
			Return db.nativeErrorMessage(errorValue)
		End If
	End Method
	
	Method Delete()
		db = Null
	End Method
End Type

Extern
	Function _strlen:Int(s:Byte Ptr) = "strlen"
End Extern

' Convert from Max to UTF8
Function convertISO8859toUTF8:String(text:String)
	If Not text Then
		Return ""
	End If
	
	Local l:Int = text.length
	If l = 0 Then
		Return ""
	End If
	
	Local count:Int = 0
	Local s:Byte[] = New Byte[l * 3]
	
	For Local i:Int = 0 Until l
		Local char:Int = text[i]

		If char < 128 Then
			s[count] = char
			count:+ 1
			Continue
		Else If char<2048
			s[count] = char/64 | 192
			count:+ 1
			s[count] = char Mod 64 | 128
			count:+ 1
			Continue
		Else
			s[count] =  char/4096 | 224
			count:+ 1
			s[count] = char/64 Mod 64 | 128
			count:+ 1
			s[count] = char Mod 64 | 128
			count:+ 1
			Continue
		EndIf
		
	Next

	Return String.fromBytes(s, count)
End Function

' Convert from UTF8 to Max
Function convertUTF8toISO8859:String(s:Byte Ptr)

	Local l:Int = _strlen(s)

	Local b:Short[] = New Short[l]
	Local bc:Int = -1
	Local c:Int
	Local d:Int
	Local e:Int
	For Local i:Int = 0 Until l

		bc:+1
		c = s[i]
		If c<128 
			b[bc] = c
			Continue
		End If
		i:+1
		d=s[i]
		If c<224 
			b[bc] = (c-192)*64+(d-128)
			Continue
		End If
		i:+1
		e = s[i]
		If c < 240 
			b[bc] = (c-224)*4096+(d-128)*64+(e-128)
			If b[bc] = 8233 Then
				b[bc] = 10
			End If
			Continue
		End If
	Next

	Return String.fromshorts(b, bc + 1)
End Function

Function sizedUTF8toISO8859:String(s:Byte Ptr, size:Int)

	Local l:Int = size
	Local b:Short[] = New Short[l]
	Local bc:Int = -1
	Local c:Int
	Local d:Int
	Local e:Int
	For Local i:Int = 0 Until l

		c = s[i]
		If c = 0 Continue

		bc:+1
		If c<128
			b[bc] = c
			Continue
		End If
		i:+1
		d=s[i]
		If c<224 
			b[bc] = (c-192)*64+(d-128)
			Continue
		End If
		i:+1
		e = s[i]
		If c < 240 
			b[bc] = (c-224)*4096+(d-128)*64+(e-128)
			If b[bc] = 8233 Then
				b[bc] = 10
			End If
			Continue
		End If
	Next

	Return String.fromshorts(b, bc + 1)
End Function



Type TDatabaseLoader
	Field _type:String
	Field _succ:TDatabaseLoader

	Method LoadDatabase:TDBConnection( dbname:String = Null, host:String = Null, ..
		port:Int = Null, user:String = Null, password:String = Null, ..
		server:String = Null, options:String = Null ) Abstract

End Type


Private

Global _loaders:TDatabaseloader

Public

Function AddDatabaseLoader( loader:TDatabaseLoader )
	If loader._succ Return
	loader._succ = _loaders
	_loaders = loader
End Function

Rem
bbdoc: Loads a database engine of the specific @dbType.
about: Optionally, the function takes a set of parameters that can be used to connect to the
database at load time.<br>
See the specific database module documentation for correct @dbType name.
End Rem
Function LoadDatabase:TDBConnection( dbType:String, dbname:String = Null, host:String = Null, ..
		port:Int = Null, user:String = Null, password:String = Null, server:String = Null, ..
		options:String = Null )

	Local loader:TDatabaseLoader = _loaders
	
	While loader
		If loader._type = dbType Then
			Local db:TDBConnection = loader.LoadDatabase(dbname, host, port, user, password, server, options)
			If db Return db
		End If
		loader = loader._succ
	Wend

End Function
