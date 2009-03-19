' Copyright (c) 2009 Bruce A Henderson
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


Import BRL.Blitz
Import Pub.StdC

Import "../boost.mod/src/*.hpp"


Import "glue.cpp"


Extern

	Function bmx_boost_math_factorial:Double(i:Int)
	Function bmx_boost_math_uncheckedfactorial:Double(i:Int)
	Function bmx_boost_math_maxfactorial:Int()
	Function bmx_boost_math_doublefactorial:Double(i:Int)
	Function bmx_boost_math_risingfactorial:Double(x:Double, i:Int)
	Function bmx_boost_math_fallingfactorial:Double(x:Double, i:Int)
	Function bmx_boost_math_binomialcoefficient:Double(n:Int, k:Int)

	Function bmx_boost_math_cbrt:Double(x:Double)
	Function bmx_boost_math_hypot:Double(x:Double, y:Double)

	Function bmx_boost_math_binomial_distribution_create:Byte Ptr(n:Int, p:Double)
	Function bmx_boost_math_binomial_distribution_successfraction:Double(handle:Byte Ptr)
	Function bmx_boost_math_binomial_distribution_trials:Int(handle:Byte Ptr)
	Function bmx_boost_math_binomial_distribution_mean:Double(handle:Byte Ptr)
	Function bmx_boost_math_binomial_distribution_mode:Double(handle:Byte Ptr)
	Function bmx_boost_math_binomial_distribution_standarddeviation:Double(handle:Byte Ptr)
	Function bmx_boost_math_binomial_distribution_skewness:Double(handle:Byte Ptr)
	Function bmx_boost_math_binomial_distribution_pdf:Double(handle:Byte Ptr, k:Double)
	Function bmx_boost_math_binomial_distribution_cdf:Double(handle:Byte Ptr, k:Double)

End Extern


Type TOverflowException Extends TRuntimeException
	
	Method CreateException:TOverflowException()
		Return Self
	End Method

	Function _create:TOverflowException()
		Return New TOverflowException.CreateException()
	End Function
	
	Method ToString:String()
		Return "Overflow Exception"
	End Method
	
End Type

Type TDomainException Extends TRuntimeException
	
	Method CreateException:TDomainException()
		Return Self
	End Method

	Function _create:TDomainException()
		Return New TDomainException.CreateException()
	End Function
	
	Method ToString:String()
		Return "Domain Exception"
	End Method
	
End Type
