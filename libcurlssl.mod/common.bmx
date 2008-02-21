' Copyright (c) 2007,2008 Bruce A Henderson
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

SuperStrict

Import BRL.Blitz
Import Pub.zlib


Import "source.bmx"
Import "consts.bmx"

?linux
Import "-ldl"
Import "-lidn"
Import "-lssl"
Import "-lcrypto"
?win32
Import "-ladvapi32"
Import "-leay32"
Import "-lssleay32"
?macos
Import "-lssl"
Import "-lcrypto"
?


Import "src/include/*.h"
Import "curl_glue.cpp"

Extern

	Function curl_global_init:Int(flags:Int)
	Function curl_easy_init:Byte Ptr()
	Function curl_easy_setopt(handle:Byte Ptr, option:Int, param:Byte Ptr)
	Function curl_easy_perform:Int(handle:Byte Ptr)
	Function curl_easy_cleanup(handle:Byte Ptr)
	Function curl_easy_reset(handle:Byte Ptr)
	Function curl_easy_strerror:Byte Ptr(code:Int)
	Function curl_slist_free_all(slist:Byte Ptr)
	Function curl_easy_escape:Byte Ptr(handle:Byte Ptr, s:Byte Ptr, length:Int)
	Function curl_free(handle:Byte Ptr)
	Function curl_easy_unescape:Byte Ptr(handle:Byte Ptr, txt:Byte Ptr, inlength:Int, outlength:Int Ptr)
	Function curl_slist_append:Byte Ptr(slist:Byte Ptr, text:Byte Ptr)
	
	Function curl_multi_init:Byte Ptr()
	Function curl_multi_cleanup(handle:Byte Ptr)
	Function curl_multi_remove_handle:Int(handle:Byte Ptr, easy:Byte Ptr)
	Function curl_multi_add_handle:Int(handle:Byte Ptr, easy:Byte Ptr)
	Function curl_multi_perform:Int(handle:Byte Ptr, running:Int Ptr)
	Function curl_multi_info_read:Byte Ptr(handle:Byte Ptr, queuesize:Int Ptr)
	
	Function bmx_curl_easy_setopt_long:Int(handle:Byte Ptr, option:Int, param:Int)
	Function bmx_curl_easy_setopt_str:Int(handle:Byte Ptr, option:Int, param:Byte Ptr)
	Function bmx_curl_easy_setopt_obj:Int(handle:Byte Ptr, option:Int, param:Object)
	Function bmx_curl_easy_setopt_bbint64:Int(handle:Byte Ptr, option:Int, param:Long)
	
	Function bmx_curl_new_httppostPtr:Byte Ptr()
	Function bmx_curl_formadd_name_content(httppostPtr:Byte Ptr, name:Byte Ptr, content:Byte Ptr)
	Function bmx_curl_formadd_name_content_type(httppostPtr:Byte Ptr, name:Byte Ptr, content:Byte Ptr, t:Byte Ptr)
	Function bmx_curl_formadd_name_file(httppostPtr:Byte Ptr, name:Byte Ptr, file:Byte Ptr, kind:Int)
	Function bmx_curl_formadd_name_file_type(httppostPtr:Byte Ptr, name:Byte Ptr, file:Byte Ptr, t:Byte Ptr, kind:Int)
	Function bmx_curl_formadd_name_buffer(httppostPtr:Byte Ptr, name:Byte Ptr, bname:Byte Ptr, buffer:Byte Ptr, length:Int)
	Function bmx_curl_httppostPtr_delete(httppostPtr:Byte Ptr)
	
	Function bmx_curl_easy_setopt_httppost(handle:Byte Ptr, httppostPtr:Byte Ptr)
	
	Function bmx_curl_easy_getinfo_string:Int(handle:Byte Ptr, option:Int, b:Byte Ptr)
	Function bmx_curl_easy_getinfo_long:Int(handle:Byte Ptr, option:Int, value:Int Ptr)
	Function bmx_curl_easy_getinfo_double:Int(handle:Byte Ptr, option:Int, value:Double Ptr)
	Function bmx_curl_easy_getinfo_obj:Object(handle:Byte Ptr, option:Int, error:Int Ptr)
	Function bmx_curl_easy_getinfo_slist:Byte Ptr(handle:Byte Ptr, option:Int, error:Int Ptr)
	
	Function bmx_curl_multiselect:Int(handle:Byte Ptr, timeout:Double)
	
	Function bmx_curl_CURLMsg_msg:Int(handle:Byte Ptr)
	Function bmx_curl_CURLMsg_result:Int(handle:Byte Ptr)
	Function bmx_curl_CURLMsg_easy_handle:Byte Ptr(handle:Byte Ptr)
	
	Function bmx_curl_get_slist:Byte Ptr(slist:Byte Ptr)
	Function bmx_curl_slist_free(slist:Byte Ptr)
	Function bmx_curl_get_slist_data:Byte Ptr(slist:Byte Ptr)
	Function bmx_curl_get_slist_next:Byte Ptr(slist:Byte Ptr)
	Function bmx_curl_slist_count:Int(slist:Byte Ptr)
	Function bmx_curl_slist_new:Byte Ptr()
	Function bmx_curl_add_element(slist:Byte Ptr, txt:Byte Ptr)
	
	Function bmx_curl_easy_setopt_slist(handle:Byte Ptr, option:Int, slist:Byte Ptr)
	
	Function bmx_curl_multi_setopt_long(handle:Byte Ptr, option:Int, param:Int)
	
End Extern


Function curlProcessSlist:String[](slistPtr:Byte Ptr)
	If slistPtr Then

		Local count:Int = bmx_curl_slist_count(slistPtr)
		Local list:String[] = New String[count]
		
		Local struct:Byte Ptr = bmx_curl_get_slist(slistPtr)
		
		For Local i:Int = 0 Until count
		
			Local s:Byte Ptr
			
			s = bmx_curl_get_slist_data(struct)
		
			If s Then
			
				list[i] = String.fromCString(s)
				
			End If
			
			struct = bmx_curl_get_slist_next(struct)
			
		Next
		
		bmx_curl_slist_free(slistPtr)
		
		Return list[..count]

	End If
	Return Null
End Function
