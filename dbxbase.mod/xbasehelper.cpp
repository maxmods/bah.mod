#include <xbase64/xbase64.h>
#include <xbsql.h>

class MaxXBase;

extern "C" {
	MaxXBase * bmx_xbase_init(void * handle, const char * dir);
	bool bmx_xbase_execCommand(MaxXBase * db, const char * statement);
	
	XBSQLQuery * bmx_xbase_openQuery(MaxXBase * db, const char * statement);
	bool bmx_xbase_executeQuery(XBSQLQuery * sql);
	int bmx_xbase_getNumFields(XBSQLQuery * sql);
	int bmx_xbase_getFieldType(XBSQLSelect * sql, int index);
	const char * bmx_xbase_getFieldName(XBSQLSelect * sql, int index);
	int bmx_xbase_getFieldLength(XBSQLSelect * sql, int index);
	const char * bmx_xbase_getFieldText(XBSQLSelect * sql, int row, int index);
	int bmx_xbase_getNumRows(XBSQLQuery * sql);
	void bmx_xbase_deleteStatement(XBSQLQuery * sql);

	const char * bmx_xbase_lastError(MaxXBase * db);
}

/* CLASSES */

class MaxXBase
{
public:
	MaxXBase(void * handle, const char * dir);
	
	bool execCommand(const char * statement);
	XBSQLQuery * openQuery(const char * statement);
	const char * lastError();
	
private:
	void * maxHandle;
	XBaseSQL xbs;
};

MaxXBase::MaxXBase(void * handle, const char * dir)
	: maxHandle(handle), xbs(dir)
{
}

bool MaxXBase::execCommand(const char * statement) {
	return xbs.execCommand(statement);
}

XBSQLQuery * MaxXBase::openQuery(const char * statement) {
	bool parsed;
	return xbs.openQuery(statement, parsed);
}

const char * MaxXBase::lastError() {
	return xbs.lastError();
}



/* FUNCTIONS */

MaxXBase * bmx_xbase_init(void * handle, const char * dir) {
	return new MaxXBase(handle, dir);
}

bool bmx_xbase_execCommand(MaxXBase * db, const char * statement) {
	return db->execCommand(statement);
}

const char * bmx_xbase_lastError(MaxXBase * db) {
	return db->lastError();
}

XBSQLQuery * bmx_xbase_openQuery(MaxXBase * db, const char * statement) {
	return db->openQuery(statement);
}

bool bmx_xbase_executeQuery(XBSQLQuery * sql) {
	return sql->execute(0,0);
}

int bmx_xbase_getNumFields(XBSQLQuery * sql) {
	return sql->getNumFields();
}

int bmx_xbase_getFieldType(XBSQLSelect * sql, int index) {
	return (int)sql->getFieldType(index);
}

const char * bmx_xbase_getFieldName(XBSQLSelect * sql, int index) {
	return sql->getFieldName(index);
}

int bmx_xbase_getFieldLength(XBSQLSelect * sql, int index) {
	return sql->getFieldLength(index);
}

const char * bmx_xbase_getFieldText(XBSQLSelect * sql, int row, int index) {
	return sql->getField(row, index).getText();
}

int bmx_xbase_getNumRows(XBSQLQuery * sql) {
	return sql->getNumRows();
}

void bmx_xbase_deleteStatement(XBSQLQuery * sql) {
	delete sql;
}


