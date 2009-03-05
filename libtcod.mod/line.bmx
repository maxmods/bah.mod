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
bbdoc: This toolkit is a very simple and lightweight implementation of the bresenham line drawing algorithm.
about: It allows you to follow straight paths on your map very easily.
End Rem
Type TCODLine

	Rem
	bbdoc: Initializes the toolkit with your starting and ending coordinates.
	End Rem
	Function Init(xFrom:Int, yFrom:Int, xTo:Int, yTo:Int)
		bmx_tcodline_init(xFrom, yFrom, xTo, yTo)
	End Function
	
	Rem
	bbdoc: Steps through each cell.
	about: It returns true when you reach the line's ending point.
	End Rem
	Function DoStep:Int(xCur:Int Var, yCur:Int Var)
		Return bmx_tcodline_dostep(Varptr xCur, Varptr yCur)
	End Function

	Rem
	bbdoc: 
	End Rem
	Function Line:Int(xFrom:Int, yFrom:Int, xTo:Int, yTo:Int, listener:Int(x:Int, y:Int))
		Return TCOD_line(xFrom, yFrom, xTo, yTo, listener)
	End Function

End Type

