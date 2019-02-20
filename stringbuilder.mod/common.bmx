' Copyright (c) 2016-2019 Bruce A Henderson
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

Import "glue.c"
?Not bmxng
Import "uni_conv.c"
?

Extern
	Function bmx_stringbuilder_new:Byte Ptr(initial:Int)
	Function bmx_stringbuilder_free(buffer:Byte Ptr)

	Function bmx_stringbuilder_count:Int(buffer:Byte Ptr)
	Function bmx_stringbuilder_capacity:Int(buffer:Byte Ptr)
	Function bmx_stringbuilder_setlength(buffer:Byte Ptr, length:Int)
	Function bmx_stringbuilder_tostring:String(buffer:Byte Ptr)
	Function bmx_stringbuilder_append_string(buffer:Byte Ptr, value:String)
	Function bmx_stringbuilder_remove(buffer:Byte Ptr, startIndex:Int, endIndex:Int)
	Function bmx_stringbuilder_insert(buffer:Byte Ptr, offset:Int, value:String)
	Function bmx_stringbuilder_reverse(buffer:Byte Ptr)
	Function bmx_stringbuilder_substring:String(buffer:Byte Ptr, beginIndex:Int, endIndex:Int)
	Function bmx_stringbuilder_append_stringbuffer(buffer:Byte Ptr, buffer2:Byte Ptr)
	Function bmx_stringbuilder_startswith:Int(buffer:Byte Ptr, subString:String)
	Function bmx_stringbuilder_endswith:Int(buffer:Byte Ptr, subString:String)
	Function bmx_stringbuilder_find:Int(buffer:Byte Ptr, subString:String, startIndex:Int)
	Function bmx_stringbuilder_findlast:Int(buffer:Byte Ptr, subString:String, startIndex:Int)
	Function bmx_stringbuilder_tolower(buffer:Byte Ptr)
	Function bmx_stringbuilder_toupper(buffer:Byte Ptr)
	Function bmx_stringbuilder_trim(buffer:Byte Ptr)
	Function bmx_stringbuilder_replace(buffer:Byte Ptr, subString:String, withString:String)
	Function bmx_stringbuilder_join(buffer:Byte Ptr, bits:String[], newBuffer:Byte Ptr)
	Function bmx_stringbuilder_split:Byte Ptr(buffer:Byte Ptr, separator:String)
	Function bmx_stringbuilder_setcharat(buffer:Byte Ptr, index:Int, char:Int)
	Function bmx_stringbuilder_charat:Int(buffer:Byte Ptr, index:Int)
	Function bmx_stringbuilder_removecharat(buffer:Byte Ptr, index:Int)
	Function bmx_stringbuilder_append_cstring(buffer:Byte Ptr, chars:Byte Ptr)
	Function bmx_stringbuilder_append_utf8string(buffer:Byte Ptr, chars:Byte Ptr)

	Function bmx_stringbuilder_splitbuffer_length:Int(splitPtr:Byte Ptr)
	Function bmx_stringbuilder_splitbuffer_text:String(splitPtr:Byte Ptr, index:Int)
	Function bmx_stringbuilder_splitbuffer_free(splitPtr:Byte Ptr)
	Function bmx_stringbuilder_splitbuffer_toarray:String[](splitPtr:Byte Ptr)

End Extern

