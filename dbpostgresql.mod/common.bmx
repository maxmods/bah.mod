' Copyright (c) 2007-2009, Bruce A Henderson
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

?linux
Import "-L/usr/lib"
Import "-lpq"
?win32
Import "-lpq"
?macos
Import "-lpq"
Import "-lssl"
Import "-lcrypto"
Import "-framework Kerberos"
?

Import "include/*.h"

Import "postgresqlhelper.cpp"



Const PGRES_EMPTY_QUERY:Int = 0          ' empty query String was executed
Const PGRES_COMMAND_OK:Int = 1           ' a query command that doesn't return
                                         ' anything was executed properly by the
                                         ' backend
Const PGRES_TUPLES_OK:Int = 2            ' a query command that returns tuples was
                                         ' executed properly by the backend, PGresult
                                         ' contains the result tuples
Const PGRES_COPY_OUT:Int = 3             ' Copy Out data transfer in progress
Const PGRES_COPY_IN:Int = 4              ' Copy In data transfer in progress
Const PGRES_BAD_RESPONSE:Int = 5         ' an unexpected response was recv'd from the
                                         ' backend
Const PGRES_NONFATAL_ERROR:Int = 6       ' notice Or warning message
Const PGRES_FATAL_ERROR:Int = 7          ' query failed

Const BOOLOID:Int = 16                    ' boolean
Const INT2OID:Int = 21                    ' -32 thousand to 32 thousand
Const INT4OID:Int = 23                    ' -2 billion to 2 billion integer
Const VOIDOID:Int = 2278                  ' void oid
Const REGPROCOID:Int = 24                 ' registered procedure
Const XIDOID:Int = 28                     ' transaction id
Const CIDOID:Int = 29                     ' command identifier type
Const INT8OID:Int = 20                    ' ~18 digit integer
Const FLOAT4OID:Int = 700                 ' single-precision floating point number
Const FLOAT8OID:Int = 701                 ' double-precision floating point number
Const NUMERICOID:Int = 1700               ' numeric(precision, decimal)
Const ABSTIMEOID:Int = 702                ' 
Const RELTIMEOID:Int = 703                ' 
Const DATEOID:Int = 1082                  ' 
Const TIMEOID:Int = 1083                  ' 
Const TIMETZOID:Int = 1266                ' 
Const TIMESTAMPOID:Int = 1114             ' 
Const TIMESTAMPTZOID:Int = 1184           ' 
Const BYTEAOID:Int = 17                   ' byte array


Extern
	Function bmx_pgsql_PQfinish(handle:Byte Ptr)
	Function bmx_pgsql_PQconnectdb:Byte Ptr(info:Byte Ptr)
	Function bmx_pgsql_PQstatus:Int(handle:Byte Ptr)
	
	Function bmx_pgsql_PQerrorMessage:Byte Ptr(handle:Byte Ptr)
	
	Function bmx_pgsql_PQexec:Byte Ptr(handle:Byte Ptr, query:Byte Ptr)
	Function bmx_pgsql_PQresultStatus:Int(result:Byte Ptr)
	Function bmx_pgsql_PQclear(result:Byte Ptr)
	Function bmx_pgsql_PQntuples:Int(result:Byte Ptr)
	Function bmx_pgsql_PQnfields:Int(result:Byte Ptr)
	Function bmx_pgsql_PQcmdTuples:Byte Ptr(result:Byte Ptr)
	
	Function bmx_pgsql_PQfname:Byte Ptr(result:Byte Ptr, index:Int)
	Function bmx_pgsql_PQftype:Int(result:Byte Ptr, index:Int)
	Function bmx_pgsql_PQfsize:Int(result:Byte Ptr, index:Int)
	Function bmx_pgsql_PQfmod:Int(result:Byte Ptr, index:Int)
	Function bmx_pgsql_PQgetisnull:Int(result:Byte Ptr, row:Int, index:Int)
	Function bmx_pgsql_PQgetlength:Int(result:Byte Ptr, row:Int, index:Int)
	Function bmx_pgsql_PQgetvalue:Byte Ptr(result:Byte Ptr, row:Int, index:Int)
	
	Function bmx_pgsql_PQprepare:Byte Ptr(handle:Byte Ptr, stmtName:Byte Ptr, query:Byte Ptr)
	Function bmx_pgsql_PQdescribePrepared:Byte Ptr(result:Byte Ptr, name:Byte Ptr)
	Function bmx_pgsql_PQnparams:Int(result:Byte Ptr)
	Function bmx_pgsql_createParamValues:Byte Ptr(size:Int)
	Function bmx_pgsql_createParamInts:Int Ptr(size:Int)
	Function bmx_pgsql_deleteParamValues(params:Byte Ptr)
	Function bmx_pgsql_deleteParamInts(params:Int Ptr)
	
	Function bmx_pgsql_setNullParam(params:Byte Ptr, index:Int)
	Function bmx_pgsql_setParam(params:Byte Ptr, lengths:Int Ptr, formats:Int Ptr, index:Int, text:Byte Ptr, length:Int)
	Function bmx_pgsql_setParamBinary(params:Byte Ptr, lengths:Int Ptr, formats:Int Ptr, index:Int, data:Byte Ptr, length:Int)
	Function bmx_pgsql_PQexecPrepared:Byte Ptr(handle:Byte Ptr, stmtName:Byte Ptr, size:Int, params:Byte Ptr, lengths:Int Ptr, formats:Int Ptr)
	Function bmx_pgsql_PQunescapeBytea:Byte Ptr(data:Byte Ptr, length:Int Ptr)
	Function bmx_pgsql_PQfreemem(data:Byte Ptr)

	Function bmx_pgqsl_PQoidValue:Int(result:Byte Ptr)
	
End Extern
