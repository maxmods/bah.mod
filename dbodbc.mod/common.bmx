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

?linux
Import "source.bmx"
Import "src/include/*.h"
Import "-ldl"
?macos
Import "source.bmx"
Import "src/include/*.h"
?win32
Import "-lodbc32"
?

Import brl.blitz
Import "odbchelper.cpp"

Const SQL_AM_CONNECTION:Int = 1
Const SQL_AM_NONE:Int = 0
Const SQL_AM_STATEMENT:Int = 2
Const SQL_API_SQLALLOCHANDLE:Int = 1001
Const SQL_API_SQLBINDPARAM:Int = 1002
Const SQL_API_SQLCLOSECURSOR:Int = 1003
Const SQL_API_SQLCOLATTRIBUTE:Int = 6
Const SQL_API_SQLCOPYDESC:Int = 1004
Const SQL_API_SQLENDTRAN:Int = 1005
Const SQL_API_SQLFETCHSCROLL:Int = 1021
Const SQL_API_SQLFREEHANDLE:Int = 1006
Const SQL_API_SQLGETCONNECTATTR:Int = 1007
Const SQL_API_SQLGETDESCFIELD:Int = 1008
Const SQL_API_SQLGETDESCREC:Int = 1009
Const SQL_API_SQLGETDIAGFIELD:Int = 1010
Const SQL_API_SQLGETDIAGREC:Int = 1011
Const SQL_API_SQLGETENVATTR:Int = 1012
Const SQL_API_SQLGETSTMTATTR:Int = 1014
Const SQL_API_SQLSETCONNECTATTR:Int = 1016
Const SQL_API_SQLSETDESCFIELD:Int = 1017
Const SQL_API_SQLSETDESCREC:Int = 1018
Const SQL_API_SQLSETENVATTR:Int = 1019
Const SQL_API_SQLSETSTMTATTR:Int = 1020
Const SQL_ARD_TYPE:Int = -99
Const SQL_AT_ADD_CONSTRAINT:Int = 8
Const SQL_ATTR_APP_PARAM_DESC:Int = 10011
Const SQL_ATTR_APP_ROW_DESC:Int = 10010
Const SQL_ATTR_AUTO_IPD:Int = 10001
Const SQL_ATTR_CURSOR_SCROLLABLE:Int = -1
Const SQL_ATTR_CURSOR_SENSITIVITY:Int = -2
Const SQL_ATTR_IMP_PARAM_DESC:Int = 10013
Const SQL_ATTR_IMP_ROW_DESC:Int = 10012
Const SQL_ATTR_METADATA_ID:Int = 10014
Const SQL_ATTR_OUTPUT_NTS:Int = 10001
Const SQL_CATALOG_NAME:Int = 10003
Const SQL_CODE_DATE:Int = 1
Const SQL_CODE_TIME:Int = 2
Const SQL_CODE_TIMESTAMP:Int = 3
Const SQL_COLLATION_SEQ:Int = 10004
Const SQL_CURSOR_SENSITIVITY:Int = 10001
Const SQL_DATE_LEN:Int = 10
Const SQL_DATETIME:Int = 9
Const SQL_DEFAULT:Int = 99
Const SQL_DESC_ALLOC_AUTO:Int = 1
Const SQL_DESC_ALLOC_TYPE:Int = 1099
Const SQL_DESC_ALLOC_USER:Int = 2
Const SQL_DESC_COUNT:Int = 1001
Const SQL_DESC_DATA_PTR:Int = 1010
Const SQL_DESC_DATETIME_INTERVAL_CODE:Int = 1007
Const SQL_DESC_INDICATOR_PTR:Int = 1009
Const SQL_DESC_LENGTH:Int = 1003
Const SQL_DESC_NAME:Int = 1011
Const SQL_DESC_NULLABLE:Int = 1008
Const SQL_DESC_OCTET_LENGTH:Int = 1013
Const SQL_DESC_OCTET_LENGTH_PTR:Int = 1004
Const SQL_DESC_PRECISION:Int = 1005
Const SQL_DESC_SCALE:Int = 1006
Const SQL_DESC_TYPE:Int = 1002
Const SQL_DESC_UNNAMED:Int = 1012
Const SQL_DESCRIBE_PARAMETER:Int = 10002
Const SQL_DIAG_ALTER_DOMAIN:Int = 3
Const SQL_DIAG_ALTER_TABLE:Int = 4
Const SQL_DIAG_CALL:Int = 7
Const SQL_DIAG_CLASS_ORIGIN:Int = 8
Const SQL_DIAG_CONNECTION_NAME:Int = 10
Const SQL_DIAG_CREATE_ASSERTION:Int = 6
Const SQL_DIAG_CREATE_CHARACTER_SET:Int = 8
Const SQL_DIAG_CREATE_COLLATION:Int = 10
Const SQL_DIAG_CREATE_DOMAIN:Int = 23
Const SQL_DIAG_CREATE_INDEX:Int = -1
Const SQL_DIAG_CREATE_SCHEMA:Int = 64
Const SQL_DIAG_CREATE_TABLE:Int = 77
Const SQL_DIAG_CREATE_TRANSLATION:Int = 79
Const SQL_DIAG_CREATE_VIEW:Int = 84
Const SQL_DIAG_DELETE_WHERE:Int = 19
Const SQL_DIAG_DROP_ASSERTION:Int = 24
Const SQL_DIAG_DROP_CHARACTER_SET:Int = 25
Const SQL_DIAG_DROP_COLLATION:Int = 26
Const SQL_DIAG_DROP_DOMAIN:Int = 27
Const SQL_DIAG_DROP_INDEX:Int = -2
Const SQL_DIAG_DROP_SCHEMA:Int = 31
Const SQL_DIAG_DROP_TABLE:Int = 32
Const SQL_DIAG_DROP_TRANSLATION:Int = 33
Const SQL_DIAG_DROP_VIEW:Int = 36
Const SQL_DIAG_DYNAMIC_DELETE_CURSOR:Int = 38
Const SQL_DIAG_DYNAMIC_FUNCTION:Int = 7
Const SQL_DIAG_DYNAMIC_FUNCTION_CODE:Int = 12
Const SQL_DIAG_DYNAMIC_UPDATE_CURSOR:Int = 81
Const SQL_DIAG_GRANT:Int = 48
Const SQL_DIAG_INSERT:Int = 50
Const SQL_DIAG_MESSAGE_TEXT:Int = 6
Const SQL_DIAG_NATIVE:Int = 5
Const SQL_DIAG_NUMBER:Int = 2
Const SQL_DIAG_RETURNCODE:Int = 1
Const SQL_DIAG_REVOKE:Int = 59
Const SQL_DIAG_ROW_COUNT:Int = 3
Const SQL_DIAG_SELECT_CURSOR:Int = 85
Const SQL_DIAG_SERVER_NAME:Int = 11
Const SQL_DIAG_SQLSTATE:Int = 4
Const SQL_DIAG_SUBCLASS_ORIGIN:Int = 9
Const SQL_DIAG_UNKNOWN_STATEMENT:Int = 0
Const SQL_DIAG_UPDATE_WHERE:Int = 82

' handle types
Const SQL_HANDLE_DBC:Int = 2
Const SQL_HANDLE_DESC:Int = 4
Const SQL_HANDLE_ENV:Int = 1
Const SQL_HANDLE_STMT:Int = 3

Const SQL_INSENSITIVE:Int = 1
Const SQL_MAX_CONCURRENT_ACTIVITIES:Int = 1
Const SQL_MAX_DRIVER_CONNECTIONS:Int = 0
Const SQL_MAX_IDENTIFIER_LEN:Int = 10005
Const SQL_MAXIMUM_CONCURRENT_ACTIVITIES:Int = SQL_MAX_CONCURRENT_ACTIVITIES
Const SQL_MAXIMUM_DRIVER_CONNECTIONS:Int = SQL_MAX_DRIVER_CONNECTIONS
Const SQL_MAXIMUM_IDENTIFIER_LENGTH:Int = SQL_MAX_IDENTIFIER_LEN
Const SQL_NAMED:Int = 0
Const SQL_NO_DATA:Int = 100
Const SQL_NONSCROLLABLE:Int = 0
Const SQL_NULL_HANDLE:Int = 0
Const SQL_NULL_HDESC:Int = 0
Const SQL_OJ_CAPABILITIES:Int = 115
Const SQL_OUTER_JOIN_CAPABILITIES:Int = SQL_OJ_CAPABILITIES
Const SQL_PC_NON_PSEUDO:Int = 1
Const SQL_PRED_BASIC:Int = 2
Const SQL_PRED_CHAR:Int = 1
Const SQL_PRED_NONE:Int = 0
Const SQL_ROW_IDENTIFIER:Int = 1
Const SQL_SCROLLABLE:Int = 1
Const SQL_SENSITIVE:Int = 2
Const SQL_TIME_LEN:Int = 8
Const SQL_TIMESTAMP_LEN:Int = 19

' booleans
Const SQL_FALSE:Int = 0
Const SQL_TRUE:Int = 1

Const SQL_TYPE_DATE:Int = 91
Const SQL_TYPE_TIME:Int = 92
Const SQL_TYPE_TIMESTAMP:Int = 93
Const SQL_UNNAMED:Int = 1
Const SQL_UNSPECIFIED:Int = 0
Const SQL_XOPEN_CLI_YEAR:Int = 10000

Const SQL_UNKNOWN_TYPE:Int = 0
Const SQL_CHAR:Int = 1
Const SQL_NUMERIC:Int = 2
Const SQL_DECIMAL:Int = 3
Const SQL_INTEGER:Int = 4
Const SQL_SMALLINT:Int = 5
Const SQL_FLOAT:Int = 6
Const SQL_REAL:Int = 7
Const SQL_DOUBLE:Int = 8
Const SQL_ALL_TYPES:Int = 0
Const SQL_LONGVARCHAR:Int = -1
Const SQL_BINARY:Int = -2
Const SQL_VARBINARY:Int = -3
Const SQL_LONGVARBINARY:Int = -4
Const SQL_BIGINT:Int = -5
Const SQL_TINYINT:Int = -6
Const SQL_BIT:Int = -7
Const SQL_GUID:Int = -11
Const SQL_DATE:Int = 9
Const SQL_TIME:Int = 10
Const SQL_TIMESTAMP:Int = 11

' status values
Const SQL_SUCCESS:Int = 0
Const SQL_SUCCESS_WITH_INFO	:Int = 1
Const SQL_STILL_EXECUTING:Int = 2
Const SQL_ERROR:Int = -1
Const SQL_INVALID_HANDLE:Int = -2
Const SQL_NEED_DATA:Int = 99
'Const SQL_NO_DATA:Int = 100

Const SQL_NO_NULLS:Int = 0
Const SQL_NULLABLE:Int = 1
Const SQL_NULLABLE_UNKNOWN:Int = 2

Const SQL_NULL_DATA:Int = -1
Const SQL_DATA_AT_EXEC:Int = -2
Const SQL_NO_TOTAL:Int = -4

?win32
Extern "win32"
?linux
Extern
?macos
Extern
?
'	Function SQLAllocHandle:Int(handleType:Int, inputHandle:Byte Ptr, outputHandle:Byte Ptr)



End Extern

Extern
	Function bmx_odbc_newEnv:Byte Ptr()
	Function bmx_odbc_newConnection:Byte Ptr()
	Function bmx_odbc_newStatement:Byte Ptr()
	Function bmx_odbc_deleteHandle(handle:Byte Ptr)
	
	Function bmx_odbc_SQLAllocHandle:Int(_type:Int, a:Byte Ptr, handle:Byte Ptr)
	Function bmx_odbc_freeEnvHandle(handle:Byte Ptr)
	Function bmx_odbc_freeStmtHandle(handle:Byte Ptr)

	Function bmx_odbc_envError:Byte Ptr(envHandle:Byte Ptr, code:Int Ptr, size:Int Ptr)
	Function bmx_odbc_connError:Byte Ptr(envHandle:Byte Ptr, code:Int Ptr, size:Int Ptr)
	Function bmx_odbc_stmtError:Byte Ptr(envHandle:Byte Ptr, code:Int Ptr, size:Int Ptr)
	Function bmx_odbc_setattr_odbc3(envHandle:Byte Ptr)
	Function bmx_odbc_SQLDriverConnect:Int(handle:Byte Ptr, connectString:Byte Ptr, connLength:Int)
	Function bmx_odbc_toggleTransaction:Int(handle:Byte Ptr, toggle:Int)
	Function bmx_odbc_commitTransaction:Int(handle:Byte Ptr)
	Function bmx_odbc_rollbackTransaction:Int(handle:Byte Ptr)
	
	Function bmx_odbc_disconnectAndFree(handle:Byte Ptr)
	Function bmx_odbc_SQLRowCount:Int(stmt:Byte Ptr, num:Int Ptr)
	
	Function bmx_odbc_setForwardCursor:Int(stmt:Byte Ptr)
	Function bmx_odbc_execute:Int(stmt:Byte Ptr, query:Byte Ptr, size:Int)
	
	Function bmx_odbc_SQLNumResultCols(stmt:Byte Ptr, count:Int Ptr)
	Function bmx_odbc_SQLDescribeCol:Int(stmt:Byte Ptr, column:Int, columnName:Byte Ptr, bufferLength:Int, ..
		nameLength:Int Ptr, dataType:Int Ptr, columnSize:Int Ptr, decimalDigits:Int Ptr, ..
		nullable:Int Ptr)
	Function bmx_odbc_SQLFetchScroll:Int(stmt:Byte Ptr)
	
	Function bmx_odbc_SQLGetData_int:Int(stmt:Byte Ptr, index:Int, value:Int Ptr, indicator:Int Ptr)
	Function bmx_odbc_SQLGetData_long:Int(stmt:Byte Ptr, index:Int, value:Long Ptr, indicator:Int Ptr)
	Function bmx_odbc_SQLGetData_double:Int(stmt:Byte Ptr, index:Int, value:Double Ptr, indicator:Int Ptr)
	Function bmx_odbc_SQLGetData_string:Int(stmt:Byte Ptr, index:Int, buffer:Byte Ptr, bufferLength:Int, indicator:Int Ptr)
	Function bmx_odbc_SQLGetData_datetime:Int(stmt:Byte Ptr, index:Int, y:Int Ptr, m:Int Ptr, d:Int Ptr, hh:Int Ptr, mm:Int Ptr, ss:Int Ptr, indicator:Int Ptr)
	Function bmx_odbc_SQLGetData_date:Int(stmt:Byte Ptr, index:Int, y:Int Ptr, m:Int Ptr, d:Int Ptr, indicator:Int Ptr)
	Function bmx_odbc_SQLGetData_time:Int(stmt:Byte Ptr, index:Int, hh:Int Ptr, mm:Int Ptr, ss:Int Ptr, indicator:Int Ptr)
	
	Function bmx_odbc_prepare:Int(stmt:Byte Ptr, query:Byte Ptr, size:Int)
	Function bmx_odbc_executePrepared:Int(stmt:Byte Ptr)
	
	Function bmx_odbc_SQLBindParameter_int:Int(stmt:Byte Ptr, index:Int, value:Int Ptr, isNull:Int Ptr)
	Function bmx_odbc_SQLBindParameter_double:Int(stmt:Byte Ptr, index:Int, value:Double Ptr, isNull:Int Ptr)
	Function bmx_odbc_SQLBindParameter_long:Int(stmt:Byte Ptr, index:Int, value:Long Ptr, isNull:Int Ptr)
	Function bmx_odbc_SQLBindParameter_string:Int(stmt:Byte Ptr, index:Int, value:Byte Ptr, length:Int, isNull:Int Ptr)
	
	Function bmx_odbc_SQLTables:Int(stmt:Byte Ptr, _type:Byte Ptr, length:Int)
	
End Extern
