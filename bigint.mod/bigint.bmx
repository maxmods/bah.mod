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

Module BaH.BigInt

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: BigInt - Placed into the Public Domain by Matt McCutchen"
ModuleInfo "Copyright: Wrapper - 2013 Bruce A Henderson"

ModuleInfo "CC_OPTS: -fexceptions"

Import "common.bmx"

Rem
bbdoc: A TBigUnsigned object represents a nonnegative integer of size limited only by available memory.
End Rem
Type TBigUnsigned

	Field bigPtr:Byte Ptr


	Method New(value:String)
		Create(value)
	End Method

	Method New(value:Int)
		CreateWithInt(value)
	End Method


	
	Method Create:TBigUnsigned(value:String = "")
		bigPtr = bmx_bigint_BigUnsigned_create(value)
	End Method
	
	Method CreateWithInt:TBigUnsigned(value:Int)
		bigPtr = bmx_bigint_BigUnsigned_CreateWithInt(value)
	End Method
	
	Method IsZero:Int()
	End Method
	
	Method Equals:Int(value:TBigUnsigned)
	End Method
	
	Method LessThan:Int(value:TBigUnsigned)
	End Method
	
	Method GreaterThan:Int(value:TBigUnsigned)
	End Method


	Method Add:TBigUnsigned(value:TBigUnsigned)
		bmx_bigint_BigUnsigned_add(bigPtr, value.bigPtr)
		return self
	End Method


	Method Divide:TBigUnsigned(value:TBigUnsigned)
		bmx_bigint_BigUnsigned_divide(bigPtr, value.bigPtr)
		return self
	End Method

	
	
	Method ToString:String()
		Return bmx_bigint_BigUnsigned_ToString(bigPtr)
	End Method
	
	Method Delete()
		If bigPtr Then
			bmx_bigint_BigUnsigned_free(bigPtr)
			bigPtr = Null
		End If
	End Method
	
End Type





Rem
bbdoc: A TBigUnsigned object represents an integer of size limited only by available memory.
End Rem
Type TBigInteger

	Field bigPtr:Byte Ptr

	
	Method New(value:String)
		Create(value)
	End Method

	Method New(value:Int)
		CreateWithInt(value)
	End Method

	
	
	Method Create:TBigInteger(value:String = "")
		bigPtr = bmx_bigint_BigInteger_create(value)
	End Method
	
	Method CreateWithInt:TBigInteger(value:Int)
		bigPtr = bmx_bigint_BigInteger_CreateWithInt(value)
	End Method
	
	Method IsZero:Int()
	End Method
	
	Method Equals:Int(value:TBigInteger)
		Return bmx_bigint_BigInteger_equal(bigPtr, value.bigPtr)
	End Method
	
	Method LessThan:Int(value:TBigInteger)
		Return bmx_bigint_BigInteger_lessThan(bigPtr, value.bigPtr)
	End Method
	
	Method LessThanOrEqual:Int(value:TBigInteger)
		Return bmx_bigint_BigInteger_lessThanOrEqual(bigPtr, value.bigPtr)
	End Method
	
	Method GreaterThan:Int(value:TBigInteger)
		Return bmx_bigint_BigInteger_greaterThan(bigPtr, value.bigPtr)
	End Method

	Method GreaterThanOrEqual:Int(value:TBigInteger)
		Return bmx_bigint_BigInteger_greaterThanOrEqual(bigPtr, value.bigPtr)
	End Method


	Method Add:TBigInteger(value:TBigInteger)
		bmx_bigint_BigInteger_add(bigPtr, value.bigPtr)
		return self
	End Method


	Method Subtract:TBigInteger(value:TBigInteger)
		bmx_bigint_BigInteger_add(bigPtr, value.bigPtr)
		return self
	End Method


	Method Modulo:TBigInteger(value:TBigInteger)
		bmx_bigint_BigInteger_modulo(bigPtr, value.bigPtr)
		return self
	End Method


	Method Divide:TBigInteger(value:TBigInteger)
		bmx_bigint_BigInteger_divide(bigPtr, value.bigPtr)
		return self
	End Method


	Method Multiply:TBigInteger(value:TBigInteger)
		bmx_bigint_BigInteger_multiply(bigPtr, value.bigPtr)
		return self
	End Method



	Method CreateAddResult:TBigInteger(value:TBigInteger)
		Local result:TBigInteger = new TBigInteger()
		result.bigPtr = bmx_bigint_new_BigInteger_add(bigPtr, value.bigPtr)
		return result
	End Method


	Method CreateSubtractResult:TBigInteger(value:TBigInteger)
		Local result:TBigInteger = new TBigInteger()
		result.bigPtr = bmx_bigint_new_BigInteger_subtract(bigPtr, value.bigPtr)
		return result
	End Method


	Method CreateModuloResult:TBigInteger(value:TBigInteger)
		Local result:TBigInteger = new TBigInteger()
		result.bigPtr = bmx_bigint_new_BigInteger_modulo(bigPtr, value.bigPtr)
		return result
	End Method


	Method CreateDivideResult:TBigInteger(value:TBigInteger)
		Local result:TBigInteger = new TBigInteger()
		result.bigPtr = bmx_bigint_new_BigInteger_divide(bigPtr, value.bigPtr)
		return result
	End Method


	Method CreateMultiplyResult:TBigInteger(value:TBigInteger)
		Local result:TBigInteger = new TBigInteger()
		result.bigPtr = bmx_bigint_new_BigInteger_multiply(bigPtr, value.bigPtr)
		return result
	End Method


	'add value directly
    Method Operator :+(value:TBigInteger)
		Add(value)
    End Method


	'add value and return sum!
    Method Operator +:TBigInteger(value:TBigInteger)
		Return CreateAddResult(value)
    End Method


	'subtract value directly
    Method Operator :-(value:TBigInteger)
		Subtract(value)
    End Method


	'subtract value and return difference!
    Method Operator -:TBigInteger(value:TBigInteger)
		Return CreateSubtractResult(value)
    End Method


	'multiply value directly
    Method Operator :*(value:TBigInteger)
		Multiply(value)
    End Method


	'multiply value and return product!
    Method Operator *:TBigInteger(value:TBigInteger)
		Return CreateMultiplyResult(value)
    End Method


	'divide value directly
    Method Operator :/(value:TBigInteger)
		Divide(value)
    End Method


	'divide value and return quotient!
    Method Operator /:TBigInteger(value:TBigInteger)
		Return CreateDivideResult(value)
    End Method


	'divide value and return remainder / modulo!
    Method Operator :mod(value:TBigInteger)
		Modulo(value)
    End Method


	'divide value and return remainder / modulo!
    Method Operator mod:TBigInteger(value:TBigInteger)
		Return CreateModuloResult(value)
    End Method


    Method Operator >:Int(value:TBigInteger)
		Return GreaterThan(value)
    End Method


    Method Operator >=:Int(value:TBigInteger)
		Return GreaterThanOrEqual(value)
    End Method


    Method Operator <:Int(value:TBigInteger)
		Return LessThan(value)
    End Method


    Method Operator <=:Int(value:TBigInteger)
		Return LessThanOrEqual(value)
    End Method


    Method Operator =:Int(value:TBigInteger)
		Return Equals(value)
    End Method

    Method Operator <>:Int(value:TBigInteger)
		Return not Equals(value)
    End Method
    	
	
	Method ToString:String()
		Return bmx_bigint_BigInteger_ToString(bigPtr)
	End Method
	
	Method Delete()
		If bigPtr Then
			bmx_bigint_BigInteger_free(bigPtr)
			bigPtr = Null
		End If
	End Method
	
End Type

