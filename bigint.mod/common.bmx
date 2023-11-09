' Copyright (c) 2013 Bruce A Henderson
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

Import "source.bmx"


Extern

	Function bmx_bigint_BigUnsigned_create:Byte Ptr(value:String)
	Function bmx_bigint_BigUnsigned_free(handle:Byte Ptr)
	Function bmx_bigint_BigUnsigned_CreateWithInt:Byte Ptr(value:Int)

	Function bmx_bigint_BigUnsigned_ToString:String(handle:Byte Ptr)

	Function bmx_bigint_BigUnsigned_add(target:Byte Ptr, value:Byte Ptr)
	Function bmx_bigint_BigUnsigned_divide(target:Byte Ptr, value:Byte Ptr)



	Function bmx_bigint_BigInteger_create:Byte Ptr(value:String)
	Function bmx_bigint_BigInteger_free(handle:Byte Ptr)
	Function bmx_bigint_BigInteger_CreateWithInt:Byte Ptr(value:Int)

	Function bmx_bigint_BigInteger_ToString:String(handle:Byte Ptr)

	Function bmx_bigint_BigInteger_equal:Int(target:Byte Ptr, value:Byte Ptr)
	Function bmx_bigint_BigInteger_notequal:Int(target:Byte Ptr, value:Byte Ptr)
	Function bmx_bigint_BigInteger_lessThan:Int(target:Byte Ptr, value:Byte Ptr)
	Function bmx_bigint_BigInteger_lessThanOrEqual:Int(target:Byte Ptr, value:Byte Ptr)
	Function bmx_bigint_BigInteger_greaterThan:Int(target:Byte Ptr, value:Byte Ptr)
	Function bmx_bigint_BigInteger_greaterThanOrEqual:Int(target:Byte Ptr, value:Byte Ptr)

	Function bmx_bigint_BigInteger_add(target:Byte Ptr, value:Byte Ptr)
	Function bmx_bigint_BigInteger_subtract(target:Byte Ptr, value:Byte Ptr)
	Function bmx_bigint_BigInteger_divide(target:Byte Ptr, value:Byte Ptr)
	Function bmx_bigint_BigInteger_multiply(target:Byte Ptr, value:Byte Ptr)
	Function bmx_bigint_BigInteger_modulo(target:Byte Ptr, value:Byte Ptr)

	Function bmx_bigint_new_BigInteger_add:Byte Ptr(target:Byte Ptr, value:Byte Ptr)
	Function bmx_bigint_new_BigInteger_subtract:Byte Ptr(target:Byte Ptr, value:Byte Ptr)
	Function bmx_bigint_new_BigInteger_divide:Byte Ptr(target:Byte Ptr, value:Byte Ptr)
	Function bmx_bigint_new_BigInteger_multiply:Byte Ptr(target:Byte Ptr, value:Byte Ptr)
	Function bmx_bigint_new_BigInteger_modulo:Byte Ptr(target:Byte Ptr, value:Byte Ptr)

End Extern


