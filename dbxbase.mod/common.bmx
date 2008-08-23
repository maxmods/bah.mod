SuperStrict


Import "src/*.h"
Import "src/xbsql/*.h"

Import "xbasehelper.cpp"


' xbase
Import "src/xbase64/xbdbf.cpp"
Import "src/xbase64/xbexp.cpp"
Import "src/xbase64/xbexpfnc.cpp"
Import "src/xbase64/xbexpprc.cpp"
Import "src/xbase64/xbfields.cpp"
Import "src/xbase64/xbmemo.cpp"
Import "src/xbase64/xbndx.cpp"
Import "src/xbase64/xbase64.cpp"
Import "src/xbase64/xbdate.cpp"
Import "src/xbase64/xbstring.cpp"
Import "src/xbase64/xbindex.cpp"
Import "src/xbase64/xbntx.cpp"
Import "src/xbase64/xbfilter.cpp"
Import "src/xbase64/xblock.cpp"
Import "src/xbase64/xbfile.cpp"
Import "src/xbase64/xbcdx.cpp"
Import "src/xbase64/xbnode.cpp"

' xbsql
Import "src/xbsql/xb_value.cpp"
Import "src/xbsql/xb_elist.cpp"
Import "src/xbsql/xb_enode.cpp"
Import "src/xbsql/xb_queryset.cpp"
Import "src/xbsql/xb_tablelist.cpp"
Import "src/xbsql/xb_select.cpp"
Import "src/xbsql/xb_table.cpp"
Import "src/xbsql/xbsql.cpp"
Import "src/xbsql/xb_tableset.cpp"
Import "src/xbsql/xb_fieldset.cpp"
Import "src/xbsql/xb_query.cpp"
Import "src/xbsql/xb_insert.cpp"
Import "src/xbsql/xb_fieldlist.cpp"
Import "src/xbsql/xb_field.cpp"
Import "src/xbsql/xb_update.cpp"
Import "src/xbsql/xb_delete.cpp"
Import "src/xbsql/xb_assignlist.cpp"
Import "src/xbsql/xb_create.cpp"
Import "src/xbsql/xbsql.tab.cpp"
Import "src/xbsql/xb_datetime.cpp"

Extern
	Function bmx_xbase_init:Byte Ptr(obj:Object, dir:Byte Ptr)
	Function bmx_xbase_execCommand:Int(handle:Byte Ptr, statement:Byte Ptr)
	
	Function bmx_xbase_openQuery:Byte Ptr(handle:Byte Ptr, statement:Byte Ptr)
	Function bmx_xbase_executeQuery:Int(stmtHandle:Byte Ptr)
	Function bmx_xbase_getNumFields:Int(stmtHandle:Byte Ptr)
	Function bmx_xbase_getFieldType:Int(stmtHandle:Byte Ptr, index:Int)
	Function bmx_xbase_getFieldName:Byte Ptr(stmtHandle:Byte Ptr, index:Int)
	Function bmx_xbase_getFieldLength:Int(stmtHandle:Byte Ptr, index:Int)
	Function bmx_xbase_getFieldText:Byte Ptr(stmtHandle:Byte Ptr, row:Int, index:Int)
	Function bmx_xbase_getNumRows:Int(stmtHandle:Byte Ptr)
	Function bmx_xbase_deleteStatement(stmtHandle:Byte Ptr)
	
	Function bmx_xbase_lastError:Byte Ptr(handle:Byte Ptr)
End Extern


Const XB_NO_ERROR:Int = 0
Const XB_EOF:Int = -100
Const XB_BOF:Int = -101
Const XB_NO_MEMORY:Int = -102
Const XB_FILE_EXISTS:Int = -103
Const XB_OPEN_ERROR:Int = -104
Const XB_WRITE_ERROR:Int = -105
Const XB_UNKNOWN_FIELD_TYPE:Int = -106
Const XB_ALREADY_OPEN:Int = -107
Const XB_NOT_XBASE:Int = -108
Const XB_INVALID_RECORD:Int = -109
Const XB_INVALID_OPTION:Int = -110
Const XB_NOT_OPEN:Int = -111
Const XB_SEEK_ERROR:Int = -112
Const XB_READ_ERROR:Int = -113
Const XB_NOT_FOUND:Int = -114
Const XB_FOUND:Int = -115
Const XB_INVALID_KEY:Int = -116
Const XB_INVALID_NODELINK:Int = -117
Const XB_KEY_NOT_UNIQUE:Int = -118
Const XB_INVALID_KEY_EXPRESSION:Int = -119
Const XB_DBF_FILE_NOT_OPEN:Int = -120
Const XB_INVALID_KEY_TYPE:Int = -121
Const XB_INVALID_NODE_NO:Int = -122
Const XB_NODE_FULL:Int = -123
Const XB_INVALID_FIELDNO:Int = -124
Const XB_INVALID_DATA:Int = -125
Const XB_NOT_LEAFNODE:Int = -126
Const XB_LOCK_FAILED:Int = -127
Const XB_CLOSE_ERROR:Int = -128
Const XB_INVALID_SCHEMA:Int = -129
Const XB_INVALID_NAME:Int = -130
Const XB_INVALID_BLOCK_SIZE:Int = -131
Const XB_INVALID_BLOCK_NO:Int = -132
Const XB_NOT_MEMO_FIELD:Int = -133
Const XB_NO_MEMO_DATA:Int = -134
Const XB_EXP_SYNTAX_ERROR:Int = -135
Const XB_PARSE_ERROR:Int = -136
Const XB_NO_DATA:Int = -137
Const XB_UNKNOWN_TOKEN_TYPE:Int = -138
Const XB_INVALID_FIELD:Int = -140
Const XB_INSUFFICIENT_PARMS:Int = -141
Const XB_TOO_MANY_PARMS:Int = -142
Const XB_INVALID_FUNCTION:Int = -143
Const XB_INVALID_FIELD_LEN:Int = -144
Const XB_HARVEST_NODE:Int = -145
Const XB_INVALID_DATE:Int = -146
Const XB_INVALID_LOCK_OPTION:Int = -147

