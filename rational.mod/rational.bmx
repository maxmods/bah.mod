' Copyright (c) 2009-2011 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Rem
bbdoc: Rational Numbers (Fractions)
End Rem
Module BaH.Rational

ModuleInfo "Version: 1.00"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: Wrapper - 2009-2011 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

ModuleInfo "CC_OPTS: -fexceptions"

Import "common.bmx"


Rem
bbdoc: Helper function to creates a rational with numerator and optional denominator.
End Rem
Function Rational:TRational(n:Int, d:Int = 1)
	Return New TRational.Create(n, d)
End Function

Rem
bbdoc: A rational number (or fraction).
End Rem
Type TRational

	Field ratPtr:Byte Ptr

	Function _create:TRational(ratPtr:Byte Ptr)
		If ratPtr Then
			Local this:TRational = New TRational
			this.ratPtr = ratPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Creates a new rational using provided numerator and optional denominator.
	End Rem
	Function CreateRational:TRational(n:Int, d:Int = 1)
		Return New TRational.Create(n, d)
	End Function

	Rem
	bbdoc: Creates a new rational using provided numerator and optional denominator.
	End Rem
	Method Create:TRational(n:Int, d:Int = 1)
		ratPtr = bmx_rational_create(n, d)
		Return Self
	End Method

	Rem
	bbdoc: Returns the numerator part of the rational.
	End Rem
	Method Numerator:Int()
		Return bmx_rational_numerator(ratPtr)
	End Method
	
	Rem
	bbdoc: Returns the denominator part of the rational.
	End Rem
	Method Denominator:Int()
		Return bmx_rational_denominator(ratPtr)
	End Method
	
	Rem
	bbdoc: Assigns a new numerator and demoniator to the rational.
	End Rem
	Method Assign:TRational(n:Int, d:Int)
		bmx_rational_assign(ratPtr, n, d)
		Return Self
	End Method
	
	Rem
	bbdoc: Adds rational and @value together, returning a new rational.
	End Rem
	Method Plus:TRational(value:Int)
		Return TRational._create(bmx_rational_plus(ratPtr, value))
	End Method

	Rem
	bbdoc: Subtracts @value from rational, returning a new rational.
	End Rem
	Method Subtract:TRational(value:Int)
		Return TRational._create(bmx_rational_subtract(ratPtr, value))
	End Method

	Rem
	bbdoc: Multiplies rational by @value, returning a new rational.
	End Rem
	Method Multiply:TRational(value:Int)
		Return TRational._create(bmx_rational_multiply(ratPtr, value))
	End Method

	Rem
	bbdoc: Divides rational by @value, returning a new rational.
	End Rem
	Method Divide:TRational(value:Int)
		Return TRational._create(bmx_rational_divide(ratPtr, value))
	End Method

	Rem
	bbdoc: Adds rational and @value together, returning a new rational.
	End Rem
	Method PlusR:TRational(value:TRational)
		Return TRational._create(bmx_rational_plusr(ratPtr, value.ratPtr))
	End Method

	Rem
	bbdoc: Subtracts @value from rational, returning a new rational.
	End Rem
	Method SubtractR:TRational(value:TRational)
		Return TRational._create(bmx_rational_subtractr(ratPtr, value.ratPtr))
	End Method

	Rem
	bbdoc: Multiplies rational by @value, returning a new rational.
	End Rem
	Method MultiplyR:TRational(value:TRational)
		Return TRational._create(bmx_rational_multiplyr(ratPtr, value.ratPtr))
	End Method

	Rem
	bbdoc: Divides rational by @value, returning a new rational.
	End Rem
	Method DivideR:TRational(value:TRational)
		Return TRational._create(bmx_rational_divider(ratPtr, value.ratPtr))
	End Method

	Rem
	bbdoc: Adds @value to rational, returning updated self.
	End Rem
	Method Add:TRational(value:Int)
		bmx_rational_add(ratPtr, value)
		Return Self
	End Method
	
	Rem
	bbdoc: Subtracts @value from rational, returning updated self.
	End Rem
	Method Sub:TRational(value:Int)
		bmx_rational_sub(ratPtr, value)
		Return Self
	End Method
	
	Rem
	bbdoc: Multiplies rational by @value, returning updated self.
	End Rem
	Method Mul:TRational(value:Int)
		bmx_rational_mul(ratPtr, value)
		Return Self
	End Method
	
	Rem
	bbdoc: Divides rational by @value, returning updated self.
	End Rem
	Method Div:TRational(value:Int)
		bmx_rational_div(ratPtr, value)
		Return Self
	End Method

	Rem
	bbdoc: Adds @value to rational, returning updated self.
	End Rem
	Method AddR:TRational(value:TRational)
		bmx_rational_addr(ratPtr, value.ratPtr)
		Return Self
	End Method
	
	Rem
	bbdoc: Subtracts @value from rational, returning updated self.
	End Rem
	Method SubR:TRational(value:TRational)
		bmx_rational_subr(ratPtr, value.ratPtr)
		Return Self
	End Method
	
	Rem
	bbdoc: Multiplies rational by @value, returning updated self.
	End Rem
	Method MulR:TRational(value:TRational)
		bmx_rational_mulr(ratPtr, value.ratPtr)
		Return Self
	End Method
	
	Rem
	bbdoc: Divides rational by @value, returning updated self.
	End Rem
	Method DivR:TRational(value:TRational)
		bmx_rational_divr(ratPtr, value.ratPtr)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns True if rational equals @value.
	End Rem
	Method Equals:Int(value:Int)
		Return bmx_rational_equals(ratPtr, value)
	End Method

	Rem
	bbdoc: Returns True if rational equals @value.
	End Rem
	Method EqualsR:Int(value:TRational)
		Return bmx_rational_equalsr(ratPtr, value.ratPtr)
	End Method
	
	Rem
	bbdoc: Returns True if rational is less than @value.
	End Rem
	Method LessThan:Int(value:Int)
		Return bmx_rational_lessthan(ratPtr, value)
	End Method

	Rem
	bbdoc: Returns True if rational is greater than @value.
	End Rem
	Method GreaterThan:Int(value:Int)
		Return bmx_rational_greaterthan(ratPtr, value)
	End Method

	Rem
	bbdoc: Returns True if rational is less than @value.
	End Rem
	Method LessThanR:Int(value:TRational)
		Return bmx_rational_lessthanr(ratPtr, value.ratPtr)
	End Method

	Rem
	bbdoc: Returns True if rational is greater than @value.
	End Rem
	Method GreaterThanR:Int(value:TRational)
		Return bmx_rational_greaterthanr(ratPtr, value.ratPtr)
	End Method
	
	Rem
	bbdoc: Returns the negated rational.
	about: ie. If rational is -1, this will return 1.
	End Rem
	Method Negative:TRational()
		Return TRational._create(bmx_rational_negative(ratPtr))
	End Method
	
	Rem
	bbdoc: Returns the absolute value of this rational.
	End Rem
	Method AbsR:TRational()
		Return TRational._create(bmx_rational_abs(ratPtr))
	End Method
	
	Rem
	bbdoc: Returns the Double approximate representation of the rational.
	End Rem
	Method ToDouble:Double()
		Return bmx_rational_todouble(ratPtr)
	End Method
	
	Rem
	bbdoc: Returns the String representation of the rational.
	End Rem
	Method ToString:String()
		Local n:Int = Numerator()
		Local d:Int = Denominator()
		If d <> 1 Then
			Return n + "/" + d
		End If
		Return n
	End Method

	Method Delete()
		If ratPtr Then
			bmx_rational_delete(ratPtr)
			ratPtr = Null
		End If
	End Method

End Type
