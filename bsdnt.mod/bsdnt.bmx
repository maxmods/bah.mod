' Copyright (c) 2018 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'
' 1. Redistributions of source code must retain the above copyright notice, this
'    list of conditions and the following disclaimer.
' 
' 2. Redistributions in binary form must reproduce the above copyright notice,
'    this list of conditions and the following disclaimer in the documentation
'    and/or other materials provided with the distribution.
'
' THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE
' FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
' DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
' SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
' CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
' OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
' OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Rem
bbdoc: BSD Bignum Library
End Rem
Module BaH.bsdnt

ModuleInfo "Version: 1.00"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."
?bmxng

Import "common.bmx"

Rem
bbdoc: Big integer.
End Rem
Type TZZ

	Field zz:S_ZZ

	Rem
	bbdoc: Creates a new TZZ instance initialised to @value.
	End Rem
	Method New(value:Int = 0)
		bmx_zz_init(zz)
		
		bmx_zz_seti(zz, value)
	End Method

	Rem
	bbdoc: Creates a new TZZ instance initialised to @value.
	End Rem
	Method New(value:String)
		bmx_zz_init(zz)
		
		bmx_zz_set_str(zz, value)
	End Method

	Rem
	bbdoc: Sets the value to @value.
	End Rem
	Method Set(value:Int)
		bmx_zz_seti(zz, value)
	End Method
	
	Rem
	bbdoc: Returns a string giving the decimal representation of the value.
	about: If the value is negative, the string is prepended with a '-' sign.
	If the value is 0, the string "0" is returned. 
	End Rem
	Method ToString:String()
		Return bmx_zz_get_str(zz)
	End Method

	Rem
	bbdoc: Adds @o to the value, returning the new value.
	End Rem
	Method Operator+:TZZ(o:TZZ)
		Local n:TZZ = New TZZ
		bmx_zz_add(n.zz, zz, o.zz)
		Return n
	End Method

	Rem
	bbdoc: Subtracts @o from the value, returning the new value.
	End Rem
	Method Operator-:TZZ(o:TZZ)
		Local n:TZZ = New TZZ
		bmx_zz_sub(n.zz, zz, o.zz)
		Return n
	End Method

	Rem
	bbdoc: Multiplies @o with the value, returning the new value.
	End Rem
	Method Operator*:TZZ(o:TZZ)
		Local n:TZZ = New TZZ
		bmx_zz_mul(n.zz, zz, o.zz)
		Return n
	End Method

	Rem
	bbdoc: Divides the value by @o, returning the new value.
	End Rem
	Method Operator/:TZZ(o:TZZ)
		Local n:TZZ = New TZZ
		bmx_zz_div(n.zz, zz, o.zz)
		Return n
	End Method

	Rem
	bbdoc: Returns the modulus/remainder from dividing the value by @o.
	End Rem
	Method Operator Mod:TZZ(o:TZZ)
		Local n:TZZ = New TZZ
		Local r:TZZ = New TZZ
		bmx_zz_divrem(n.zz, r.zz, zz, o.zz)
		Return r
	End Method

	Rem
	bbdoc: Returns the value shifted left by @bits bits.
	End Rem
	Method Operator Shl:TZZ(bits:Int)
		Local n:TZZ = New TZZ
		bmx_zz_muli(n.zz, zz, bits * 2)
		Return n
	End Method

	Rem
	bbdoc: Returns the value shifted right by @bits bits.
	End Rem
	Method Operator Shr:TZZ(bits:Int)
		Local n:TZZ = New TZZ
		bmx_zz_divremi(n.zz, zz, bits * 2)
		Return n
	End Method

	Rem
	bbdoc: Adds @o To the value.
	End Rem
	Method Operator:+(o:TZZ)
		bmx_zz_add(zz, zz, o.zz)
	End Method

	Rem
	bbdoc: Subtracts @o from the value.
	End Rem
	Method Operator:-(o:TZZ)
		bmx_zz_sub(zz, zz, o.zz)
	End Method
	
	Rem
	bbdoc: Multiplies the value by @o.
	End Rem
	Method Operator:*(o:TZZ)
		bmx_zz_mul(zz, zz, o.zz)
	End Method

	Rem
	bbdoc: Divides the value by @o.
	End Rem
	Method Operator:/(o:TZZ)
		bmx_zz_div(zz, zz, o.zz)
	End Method

	Rem
	bbdoc: Sets the value to the modulus of the value divided by @o.
	End Rem
	Method Operator:Mod(o:TZZ)
		Local n:TZZ = New TZZ
		bmx_zz_divrem(n.zz, zz, zz, o.zz)
	End Method

	Rem
	bbdoc: Shifts the value left by @bits bits.
	End Rem
	Method Operator:Shl(bits:Int)
		bmx_zz_muli(zz, zz, bits * 2)
	End Method

	Rem
	bbdoc: Shifts the value right by @bits bits.
	End Rem
	Method Operator:Shr(bits:Int)
		bmx_zz_divremi(zz, zz, bits * 2)
	End Method

	Rem
	bbdoc: Adds @o to the value, returning the new value.
	End Rem
	Method Operator+:TZZ(o:Int)
		Local n:TZZ = New TZZ
		bmx_zz_addi(n.zz, zz, o)
		Return n
	End Method

	Rem
	bbdoc: Subtracts @o from the value, returning the new value.
	End Rem
	Method Operator-:TZZ(o:Int)
		Local n:TZZ = New TZZ
		bmx_zz_subi(n.zz, zz, o)
		Return n
	End Method

	Rem
	bbdoc: Multiplies the value by @o, returning the new value.
	End Rem
	Method Operator*:TZZ(o:Int)
		Local n:TZZ = New TZZ
		bmx_zz_muli(n.zz, zz, o)
		Return n
	End Method

	Rem
	bbdoc: Divides the value by @o, returning the new value.
	End Rem
	Method Operator/:TZZ(o:Int)
		Local n:TZZ = New TZZ
		bmx_zz_divremi(n.zz, zz, o)
		Return n
	End Method

	Rem
	bbdoc: Adds @o to the value.
	End Rem
	Method Operator:+(o:Int)
		bmx_zz_addi(zz, zz, o)
	End Method

	Rem
	bbdoc: Subtracts @o from the value.
	End Rem
	Method Operator:-(o:Int)
		bmx_zz_subi(zz, zz, o)
	End Method

	Rem
	bbdoc: Multiplies the value by @o.
	End Rem
	Method Operator:*(o:Int)
		bmx_zz_muli(zz, zz, o)
	End Method

	Rem
	bbdoc: Divides the value by @o.
	End Rem
	Method Operator:/(o:Int)
		bmx_zz_divremi(zz, zz, o)
	End Method

	Rem
	bbdoc: Returns the value negated.
	End Rem
	Method Operator-:TZZ()
		Local n:TZZ = New TZZ
		bmx_zz_neg(n.zz, zz)
		Return n
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Operator+:TZZ()
		Local n:TZZ = New TZZ
		bmx_zz_set(n.zz, zz)
		Return n
	End Method

	Rem
	bbdoc: Returns #True if the value is equal to @o.
	End Rem
	Method Operator=:Int(o:Int)
		Return bmx_zz_equali(zz, o)
	End Method

	Rem
	bbdoc: Returns #True if the value is equal to @o.
	End Rem
	Method Operator=:Int(o:TZZ)
		Return bmx_zz_equal(zz, o.zz)
	End Method

	Rem
	bbdoc: Sets the value to zero.
	End Rem
	Method Zero()
		bmx_zz_zero(zz)
	End Method
	
	Rem
	bbdoc: Returns #True is the value is zero.
	End Rem
	Method IsZero:Int()
		Return bmx_zz_is_zero(zz)
	End Method

	Method Delete()
		bmx_zz_clear(zz)
	End Method
	
End Type

?
