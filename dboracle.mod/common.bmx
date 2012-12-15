' Copyright (c) 2008-2012 Bruce A Henderson
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

Import BRL.Blitz

?macos
Import "-locilib"
?win32
Import "-locilibm"
?linux
Import "-locilib"
?

Import "include/*.h"

Import "oracleglueoci.cpp"

Extern

	Function bmx_ora_environment_createEnvironment:Byte Ptr()
	Function bmx_ora_environment_createConnection:Byte Ptr(envHandle:Byte Ptr, user:String, pass:String, connect:String)
	Function bmx_ora_environment_terminateConnection(envHandle:Byte Ptr, handle:Byte Ptr)
	Function bmx_ora_environment_terminateEnvironment(envHandle:Byte Ptr)

	Function bmx_ora_connection_commit(handle:Byte Ptr)
	Function bmx_ora_connection_rollback(handle:Byte Ptr)
	Function bmx_ora_connection_createStatement:Byte Ptr(conn:Byte Ptr)

	Function bmx_ora_statement_closeResultSet(stmtHandle:Byte Ptr, resultSetHandle:Byte Ptr)
	Function bmx_ora_statement_execute:Int(stmtHandle:Byte Ptr, statement:String)
	Function bmx_ora_statement_getAutoCommit:Int(stmtHandle:Byte Ptr)
	Function bmx_ora_statement_setAutoCommit(stmtHandle:Byte Ptr, autoCommit:Int)
	Function bmx_ora_statement_getUpdateCount:Int(stmtHandle:Byte Ptr)
	Function bmx_ora_statement_getResultSet:Byte Ptr(stmtHandle:Byte Ptr)
	Function bmx_ora_statement_free:Int(stmtHandle:Byte Ptr)
	Function bmx_ora_statement_prepare:Int(stmtHandle:Byte Ptr, statement:String)
	Function bmx_ora_statement_executeprepared:Int(stmtHandle:Byte Ptr)

	Function bmx_ora_resultset_getColCount:Int(resultSetHandle:Byte Ptr)
	Function bmx_ora_resultset_getColInfo:String(resultSetHandle:Byte Ptr, index:Int, dataType:Int Ptr, columnSize:Int Ptr, ..
			precision:Int Ptr, scale:Int Ptr, nullable:Int Ptr)
	Function bmx_ora_resultset_next:Int(resultSetHandle:Byte Ptr)
	Function bmx_ora_resultset_isNull:Int(resultSetHandle:Byte Ptr, index:Int)
	Function bmx_ora_resultset_getInt:Int(resultSetHandle:Byte Ptr, index:Int)
	'Function bmx_ora_resultset_getFloat:Float(resultSetHandle:Byte Ptr, index:Int)
	Function bmx_ora_resultset_getDouble:Double(resultSetHandle:Byte Ptr, index:Int)
	Function bmx_ora_resultset_getString:String(resultSetHandle:Byte Ptr, index:Int)
	Function bmx_ora_resultset_getBlob:Byte Ptr(resultSetHandle:Byte Ptr, index:Int)
	Function bmx_ora_resultset_getLong(resultSetHandle:Byte Ptr, index:Int, value:Long Ptr)
	Function bmx_ora_resultset_getDatetime(resultSetHandle:Byte Ptr, index:Int, y:Int Ptr, m:Int Ptr, d:Int Ptr, hh:Int Ptr, mm:Int Ptr, ss:Int Ptr)

	Function bmx_ora_bind_int:Int(stmtHandle:Byte Ptr, name:Byte Ptr, value:Int Ptr)
	Function bmx_ora_bind_double:Int(stmtHandle:Byte Ptr, name:Byte Ptr, value:Double Ptr)
	Function bmx_ora_bind_string:Int(stmtHandle:Byte Ptr, name:Byte Ptr, str:Short Ptr, length:Int)
	Function bmx_ora_bind_setnull:Int(stmtHandle:Byte Ptr, index:Int)
	Function bmx_ora_bind_long:Int(stmtHandle:Byte Ptr, name:Byte Ptr, value:Long Ptr)
	Function bmx_ora_bind_blob:Int(stmtHandle:Byte Ptr, name:Byte Ptr, blob:Byte Ptr, data:Byte Ptr, length:Int)
	Function bmx_ora_bind_date:Int(stmtHandle:Byte Ptr, name:Byte Ptr, date:Byte Ptr, y:Int, m:Int, d:Int)
	Function bmx_ora_bind_datetime:Int(stmtHandle:Byte Ptr, name:Byte Ptr, date:Byte Ptr, y:Int, m:Int, d:Int, hh:Int, mm:Int, ss:Int)
	Function bmx_ora_bind_time:Int(stmtHandle:Byte Ptr, name:Byte Ptr, date:Byte Ptr, hh:Int, mm:Int, ss:Int)

	Function bmx_ora_blob_create:Byte Ptr(conn:Byte Ptr)
	Function bmx_ora_blob_free(handle:Byte Ptr)
	Function bmx_ora_blob_getdata(handle:Byte Ptr, data:Byte Ptr)
	Function bmx_ora_blob_length:Int(handle:Byte Ptr)

	Function bmx_ora_date_create:Byte Ptr(conn:Byte Ptr)
	Function bmx_ora_date_free(handle:Byte Ptr)

End Extern


Const STATEMENT_UNPREPARED:Int = 0
Const STATEMENT_PREPARED:Int = 1
Const STATEMENT_RESULT_SET_AVAILABLE:Int = 2
Const STATEMENT_UPDATE_COUNT_AVAILABLE:Int = 3
Const STATEMENT_NEEDS_STREAM_DATA:Int = 4
Const STATEMENT_STREAM_DATA_AVAILABLE:Int = 5


Const RESULTSET_END_OF_FETCH:Int = 0
Const RESULTSET_DATA_AVAILABLE:Int = 1
Const RESULTSET_STREAM_DATA_AVAILABLE:Int = 2

Const OCI_CDT_NUMERIC:Int = 1        
Const OCI_CDT_DATETIME:Int = 3        
Const OCI_CDT_TEXT:Int = 4        
Const OCI_CDT_LONG:Int = 5        
Const OCI_CDT_CURSOR:Int = 6        
Const OCI_CDT_LOB:Int = 7    
Const OCI_CDT_FILE:Int = 8
Const OCI_CDT_TIMESTAMP:Int = 9
Const OCI_CDT_INTERVAL:Int = 10
Const OCI_CDT_RAW:Int = 11
Const OCI_CDT_OBJECT:Int = 12
Const OCI_CDT_COLLECTION:Int = 13
Const OCI_CDT_REF:Int = 14


Const OCCI_SQLT_CHR:Int = 1
Const OCCI_SQLT_NUM:Int = 2
Const OCCIINT:Int = 3
Const OCCIFLOAT:Int = 4
Const OCCIBFLOAT:Int = 21
Const OCCIBDOUBLE:Int = 22
Const OCCIIBFLOAT:Int = 100
Const OCCIIBDOUBLE:Int = 101
Const OCCI_SQLT_STR:Int = 5
Const OCCI_SQLT_VNU:Int = 6
Const OCCI_SQLT_PDN:Int = 7
Const OCCI_SQLT_LNG:Int = 8
Const OCCI_SQLT_VCS:Int = 9
Const OCCI_SQLT_NON:Int = 10
Const OCCI_SQLT_RID:Int = 11
Const OCCI_SQLT_DAT:Int = 12
Const OCCI_SQLT_VBI:Int = 15
Const OCCI_SQLT_BIN:Int = 23
Const OCCI_SQLT_LBI:Int = 24
Const OCCIUNSIGNED_INT:Int = 68
Const OCCI_SQLT_SLS:Int = 91
Const OCCI_SQLT_LVC:Int = 94
Const OCCI_SQLT_LVB:Int = 95
Const OCCI_SQLT_AFC:Int = 96
Const OCCI_SQLT_AVC:Int = 97
Const OCCI_SQLT_CUR:Int = 102
Const OCCI_SQLT_RDD:Int = 104
Const OCCI_SQLT_LAB:Int = 105
Const OCCI_SQLT_OSL:Int = 106
Const OCCI_SQLT_NTY:Int = 108
Const OCCI_SQLT_REF:Int = 110
Const OCCI_SQLT_CLOB:Int = 112
Const OCCI_SQLT_BLOB:Int = 113
Const OCCI_SQLT_BFILEE:Int = 114
Const OCCI_SQLT_CFILEE:Int = 115
Const OCCI_SQLT_RSET:Int = 116
Const OCCI_SQLT_NCO:Int = 122
Const OCCI_SQLT_VST:Int = 155
Const OCCI_SQLT_ODT:Int = 156
Const OCCI_SQLT_DATE:Int = 184
Const OCCI_SQLT_TIME:Int = 185
Const OCCI_SQLT_TIME_TZ:Int = 186
Const OCCI_SQLT_TIMESTAMP:Int = 187
Const OCCI_SQLT_TIMESTAMP_TZ:Int = 188
Const OCCI_SQLT_INTERVAL_YM:Int = 189
Const OCCI_SQLT_INTERVAL_DS:Int = 190
Const OCCI_SQLT_TIMESTAMP_LTZ:Int = 232
Const OCCI_SQLT_FILE:Int = 114
Const OCCI_SQLT_CFILE:Int = 115
Const OCCI_SQLT_BFILE:Int = 114
Const OCCICHAR:Int = 32768
Const OCCIDOUBLE:Int = 32769
Const OCCIBOOL:Int = 32770
Const OCCIANYDATA:Int = 32771
Const OCCINUMBER:Int = 32772
Const OCCIBLOB:Int = 32773
Const OCCIBFILE:Int = 32774
Const OCCIBYTES:Int = 32775
Const OCCICLOB:Int = 32776
Const OCCIVECTOR:Int = 32777
Const OCCIMETADATA:Int = 32778
Const OCCIPOBJECT:Int = 32779
Const OCCIREF:Int = 32780
Const OCCIREFANY:Int = 32781
Const OCCISTRING:Int = 32782
Const OCCISTREAM:Int = 32783
Const OCCIDATE:Int = 32784
Const OCCIINTERVALDS:Int = 32785
Const OCCIINTERVALYM:Int = 32786
Const OCCITIMESTAMP:Int = 32787
Const OCCIROWID:Int = 32788
Const OCCICURSOR:Int = 32789

