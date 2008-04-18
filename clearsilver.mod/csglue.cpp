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

extern "C" {

#include <cs/cs.h>
#include <cgi/cgi.h>
#include <string.h>


	HDF * bmx_hdf_new();
	STRING * bmx_hdf_dump_str(HDF * hdf);
	char * bmx_string_buf(STRING * str);
	int bmx_string_len(STRING * str);
	void bmx_string_clear(STRING * str);
	
	CSPARSE * bmx_cs_new(HDF * hdf);
	STRING * bmx_cs_render(CSPARSE * cs);
	
	void bmx_cs_parse_string(CSPARSE * cs, char * content, int len);
	
	CGI * bmx_cgi_new();
	HDF * bmx_cgi_hdf(CGI * cgi);
	
	void bmx_free(STRING * s);
	
	NEOERR * bmx_nerr_raise(NERR_TYPE error);
	
	void bmx_cgi_setuploadcb(CGI * cgi, UPLOAD_CB upload_cb);
}


HDF * bmx_hdf_new() {
	
	HDF *hdf = NULL;
	NEOERR *err;

 	err = hdf_init(&hdf);

	if (err != STATUS_OK) {
		nerr_log_error(err);
		return 0;
	}

	return hdf;
}

STRING * bmx_hdf_dump_str(HDF * hdf) {
	NEOERR *err;
	STRING * str = new STRING;
	
	string_init(str);

	err = hdf_dump_str(hdf, NULL, 0, str);
	
	if (err != STATUS_OK) {
		nerr_log_error(err);
		string_clear(str);
		delete str;
		return 0;
	}
	
	return str;
}


char * bmx_string_buf(STRING * str) {
	return str->buf;
}

int bmx_string_len(STRING * str) {
	return str->len;
}

void bmx_string_clear(STRING * str) {
	string_clear(str);
	delete str;
}

CSPARSE * bmx_cs_new(HDF * hdf) {

	CSPARSE *cs = NULL;
	NEOERR *err;
	
	err = cs_init(&cs, hdf);
	if (err != STATUS_OK) {
		return 0;
	}
	
	err = cgi_register_strfuncs(cs);
	if (err != STATUS_OK) {
		return 0;
	}
	
	return cs;

}

static NEOERR *render_cb(void *ctx, char *buf) {
  STRING *str= (STRING *)ctx;

  return nerr_pass(string_append(str, buf));
}

STRING * bmx_cs_render(CSPARSE * cs) {

	STRING * str = new STRING;
	NEOERR *err;

	int ws_strip_level = 0;
	int do_debug = 0;
	
	do_debug = hdf_get_int_value(cs->hdf, "ClearSilver.DisplayDebug", 0);
	ws_strip_level = hdf_get_int_value(cs->hdf, "ClearSilver.WhiteSpaceStrip", 0);
	
	string_init(str);
	err = cs_render(cs, str, render_cb);
	if (err) { 
		string_clear(str);
		delete str;
		return 0; 
	}
	
	if (ws_strip_level) {
		cgi_html_ws_strip(str, ws_strip_level);
	}
	
	if (do_debug) {
		do {
			err = string_append (str, "<hr>");
			if (err != STATUS_OK) break;
			err = string_append (str, "<pre>");
			if (err != STATUS_OK) break;
			err = hdf_dump_str (cs->hdf, NULL, 0, str);
			if (err != STATUS_OK) break;
			err = string_append (str, "</pre>");
			if (err != STATUS_OK) break;
		} while (0);
		if (err) { 
			string_clear(str);
			delete str;
			return 0; 
		}
	}
	
	return str;
}

void bmx_cs_parse_string(CSPARSE * cs, char * content, int len) {
	char * ms = strdup(content);
	cs_parse_string(cs, ms, len);
}

CGI * bmx_cgi_new() {
	
	CGI *cgi = NULL;
	NEOERR *err;

 	err = cgi_init(&cgi, NULL);

	if (err != STATUS_OK) {
		/* cgi_neo_error renders a NEOERR as an error CGI result */
		cgi_neo_error(cgi, err);
		/* nerr_log_error logs the error to stderr and cleans up */
		nerr_log_error(err);
		return 0;
	}

	return cgi;
}

HDF * bmx_cgi_hdf(CGI * cgi) {
	return cgi->hdf;
}

void bmx_free(STRING * s) {
	free(s);
}

NEOERR * bmx_nerr_raise(NERR_TYPE error) {
	return nerr_raise(error, NULL);
}

void bmx_cgi_setuploadcb(CGI * cgi, UPLOAD_CB upload_cb) {
	cgi->upload_cb = upload_cb;
}


