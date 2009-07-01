/*
 Copyright (c) 2008-2009 Bruce A Henderson
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

#include "ocilib.h"
#include <exception>
#include <string>

class MaxSQLException;

using namespace std;

extern "C" {

#include "blitz.h"

	BBObject * _bah_dboracle_TOracleSQLException__create(int errorCode, BBString * message);


	void * bmx_ora_environment_createEnvironment();
	OCI_Connection * bmx_ora_environment_createConnection(void * envHandle, BBString * user, BBString * pass, BBString * connect);

	void bmx_ora_environment_terminateConnection(void * envHandle, OCI_Connection * conn);
	void bmx_ora_environment_terminateEnvironment(void * envHandle);

	void bmx_ora_connection_commit(OCI_Connection * conn);
	void bmx_ora_connection_rollback(OCI_Connection * conn);
	OCI_Statement * bmx_ora_connection_createStatement(OCI_Connection * conn);

	void bmx_ora_statement_closeResultSet(OCI_Statement * stmt, OCI_Resultset * rs);
	int bmx_ora_statement_execute(OCI_Statement * stmt, BBString * statement);
	int bmx_ora_statement_getAutoCommit(OCI_Connection * stmt);
	void bmx_ora_statement_setAutoCommit(OCI_Connection * stmt, int autoCommit);
	OCI_Resultset * bmx_ora_statement_getResultSet(OCI_Statement * stmt);
	int bmx_ora_statement_getUpdateCount(OCI_Statement * stmt);
	int bmx_ora_statement_free(OCI_Statement * stmt);
	int bmx_ora_statement_prepare(OCI_Statement * stmt, BBString * statement);
	int bmx_ora_statement_executeprepared(OCI_Statement * stmt);

	int bmx_ora_resultset_getColCount(OCI_Resultset * rs);
	BBString * bmx_ora_resultset_getColInfo(OCI_Resultset * rs, int index, int * dataType, int * columnSize,
			int * precision, int * scale, int * nullable);
	int bmx_ora_resultset_next(OCI_Resultset * rs);
	int bmx_ora_resultset_isNull(OCI_Resultset * rs, unsigned int index);
	int bmx_ora_resultset_getInt(OCI_Resultset * rs, unsigned int index);
	//float bmx_ora_resultset_getFloat(OCI_Resultset * rs, unsigned int index);
	double bmx_ora_resultset_getDouble(OCI_Resultset * rs, unsigned int index);
	BBString * bmx_ora_resultset_getString(OCI_Resultset * rs, unsigned int index);
	OCI_Lob * bmx_ora_resultset_getBlob(OCI_Resultset * rs, unsigned int index);
	void bmx_ora_resultset_getLong(OCI_Resultset * rs, unsigned int index, big_int * value);
	void bmx_ora_resultset_getDatetime(OCI_Resultset * rs, unsigned int index, int * y, int * m, int * d, int * hh, int * mm, int * ss);

	int bmx_ora_bind_int(OCI_Statement * stmt, char * name, int * value);
	int bmx_ora_bind_double(OCI_Statement * stmt, char * name, double * value);
	int bmx_ora_bind_string(OCI_Statement * stmt, char * name, char * str, unsigned int length);
	int bmx_ora_bind_setnull(OCI_Statement * stmt, unsigned int index);
	int bmx_ora_bind_long(OCI_Statement * stmt, char * name, big_int * value);
	int bmx_ora_bind_blob(OCI_Statement * stmt, char * name, OCI_Lob * blob, void * data, unsigned int length);
	int bmx_ora_bind_date(OCI_Statement * stmt, char * name, OCI_Date * date, int y, int m, int d);
	int bmx_ora_bind_datetime(OCI_Statement * stmt, char * name, OCI_Date * date, int y, int m, int d, int hh, int mm, int ss);
	int bmx_ora_bind_time(OCI_Statement * stmt, char * name, OCI_Date * date, int hh, int mm, int ss);

	OCI_Lob * bmx_ora_blob_create(OCI_Connection * conn);
	void bmx_ora_blob_free(OCI_Lob * blob);
	void bmx_ora_blob_getdata(OCI_Lob * blob, void * data);
	unsigned int bmx_ora_blob_length(OCI_Lob * blob);

	OCI_Date * bmx_ora_date_create(OCI_Connection * conn);
	void bmx_ora_date_free(OCI_Date * date);

}

static bool oci_initialized = false;

// ****************************************************

BBString * bmx_stringToBBString(string s) {
	return bbStringFromCString(s.c_str());
}

// ****************************************************

class MaxSQLException: public exception
{
public:
	MaxSQLException(int err, string message)
		: err(err), message(message)
	{
	}
	
	~MaxSQLException() throw()
	{
	}
	
	int getErrorCode() {
		return err;
	}
	
	BBString * getMessage() {
		return bbStringFromCString(message.c_str());
	}
	
private:
	int err;
	string message;
};

// **********************************

/*
class MaxEnvironment
{
public:
	MaxEnvironment()
	{
	}
	
	~MaxEnvironment()
	{
		if (errHandle) {
			(void) OCIHandleFree((dvoid *) errHandle, OCI_HTYPE_ERROR);
		}
	
		if (envHandle) {
			(void) OCIHandleFree((dvoid *) envHandle, OCI_HTYPE_ENV);
		}
	}
	
	int Create() {
		int err = OCIEnvCreate((OCIEnv **) &envHandle, (ub4) OCI_DEFAULT,

                  (dvoid *) 0, (dvoid * (*)(dvoid *,size_t)) 0,

                  (dvoid * (*)(dvoid *, dvoid *, size_t)) 0,

                  (void (*)(dvoid *, dvoid *)) 0, (size_t) 0, (dvoid **) 0);

		if (err == 0) {
			(void) OCIHandleAlloc( (dvoid *) envHandle, (dvoid **) &errHandle, OCI_HTYPE_ERROR,

                   (size_t) 0, (dvoid **) 0);
		}

		return err;
	}

	static MaxEnvironment * createEnvironment() {
		MaxEnvironment * env = new MaxEnvironment();
		int err = env->Create();
		if (err != 0) {
			delete env;
			throw MaxSQLException(err, "OCIEnvCreate failed");
		}
		return env;
	}
	
	
private:
	OCIEnv * envHandle;
	OCIError * errHandle;
};


class MaxConnection
{
public:

private:

};
*/

#define STATEMENT_RESULT_SET_AVAILABLE  2
#define STATEMENT_UPDATE_COUNT_AVAILABLE  3

static bool hasError;
static int lastErrorCode;
static string lastErrorMessage;

void ocilib_error_reset() {
	hasError = false;
	lastErrorCode = 0;
}

void ocilib_error_handler(OCI_Error *err) {
	hasError = true;
	lastErrorCode = OCI_ErrorGetOCICode(err);
	lastErrorMessage = OCI_ErrorGetString(err);
} 

// ************************************

void * bmx_ora_environment_createEnvironment() {
	
	ocilib_error_reset();
	
	try {
		if (!oci_initialized) {
			
			if (!OCI_Initialize(ocilib_error_handler, NULL, OCI_ENV_DEFAULT)) {
			
				if (hasError) {
					throw MaxSQLException(lastErrorCode, lastErrorMessage);
				}

				throw MaxSQLException(-1, "Error initializing OCI.");
			}
		
			oci_initialized = true;
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	return 0;
}

OCI_Connection * bmx_ora_environment_createConnection(void * envHandle, BBString * user, BBString * pass, BBString * connect) {

	ocilib_error_reset();

	OCI_Connection * conn;
	
	char *u = bbStringToCString( user );
	char *p = bbStringToCString( pass );
	char *c = bbStringToCString( connect );
	
	try {
		conn = OCI_ConnectionCreate(c, u, p, OCI_SESSION_DEFAULT);
		
		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbMemFree( u );
		bbMemFree( p );
		bbMemFree( c );
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	bbMemFree( u );
	bbMemFree( p );
	bbMemFree( c );

	return conn;
}

void bmx_ora_environment_terminateConnection(void * envHandle, OCI_Connection * conn) {

	ocilib_error_reset();

	try {
		OCI_ConnectionFree(conn);

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}


}

void bmx_ora_environment_terminateEnvironment(void * envHandle) {

	ocilib_error_reset();

	try {
		OCI_Cleanup();

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}

}

// ****************************************************

void bmx_ora_connection_commit(OCI_Connection * conn) {

	ocilib_error_reset();

	try {
		OCI_Commit(conn);

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}

}

void bmx_ora_connection_rollback(OCI_Connection * conn) {

	ocilib_error_reset();

	try {
		OCI_Rollback(conn);

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}

}

OCI_Statement * bmx_ora_connection_createStatement(OCI_Connection * conn) {

	ocilib_error_reset();

	OCI_Statement * stmt = 0;
	
	try {
		stmt = OCI_StatementCreate(conn);

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	return stmt;

}

// ****************************************************

void bmx_ora_statement_closeResultSet(OCI_Statement * stmt, OCI_Resultset * rs) {

	ocilib_error_reset();

	try {
		OCI_ReleaseResultsets(stmt);

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}

}

int bmx_ora_statement_execute(OCI_Statement * stmt, BBString * statement) {

	ocilib_error_reset();

	int res = 0;

	char *sql = bbStringToCString( statement );
	
	try {
		OCI_ExecuteStmt(stmt, sql);

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
		if (OCI_GetAffectedRows(stmt) > 0) {
			res = STATEMENT_UPDATE_COUNT_AVAILABLE;
		} else if (OCI_GetResultset(stmt)) {
			res = STATEMENT_RESULT_SET_AVAILABLE;
		}

	} catch (MaxSQLException sqlEx) {
		bbMemFree( sql );
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	bbMemFree( sql );

	return res;

}

int bmx_ora_statement_getAutoCommit(OCI_Connection * conn) {

	ocilib_error_reset();

	bool autoCommit = false;
	
	try {
		autoCommit = OCI_GetAutoCommit(conn);

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	return static_cast<int>(autoCommit);

}

void bmx_ora_statement_setAutoCommit(OCI_Connection * conn, int autoCommit) {

	ocilib_error_reset();

	try {
		OCI_SetAutoCommit(conn, static_cast<bool>(autoCommit));

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}

}

OCI_Resultset * bmx_ora_statement_getResultSet(OCI_Statement * stmt) {

	ocilib_error_reset();

	OCI_Resultset * rs = 0;
	
	try {
		rs = OCI_GetResultset(stmt);

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	return rs;

}

int bmx_ora_statement_getUpdateCount(OCI_Statement * stmt) {

	ocilib_error_reset();

	int count = 0;
	
	try {
		count = OCI_GetAffectedRows(stmt);

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	return count;
}

int bmx_ora_statement_free(OCI_Statement * stmt) {

	ocilib_error_reset();

	bool res = false;
	
	try {
		res = OCI_StatementFree(stmt);

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	return static_cast<int>(res);
}

int bmx_ora_statement_prepare(OCI_Statement * stmt, BBString * statement) {
	ocilib_error_reset();

	bool res = false;
	char *sql = bbStringToCString( statement );
	
	try {
		OCI_AllowRebinding(stmt, TRUE);
	
		res = OCI_Prepare(stmt, sql);

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbMemFree( sql );
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	bbMemFree( sql );

	return static_cast<int>(res);
}

int bmx_ora_statement_executeprepared(OCI_Statement * stmt) {
	ocilib_error_reset();

	int res = 0;

	try {
		OCI_Execute(stmt);

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
		if (OCI_GetAffectedRows(stmt) > 0) {
			res = STATEMENT_UPDATE_COUNT_AVAILABLE;
		} else if (OCI_GetResultset(stmt)) {
			res = STATEMENT_RESULT_SET_AVAILABLE;
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	return res;
}

// ****************************************************

int bmx_ora_resultset_getColCount(OCI_Resultset * rs) {

	ocilib_error_reset();

	int count = 0;
	
	try {
		count = OCI_GetColumnCount(rs);

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	return count;
}

BBString * bmx_ora_resultset_getColInfo(OCI_Resultset * rs, int index, int * dataType, int * columnSize,
		int * precision, int * scale, int * nullable) {

	ocilib_error_reset();

	BBString * colName = &bbEmptyString;
	
	try {
		OCI_Column * col = OCI_GetColumn(rs, index);

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
		const mtext * name = OCI_GetColumnName(col);
		if (name) {
			colName = bmx_stringToBBString(name);
		}
		
		*dataType = OCI_GetColumnType(col);
		*columnSize = OCI_GetColumnSize(col);
		*precision = OCI_GetColumnPrecision(col);
		*scale = OCI_GetColumnScale(col);
		*nullable = static_cast<int>(OCI_GetColumnNullable(col));
	
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	return colName;
}

int bmx_ora_resultset_next(OCI_Resultset * rs) {

	ocilib_error_reset();

	bool ret;
	
	try {
		ret = OCI_FetchNext(rs);

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	return static_cast<int>(ret);
}

int bmx_ora_resultset_isNull(OCI_Resultset * rs, unsigned int index) {

	ocilib_error_reset();

	bool ret = false;

	try {
		ret = OCI_IsNull(rs, index);
		
		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}

	return static_cast<int>(ret);

}

int bmx_ora_resultset_getInt(OCI_Resultset * rs, unsigned int index) {

	ocilib_error_reset();

	int value = 0;

	try {
		value = OCI_GetInt(rs, index);
		
		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	return value;
}

/*
float bmx_ora_resultset_getFloat(OCI_Resultset * rs, unsigned int index) {

	ocilib_error_reset();

	float value = 0;

	try {
		value = rs->getFloat(index);
		
		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	return value;
}
*/

double bmx_ora_resultset_getDouble(OCI_Resultset * rs, unsigned int index) {

	ocilib_error_reset();

	double value = 0;

	try {
		value = OCI_GetDouble(rs, index);
		
		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	return value;
}

BBString * bmx_ora_resultset_getString(OCI_Resultset * rs, unsigned int index) {

	ocilib_error_reset();

	BBString * value = &bbEmptyString;

	try {
		string v = OCI_GetString(rs, index);
		
		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
		value = bmx_stringToBBString(v);

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	return value;
}

OCI_Lob * bmx_ora_resultset_getBlob(OCI_Resultset * rs, unsigned int index) {

	ocilib_error_reset();

	OCI_Lob * value = 0;

	try {
		value = OCI_GetLob(rs, index);
		
		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
	return value;
}

void bmx_ora_resultset_getLong(OCI_Resultset * rs, unsigned int index, big_int * value) {

	ocilib_error_reset();

	try {
		*value = OCI_GetBigInt(rs, index);
		
		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
}

void bmx_ora_resultset_getDatetime(OCI_Resultset * rs, unsigned int index, int * y, int * m, int * d, int * hh, int * mm, int * ss) {

	ocilib_error_reset();

	try {
		OCI_Date * date = OCI_GetDate(rs, index);
		
		if (date) {
			bool res = OCI_DateGetDateTime(date, y, m, d, hh, mm, ss);
		}
		
		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}
	
}

// ****************************************************

int bmx_ora_bind_int(OCI_Statement * stmt, char * name, int * value) {
	ocilib_error_reset();

	bool ret = false;

	try {
		ret = OCI_BindInt(stmt, name, value);
		
		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}

	return static_cast<int>(ret);
}

int bmx_ora_bind_double(OCI_Statement * stmt, char * name, double * value) {
	ocilib_error_reset();

	bool ret = false;

	try {
		ret = OCI_BindDouble(stmt, name, value);
		
		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}

	return static_cast<int>(ret);
}

int bmx_ora_bind_string(OCI_Statement * stmt, char * name, char * str, unsigned int length) {
	ocilib_error_reset();

	bool ret = false;

	try {
		ret = OCI_BindString(stmt, name, str, length);

		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}

	return static_cast<int>(ret);
}

int bmx_ora_bind_setnull(OCI_Statement * stmt, unsigned int index) {
	ocilib_error_reset();

	bool ret = false;

	try {
		ret = OCI_SetNull(stmt, index);
		
		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}

	return static_cast<int>(ret);
}

int bmx_ora_bind_long(OCI_Statement * stmt, char * name, big_int * value) {
	ocilib_error_reset();

	bool ret = false;

	try {
		ret = OCI_BindBigInt(stmt, name, value);
		
		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}

	return static_cast<int>(ret);
}

int bmx_ora_bind_blob(OCI_Statement * stmt, char * name, OCI_Lob * blob, void * data, unsigned int length) {
	ocilib_error_reset();

	bool ret = false;

	OCI_LobWrite(blob, data, length);

	try {
		ret = OCI_BindLob(stmt, name, blob);
		
		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}

	return static_cast<int>(ret);
}

int bmx_ora_bind_date(OCI_Statement * stmt, char * name, OCI_Date * date, int y, int m, int d) {
	ocilib_error_reset();

	bool ret = false;

	OCI_DateSetDate(date, y, m, d);

	try {
		ret = OCI_BindDate(stmt, name, date);
		
		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}

	return static_cast<int>(ret);
}

int bmx_ora_bind_datetime(OCI_Statement * stmt, char * name, OCI_Date * date, int y, int m, int d, int hh, int mm, int ss) {
	ocilib_error_reset();

	bool ret = false;

	OCI_DateSetDateTime(date, y, m, d, hh, mm, ss);

	try {
		ret = OCI_BindDate(stmt, name, date);
		
		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}

	return static_cast<int>(ret);
}

int bmx_ora_bind_time(OCI_Statement * stmt, char * name, OCI_Date * date, int hh, int mm, int ss) {
	ocilib_error_reset();

	bool ret = false;

	OCI_DateSetDateTime(date, 0, 0, 0, hh, mm, ss);

	try {
		ret = OCI_BindDate(stmt, name, date);
		
		if (hasError) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), sqlEx.getMessage()));
	}

	return static_cast<int>(ret);
}


// ****************************************************

OCI_Lob * bmx_ora_blob_create(OCI_Connection * conn) {
	return OCI_LobCreate(conn, OCI_BLOB);
}

void bmx_ora_blob_free(OCI_Lob * blob) {
	OCI_LobFree(blob);
}

void bmx_ora_blob_getdata(OCI_Lob * blob, void * data) {
	OCI_LobRead(blob, data, OCI_LobGetLength(blob));
}

unsigned int bmx_ora_blob_length(OCI_Lob * blob) {
	return OCI_LobGetLength(blob);
}

// ****************************************************

OCI_Date * bmx_ora_date_create(OCI_Connection * conn) {
	return OCI_DateCreate(conn);
}

void bmx_ora_date_free(OCI_Date * date) {
	OCI_DateFree(date);
}


