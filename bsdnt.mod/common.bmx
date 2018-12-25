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

Import "source.bmx"

Extern

	Function bmx_zz_init(zz:S_ZZ Var)
	Function bmx_zz_clear(zz:S_ZZ Var)
	Function bmx_zz_set(zz:S_ZZ Var, o:S_ZZ Var)
	Function bmx_zz_seti(zz:S_ZZ Var, value:Int)
	Function bmx_zz_set_str(zz:S_ZZ Var, value:String)
	
	Function bmx_zz_add(n:S_ZZ Var, zz:S_ZZ Var, o:S_ZZ Var)
	Function bmx_zz_sub(n:S_ZZ Var, zz:S_ZZ Var, o:S_ZZ Var)
	Function bmx_zz_mul(n:S_ZZ Var, zz:S_ZZ Var, o:S_ZZ Var)
	Function bmx_zz_div(n:S_ZZ Var, zz:S_ZZ Var, o:S_ZZ Var)
	Function bmx_zz_divrem(n:S_ZZ Var, r:S_ZZ Var, zz:S_ZZ Var, o:S_ZZ Var)

	Function bmx_zz_addi(n:S_ZZ Var, zz:S_ZZ Var, o:Int)
	Function bmx_zz_subi(n:S_ZZ Var, zz:S_ZZ Var, o:Int)
	Function bmx_zz_muli(n:S_ZZ Var, zz:S_ZZ Var, o:Int)
	Function bmx_zz_divremi:Int(n:S_ZZ Var, zz:S_ZZ Var, o:Int)

	Function bmx_zz_neg(n:S_ZZ Var, zz:S_ZZ Var)
	Function bmx_zz_equali:Int(n:S_ZZ Var, o:Int)
	Function bmx_zz_equal:Int(n:S_ZZ Var, o:S_ZZ Var)

	Function bmx_zz_zero(zz:S_ZZ Var)
	Function bmx_zz_is_zero:Int(zz:S_ZZ Var)
	
	Function bmx_zz_get_str:String(zz:S_ZZ Var)
End Extern


Struct S_ZZ
	Field n:UInt Ptr
	Field size:Int
	Field alloc:Int
End Struct
