' Copyright (c) 2007-2011 Bruce A Henderson
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
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Import BRL.Blitz

' source
Import "src/*.h"

Import "src/sqlite3.c"

?linux
Import "-ldl" ' links to the dynamic link library library
?

Import "sqlitehelper.cpp"

' Constants
Const SQLITE_OK:Int =           0   ' Successful result
Const SQLITE_ERROR:Int =        1   ' SQL error Or missing database
Const SQLITE_INTERNAL:Int =     2   ' An internal logic error in SQLite
Const SQLITE_PERM:Int =         3   ' Access permission denied
Const SQLITE_ABORT:Int =        4   ' Callback routine requested an abort
Const SQLITE_BUSY:Int =         5   ' The database file is locked
Const SQLITE_LOCKED:Int =       6   ' A table in the database is locked
Const SQLITE_NOMEM:Int =        7   ' A malloc() failed
Const SQLITE_READONLY:Int =     8   ' Attempt To write a readonly database
Const SQLITE_INTERRUPT:Int =    9   ' Operation terminated by sqlite_interrupt()
Const SQLITE_IOERR:Int =       10   ' Some kind of disk I/O error occurred
Const SQLITE_CORRUPT:Int =     11   ' The database disk image is malformed
Const SQLITE_NOTFOUND:Int =    12   ' (Internal Only) Table Or record Not found
Const SQLITE_FULL:Int =        13   ' Insertion failed because database is full
Const SQLITE_CANTOPEN:Int =    14   ' Unable To open the database file
Const SQLITE_PROTOCOL:Int =    15   ' Database lock protocol error
Const SQLITE_EMPTY:Int =       16   ' (Internal Only) Database table is empty
Const SQLITE_SCHEMA:Int =      17   ' The database schema changed
Const SQLITE_TOOBIG:Int =      18   ' Too much data For one row of a table
Const SQLITE_CONSTRAINT:Int =  19   ' Abort due To constraint violation
Const SQLITE_MISMATCH:Int =    20   ' Data Type mismatch
Const SQLITE_MISUSE:Int =      21   ' Library used incorrectly
Const SQLITE_NOLFS:Int =       22   ' Uses OS features Not supported on host
Const SQLITE_AUTH:Int =        23   ' Authorization denied
Const SQLITE_ROW:Int =         100  ' sqlite_step() has another row ready
Const SQLITE_DONE:Int =        101  ' sqlite_step() has finished executing

Const SQLITE_INTEGER:Int =  1
Const SQLITE_FLOAT:Int =    2
Const SQLITE_TEXT:Int =     3
Const SQLITE_BLOB:Int =     4
Const SQLITE_NULL:Int =     5

' Externs
Extern
	Function sqlite3_close:Int(handle:Byte Ptr)
	Function sqlite3_open:Int(dbname:Byte Ptr, handle:Byte Ptr)
	Function sqlite3_prepare_v2:Int(dbhandle:Byte Ptr, sql:Byte Ptr, size:Int, stmtHandle:Byte Ptr Ptr, pzTail:Int)
	Function sqlite3_finalize(stmtHandle:Byte Ptr)
	Function sqlite3_reset:Int(stmtHandle:Byte Ptr)
	Function sqlite3_step:Int(stmtHandle:Byte Ptr)
	Function sqlite3_column_count:Int(stmtHandle:Byte Ptr)
	Function sqlite3_column_name:Byte Ptr(stmtHandle:Byte Ptr, index:Int)
	Function sqlite3_column_decltype:Byte Ptr(stmtHandle:Byte Ptr, index:Int)
	Function sqlite3_column_type:Int(stmtHandle:Byte Ptr, index:Int)
	Function sqlite3_column_int:Int(stmtHandle:Byte Ptr, index:Int)
	Function sqlite3_column_int64:Long(stmtHandle:Byte Ptr, index:Int)
	Function sqlite3_column_double:Double(stmtHandle:Byte Ptr, index:Int)
	Function sqlite3_column_blob:Byte Ptr(stmtHandle:Byte Ptr, index:Int)
	Function sqlite3_column_bytes:Int(stmtHandle:Byte Ptr, index:Int)
	Function sqlite3_column_text:Byte Ptr(stmtHandle:Byte Ptr, index:Int)
	Function sqlite3_bind_parameter_count:Int(stmtHandle:Byte Ptr)
	Function sqlite3_bind_null:Int(stmtHandle:Byte Ptr, index:Int)
	Function sqlite3_bind_int:Int(stmtHandle:Byte Ptr, index:Int, value:Int)
	Function sqlite3_bind_int64:Int(stmtHandle:Byte Ptr, index:Int, value:Long)
	Function sqlite3_bind_double:Int(stmtHandle:Byte Ptr, index:Int, value:Double)
	Function sqlite3_bind_text:Int(stmtHandle:Byte Ptr, index:Int, value:Byte Ptr, size:Int, how:Int)
	Function sqlite3_bind_blob:Int(stmtHandle:Byte Ptr, index:Int, value:Byte Ptr, size:Int, how:Int)
	Function sqlite3_changes:Int(handle:Byte Ptr)
	Function sqlite3_errmsg:Byte Ptr(handle:Byte Ptr)
	Function sqlite3_backup_init:Byte Ptr(toHandle:Byte Ptr, dbTo:Byte Ptr, fromHandle:Byte Ptr, dbFrom:Byte Ptr)
	Function sqlite3_backup_step:Int(handle:Byte Ptr, page:Int)
	Function sqlite3_backup_finish:Int(handle:Byte Ptr)
	Function sqlite3_errcode:Int(handle:Byte Ptr)

	Function bmx_sqlite3_column_int64(stmtHandle:Byte Ptr, index:Int, value:Long Ptr)
	Function bmx_sqlite3_last_insert_rowid(handle:Byte Ptr, id:Long Ptr)

End Extern

