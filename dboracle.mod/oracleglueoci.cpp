/*
 Copyright (c) 2008 Bruce A Henderson
 
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
	bool bmx_ora_statement_getAutoCommit(OCI_Connection * stmt);
	void bmx_ora_statement_setAutoCommit(OCI_Connection * stmt, bool autoCommit);
	OCI_Resultset * bmx_ora_statement_getResultSet(OCI_Statement * stmt);
	int bmx_ora_statement_getUpdateCount(OCI_Statement * stmt);
	bool bmx_ora_statement_free(OCI_Statement * stmt);

	int bmx_ora_resultset_getColCount(OCI_Resultset * rs);
	BBString * bmx_ora_resultset_getColInfo(OCI_Resultset * rs, int index, int * dataType, int * columnSize,
			int * precision, int * scale, bool * nullable);
	bool bmx_ora_resultset_next(OCI_Resultset * rs);
	bool bmx_ora_resultset_isNull(OCI_Resultset * rs, unsigned int index);
	int bmx_ora_resultset_getInt(OCI_Resultset * rs, unsigned int index);
	//float bmx_ora_resultset_getFloat(OCI_Resultset * rs, unsigned int index);
	double bmx_ora_resultset_getDouble(OCI_Resultset * rs, unsigned int index);
	BBString * bmx_ora_resultset_getString(OCI_Resultset * rs, unsigned int index);


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
	
	string getMessage() {
		return message;
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

static int lastErrorCode;
static string lastErrorMessage;

void ocilib_error_reset() {
	lastErrorCode = 0;
}

void ocilib_error_handler(OCI_Error *err) {
	lastErrorCode = OCI_ErrorGetOCICode(err);
	lastErrorMessage = OCI_ErrorGetString(err);
} 

// ************************************

void * bmx_ora_environment_createEnvironment() {
	
	ocilib_error_reset();
	
	try {
		if (!oci_initialized) {
			
			if (!OCI_Initialize(ocilib_error_handler, NULL, OCI_ENV_DEFAULT)) {
				throw MaxSQLException(-1, "Error initializing OCI.");
			}
		
			oci_initialized = true;
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
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
		
		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbMemFree( u );
		bbMemFree( p );
		bbMemFree( c );
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
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

		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}


}

void bmx_ora_environment_terminateEnvironment(void * envHandle) {

	ocilib_error_reset();

	try {
		OCI_Cleanup();

		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}

}

// ****************************************************

void bmx_ora_connection_commit(OCI_Connection * conn) {

	ocilib_error_reset();

	try {
		OCI_Commit(conn);

		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}

}

void bmx_ora_connection_rollback(OCI_Connection * conn) {

	ocilib_error_reset();

	try {
		OCI_Rollback(conn);

		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}

}

OCI_Statement * bmx_ora_connection_createStatement(OCI_Connection * conn) {

	ocilib_error_reset();

	OCI_Statement * stmt = 0;
	
	try {
		stmt = OCI_StatementCreate(conn);

		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return stmt;

}

// ****************************************************

void bmx_ora_statement_closeResultSet(OCI_Statement * stmt, OCI_Resultset * rs) {

	ocilib_error_reset();

	try {
		OCI_ReleaseResultsets(stmt);

		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}

}

int bmx_ora_statement_execute(OCI_Statement * stmt, BBString * statement) {

	ocilib_error_reset();

	int res = 0;

	char *sql = bbStringToCString( statement );
	
	try {
		OCI_ExecuteStmt(stmt, sql);

		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
		if (OCI_GetAffectedRows(stmt) > 0) {
			res = STATEMENT_UPDATE_COUNT_AVAILABLE;
		} else if (OCI_GetResultset(stmt)) {
			res = STATEMENT_RESULT_SET_AVAILABLE;
		}

	} catch (MaxSQLException sqlEx) {
		bbMemFree( sql );
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	bbMemFree( sql );

	return res;

}

bool bmx_ora_statement_getAutoCommit(OCI_Connection * conn) {

	ocilib_error_reset();

	bool autoCommit = false;
	
	try {
		autoCommit = OCI_GetAutoCommit(conn);

		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return autoCommit;

}

void bmx_ora_statement_setAutoCommit(OCI_Connection * conn, bool autoCommit) {

	ocilib_error_reset();

	try {
		OCI_SetAutoCommit(conn, autoCommit);

		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}

}

OCI_Resultset * bmx_ora_statement_getResultSet(OCI_Statement * stmt) {

	ocilib_error_reset();

	OCI_Resultset * rs = 0;
	
	try {
		rs = OCI_GetResultset(stmt);

		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return rs;

}

int bmx_ora_statement_getUpdateCount(OCI_Statement * stmt) {

	ocilib_error_reset();

	int count = 0;
	
	try {
		count = OCI_GetAffectedRows(stmt);

		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return count;
}

bool bmx_ora_statement_free(OCI_Statement * stmt) {

	ocilib_error_reset();

	bool res = false;
	
	try {
		res = OCI_StatementFree(stmt);

		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return res;
}


// ****************************************************

int bmx_ora_resultset_getColCount(OCI_Resultset * rs) {

	ocilib_error_reset();

	int count = 0;
	
	try {
		count = OCI_GetColumnCount(rs);

		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return count;
}

BBString * bmx_ora_resultset_getColInfo(OCI_Resultset * rs, int index, int * dataType, int * columnSize,
		int * precision, int * scale, bool * nullable) {

	ocilib_error_reset();

	BBString * colName = &bbEmptyString;
	
	try {
		OCI_Column * col = OCI_GetColumn(rs, index);

		if (lastErrorCode) {
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
		*nullable = OCI_GetColumnNullable(col);
	
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return colName;
}

bool bmx_ora_resultset_next(OCI_Resultset * rs) {

	ocilib_error_reset();

	bool ret;
	
	try {
		ret = OCI_FetchNext(rs);

		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return ret;
}

bool bmx_ora_resultset_isNull(OCI_Resultset * rs, unsigned int index) {

	ocilib_error_reset();

	bool ret = false;

	try {
		ret = OCI_IsNull(rs, index);
		
		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}

	return ret;

}

int bmx_ora_resultset_getInt(OCI_Resultset * rs, unsigned int index) {

	ocilib_error_reset();

	int value = 0;

	try {
		value = OCI_GetInt(rs, index);
		
		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return value;
}

/*
float bmx_ora_resultset_getFloat(OCI_Resultset * rs, unsigned int index) {

	ocilib_error_reset();

	float value = 0;

	try {
		value = rs->getFloat(index);
		
		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return value;
}
*/

double bmx_ora_resultset_getDouble(OCI_Resultset * rs, unsigned int index) {

	ocilib_error_reset();

	double value = 0;

	try {
		value = OCI_GetDouble(rs, index);
		
		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return value;
}

BBString * bmx_ora_resultset_getString(OCI_Resultset * rs, unsigned int index) {

	ocilib_error_reset();

	BBString * value = &bbEmptyString;

	try {
		string v = OCI_GetString(rs, index);
		
		if (lastErrorCode) {
			throw MaxSQLException(lastErrorCode, lastErrorMessage);
		}
		
		value = bmx_stringToBBString(v);

	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return value;
}



