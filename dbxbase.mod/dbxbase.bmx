SuperStrict

Rem
bbdoc: Database Driver - Xbasease
about: An Xbase database driver for #bah.database.Database
End Rem
Module BaH.DBXbase

ModuleInfo "Version: 1.01"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: Libraries - LGPL"
ModuleInfo "Copyright: Wrapper - 2007,2008 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.01"
ModuleInfo "History: Fixed undefined uint on MacOS."
ModuleInfo "History: Fixed lots of compile errors on Win32."
ModuleInfo "History: 1.00 Initial Release"
ModuleInfo "History: Includes Xbase64 and XbSQL source."


Import BaH.Database

Import "common.bmx"


' Notes :
'
' xbase64.h       - Added missing _ to __GNU_LesserC__ for WIN32.
'
' xbsql.h         - Added uint define for MacOS
'                 - Many WIN32 fixes.
'
' xbsql.cpp       - Removed strcasecmp() decl for WIN32
'
' xb_tableset.cpp - WIN32 fixes.


Type TDBXbase Extends TDBConnection

	Function Create:TDBConnection(dbname:String = Null, host:String = Null, ..
		port:Int = Null, user:String = Null, password:String = Null, ..
		server:String = Null, options:String = Null)
		
		Local this:TDBXbase = New TDBXbase
		
		this.init(dbname, host, port, user, password, server, options)
		
		If this._dbname Then
			this.open(user, password)
		End If
		
		Return this
		
	End Function

	Method close()
	End Method
	
	Method commit:Int()
		Return True
	End Method
	
	Method getTables:String[]()
	End Method
	
	Method open:Int(user:String = Null, pass:String = Null)

		' close if the connection is already open
		If _isOpen Then
			close()
		End If

		handle = bmx_xbase_init(Self, _dbname)

		_isOpen = True
		Return True
	End Method

	Method rollback:Int()
		Return True
	End Method
	
	Method startTransaction:Int()
		Return True
	End Method
	
	Method databaseHandle:Byte Ptr()
		Return handle
	End Method
	
	Method createResultSet:TQueryResultSet()
		Return TXbaseResultSet.Create(Self)
	End Method
	
	Method nativeErrorMessage:String(err:Int)
		Return String.fromCString(bmx_xbase_lastError(handle))
	End Method
	
	Method hasPrepareSupport:Int()
		Return True
	End Method

	Method hasTransactionSupport:Int()
		Return False
	End Method

End Type

Type TXbaseResultSet Extends TQueryResultSet

	Function Create:TQueryResultSet(db:TDBConnection, sql:String = Null)
		Local this:TXbaseResultSet = New TXbaseResultSet
		
		this.init(db, sql)
		this.rec = TQueryRecord.Create()
		
		Return this
	End Function
	
	Method Delete()
		cleanup()
	End Method
	
	Method cleanup()
		If stmtHandle Then
			bmx_xbase_deleteStatement(stmtHandle)
			stmtHandle = Null
		End If
		
		index = SQL_BeforeFirstRow
	End Method

	Method executeQuery:Int(statement:String)
	
		If Not conn.isOpen() Then
			Return False
		End If
		
		' try an execCommand - for Create and Drop.
		' if this fails, try a query...
		Local result:Int = bmx_xbase_execCommand(conn.handle, statement)
		
		If Not result Then
		
			If conn.nativeErrorMessage(0) = "Unrecognised XBSQL SQL command" Then
			
				' must be a "query" type...
				stmtHandle = bmx_xbase_openQuery(conn.handle, statement)
				
				If Not stmtHandle Then
					conn.setError("Error executing statement", "", TDatabaseError.ERROR_STATEMENT, 0)				
					Return False
				End If
				
				If Not bmx_xbase_executeQuery(stmtHandle) Then
					conn.setError("Error executing statement", "", TDatabaseError.ERROR_STATEMENT, 0)				
					Return False
				End If

				
				Local fieldCount:Int = bmx_xbase_getNumFields(stmtHandle)

				initRecord(fieldCount)
	
				If fieldCount > 0 Then

					For Local i:Int = 0 Until fieldCount
						Local dtype:Int = dbTypeFromNative(Null, bmx_xbase_getFieldType(stmtHandle, i))
						
						Local qf:TQueryField = TQueryField.Create(convertUTF8toISO8859(bmx_xbase_getFieldName(stmtHandle, i)), dtype)
						qf.length = bmx_xbase_getFieldLength(stmtHandle, i)
						qf.dtype = dtype
						
						rec.setField(i, qf)
						
					Next
				
				End If


			Else
				conn.setError("Error executing statement", "", TDatabaseError.ERROR_STATEMENT, 0)				
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
	
	Method prepare:Int(statement:String)

		If Not conn.isOpen() Then
			Return False
		End If

		cleanup()
		
		If Not statement Or statement.length = 0 Then
			Return False
		End If
		
		' initialize the statement if required
		If Not stmtHandle Then
			stmtHandle = bmx_xbase_openQuery(conn.handle, statement)
		End If
		
		

			
	End Method
	
	Method execute:Int()
	
	End Method
	
	Method firstRow:Int()
		If index = SQL_BeforeFirstRow Then
			Return nextRow()
		End If
		
		Return False
	End Method
	
	Method nextRow:Int()
		If Not _isActive
			Return False
		End If

		index:+ 1
		
		If index = bmx_xbase_getNumRows(stmtHandle) Then
			Return False
		End If

		' now populate the values[] array with the fetched data !
		For Local i:Int = 0 Until rec.count()
		
			If values[i] Then
				values[i].clear()
			End If

			Select rec.fields[i].dType
				Case DBTYPE_INT
					values[i] = New TDBInt
					values[i].setInt(String.fromCString(bmx_xbase_getFieldText(stmtHandle, index, i)).toInt())
				Case DBTYPE_LONG
					values[i] = New TDBLong
					values[i].setLong(String.fromCString(bmx_xbase_getFieldText(stmtHandle, index, i)).toLong())
				Case DBTYPE_FLOAT
					values[i] = New TDBFloat
					values[i].setFloat(String.fromCString(bmx_xbase_getFieldText(stmtHandle, index, i)).toFloat())
				Case DBTYPE_DOUBLE
					values[i] = New TDBDouble
					values[i].setDouble(String.fromCString(bmx_xbase_getFieldText(stmtHandle, index, i)).toDouble())
				Case DBTYPE_DATE
					' TODO
				Case DBTYPE_BLOB
					' TODO
				Default
					values[i] = New TDBString
					values[i].setString(String.fromCString(bmx_xbase_getFieldText(stmtHandle, index, i)))
			End Select

		Next

		Return True
	End Method
	
	Method lastInsertedId:Long()
	End Method
	
	Method rowsAffected:Int()
	End Method

	Method dbTypeFromNative:Int(name:String, _type:Int = 0, _flags:Int = 0)

		Local dbType:Int
		
		Select _type
			Case 76, 78 ' L (bool),  N (int)
				dbType = DBTYPE_INT
			Case 70 ' F (double)
				dbType = DBTYPE_DOUBLE
			Case 68 ' D (date)
				dbType = DBTYPE_DATE
			Case 67 ' C (text)
				dbType = DBTYPE_STRING
			Case 77 ' M (memo)
				dbType = DBTYPE_BLOB
		End Select

		Return dbType
	End Method

End Type


Type TXbaseDatabaseLoader Extends TDatabaseLoader

	Method New()
		_type = "XBASE"
	End Method

	Method LoadDatabase:TDBConnection( dbname:String = Null, host:String = Null, ..
		port:Int = Null, user:String = Null, password:String = Null, ..
		server:String = Null, options:String = Null )
	
		Return TDBXbase.Create(dbName, host, port, user, password, server, options)
		
	End Method

End Type

AddDatabaseLoader New TXbaseDatabaseLoader
