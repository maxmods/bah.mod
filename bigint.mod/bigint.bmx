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
	
	Method Create:TBigUnsigned(value:String = "")
		Local this:TBigUnsigned = New TBigUnsigned
		this.bigPtr = bmx_bigint_BigUnsigned_create(value)
		Return this
	End Method
	
	Method CreateWithInt:TBigUnsigned(value:Int)
		Local this:TBigUnsigned = New TBigUnsigned
		this.bigPtr = bmx_bigint_BigUnsigned_CreateWithInt(value)
		Return this
	End Method
	
	Method IsZero:Int()
	End Method
	
	Method Equals:Int(value:TBigUnsigned)
	End Method
	
	Method LessThan:Int(value:TBigUnsigned)
	End Method
	
	Method GreaterThan:Int(value:TBigUnsigned)
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



