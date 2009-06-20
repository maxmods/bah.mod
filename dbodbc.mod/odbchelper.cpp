/*
  Copyright (c) 2007-2009 Bruce A Henderson
  All rights reserved.
 
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the auther nor the names of its contributors may be used to 
        endorse or promote products derived from this software without specific
        prior written permission.
 
  THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef WIN32
#include <windows.h>
#include <sqlext.h>
#include <odbcinst.h>
#else
#include <isqlext.h>
#include <iodbcinst.h>
#endif
#include <stdio.h>

#include <blitz.h>

extern "C" {

SQLHENV * bmx_odbc_newEnv() {
	return new SQLHENV;
}

SQLHDBC * bmx_odbc_newConnection() {
	return new SQLHDBC;
}

SQLHSTMT * bmx_odbc_newStatement() {
	return new SQLHSTMT;
}

void bmx_odbc_deleteHandle(SQLHANDLE * handle) {
	delete handle;
}

int bmx_odbc_SQLAllocHandle(SQLSMALLINT type, SQLHANDLE * inputHandle, SQLHANDLE * outputHandle) {
	int r = 0;
	
	if (inputHandle) {
		r = SQLAllocHandle(type, inputHandle, outputHandle);
	}
	else {
		r = SQLAllocHandle(type, SQL_NULL_HANDLE, outputHandle);
	}

	return r;
}

const char * bmx_odbc_envError(SQLHANDLE * handle, int * code, int * size) {

	SQLTCHAR state[SQL_SQLSTATE_SIZE+1];
	SQLTCHAR * description = new SQLTCHAR[SQL_MAX_MESSAGE_LENGTH + 1];
	description[0] = 0;

	SQLINTEGER actualCode;
	SQLSMALLINT actualSize;
	
	SQLRETURN result = SQLGetDiagRec(SQL_HANDLE_ENV, handle, 1, (SQLTCHAR*)state, &actualCode, description, SQL_MAX_MESSAGE_LENGTH, &actualSize);
	
	if (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO) {
		// we got error details ok, so we can fill in the code, size and return the text...
		*code = actualCode;
		*size = actualSize;
		return (const char *)description;
	}
	
	return 0;
}

const char * bmx_odbc_connError(SQLHANDLE * handle, int * code, int * size) {

	SQLTCHAR state[SQL_SQLSTATE_SIZE+1];
	SQLTCHAR * description = new SQLTCHAR[SQL_MAX_MESSAGE_LENGTH + 1];
	description[0] = 0;

	SQLINTEGER actualCode;
	SQLSMALLINT actualSize;
	
	SQLRETURN result = SQLGetDiagRec(SQL_HANDLE_DBC, handle, 1, (SQLTCHAR*)state, &actualCode, description, SQL_MAX_MESSAGE_LENGTH, &actualSize);
	
	if (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO) {
		// we got error details ok, so we can fill in the code, size and return the text...
		*code = actualCode;
		*size = actualSize;
		return (const char *)description;
	}
	
	return 0;
}

const char * bmx_odbc_stmtError(SQLHANDLE * handle, int * code, int * size) {

	SQLTCHAR state[SQL_SQLSTATE_SIZE+1];
	SQLTCHAR * description = new SQLTCHAR[SQL_MAX_MESSAGE_LENGTH + 1];
	description[0] = 0;

	SQLINTEGER actualCode;
	SQLSMALLINT actualSize;
	
	SQLRETURN result = SQLGetDiagRec(SQL_HANDLE_STMT, handle, 1, (SQLTCHAR*)state, &actualCode, description, SQL_MAX_MESSAGE_LENGTH, &actualSize);
	
	if (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO) {
		// we got error details ok, so we can fill in the code, size and return the text...
		*code = actualCode;
		*size = actualSize;
		return (const char *)description;
	}
	
	return 0;
}

void bmx_odbc_setattr_odbc3(SQLHANDLE * handle) {

	SQLSetEnvAttr(handle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_UINTEGER);

}

int bmx_odbc_SQLDriverConnect(SQLHANDLE * handle, const char * connectString, int connectLength) {

	SQLTCHAR buffer[1024];
	SQLSMALLINT buffSize;
	int result = SQLDriverConnect(handle, NULL, (SQLCHAR*)connectString, connectLength, buffer, 1024, &buffSize, SQL_DRIVER_NOPROMPT);
	return result;
}

int bmx_odbc_toggleTransaction(SQLHANDLE * handle, bool toggle) {

	int result;
	if (toggle) {
		SQLUINTEGER enableAutoCommit(SQL_AUTOCOMMIT_ON);
		result = SQLSetConnectAttr(handle, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)enableAutoCommit, sizeof(enableAutoCommit));
	} else {
		SQLUINTEGER disableAutoCommit(SQL_AUTOCOMMIT_OFF);
		result = SQLSetConnectAttr(handle, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)disableAutoCommit, sizeof(disableAutoCommit));
	}
		
	return result;
}

int bmx_odbc_commitTransaction(SQLHANDLE * handle) {

	int result = SQLEndTran(SQL_HANDLE_DBC, handle, SQL_COMMIT);
	return result;
}

int bmx_odbc_rollbackTransaction(SQLHANDLE * handle) {

	int result = SQLEndTran(SQL_HANDLE_DBC, handle, SQL_ROLLBACK);
	return result;
}

void bmx_odbc_disconnectAndFree(SQLHANDLE * handle) {

	SQLDisconnect(handle);
	SQLFreeHandle(SQL_HANDLE_DBC, handle);
	
}

int bmx_odbc_freeEnvHandle(SQLHANDLE * handle) {

	int result = SQLFreeHandle(SQL_HANDLE_ENV, handle);
	return result;

}

int bmx_odbc_freeStmtHandle(SQLHANDLE * handle) {

	int result = SQLFreeHandle(SQL_HANDLE_STMT, handle);
	return result;

}


int bmx_odbc_SQLRowCount(SQLHSTMT * handle, int * num) {

	int result = SQLRowCount(handle, (SQLINTEGER *)num);
	return result;
}

int bmx_odbc_setForwardCursor(SQLHSTMT * handle) {

	int result = SQLSetStmtAttr(handle, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_FORWARD_ONLY, SQL_IS_UINTEGER);
	return result;
	
}

int bmx_odbc_execute(SQLHSTMT * handle, const char * query, int length) {

	int result = SQLExecDirect(handle, (SQLCHAR *) query, (SQLINTEGER) length);
	return result;
}


void bmx_odbc_SQLNumResultCols(SQLHSTMT * handle, int * count) {
	SQLSMALLINT sCount;
	SQLNumResultCols(handle, &sCount);
	*count = (int)sCount;
}

int bmx_odbc_SQLDescribeCol(SQLHSTMT * handle, int column, char * columnName, int bufferLength,
		int * nameLength, int * dataType, int * columnSize, int * decimalDigits, int * nullable) {

	SQLSMALLINT sNameLength;
	SQLSMALLINT sDataType;
	SQLSMALLINT sDecimalDigits;
	SQLSMALLINT sNullable;

	int result = SQLDescribeCol(handle, column, (SQLCHAR *)columnName, bufferLength, 
		&sNameLength, &sDataType,
		(SQLUINTEGER *)columnSize, &sDecimalDigits, &sNullable);
		
	*nameLength = (int)sNameLength;
	*dataType = (int)sDataType;
	*decimalDigits = (int)sDecimalDigits;
	*nullable = (int)sNullable;
	
	return result;
		
}

int bmx_odbc_SQLFetchScroll(SQLHSTMT * handle) {

	int result = SQLFetchScroll(handle, SQL_FETCH_NEXT, 0);
	return result;
}

int bmx_odbc_SQLGetData_int(SQLHSTMT * handle, int index, int * value, long * indicator) {

	int result = SQLGetData(handle, index, SQL_C_SLONG, value, 0, indicator);
	return result;
}

int bmx_odbc_SQLGetData_long(SQLHSTMT * handle, int index, BBInt64 * value, long * indicator) {

	int result = SQLGetData(handle, index, SQL_C_SBIGINT, value, 0, indicator);
	return result;
}

int bmx_odbc_SQLGetData_double(SQLHSTMT * handle, int index, double * value, long * indicator) {

	int result = SQLGetData(handle, index, SQL_C_DOUBLE, value, 0, indicator);
	return result;
}

int bmx_odbc_SQLGetData_string(SQLHSTMT * handle, int index, char * buffer, int bufferLength, long * indicator) {

	int result = SQLGetData(handle, index, SQL_C_CHAR, buffer, bufferLength, indicator);
	return result;
}

int bmx_odbc_SQLGetData_date(SQLHSTMT * handle, int index, int * y, int * m, int * d, long * indicator) {

	DATE_STRUCT date;
	int result = SQLGetData(handle, index, SQL_C_TYPE_DATE, &date, 0, indicator);

	*y = static_cast<int>(date.year);
	*m = static_cast<int>(date.month);
	*d = static_cast<int>(date.day);

	return result;
}

int bmx_odbc_SQLGetData_time(SQLHSTMT * handle, int index, int * hh, int * mm, int * ss, long * indicator) {

	SQL_TIME_STRUCT time;
	int result = SQLGetData(handle, index, SQL_C_TYPE_TIMESTAMP, &time, 0, indicator);

	*hh = static_cast<int>(time.hour);
	*mm = static_cast<int>(time.minute);
	*ss = static_cast<int>(time.second);
	return result;
}

int bmx_odbc_SQLGetData_datetime(SQLHSTMT * handle, int index, int * y, int * m, int * d,
		int * hh, int * mm, int * ss, long * indicator) {

	SQL_TIMESTAMP_STRUCT datetime;
	int result = SQLGetData(handle, index, SQL_C_TYPE_TIMESTAMP, &datetime, 0, indicator);

	*y = static_cast<int>(datetime.year);
	*m = static_cast<int>(datetime.month);
	*d = static_cast<int>(datetime.day);
	*hh = static_cast<int>(datetime.hour);
	*mm = static_cast<int>(datetime.minute);
	*ss = static_cast<int>(datetime.second);
	return result;
}

int bmx_odbc_prepare(SQLHSTMT * handle, const char * query, int length) {

	int result = SQLPrepare(handle, (SQLCHAR *) query, (SQLINTEGER) length);
	return result;
}

int bmx_odbc_executePrepared(SQLHSTMT * handle) {

	int result = SQLExecute(handle);
	return result;
}

int bmx_odbc_SQLBindParameter_int(SQLHSTMT * handle, int index, int * paramValue, long * isNull) {

	int result = SQLBindParameter(handle, index, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0,
			paramValue, 0, *isNull == SQL_NULL_DATA ? isNull : NULL);
	return result;
}

int bmx_odbc_SQLBindParameter_double(SQLHSTMT * handle, int index, double * paramValue, long * isNull) {

	int result = SQLBindParameter(handle, index, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0,
			paramValue, 0, *isNull == SQL_NULL_DATA ? isNull : NULL);
	return result;
}

int bmx_odbc_SQLBindParameter_long(SQLHSTMT * handle, int index, BBInt64 * paramValue, long * isNull) {

	int result = SQLBindParameter(handle, index, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0,
			paramValue, 0, *isNull == SQL_NULL_DATA ? isNull : NULL);
	return result;
}

int bmx_odbc_SQLBindParameter_string(SQLHSTMT * handle, int index, char * paramValue, int length, long * isNull) {

	int result = SQLBindParameter(handle, index, SQL_PARAM_INPUT, SQL_C_CHAR, (length > 4000) ? SQL_LONGVARCHAR : SQL_VARCHAR,
		 length + 1, 0, paramValue, length + 1, isNull);
	return result;
}

int bmx_odbc_SQLTables(SQLHSTMT * handle, char * type, int length) {

	int result = SQLTables(handle, NULL, 0, NULL, 0, NULL, 0, (SQLCHAR *)type, length);
	return result;
}


}
