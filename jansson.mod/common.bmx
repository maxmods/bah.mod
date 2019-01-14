' Copyright (c) 2014-2019 Bruce A Henderson
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

	Function bmx_json_string_nocheck:Byte Ptr(Text:String)
	Function bmx_json_string_value:String(handle:Byte Ptr)
	
	Function bmx_json_array_get:Object(handle:Byte Ptr, index:Int)
	Function json_array_clear:Int(handle:Byte Ptr)
	Function json_array_remove:Int(handle:Byte Ptr, index:Int)
	Function bmx_json_array_size:Int(handle:Byte Ptr)
	Function bmx_json_array_set:Int(handle:Byte Ptr, index:Int, value:Byte Ptr)
	Function bmx_json_array_append:Int(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_json_array_insert:Int(handle:Byte Ptr, index:Int, value:Byte Ptr)
	
	Function bmx_json_dumps:String(handle:Byte Ptr, flags:Int, indent:Int, precision:Int)
?bmxng
	Function bmx_json_dump_callback:Int(handle:Byte Ptr, callback:Size_T(buffer:Byte Ptr, size:Size_T, data:TStream), stream:TStream, flags:Int, indent:Int, precision:Int)
	Function bmx_json_load_callback:Object(callback:Size_T(buffer:Byte Ptr, size:Size_T, data:TStream), Text:TStream, flags:Int)
?Not bmxng
	Function bmx_json_dump_callback:Int(handle:Byte Ptr, callback:Int(buffer:Byte Ptr, size:Int, data:TStream), stream:TStream, flags:Int, indent:Int, precision:Int)
	Function bmx_json_load_callback:Object(callback:Int(buffer:Byte Ptr, size:Int, data:TStream), Text:TStream, flags:Int)
?
	Function bmx_json_loads:Object(Text:String, flags:Int)
	
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


Const JSON_MAX_INDENT:Int = $1F
Rem
bbdoc: Enables a compact representation, i.e. sets the separator between array and object items to "," and between object keys and values to ":".
about: Without this flag, the corresponding separators are ", " and ": " for more readable output.
End Rem
Const JSON_COMPACT:Int = $20
Rem
bbdoc: Guarantees the output to consist only of ASCII characters.
about: This is achived by escaping all Unicode characters outside the ASCII range.
End Rem
Const JSON_ENSURE_ASCII:Int = $40
Rem
bbdoc: All the objects in output are sorted by key.
about: This is useful e.g. if two JSON texts are diffed or visually compared.
End Rem
Const JSON_SORT_KEYS:Int = $80
Rem
bbdoc: Object keys in the output are sorted into the same order in which they were first inserted to the object.
about: For example, decoding a JSON text and then encoding with this flag preserves the order of object keys.
End Rem
Const JSON_PRESERVE_ORDER:Int = $100
Rem
bbdoc: Makes it possible to encode any JSON value on its own.
about: Without it, only objects and arrays can be passed as the root value to the encoding functions.
Note: Encoding any value may be useful in some scenarios, but it's generally discouraged as it violates strict compatiblity with
RFC 4627. If you use this flag, don't expect interoperatibility with other JSON systems.
End Rem
Const JSON_ENCODE_ANY:Int = $200
Rem
bbdoc: Escapes the / characters in strings with \/.
End Rem
Const JSON_ESCAPE_SLASH:Int = $400
Rem
bbdoc: The opening and closing characters of the top-level array ('[', ']') or object ('{', '}') are omitted during encoding.
about: This flag is useful when concatenating multiple arrays or objects into a stream.
End Rem
Const JSON_EMBED:Int = $10000

Rem
bbdoc: Pretty-prints the result, using newlines between array and object items, and indenting with n spaces.
about: The valid range for n is between 0 and 31 (inclusive), other values result in an undefined output. If JSON_INDENT is not used or n is 0,
no newlines are inserted between array and object items.
End Rem
Function JSON_INDENT:Int(n:Int)
	Return n & JSON_MAX_INDENT
End Function

Rem
bbdoc: Outputs all real numbers with at most n digits of precision.
about: The valid range for n is between 0 and 31 (inclusive), and other values result in an undefined behavior.
By default, the precision is 17, to correctly and losslessly encode all IEEE 754 double precision floating point numbers.
End Rem
Function JSON_REAL_PRECISION:Int(n:Int)
	Return (n & $1F) Shl 11
End Function

Rem
bbdoc: Issues a decoding error if any JSON object in the input text contains duplicate keys.
about: Without this flag, the value of the last occurence of each key ends up in the result.
Key equivalence is checked byte-by-byte, without special Unicode comparison algorithms.
End Rem
Const JSON_REJECT_DUPLICATES:Int = $1
Rem
bbdoc: With this flag enabled, the decoder stops after decoding a valid JSON array or object, and thus allows extra data after the JSON text. 
about: By default, the decoder expects that its whole input constitutes a valid JSON text, and issues an error if there's extra data after the otherwise valid JSON input.
Normally, reading will stop when the last ] or } in the JSON input is encountered. If both JSON_DISABLE_EOF_CHECK and JSON_DECODE_ANY flags
are used, the decoder may read one extra UTF-8 code unit (up to 4 bytes of input). For example, decoding 4true correctly decodes
the integer 4, but also reads the t. For this reason, if reading multiple consecutive values that are not arrays or objects,
they should be separated by at least one whitespace character.
End Rem
Const JSON_DISABLE_EOF_CHECK:Int = $2
Rem
bbdoc: With this flag enabled, the decoder accepts any valid JSON value.
about: By default, the decoder expects an array or object as the input.
Note: Decoding any value may be useful in some scenarios, but it's generally discouraged as it violates strict compatiblity
with RFC 4627. If you use this flag, don't expect interoperatibility with other JSON systems.
End Rem
Const JSON_DECODE_ANY:Int = $4
Rem
bbdoc: With this flag enabled the decoder interprets all numbers as real values. 
about: JSON defines only one number type. Jansson distinguishes between ints and reals. For more information see Real vs. Integer. Integers that do
not have an exact double representation will silently result in a loss of precision. Integers that cause a double overflow will cause an error.
End Rem
Const JSON_DECODE_INT_AS_REAL:Int = $8
Rem
bbdoc: Allows \u0000 escape inside string values.
about: This is a safety measure; If you know your input can contain NUL bytes, use this flag. If you don't use this flag, you don't have
to worry about NUL bytes inside strings.
Object keys cannot have embedded NUL bytes even if this flag is used.
End Rem
Const JSON_ALLOW_NUL:Int = $10

Const JSON_ERROR_UNKNOWN:Int = 0
Const JSON_ERROR_OUT_OF_MEMORY:Int = 1
Const JSON_ERROR_STACK_OVERFLOW:Int = 2
Const JSON_ERROR_CANNOT_OPEN_FILE:Int = 3
Const JSON_ERROR_INVALID_ARGUMENT:Int = 4
Const JSON_ERROR_INVALID_UTF8:Int = 5
Const JSON_ERROR_PREMATURE_END_OF_INPUT:Int = 6
Const JSON_ERROR_END_OF_INPUT_EXPECTED:Int = 7
Const JSON_ERROR_INVALID_SYNTAX:Int = 8
Const JSON_ERROR_INVALID_FORMAT:Int = 9
Const JSON_ERROR_WRONG_TYPE:Int = 10
Const JSON_ERROR_NULL_CHARACTER:Int = 11
Const JSON_ERROR_NULL_VALUE:Int = 12
Const JSON_ERROR_NULL_BYTE_IN_KEY:Int = 13
Const JSON_ERROR_DUPLICATE_KEY:Int = 14
Const JSON_ERROR_NUMERIC_OVERFLOW:Int = 15
Const JSON_ERROR_ITEM_NOT_FOUND:Int = 16
Const JSON_ERROR_INDEX_OUT_OF_RANGE:Int = 17
