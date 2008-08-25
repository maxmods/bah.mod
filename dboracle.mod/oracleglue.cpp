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

#include "occi.h"

using namespace oracle::occi;
using namespace std;


extern "C" {

#include "blitz.h"

	BBObject * _bah_dboracle_TOracleSQLException__create(int errorCode, BBString * message);

	Environment * bmx_ora_environment_createEnvironment();
	Connection * bmx_ora_environment_createConnection(Environment * envHandle, BBString * user, BBString * pass, BBString * connect);
	void bmx_ora_environment_terminateConnection(Environment * envHandle, Connection * conn);
	void bmx_ora_environment_terminateEnvironment(Environment * envHandle);

	void bmx_ora_connection_commit(Connection * conn);
	void bmx_ora_connection_rollback(Connection * conn);
	Statement * bmx_ora_connection_createStatement(Connection * conn);

	void bmx_ora_statement_closeResultSet(Statement * stmt, ResultSet * rs);
	Statement::Status bmx_ora_statement_execute(Statement * stmt, BBString * statement);
	bool bmx_ora_statement_getAutoCommit(Statement * stmt);
	void bmx_ora_statement_setAutoCommit(Statement * stmt, bool autoCommit);
	ResultSet * bmx_ora_statement_getResultSet(Statement * stmt);
	unsigned int bmx_ora_statement_getUpdateCount(Statement * stmt);

	int bmx_ora_resultset_getColCount(ResultSet * rs);
	BBString * bmx_ora_resultset_getColInfo(ResultSet * rs, int index, int * dataType, int * columnSize,
			int * precision, int * scale, bool * nullable);
	ResultSet::Status bmx_ora_resultset_next(ResultSet * rs);
	bool bmx_ora_resultset_isNull(ResultSet * rs, unsigned int index);
	int bmx_ora_resultset_getInt(ResultSet * rs, unsigned int index);
	float bmx_ora_resultset_getFloat(ResultSet * rs, unsigned int index);
	double bmx_ora_resultset_getDouble(ResultSet * rs, unsigned int index);
	BBString * bmx_ora_resultset_getString(ResultSet * rs, unsigned int index);

}

// ****************************************************

BBString * bmx_stringToBBString(string s) {
	return bbStringFromCString(s.c_str());
}

// ****************************************************

Environment * bmx_ora_environment_createEnvironment() {
	Environment * env = 0;
	
	try {
		env = Environment::createEnvironment();
	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return env;
}

Connection * bmx_ora_environment_createConnection(Environment * envHandle, BBString * user, BBString * pass, BBString * connect) {

	Connection * conn = 0;

	char *u = bbStringToCString( user );
	char *p = bbStringToCString( pass );
	char *c = bbStringToCString( connect );
	
	try {
		conn = envHandle->createConnection(string(u), string(p), string(c));
		
	} catch (SQLException sqlEx) {
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

void bmx_ora_environment_terminateConnection(Environment * envHandle, Connection * conn) {
	try {
		envHandle->terminateConnection(conn);
	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
}

void bmx_ora_environment_terminateEnvironment(Environment * envHandle) {
	try {
		Environment::terminateEnvironment(envHandle);
	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
}


// ****************************************************

void bmx_ora_connection_commit(Connection * conn) {
	try {
		conn->commit();

	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
}

void bmx_ora_connection_rollback(Connection * conn) {
	try {
		conn->rollback();

	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
}

Statement * bmx_ora_connection_createStatement(Connection * conn) {
	Statement * stmt = 0;
	
	try {
		stmt = conn->createStatement();

	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return stmt;
}

// ****************************************************

void bmx_ora_statement_closeResultSet(Statement * stmt, ResultSet * rs) {
	try {
		stmt->closeResultSet(rs);

	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
}

Statement::Status bmx_ora_statement_execute(Statement * stmt, BBString * statement) {
	Statement::Status s;

	char *sql = bbStringToCString( statement );
	
	try {
		s = stmt->execute(string(sql));

	} catch (SQLException sqlEx) {
		bbMemFree( sql );
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	bbMemFree( sql );

	return s;
}

bool bmx_ora_statement_getAutoCommit(Statement * stmt) {
	bool autoCommit = false;
	
	try {
		autoCommit = stmt->getAutoCommit();

	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return autoCommit;
}

void bmx_ora_statement_setAutoCommit(Statement * stmt, bool autoCommit) {
	try {
		stmt->setAutoCommit(autoCommit);

	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
}

ResultSet * bmx_ora_statement_getResultSet(Statement * stmt) {
	ResultSet * rs = 0;
	
	try {
		rs = stmt->getResultSet();

	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return rs;
}

unsigned int bmx_ora_statement_getUpdateCount(Statement * stmt) {
	unsigned int count = 0;
	
	try {
		count = stmt->getUpdateCount();
	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return count;
}


// ****************************************************

int bmx_ora_resultset_getColCount(ResultSet * rs) {
	int count = 0;
	
	try {
		vector<MetaData> md = rs->getColumnListMetaData();
		count = md.size();

	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return count;
}

BBString * bmx_ora_resultset_getColInfo(ResultSet * rs, int index, int * dataType, int * columnSize,
		int * precision, int * scale, bool * nullable) {

	BBString * colName = &bbEmptyString;
	
	try {
		vector<MetaData> md = rs->getColumnListMetaData();
		MetaData meta = md[index];
		
		string name = meta.getString(MetaData::ATTR_NAME);
		colName = bmx_stringToBBString(name);
		
		*dataType = meta.getInt(MetaData::ATTR_DATA_TYPE);
		*columnSize = meta.getInt(MetaData::ATTR_DATA_SIZE);
		*precision = meta.getInt(MetaData::ATTR_PRECISION);
		*scale = meta.getInt(MetaData::ATTR_SCALE);
		*nullable = meta.getBoolean(MetaData::ATTR_IS_NULL);
	
	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return colName;

}

ResultSet::Status bmx_ora_resultset_next(ResultSet * rs) {
	ResultSet::Status ret;
	
	try {
		ret = rs->next();
		
	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return ret;
}

bool bmx_ora_resultset_isNull(ResultSet * rs, unsigned int index) {
	bool ret = false;

	try {
		ret = rs->isNull(index);
		
	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}

	return ret;
}

int bmx_ora_resultset_getInt(ResultSet * rs, unsigned int index) {
	int value = 0;

	try {
		value = rs->getInt(index);
		
	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return value;
}

float bmx_ora_resultset_getFloat(ResultSet * rs, unsigned int index) {
	float value = 0;

	try {
		value = rs->getFloat(index);
		
	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return value;
}

double bmx_ora_resultset_getDouble(ResultSet * rs, unsigned int index) {
	double value = 0;

	try {
		value = rs->getDouble(index);
		
	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return value;
}

BBString * bmx_ora_resultset_getString(ResultSet * rs, unsigned int index) {
	BBString * value = &bbEmptyString;

	try {
		string v = rs->getString(index);
		value = bmx_stringToBBString(v);
		
	} catch (SQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return value;
}

// ****************************************************

/*
printf("const OCCI_SQLT_CHR:int = %d\n", OCCI_SQLT_CHR);
printf("const OCCI_SQLT_NUM:int = %d\n", OCCI_SQLT_NUM);
printf("const OCCIINT:int = %d\n", OCCIINT);
printf("const OCCIFLOAT:int = %d\n", OCCIFLOAT);
printf("const OCCIBFLOAT:int = %d\n", OCCIBFLOAT);
printf("const OCCIBDOUBLE:int = %d\n", OCCIBDOUBLE);
printf("const OCCIIBFLOAT:int = %d\n", OCCIIBFLOAT);
printf("const OCCIIBDOUBLE:int = %d\n", OCCIIBDOUBLE);
printf("const OCCI_SQLT_STR:int = %d\n", OCCI_SQLT_STR);
printf("const OCCI_SQLT_VNU:int = %d\n", OCCI_SQLT_VNU);
printf("const OCCI_SQLT_PDN:int = %d\n", OCCI_SQLT_PDN);
printf("const OCCI_SQLT_LNG:int = %d\n", OCCI_SQLT_LNG);
printf("const OCCI_SQLT_VCS:int = %d\n", OCCI_SQLT_VCS);
printf("const OCCI_SQLT_NON:int = %d\n", OCCI_SQLT_NON);
printf("const OCCI_SQLT_RID:int = %d\n", OCCI_SQLT_RID);
printf("const OCCI_SQLT_DAT:int = %d\n", OCCI_SQLT_DAT);
printf("const OCCI_SQLT_VBI:int = %d\n", OCCI_SQLT_VBI);
printf("const OCCI_SQLT_BIN:int = %d\n", OCCI_SQLT_BIN);
printf("const OCCI_SQLT_LBI:int = %d\n", OCCI_SQLT_LBI);
printf("const OCCIUNSIGNED_INT:int = %d\n", OCCIUNSIGNED_INT);
printf("const OCCI_SQLT_SLS:int = %d\n", OCCI_SQLT_SLS);
printf("const OCCI_SQLT_LVC:int = %d\n", OCCI_SQLT_LVC);
printf("const OCCI_SQLT_LVB:int = %d\n", OCCI_SQLT_LVB);
printf("const OCCI_SQLT_AFC:int = %d\n", OCCI_SQLT_AFC);
printf("const OCCI_SQLT_AVC:int = %d\n", OCCI_SQLT_AVC);
printf("const OCCI_SQLT_CUR:int = %d\n", OCCI_SQLT_CUR);
printf("const OCCI_SQLT_RDD:int = %d\n", OCCI_SQLT_RDD);
printf("const OCCI_SQLT_LAB:int = %d\n", OCCI_SQLT_LAB);
printf("const OCCI_SQLT_OSL:int = %d\n", OCCI_SQLT_OSL);
printf("const OCCI_SQLT_NTY:int = %d\n", OCCI_SQLT_NTY);
printf("const OCCI_SQLT_REF:int = %d\n", OCCI_SQLT_REF);
printf("const OCCI_SQLT_CLOB:int = %d\n", OCCI_SQLT_CLOB);
printf("const OCCI_SQLT_BLOB:int = %d\n", OCCI_SQLT_BLOB);
printf("const OCCI_SQLT_BFILEE:int = %d\n", OCCI_SQLT_BFILEE);
printf("const OCCI_SQLT_CFILEE:int = %d\n", OCCI_SQLT_CFILEE);
printf("const OCCI_SQLT_RSET:int = %d\n", OCCI_SQLT_RSET);
printf("const OCCI_SQLT_NCO:int = %d\n", OCCI_SQLT_NCO);
printf("const OCCI_SQLT_VST:int = %d\n", OCCI_SQLT_VST);
printf("const OCCI_SQLT_ODT:int = %d\n", OCCI_SQLT_ODT);
printf("const OCCI_SQLT_DATE:int = %d\n", OCCI_SQLT_DATE);
printf("const OCCI_SQLT_TIME:int = %d\n", OCCI_SQLT_TIME);
printf("const OCCI_SQLT_TIME_TZ:int = %d\n", OCCI_SQLT_TIME_TZ);
printf("const OCCI_SQLT_TIMESTAMP:int = %d\n", OCCI_SQLT_TIMESTAMP);
printf("const OCCI_SQLT_TIMESTAMP_TZ:int = %d\n", OCCI_SQLT_TIMESTAMP_TZ);
printf("const OCCI_SQLT_INTERVAL_YM:int = %d\n", OCCI_SQLT_INTERVAL_YM);
printf("const OCCI_SQLT_INTERVAL_DS:int = %d\n", OCCI_SQLT_INTERVAL_DS);
printf("const OCCI_SQLT_TIMESTAMP_LTZ:int = %d\n", OCCI_SQLT_TIMESTAMP_LTZ);
printf("const OCCI_SQLT_FILE:int = %d\n", OCCI_SQLT_FILE);
printf("const OCCI_SQLT_CFILE:int = %d\n", OCCI_SQLT_CFILE);
printf("const OCCI_SQLT_BFILE:int = %d\n", OCCI_SQLT_BFILE);
 
printf("const OCCICHAR:int = %d\n", OCCICHAR);
printf("const OCCIDOUBLE:int = %d\n", OCCIDOUBLE);
printf("const OCCIBOOL:int = %d\n", OCCIBOOL);
printf("const OCCIANYDATA:int = %d\n", OCCIANYDATA);
printf("const OCCINUMBER:int = %d\n", OCCINUMBER);
printf("const OCCIBLOB:int = %d\n", OCCIBLOB);
printf("const OCCIBFILE:int = %d\n", OCCIBFILE);
printf("const OCCIBYTES:int = %d\n", OCCIBYTES);
printf("const OCCICLOB:int = %d\n", OCCICLOB);
printf("const OCCIVECTOR:int = %d\n", OCCIVECTOR);
printf("const OCCIMETADATA:int = %d\n", OCCIMETADATA);
printf("const OCCIPOBJECT:int = %d\n", OCCIPOBJECT);
printf("const OCCIREF:int = %d\n", OCCIREF);
printf("const OCCIREFANY:int = %d\n", OCCIREFANY);
printf("const OCCISTRING:int = %d\n", OCCISTRING);
printf("const OCCISTREAM:int = %d\n", OCCISTREAM);
printf("const OCCIDATE:int = %d\n", OCCIDATE);
printf("const OCCIINTERVALDS:int = %d\n", OCCIINTERVALDS);
printf("const OCCIINTERVALYM:int = %d\n", OCCIINTERVALYM);
printf("const OCCITIMESTAMP:int = %d\n", OCCITIMESTAMP);
printf("const OCCIROWID:int = %d\n", OCCIROWID);
printf("const OCCICURSOR:int = %d\n", OCCICURSOR);

fflush(stdout);
*/

