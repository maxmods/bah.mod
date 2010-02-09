' Copyright (c) 2007-2010 Bruce A Henderson
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

?linux
Import "-L/usr/lib/mysql"
Import "-lmysqlclient"
Import "/usr/include/mysql/*.h"
?win32
Import "-lmysql"
Import "include/*.h"
?macos
Import "-L/usr/local/mysql/lib"
Import "-lmysqlclient"
Import "-lz"
Import "/usr/local/mysql/include/*.h"
?

Import "mysqlhelper.cpp"

Const MYSQL_TYPE_DECIMAL:Int = 0
Const MYSQL_TYPE_TINY:Int = 1
Const MYSQL_TYPE_SHORT:Int = 2 
Const MYSQL_TYPE_LONG:Int = 3
Const MYSQL_TYPE_FLOAT:Int = 4 
Const MYSQL_TYPE_DOUBLE:Int = 5
Const MYSQL_TYPE_NULL:Int = 6
Const MYSQL_TYPE_TIMESTAMP:Int = 7
Const MYSQL_TYPE_LONGLONG:Int = 8
Const MYSQL_TYPE_INT24:Int = 9
Const MYSQL_TYPE_DATE:Int = 10
Const MYSQL_TYPE_TIME:Int = 11
Const MYSQL_TYPE_DATETIME:Int = 12
Const MYSQL_TYPE_YEAR:Int = 13
Const MYSQL_TYPE_NEWDATE:Int = 14
Const MYSQL_TYPE_VARCHAR:Int = 15
Const MYSQL_TYPE_BIT:Int = 16
Const MYSQL_TYPE_NEWDECIMAL:Int = 246
Const MYSQL_TYPE_ENUM:Int = 247
Const MYSQL_TYPE_SET:Int = 248
Const MYSQL_TYPE_TINY_BLOB:Int = 249
Const MYSQL_TYPE_MEDIUM_BLOB:Int = 250
Const MYSQL_TYPE_LONG_BLOB:Int = 251
Const MYSQL_TYPE_BLOB:Int = 252
Const MYSQL_TYPE_VAR_STRING:Int = 253
Const MYSQL_TYPE_STRING:Int = 254
Const MYSQL_TYPE_GEOMETRY:Int = 255


?win32
Extern "win32"
?linux
Extern
?macos
Extern
?
	Function mysql_close(handle:Byte Ptr)
	Function mysql_ping:Int(handle:Byte Ptr)
	Function mysql_query:Int(handle:Byte Ptr, query:Byte Ptr)
	Function mysql_errno:Int(handle:Byte Ptr)
	Function mysql_error:Byte Ptr(handle:Byte Ptr)
	Function mysql_init:Byte Ptr(h:Int = 0)
	Function mysql_get_client_version:Int()
	Function mysql_real_connect:Int(handle:Byte Ptr, host:Byte Ptr, user:Byte Ptr, ..
		passwd:Byte Ptr, db:Byte Ptr, port:Int, unix_socket:Byte Ptr, client_flag:Int)
	Function mysql_select_db:Int(handle:Byte Ptr, db:Byte Ptr)
	Function mysql_set_character_set(handle:Byte Ptr, text:Byte Ptr)
	Function mysql_get_server_version:Int(handle:Byte Ptr)
	Function mysql_real_query:Int(handle:Byte Ptr, query:Byte Ptr, size:Int)
	Function mysql_store_result:Byte Ptr(handle:Byte Ptr)
	Function mysql_field_count:Int(handle:Byte Ptr)
	Function mysql_fetch_field_direct:Byte Ptr(handle:Byte Ptr, index:Int)
	Function mysql_stmt_fetch:Int(stmt:Byte Ptr)
	Function mysql_fetch_row:Byte Ptr(stmt:Byte Ptr)
	Function mysql_free_result(result:Byte Ptr)

	Function mysql_stmt_init:Byte Ptr(handle:Byte Ptr)
	Function mysql_stmt_prepare:Int(stmt:Byte Ptr, query:Byte Ptr, size:Int)
	Function mysql_stmt_param_count:Int(stmt:Byte Ptr)
	Function mysql_stmt_execute:Int(stmt:Byte Ptr)
	Function mysql_stmt_error:Byte Ptr(stmt:Byte Ptr)
	Function mysql_stmt_result_metadata:Byte Ptr(stmt:Byte Ptr)
	
	Function mysql_num_fields:Int(meta:Byte Ptr)
	Function mysql_fetch_lengths:Byte Ptr(result:Byte Ptr)
	Function mysql_fetch_field:Byte Ptr(meta:Byte Ptr)
	Function mysql_stmt_store_result:Int(stmt:Byte Ptr)
	
	Function mysql_list_tables:Byte Ptr(handle:Byte Ptr, name:Byte Ptr)
End Extern

Extern
	Function bmx_mysql_field_name:Byte Ptr(f:Byte Ptr)
	Function bmx_mysql_field_org_name:Byte Ptr(f:Byte Ptr)
	Function bmx_mysql_field_table:Byte Ptr(f:Byte Ptr)
	Function bmx_mysql_field_org_table:Byte Ptr(f:Byte Ptr)
	Function bmx_mysql_field_db:Byte Ptr(f:Byte Ptr)
	Function bmx_mysql_field_catalog:Byte Ptr(f:Byte Ptr)
	Function bmx_mysql_field_def:Byte Ptr(f:Byte Ptr)
	Function bmx_mysql_field_length:Int(f:Byte Ptr)
	Function bmx_mysql_field_max_length:Int(f:Byte Ptr)
	Function bmx_mysql_field_flags:Int(f:Byte Ptr)
	Function bmx_mysql_field_type:Int(f:Byte Ptr)
	Function bmx_mysql_field_decimals:Int(f:Byte Ptr)
	
	Function bmx_mysql_makeBindings:Byte Ptr(size:Int)
	Function bmx_mysql_deleteBindings(bindings:Byte Ptr)
	'Function bmx_mysql_makeBools:Byte Ptr(size:Int)
	Function bmx_mysql_setBool(bools:Byte Ptr, index:Int, isNull:Int)
	Function bmx_mysql_deleteBools(bools:Byte Ptr)
	
	Function bmx_mysql_bind_int(params:Byte Ptr, index:Int, value:Int Ptr)
	Function bmx_mysql_bind_float(params:Byte Ptr, index:Int, value:Float Ptr)
	Function bmx_mysql_bind_double(params:Byte Ptr, index:Int, value:Double Ptr)
	Function bmx_mysql_bind_long(params:Byte Ptr, index:Int, value:Long Ptr)
	Function bmx_mysql_bind_string(params:Byte Ptr, index:Int, value:Byte Ptr, size:Int)
	Function bmx_mysql_bind_date(params:Byte Ptr, index:Int, value:Byte Ptr, _year:Int, _month:Int, _day:Int)
	Function bmx_mysql_bind_time(params:Byte Ptr, index:Int, value:Byte Ptr, _hour:Int, _min:Int, _sec:Int)
	Function bmx_mysql_bind_datetime(params:Byte Ptr, index:Int, value:Byte Ptr, _year:Int, _month:Int, _day:Int, _hour:Int, _min:Int, _sec:Int)
	Function bmx_mysql_bind_blob(params:Byte Ptr, index:Int, value:Byte Ptr, size:Int)
	
	Function bmx_mysql_makeVals:Byte Ptr(size:Int)
	Function bmx_mysql_deleteVals(vals:Byte Ptr)
	
	Function examine_bindings(params:Byte Ptr, size:Int, stmt:Byte Ptr)
	Function bmx_mysql_getLength:Int(lengths:Byte Ptr, index:Int)
	Function bmx_mysql_rowField_isNull:Int(row:Byte Ptr, index:Int)
	Function bmx_mysql_rowField_chars:Byte Ptr(row:Byte Ptr, index:Int)
	
	Function bmx_mysql_inbind(params:Byte Ptr, index:Int, _field:Byte Ptr, dataValue:Byte Ptr, dataLength:Int Ptr, isNull:Int Ptr)
	Function bmx_mysql_stmt_bind_result:Int(stmt:Byte Ptr, bindings:Byte Ptr)
	Function bmx_mysql_stmt_fetch:Int(stmt:Byte Ptr)
	
	Function bmx_mysql_stmt_bind_param:Int(stmt:Byte Ptr, bindings:Byte Ptr)
	Function bmx_mysql_getBindings:Byte Ptr(stmt:Byte Ptr)
	Function bmx_mysql_getParams:Byte Ptr(stmt:Byte Ptr)

	Function bmx_mysql_stmt_insert_id(stmt:Byte Ptr, id:Long Ptr)
	Function bmx_mysql_insert_id(handle:Byte Ptr, id:Long Ptr)
	
	Function bmx_mysql_makeTime:Byte Ptr()
	Function bmx_mysql_deleteTime(handle:Byte Ptr)
	
	Function bmx_mysql_stmt_close:Int(stmt:Byte Ptr)
	Function bmx_mysql_affected_rows(handle:Byte Ptr, rows:Long Ptr)
	Function bmx_mysql_stmt_reset:Int(stmt:Byte Ptr)
	Function bmx_mysql_stmt_affected_rows(stmt:Byte Ptr, rows:Long Ptr)

End Extern


Const CLIENT_FOUND_ROWS:Int = 2
Const CLIENT_NO_SCHEMA:Int = 16
Const CLIENT_COMPRESS:Int = 32
Const CLIENT_LOCAL_FILES:Int = 128
Const CLIENT_IGNORE_SPACE:Int = 256
Const CLIENT_INTERACTIVE:Int = 1024
Const CLIENT_SSL:Int = 2048
Const CLIENT_IGNORE_SIGPIPE:Int = 4096
Const CLIENT_MULTI_STATEMENTS:Int = 1 Shl 16
Const CLIENT_MULTI_RESULTS:Int = 1 Shl 17
Const CLIENT_REMEMBER_OPTIONS:Int = 1 Shl 31


