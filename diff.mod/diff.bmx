' Copyright (c) 2014 Bruce A Henderson
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
bbdoc: Diff Template Library
End Rem
Module BaH.Diff

ModuleInfo "Version: 1.00"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: dtl - 2013 Tatsuhiko Kubo <cubicdaiya@gmail.com>"
ModuleInfo "Copyright: Wrapper - 2014 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

Import "common.bmx"


Type TDiff

	Field diffPtr:Byte Ptr

	Method compose() Abstract
	
	Method composeUnifiedHunks() Abstract
	
	Method printUnifiedFormat(stream:TStream = Null) Abstract

End Type

Rem
bbdoc: 
End Rem
Type TDiffStrings Extends TDiff

	Method Create:TDiff(a:String, b:String)
		diffPtr = bmx_diff_screate(a, b)
		Return Self
	End Method

	Rem
	bbdoc: Compose Longest Common Subsequence and Shortest Edit Script.
	End Rem
	Method compose()
		bmx_diff_scompose(diffPtr)
	End Method
	
	Rem
	bbdoc: Compose Unified Format Hunks from Shortest Edit Script
	End Rem
	Method composeUnifiedHunks()
		bmx_diff_scomposeUnifiedHunks(diffPtr)
	End Method
	
	Rem
	bbdoc: Print difference between A and B in the Unified Format
	End Rem
	Method printUnifiedFormat(stream:TStream = Null)
		If stream Then
			' TODO
		Else
			bmx_diff_sprintUnifiedFormat(diffPtr)
		End If
	End Method

	Method Delete()
		If diffPtr Then
			bmx_diff_sfree(diffPtr)
			diffPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TDiffLines Extends TDiff

	Field diffPtr:Byte Ptr

	Method Create:TDiff(a:String[], b:String[])
		diffPtr = bmx_diff_lcreate(a, b)
		Return Self
	End Method

	Rem
	bbdoc: Compose Longest Common Subsequence and Shortest Edit Script.
	End Rem
	Method compose()
		bmx_diff_lcompose(diffPtr)
	End Method
	
	Rem
	bbdoc: Compose Unified Format Hunks from Shortest Edit Script
	End Rem
	Method composeUnifiedHunks()
		bmx_diff_lcomposeUnifiedHunks(diffPtr)
	End Method
	
	Rem
	bbdoc: Print difference between A and B in the Unified Format
	End Rem
	Method printUnifiedFormat(stream:TStream = Null)
		If stream Then
			' TODO
		Else
			bmx_diff_lprintUnifiedFormat(diffPtr)
		End If
	End Method

	Method Delete()
		If diffPtr Then
			bmx_diff_lfree(diffPtr)
			diffPtr = Null
		End If
	End Method
	
End Type

