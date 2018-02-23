' Copyright (c) 2007-2018, Bruce A Henderson
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
bbdoc: Random Numbers - SFMT
End Rem
Module BaH.Random

ModuleInfo "Version: 1.05"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: SFMT - 2006-2017 Mutsuo Saito, Makoto Matsumoto and Hiroshima"
ModuleInfo "Copyright: Wrapper - 2007-2018 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.05"
ModuleInfo "History: Created TRandom to support multiple generators."
ModuleInfo "History: Aligned pointer creation."
ModuleInfo "History: 1.04"
ModuleInfo "History: Update to SFMT 1.5.1"
ModuleInfo "History: Refactored."
ModuleInfo "History: 1.03"
ModuleInfo "History: Update to SFMT 1.4.1"
ModuleInfo "History: Updated for NG."
ModuleInfo "History: 1.02"
ModuleInfo "History: Fix for PPC Mac compile."
ModuleInfo "History: 1.01"
ModuleInfo "History: Automatically initializes via SeedRnd() if required."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Version (SFMT 1.2)"

ModuleInfo "CC_OPTS: -DMEXP=19937"
ModuleInfo "CC_OPTS: -fno-strict-aliasing"
ModuleInfo "CC_OPTS: -std=c11"

?x86
ModuleInfo "CC_OPTS: -msse2 -DHAVE_SSE2"
?x64
ModuleInfo "CC_OPTS: -msse2 -DHAVE_SSE2"
?

Import "common.bmx"

Private

Global _random:TRandom = New TRandom

Public
Rem
bbdoc: This function initializes the internal state array with a 32-bit integer seed.
End Rem
Function SeedRnd(seed:Int)
	_random.SeedRnd(seed)
End Function

Rem
bbdoc: Generate random 32-bit integer.
returns: A random 32-bit integer in the range min (inclusive) to max (inclusive)
about:
You should call #SeedRandom to initialize the pseudorandom number generator, before calling this function
for the first time.<br>
The optional parameter allows you to use #Rand32 in 2 ways:<br>
<table>
<tr><th>Format</th><th>Result</th></tr>
<tr><td><font class=token>Rand32(x)</font></td><td>random 32-bit integer in the range 1 to x (inclusive)</td></tr>
<tr><td><font class=token>Rand32(x,y)</font></td><td>random 32-bit integer in the range x to y (inclusive)</td></tr>
</table>
End Rem
Function Rand:Int( min_value:Int, max_value:Int = 1 )
	Return _random.Rand(min_value, max_value)
End Function

Rem
bbdoc: Generate random double
returns: A random double in the range min (inclusive) to max (exclusive)
about: 
The optional parameters allow you to use Rnd in 3 ways:<br>
<br>
<table>
<tr><th>Format</th><th>Result</th></tr>
<tr><td><font class=token>Rnd()</font></td><td>random double in the range 0 (inclusive) to 1 (exclusive)</td></tr>
<tr><td><font class=token>Rnd(x)</font></td><td>random double in the range 0 (inclusive) to n (exclusive)</td></tr>
<tr><td><font class=token>Rnd(x,y)</font></td><td>random double in the range x (inclusive) to y (exclusive)</td></tr>
</table>
End Rem
Function Rnd:Double( min_value!=1,max_value!=0 )
	Return _random.Rnd(min_value, max_value)
End Function

Rem
bbdoc: Generate random 64-bit Long.
returns: A random 64-bit Long in the range min (inclusive) to max (inclusive)
about:
You should call #SeedRandom to initialize the pseudorandom number generator, before calling this function
for the first time, or if you have used #Rand32 previously.<br>
The optional parameter allows you to use #Rand64 in 2 ways:<br>
<table>
<tr><th>Format</th><th>Result</th></tr>
<tr><td><font class=token>Rand64(x)</font></td><td>random 64-bit Long in the range 1 to x (inclusive)</td></tr>
<tr><td><font class=token>Rand64(x,y)</font></td><td>random 64-bit Long in the range x to y (inclusive)</td></tr>
</table>
End Rem
Function Rand64:Long( min_value:Long, max_value:Long = 1 )
	Return _random.Rand64(min_value, max_value)
End Function

Rem
bbdoc: Generate random float
returns: A random float in the range 0 (inclusive) to 1 (exclusive)
End Rem
Function RndFloat:Float()
	Return _random.RndFloat()
End Function

Rem
bbdoc: Generate random double
returns: A random double in the range 0 (inclusive) to 1 (exclusive)
End Rem
Function RndDouble:Double()
	Return _random.RndDouble()
End Function

Rem
bbdoc: An instance of a random number generator.
End Rem
Type TRandom

	Field sfmtPtr:Byte Ptr
	
	Method New()
		sfmtPtr = bmx_sfmt_init_gen_rand(Null, 0)
	End Method
	
	Method SeedRnd(seed:Int)
		bmx_sfmt_init_gen_rand(sfmtPtr, seed)
	End Method

	Method Rand:Int( min_value:Int, max_value:Int = 1 )
		Local Range:Double = max_value - min_value
		If Range > 0 Return Int( bmx_genrand_res53(sfmtPtr)*(1:Double+Range) )+min_value
		Return Int( bmx_genrand_res53(sfmtPtr)*(1:Double-Range) )+max_value
	End Method

	Method Rnd:Double( min_value!=1,max_value!=0 )
		If max_value > min_value Return RndDouble() * (max_value - min_value) + min_value
		Return RndDouble() * (min_value - max_value) + max_value
	End Method

	Method Rand64:Long( min_value:Long, max_value:Long = 1 )
		Local Range:Long = max_value - min_value
		If Range > 0 Return Long( bmx_genrand_res53(sfmtPtr) * (1:Long + Range) ) + min_value
		Return Long( bmx_genrand_res53(sfmtPtr) * (1:Long - Range) ) + max_value
	End Method
	
	Method RndFloat:Float()
		Return Float(bmx_genrand_real3(sfmtPtr))
	End Method
	
	Method RndDouble:Double()
		Return bmx_genrand_res53(sfmtPtr)
	End Method
	
	Method Delete()
		If sfmtPtr Then
			bmx_sfmt_free(sfmtPtr)
			sfmtPtr = Null
		End If
	End Method
	
End Type
