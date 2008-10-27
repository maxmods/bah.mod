/*
 Copyright (c) 2007,2008 Bruce A Henderson
 
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


