' Copyright (c) 2008-2009 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the auther nor the names of its contributors may be used to 
'       endorse or promote products derived from this software without specific
'       prior written permission.
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

Import "common.bmx"

Rem
bbdoc: 
End Rem
Type TCODRandom

	Field randomPtr:Byte Ptr
	
	Global random:TCODRandom

	Rem
	bbdoc: 
	End Rem
	Method Create:TCODRandom(seed:Int = -1)
		If seed < 0 Then
			randomPtr = bmx_tcodrandom_create(seed, True)
		Else
			randomPtr = bmx_tcodrandom_create(seed, False)
		End If
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function GetInstance:TCODRandom()
		If Not random Then
			random = New TCODRandom
			random.randomPtr = bmx_tcodrandom_getinstance()
		End If
		Return random
	End Function

	Rem
	bbdoc: Get a random integer
	End Rem
	Method GetInt:Int(_min:Int, _max:Int)
		Return bmx_tcodrandom_getint(randomPtr, _min, _max)
	End Method
	
	Rem
	bbdoc: Get a random floating point number
	End Rem
	Method GetFloat:Float(_min:Float, _max:Float)
		Return bmx_tcodrandom_getfloat(randomPtr, _min, _max)
	End Method

	Method Delete()
		If randomPtr Then
			bmx_tcodrandom_delete(randomPtr)
			randomPtr = Null
		End If
	End Method
	
End Type
