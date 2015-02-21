#include "jansson.h"

extern "C" {

#include "blitz.h"

#ifdef BMX_NG
#define CB_PREF(func) func
#else
#define CB_PREF(func) _##func
#endif

	BBObject * CB_PREF(bah_jansson_TJSON__create)(json_t * handle, int jsonType);
	BBObject * CB_PREF(bah_jansson_TJSONError__createError)(BBString * text, BBString * source, int line, int column, int position);
	BBObject * CB_PREF(bah_jansson_TJSONError__createNoError)(BBObject * js);

	void bmx_json_decref(json_t * handle);

	json_t * bmx_json_string_nocheck(BBString * text);
	BBString * bmx_json_string_value(json_t * handle);

	BBObject * bmx_json_array_get(json_t * handle, int index);
	int bmx_json_array_size(json_t * handle);
	int bmx_json_array_set(json_t * handle, int index, json_t * value);
	int bmx_json_array_append(json_t * handle, json_t * value);
	int bmx_json_array_insert(json_t * handle, int index, json_t * value);

	BBString * bmx_json_dumps(json_t * handle, int flags, int indent, int precision);
	BBObject * bmx_json_loads(BBString * text, int flags);

	json_t * bmx_json_integer(BBInt64 v);
	void bmx_json_integer_value(json_t * handle, BBInt64 * v);
	int bmx_json_integer_set(json_t * handle, BBInt64 v);

	int bmx_json_object_size(json_t * handle);
	BBObject * bmx_json_object_get(json_t * handle, BBString * key);
	int bmx_json_object_set_nocheck(json_t * handle, BBString * key, json_t * value);
	int bmx_json_object_del(json_t * handle, BBString * key);
	
	BBObject * bmx_json_object_iter_value(void * iter);
}



void bmx_json_decref(json_t * handle) {
	json_decref(handle);
}


json_t * bmx_json_string_nocheck(BBString * text) {
	char * s = bbStringToUTF8String(text);
	json_t * ref = json_string_nocheck(s);
	bbMemFree(s);
	return ref;
}

BBString * bmx_json_string_value(json_t * handle) {
	return bbStringFromUTF8String(json_string_value(handle));
}

BBObject * bmx_json_array_get(json_t * handle, int index) {
	json_t * value = json_array_get(handle, index);
	if (value) {
		return CB_PREF(bah_jansson_TJSON__create)(json_incref(value), json_typeof(value));
	} else {
		return &bbNullObject;
	}
}

int bmx_json_array_size(json_t * handle) {
	return json_array_size(handle);
}

int bmx_json_array_set(json_t * handle, int index, json_t * value) {
	return json_array_set(handle, index, value);
}

int bmx_json_array_append(json_t * handle, json_t * value) {
	return json_array_append(handle, value);
}

int bmx_json_array_insert(json_t * handle, int index, json_t * value) {
	return json_array_insert(handle, index, value);
}


BBString * bmx_json_dumps(json_t * handle, int flags, int indent, int precision) {
	char * s = json_dumps(handle, flags | JSON_INDENT(indent) | JSON_REAL_PRECISION(precision));
	if (s) {
		BBString * st = bbStringFromUTF8String(s);
		free(s);
		return st;
	} else {
		return &bbEmptyString;
	}
}

BBObject * bmx_json_loads(BBString * text, int flags) {
	char * t = bbStringToUTF8String(text);
	
	json_error_t error;
	json_t * js = json_loads(t, flags, &error);
	
	if (!js) {
		return CB_PREF(bah_jansson_TJSONError__createError)(bbStringFromUTF8String(error.text), bbStringFromUTF8String(error.source),
				error.line, error.column, error.position);
	}
	
	BBObject * ref = CB_PREF(bah_jansson_TJSON__create)(js, json_typeof(js));
	return CB_PREF(bah_jansson_TJSONError__createNoError)(ref);	
}

json_t * bmx_json_integer(BBInt64 v) {
	return json_integer(v);
}

void bmx_json_integer_value(json_t * handle, BBInt64 * v) {
	*v = json_integer_value(handle);
}

int bmx_json_integer_set(json_t * handle, BBInt64 v) {
	return json_integer_set(handle, v);
}

int bmx_json_object_size(json_t * handle) {
	return json_object_size(handle);
}

BBObject * bmx_json_object_get(json_t * handle, BBString * key) {
	char * k = bbStringToUTF8String(key);
	json_t * obj = json_object_get(handle, k);
	bbMemFree(k);
	if (obj) {
		return CB_PREF(bah_jansson_TJSON__create)(json_incref(obj), json_typeof(obj));
	} else {
		return &bbNullObject;
	}
}

int bmx_json_object_set_nocheck(json_t * handle, BBString * key, json_t * value) {
	char * k = bbStringToUTF8String(key);
	int res = json_object_set_nocheck(handle, k, value);
	bbMemFree(k);
	return res;
}

int bmx_json_object_del(json_t * handle, BBString * key) {
	char * k = bbStringToUTF8String(key);
	int res = json_object_del(handle, k);
	bbMemFree(k);
	return res;
}

BBObject * bmx_json_object_iter_value(void * iter) {
	json_t * value = json_object_iter_value(iter);
	if (value) {
		return CB_PREF(bah_jansson_TJSON__create)(json_incref(value), json_typeof(value));
	} else {
		return &bbNullObject;
	}
}
