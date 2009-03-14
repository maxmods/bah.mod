/*
  Copyright (c) 2007-2009, Bruce A Henderson
  All rights reserved.
 
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of Bruce A Henderson nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.
 
  THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ''AS IS'' AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <libpq-fe.h>

extern "C" {

void bmx_pgsql_PQfinish(PGconn * handle) {
	PQfinish(handle);
}


PGconn * bmx_pgsql_PQconnectdb(const char * conninfo) {

	return PQconnectdb(conninfo);

}

int bmx_pgsql_PQstatus(const PGconn * handle) {
	return PQstatus(handle);
}

char * bmx_pgsql_PQerrorMessage(const PGconn * handle) {
	return PQerrorMessage(handle);
}

PGresult * bmx_pgsql_PQexec(PGconn * handle, const char * query) {
	return PQexec(handle, query);
}

int bmx_pgsql_PQresultStatus(const PGresult * result) {
	return PQresultStatus(result);
}

void bmx_pgsql_PQclear(PGresult * result) {
	PQclear(result);
}

int bmx_pgsql_PQntuples(PGresult * result) {
	return PQntuples(result);
}

int bmx_pgsql_PQnfields(PGresult * result) {
	return PQnfields(result);
}

char * bmx_pgsql_PQcmdTuples(PGresult * result) {
	return PQcmdTuples(result);
}

char * bmx_pgsql_PQfname(PGresult * result, int index) {
	return PQfname(result, index);
}


int bmx_pgsql_PQftype(PGresult * result, int index) {
	return PQftype(result, index);
}

int bmx_pgsql_PQfsize(PGresult * result, int index) {
	return PQfsize(result, index);
}

int bmx_pgsql_PQfmod(PGresult * result, int index) {
	return PQfmod(result, index);
}

int bmx_pgsql_PQgetisnull(PGresult * result, int row, int index) {
	return PQgetisnull(result, row, index);
}

char * bmx_pgsql_PQgetvalue(PGresult * result, int row, int index) {
	return PQgetvalue(result, row, index);
}

int bmx_pgsql_PQgetlength(PGresult * result, int row, int index) {
	return PQgetlength(result, row, index);
}

PGresult * bmx_pgsql_PQprepare(PGconn * conn, const char * stmtName, const char * query) {
	return PQprepare(conn, stmtName, query, 0, NULL);
}

//PGresult * bmx_pgsql_PQdescribePrepared(PGconn * conn, const char * stmtName) {
//	return PQdescribePrepared(conn, stmtName);
//}

char ** bmx_pgsql_createParamValues(int size) {
	return new char*[size];
}

int * bmx_pgsql_createParamInts(int size) {
	return new int[size];
}

void bmx_pgsql_deleteParamValues(const char * const * arr) {
	delete arr;
}

void bmx_pgsql_deleteParamInts(int * arr) {
	delete arr;
}

void bmx_pgsql_setNullParam(char * * params, int index) {
	params[index] = 0;
}

void bmx_pgsql_setParam(char * * params, int * lengths, int * formats, int index, char * text, int length) {
	params[index] = text;
	lengths[index] = length;
	formats[index] = 0;
}

void bmx_pgsql_setParamBinary(char * * params, int * lengths, int * formats, int index, char * data, int length) {
	params[index] = data;
	lengths[index] = length;
	formats[index] = 1;
}

PGresult * bmx_pgsql_PQexecPrepared(PGconn * conn, const char * stmtName, int nParams, const char * const * params, int * lengths, int * formats) {
	return PQexecPrepared(conn, stmtName, nParams, params, lengths, formats, 0);
}

unsigned char * bmx_pgsql_PQunescapeBytea(unsigned char * data, size_t * length) {
	return PQunescapeBytea(data, length);
}

void bmx_pgsql_PQfreemem(void * data) {
	PQfreemem(data);
}

int bmx_pgqsl_PQoidValue(PGresult * result) {

	Oid value = PQoidValue(result);
	
	if (value != InvalidOid) {
		return (int)value;
	}
	return -1;
}

}

