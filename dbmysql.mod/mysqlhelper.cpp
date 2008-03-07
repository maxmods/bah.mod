#include <blitz.h>

#ifdef _WIN32
#include <winsock.h>
#else
#include <my_global.h>
#endif
#include <mysql.h>
#include <stdio.h>

extern "C" {

char * bmx_mysql_field_name(MYSQL_FIELD * field) {
	return field->name;
}

char * bmx_mysql_field_org_name(MYSQL_FIELD * field) {
	return field->org_name;
}

char * bmx_mysql_field_table(MYSQL_FIELD * field) {
	return field->table;
}

char * bmx_mysql_field_org_table(MYSQL_FIELD * field) {
	return field->org_table;
}

char * bmx_mysql_field_db(MYSQL_FIELD * field) {
	return field->db;
}

char * bmx_mysql_field_catalog(MYSQL_FIELD * field) {
	return field->catalog;
}

char * bmx_mysql_field_def(MYSQL_FIELD * field) {
	return field->def;
}

unsigned long bmx_mysql_field_length(MYSQL_FIELD * field) {
	return field->length;
}

unsigned long bmx_mysql_field_max_length(MYSQL_FIELD * field) {
	return field->max_length;
}

int bmx_mysql_field_flags(MYSQL_FIELD * field) {
	return field->flags;
}

int bmx_mysql_field_type(MYSQL_FIELD * field) {
	return field->type;
}

int bmx_mysql_field_decimals(MYSQL_FIELD * field) {
	return field->decimals;
}


MYSQL_BIND * bmx_mysql_makeBindings(int size) {
	MYSQL_BIND * bindings = new MYSQL_BIND[size];
	// important that we clear the memory... otherwise it nukes when we try to "fetch"!!
	memset(bindings, 0, size * sizeof(MYSQL_BIND));
	return bindings;
}

// tidy up our memory
void bmx_mysql_deleteBindings(MYSQL_BIND * bindings) {
	delete[] bindings;
}

//my_bool * bmx_mysql_makeBools(int size) {
//	return new my_bool[size];
//}

void bmx_mysql_deleteBools(my_bool * bools) {
	delete[] bools;
}

void bmx_mysql_setBool(my_bool * bools, int index, bool isNull) {
	bools[index] = static_cast<my_bool>(isNull);
}

char ** bmx_mysql_makeVals(int size) {
	return new char*[size];
}

void bmx_mysql_deleteVals(char ** vals) {
	delete[] vals;
}

void bmx_mysql_bind_null(MYSQL_BIND* bindings, int index) {

	MYSQL_BIND* bind = &bindings[index];
	bind->buffer_type = MYSQL_TYPE_NULL;
}

void bmx_mysql_bind_int(MYSQL_BIND* bindings, int index, int * value) {

	MYSQL_BIND* bind = &bindings[index];
	bind->is_null = (my_bool*)0;
	bind->length = 0;
	
	bind->buffer_type = MYSQL_TYPE_LONG;
	bind->buffer = value;
	bind->buffer_length = sizeof(int);
	bind->is_unsigned = false;
}

void bmx_mysql_bind_float(MYSQL_BIND* bindings, int index, float * value) {

	MYSQL_BIND* bind = &bindings[index];
	bind->is_null = (my_bool*)0;
	bind->length = 0;

	bind->buffer_type = MYSQL_TYPE_FLOAT;
	bind->buffer = value;
	bind->buffer_length = sizeof(float);
	bind->is_unsigned = false;
}

void bmx_mysql_bind_double(MYSQL_BIND* bindings, int index, double * value) {

	MYSQL_BIND* bind = &bindings[index];
	bind->is_null = (my_bool*)0;
	bind->length = 0;

	bind->buffer_type = MYSQL_TYPE_DOUBLE;
	bind->buffer = value;
	bind->buffer_length = sizeof(double);
	bind->is_unsigned = false;
}

void bmx_mysql_bind_long(MYSQL_BIND* bindings, int index, long * value) {

	MYSQL_BIND* bind = &bindings[index];
	bind->is_null = (my_bool*)0;
	bind->length = 0;

	bind->buffer_type = MYSQL_TYPE_LONGLONG;
	bind->buffer = value;
	bind->buffer_length = 8;
	bind->is_unsigned = false;
}

void bmx_mysql_bind_string(MYSQL_BIND* bindings, int index, char * value, int size) {

	MYSQL_BIND* bind = &bindings[index];
	bind->is_null = (my_bool*)0;
	bind->length = 0;

	bind->buffer_type = MYSQL_TYPE_STRING;
	bind->buffer = value;
	bind->buffer_length = size;

	bind->is_unsigned = 0;
}

void examine_bindings(MYSQL_BIND* bindings, int size, MYSQL_STMT *stmt) {
	for (int i = 0; i < size; i++) {
		MYSQL_BIND* bind = &bindings[i];
		printf("Number = %d\n", i);fflush(stdout);
		printf("type = %d\n", bind->buffer_type);fflush(stdout);
		printf("lgth = %d\n", bind->buffer_length);fflush(stdout);
		if (MYSQL_TYPE_STRING == bind->buffer_type) {
			printf("data = %s\n", bind->buffer);fflush(stdout);
		}
	}
}

bool bmx_mysql_rowField_isNull(MYSQL_ROW row, int index) {
	return (row[index] == NULL);
}

unsigned long bmx_mysql_getLength(unsigned long * lengths, int index) {
	return lengths[index];
}

char * bmx_mysql_rowField_chars(MYSQL_ROW row, int index) {
	return row[index];
}


void bmx_mysql_inbind(MYSQL_BIND* bindings, int index, MYSQL_FIELD * field, char * dataValue, long unsigned * dataLength, my_bool * isNull) {

	// important... otherwise we get a "unsupported buffer type" error
	field->type = MYSQL_TYPE_STRING;

	MYSQL_BIND* bind = &bindings[index];
     bind->buffer_type = field->type;
     bind->buffer_length = field->length + 1;
     bind->is_null = isNull;
     bind->length = dataLength;

	bind->buffer = dataValue;
}

int bmx_mysql_stmt_bind_result(MYSQL_STMT *stmt, MYSQL_BIND* bindings) {
	int result = mysql_stmt_bind_result(stmt, bindings);
	return result;
}

int bmx_mysql_stmt_bind_param(MYSQL_STMT *stmt, MYSQL_BIND* bindings) {
	int result = mysql_stmt_bind_param(stmt, bindings);
	return result;
}

MYSQL_BIND * bmx_mysql_getBindings(MYSQL_STMT *stmt) {
	return stmt->bind;
}

MYSQL_BIND * bmx_mysql_getParams(MYSQL_STMT *stmt) {
	return stmt->params;
}


int bmx_mysql_stmt_fetch(MYSQL_STMT *stmt) {
	int result = mysql_stmt_fetch(stmt);
	return result;
}

void bmx_mysql_stmt_insert_id(MYSQL_STMT *stmt, BBInt64 * id) {
	*id = mysql_stmt_insert_id(stmt);
}

void bmx_mysql_insert_id(MYSQL * mysql, BBInt64 * id) {
	*id = mysql_insert_id(mysql);
}


}

