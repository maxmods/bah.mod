' Copyright (c) 2007-2008, Bruce A Henderson
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

ModuleInfo "Version: 1.02"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: SFMT - 2006,2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima"
ModuleInfo "Copyright: Wrapper - 2007,2008 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.02"
ModuleInfo "History: Fix for PPC Mac compile."
ModuleInfo "History: 1.01"
ModuleInfo "History: Automatically initializes via SeedRnd() if required."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Version (SFMT 1.2)"

ModuleInfo "CC_OPTS: -DMEXP=19937"
ModuleInfo "CC_OPTS: -fno-strict-aliasing"

?x86
ModuleInfo "CC_OPTS: -msse2"
?macosppc
ModuleInfo "CC_OPTS: -DALTIVEC=1"
ModuleInfo "CC_OPTS: -faltivec -maltivec"
?

Import "common.bmx"

Global kRndInitialized:Int = False

Rem
bbdoc: This function initializes the internal state array with a 32-bit integer seed.
End Rem
Function SeedRnd(seed:Int)
	kRndInitialized = True
	init_gen_rand(seed)
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
	If Not kRndInitialized SeedRnd(0)
	Local range:Double = max_value - min_value
	If range > 0 Return Int( bmx_genrand_res53()*(1:Double+range) )+min_value
	Return Int( bmx_genrand_res53()*(1-range) )+max_value
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
	If max_value > min_value Return RndDouble() * (max_value - min_value) + min_value
	Return RndDouble() * (min_value - max_value) + max_value
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
	If Not kRndInitialized SeedRnd(0)
	Local range:Long = max_value - min_value
	If range > 0 Return Long( bmx_genrand_res53() * (1:Long + range) ) + min_value
	Return Long( bmx_genrand_res53() * (1:Long - range) ) + max_value
End Function

Rem
bbdoc: Generate random float
returns: A random float in the range 0 (inclusive) to 1 (exclusive)
End Rem
Function RndFloat:Float()
	If Not kRndInitialized SeedRnd(0)
	Return Float(bmx_genrand_real3())
End Function

Rem
bbdoc: Generate random double
returns: A random double in the range 0 (inclusive) to 1 (exclusive)
End Rem
Function RndDouble:Double()
	If Not kRndInitialized SeedRnd(0)
	Return bmx_genrand_res53()
End Function

