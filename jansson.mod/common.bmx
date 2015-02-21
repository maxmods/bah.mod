' Copyright (c) 2014 Bruce A Henderson
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

Import BRL.Stream

Import "source.bmx"


Extern

	Function json_array:Byte Ptr()
	
	Function bmx_json_decref(handle:Byte Ptr)

	Function bmx_json_string_nocheck:Byte Ptr(text:String)
	Function bmx_json_string_value:String(handle:Byte Ptr)
	
	Function bmx_json_array_get:Object(handle:Byte Ptr, index:Int)
	Function json_array_clear:Int(handle:Byte Ptr)
	Function json_array_remove:Int(handle:Byte Ptr, index:Int)
	Function bmx_json_array_size:Int(handle:Byte Ptr)
	Function bmx_json_array_set:Int(handle:Byte Ptr, index:Int, value:Byte Ptr)
	Function bmx_json_array_append:Int(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_json_array_insert:Int(handle:Byte Ptr, index:Int, value:Byte Ptr)
	
	Function bmx_json_dumps:String(handle:Byte Ptr, flags:Int, indent:Int, precision:Int)
	Function bmx_json_loads:Object(text:String, flags:Int)
	
	Function bmx_json_integer:Byte Ptr(v:Long)
	Function bmx_json_integer_value(handle:Byte Ptr, v:Long Ptr)
	Function bmx_json_integer_set:Int(handle:Byte Ptr, v:Long)
	
	Function json_real:Byte Ptr(v:Double)
	Function json_real_value:Double(handle:Byte Ptr)
	Function json_real_set:Int(handle:Byte Ptr, v:Double)
	
	Function json_object:Byte Ptr()
	Function bmx_json_object_size:Int(handle:Byte Ptr)
	Function bmx_json_object_get:Object(handle:Byte Ptr, key:String)
	Function bmx_json_object_set_nocheck:Int(handle:Byte Ptr, key:String, value:Byte Ptr)
	Function bmx_json_object_del:Int(handle:Byte Ptr, key:String)
	Function json_object_clear:Int(handle:Byte Ptr)
	Function json_object_update:Int(handle:Byte Ptr, other:Byte Ptr)
	Function json_object_update_existing:Int(handle:Byte Ptr, other:Byte Ptr)
	Function json_object_update_missing:Int(handle:Byte Ptr, other:Byte Ptr)
	
	Function json_object_iter:Byte Ptr(handle:Byte Ptr)
	Function json_object_iter_next:Byte Ptr(handle:Byte Ptr, iter:Byte Ptr)
	Function bmx_json_object_iter_value:Object(iter:Byte Ptr)
	
End Extern


Const JSON_TYPE_OBJECT:Int = 0
Const JSON_TYPE_ARRAY:Int = 1
Const JSON_TYPE_STRING:Int = 2
Const JSON_TYPE_INTEGER:Int = 3
Const JSON_TYPE_REAL:Int = 4
Const JSON_TYPE_TRUE:Int = 5
Const JSON_TYPE_FALSE:Int = 6
Const JSON_TYPE_NULL:Int = 7
