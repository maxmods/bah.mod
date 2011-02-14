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


Import BaH.Boost

Import "../boost.mod/src/*.hpp"
Import "glue.cpp"


Extern

	Function bmx_rational_create:Byte Ptr(n:Int, d:Int)
	Function bmx_rational_delete(handle:Byte Ptr)

	Function bmx_rational_todouble:Double(handle:Byte Ptr)
	Function bmx_rational_numerator:Int(handle:Byte Ptr)
	Function bmx_rational_denominator:Int(handle:Byte Ptr)
	Function bmx_rational_assign(handle:Byte Ptr, n:Int, d:Int)
	Function bmx_rational_add(handle:Byte Ptr, value:Int)
	Function bmx_rational_sub(handle:Byte Ptr, value:Int)
	Function bmx_rational_mul(handle:Byte Ptr, value:Int)
	Function bmx_rational_div(handle:Byte Ptr, value:Int)
	Function bmx_rational_addr(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_rational_subr(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_rational_mulr(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_rational_divr(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_rational_equals:Int(handle:Byte Ptr, value:Int)
	Function bmx_rational_equalsr:Int(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_rational_plus:Byte Ptr(handle:Byte Ptr, value:Int)
	Function bmx_rational_subtract:Byte Ptr(handle:Byte Ptr, value:Int)
	Function bmx_rational_multiply:Byte Ptr(handle:Byte Ptr, value:Int)
	Function bmx_rational_divide:Byte Ptr(handle:Byte Ptr, value:Int)
	Function bmx_rational_plusr:Byte Ptr(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_rational_subtractr:Byte Ptr(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_rational_multiplyr:Byte Ptr(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_rational_divider:Byte Ptr(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_rational_negative:Byte Ptr(handle:Byte Ptr)
	Function bmx_rational_abs:Byte Ptr(handle:Byte Ptr)
	Function bmx_rational_lessthan:Int(handle:Byte Ptr, value:Int)
	Function bmx_rational_greaterthan:Int(handle:Byte Ptr, value:Int)
	Function bmx_rational_lessthanr:Int(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_rational_greaterthanr:Int(handle:Byte Ptr, value:Byte Ptr)

End Extern

