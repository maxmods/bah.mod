' Copyright (c) 2007-2013 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
'
SuperStrict

Import Pub.ZLib
Import BaH.StreamHtmlParser

Import "src/*.h"
Import "../streamhtmlparser.mod/src/*.h"

' util

?win32
Import "src/util/hsregex/*.h"
Import "src/util/hsregex/regexec.c"
Import "src/util/hsregex/regcomp.c"
Import "src/util/hsregex/regfree.c"
Import "src/util/hsregex/regerror.c"
?linux
Import "src/util/dict.c"
Import "src/util/skiplist.c"
Import "src/util/filter.c"
Import "src/util/neo_server.c"
Import "src/util/ulocks.c"
?macos
Import "src/util/dict.c"
Import "src/util/skiplist.c"
Import "src/util/filter.c"
Import "src/util/neo_server.c"
Import "src/util/ulocks.c"
?

Import "src/util/neo_auto.c"
Import "src/util/neo_date.c"
Import "src/util/neo_hash.c"
Import "src/util/neo_str.c"
Import "src/util/snprintf.c"
'Import "src/util/wdb.c"
Import "src/util/neo_err.c"
Import "src/util/neo_hdf.c"
Import "src/util/neo_rand.c"
Import "src/util/rcfs.c"
Import "src/util/ulist.c"
Import "src/util/wildmat.c"
Import "src/util/missing.c"
Import "src/util/neo_files.c"
Import "src/util/neo_misc.c"

' cgi
Import "src/cgi/cgi.c"
Import "src/cgi/cgiwrap.c"
Import "src/cgi/date.c"
Import "src/cgi/html.c"
Import "src/cgi/rfc2388.c"
'Import "src/cgi/static.c"

' cs
Import "src/cs/csparse.c"

Import "csglue.cpp"

Extern

	Function bmx_hdf_new:Byte Ptr()
	
	Function hdf_read_file:Byte Ptr(hdf:Byte Ptr, filename:Byte Ptr)
	Function hdf_destroy(hdf:Byte Ptr)
	Function hdf_set_value:Byte Ptr(hdf:Byte Ptr, name:Byte Ptr, value:Byte Ptr)

	Function bmx_hdf_dump_str:Byte Ptr(hdf:Byte Ptr)
	Function bmx_string_buf:Byte Ptr(s:Byte Ptr)
	Function bmx_string_len:Int(s:Byte Ptr)
	Function bmx_string_clear(s:Byte Ptr)
	
	Function hdf_get_value:Byte Ptr(hdf:Byte Ptr, name:Byte Ptr, defaultValue:Byte Ptr)
	Function hdf_set_symlink(hdf:Byte Ptr, src:Byte Ptr, dest:Byte Ptr)
	Function hdf_get_int_value:Int(hdf:Byte Ptr, name:Byte Ptr, defaultValue:Int)
	
	Function bmx_cs_new:Byte Ptr(hdf:Byte Ptr)
	Function bmx_cs_parse_string(cs:Byte Ptr, content:Byte Ptr, size:Int)
	Function bmx_cs_render:Byte Ptr(cs:Byte Ptr)
	
	Function cs_parse_file(cs:Byte Ptr, filename:Byte Ptr)
	Function cs_destroy(cs:Byte Ptr)
	
	Function hdf_read_string(hdf:Byte Ptr, data:Byte Ptr)
	Function hdf_write_file(hdf:Byte Ptr, filename:Byte Ptr)
	Function hdf_write_string(hdf:Byte Ptr, s:Byte Ptr)

	Function hdf_remove_tree(hdfPtr:Byte Ptr, hdfName:Byte Ptr)
	Function hdf_copy(hdfPtr:Byte Ptr, path:Byte Ptr, src:Byte Ptr)
	Function hdf_get_obj:Byte Ptr(hdf:Byte Ptr, path:Byte Ptr)
	Function hdf_get_child:Byte Ptr(hdf:Byte Ptr, path:Byte Ptr)
	Function hdf_obj_name:Byte Ptr(hdf:Byte Ptr)
	Function hdf_obj_value:Byte Ptr(hdf:Byte Ptr)
	Function hdf_obj_child:Byte Ptr(hdf:Byte Ptr)
	Function hdf_obj_next:Byte Ptr(hdf:Byte Ptr)
	
	Function hdf_set_int_value(hdf:Byte Ptr, name:Byte Ptr, value:Int)
	
	Function bmx_cgi_new:Byte Ptr()
	Function bmx_cgi_hdf:Byte Ptr(cgi:Byte Ptr)
	
	Function bmx_free(s:Byte Ptr)
	
	Function cgi_url_unescape:Byte Ptr(text:Byte Ptr)
	Function cgi_url_escape(text:Byte Ptr, escaped:Byte Ptr)
	Function cgi_url_escape_more(text:Byte Ptr, escaped:Byte Ptr, other:Byte Ptr)
	
	Function cgi_cookie_clear(cgi:Byte Ptr, name:Byte Ptr, domain:Byte Ptr, path:Byte Ptr)
	Function cgi_cookie_set(cgi:Byte Ptr, name:Byte Ptr, value:Byte Ptr, path:Byte Ptr, domain:Byte Ptr, ..
		timeStr:Byte Ptr, persistent:Int, secure:Int)
	Function cgi_cookie_authority:Byte Ptr(cgi:Byte Ptr, host:Byte Ptr)
	Function cgi_display(cgi:Byte Ptr, file:Byte Ptr)
	Function cgi_redirect(cgi:Byte Ptr, fmt:Byte Ptr, path:Byte Ptr)
	Function cgi_redirect_uri(cgi:Byte Ptr, fmt:Byte Ptr, uri:Byte Ptr)
	
	Function cgi_register_parse_cb:Byte Ptr(cgiPtr:Byte Ptr, httpMethod:Byte Ptr, ctype:Byte Ptr, rock:Object, callback:Byte Ptr(cgiPtr:Byte Ptr, httpMethod:Byte Ptr, ctype:Byte Ptr, rock:Object))

	Function cgi_filehandle:Int(cgi:Byte Ptr, formName:Byte Ptr)
	Function cgiwrap_read(buffer:Byte Ptr, buflen:Int, read_len:Int Ptr)
	Function cgi_parse(cgi:Byte Ptr)

	Function bmx_cgi_setuploadcb(cgi:Byte Ptr, callback:Int(cgi:Byte Ptr, nread:Int, expected:Int))
	Function bmx_nerr_raise:Byte Ptr(errType:Int)
	
	Global NERR_PASS:Int
	Global NERR_ASSERT:Int
	Global NERR_NOT_FOUND:Int
	Global NERR_DUPLICATE:Int
	Global NERR_NOMEM:Int
	Global NERR_PARSE:Int
	Global NERR_OUTOFRANGE:Int
	Global NERR_SYSTEM:Int
	Global NERR_IO:Int
	Global NERR_LOCK:Int
	Global NERR_DB:Int
	Global NERR_EXISTS:Int
	
	Global CGIFinished:Int
	Global CGIUploadCancelled:Int
	Global CGIParseNotHandled:Int
	
End Extern

