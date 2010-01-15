' Copyright (c) 2007-2010 Bruce A Henderson
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
' THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
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

Import "src/*.h"

Import "src/pcre_chartables.c"
Import "src/pcre_compile.c"
Import "src/pcre_config.c"
Import "src/pcre_dfa_exec.c"
Import "src/pcre_exec.c"
Import "src/pcre_fullinfo.c"
Import "src/pcre_get.c"
Import "src/pcre_globals.c"
Import "src/pcre_info.c"
Import "src/pcre_maketables.c"
Import "src/pcre_newline.c"
Import "src/pcre_ord2utf8.c"
Import "src/pcre_refcount.c"
Import "src/pcre_study.c"
Import "src/pcre_tables.c"
Import "src/pcre_try_flipped.c"
'Import "src/pcre_ucp_searchfuncs.c"
Import "src/pcre_valid_utf8.c"
Import "src/pcre_version.c"
Import "src/pcre_xclass.c"
Import "src/pcre_ucd.c"


' Request types for pcre_config().
Const PCRE_CONFIG_UTF8:Int =                    0
Const PCRE_CONFIG_NEWLINE:Int =                 1
Const PCRE_CONFIG_LINK_SIZE:Int =               2
Const PCRE_CONFIG_POSIX_MALLOC_THRESHOLD:Int =  3
Const PCRE_CONFIG_MATCH_LIMIT:Int =             4
Const PCRE_CONFIG_STACKRECURSE:Int =            5
Const PCRE_CONFIG_UNICODE_PROPERTIES:Int =      6
Const PCRE_CONFIG_MATCH_LIMIT_RECURSION:Int =   7

' Exec-time and get/set-time error codes
Const PCRE_ERROR_NOMATCH:Int =         -1
Const REGEX_NOMATCH:Int = PCRE_ERROR_NOMATCH
Const PCRE_ERROR_NULL:Int =            -2
Const PCRE_ERROR_BADOPTION:Int =       -3
Const PCRE_ERROR_BADMAGIC:Int =        -4
Const PCRE_ERROR_UNKNOWN_OPCODE:Int =  -5
Const PCRE_ERROR_UNKNOWN_NODE:Int =    -5  ' For backward compatibility
Const PCRE_ERROR_NOMEMORY:Int =        -6
Const PCRE_ERROR_NOSUBSTRING:Int =     -7
Const PCRE_ERROR_MATCHLIMIT:Int =      -8
Const PCRE_ERROR_CALLOUT:Int =         -9  ' Never used by PCRE itself
Const PCRE_ERROR_BADUTF8:Int =        -10
Const PCRE_ERROR_BADUTF8_OFFSET:Int = -11
Const PCRE_ERROR_PARTIAL:Int =        -12
Const PCRE_ERROR_BADPARTIAL:Int =     -13
Const PCRE_ERROR_INTERNAL:Int =       -14
Const PCRE_ERROR_BADCOUNT:Int =       -15
Const PCRE_ERROR_DFA_UITEM:Int =      -16
Const PCRE_ERROR_DFA_UCOND:Int =      -17
Const PCRE_ERROR_DFA_UMLIMIT:Int =    -18
Const PCRE_ERROR_DFA_WSSIZE:Int =     -19
Const PCRE_ERROR_DFA_RECURSE:Int =    -20
Const PCRE_ERROR_RECURSIONLIMIT:Int = -21
Const PCRE_ERROR_NULLWSLIMIT:Int =    -22
Const PCRE_ERROR_BADNEWLINE:Int =     -23

' Options
Const PCRE_CASELESS:Int =           $00000001
Const PCRE_MULTILINE:Int =          $00000002
Const PCRE_DOTALL:Int =             $00000004
Const PCRE_EXTENDED:Int =           $00000008
Const PCRE_ANCHORED:Int =           $00000010
Const PCRE_DOLLAR_ENDONLY:Int =     $00000020
Const PCRE_EXTRA:Int =              $00000040
Const PCRE_NOTBOL:Int =             $00000080
Const PCRE_NOTEOL:Int =             $00000100
Const PCRE_UNGREEDY:Int =           $00000200
Const PCRE_NOTEMPTY:Int =           $00000400
Const PCRE_UTF8:Int =               $00000800
Const PCRE_NO_AUTO_CAPTURE:Int =    $00001000
Const PCRE_NO_UTF8_CHECK:Int =      $00002000
Const PCRE_AUTO_CALLOUT:Int =       $00004000
Const PCRE_PARTIAL:Int =            $00008000
Const PCRE_DFA_SHORTEST:Int =       $00010000
Const PCRE_DFA_RESTART:Int =        $00020000
Const PCRE_FIRSTLINE:Int =          $00040000
Const PCRE_DUPNAMES:Int =           $00080000
Const PCRE_NEWLINE_CR:Int =         $00100000
Const PCRE_NEWLINE_LF:Int =         $00200000
Const PCRE_NEWLINE_CRLF:Int =       $00300000
Const PCRE_NEWLINE_ANY:Int =        $00400000

Const PCRE_INFO_OPTIONS:Int =            0
Const PCRE_INFO_SIZE:Int =               1
Const PCRE_INFO_CAPTURECOUNT:Int =       2
Const PCRE_INFO_BACKREFMAX:Int =         3
Const PCRE_INFO_FIRSTBYTE:Int =          4
Const PCRE_INFO_FIRSTCHAR:Int =          4  ' For backwards compatibility
Const PCRE_INFO_FIRSTTABLE:Int =         5
Const PCRE_INFO_LASTLITERAL:Int =        6
Const PCRE_INFO_NAMEENTRYSIZE:Int =      7
Const PCRE_INFO_NAMECOUNT:Int =          8
Const PCRE_INFO_NAMETABLE:Int =          9
Const PCRE_INFO_STUDYSIZE:Int =         10
Const PCRE_INFO_DEFAULT_TABLES:Int =    11


Extern
	Function _strlen:Int(s:Byte Ptr) = "strlen"

	Function pcre_config:Int(what:Int, where:Int Ptr)
	
	Function pcre_compile2:Byte Ptr(pattern:Byte Ptr, options:Int, errorcodeptr:Int Ptr, ..
		errptr:Byte Ptr, erroffset:Int Ptr, tableptr:Byte Ptr)
	
	Function pcre_exec:Int(pcre:Byte Ptr, extra:Byte Ptr, subject:Byte Ptr, ..
		length:Int, startoffset:Int, options:Int, ovector:Byte Ptr, ovecsize:Int)
	
	Function pcre_get_substring:Int(subject:Byte Ptr, ovector:Byte Ptr, stringcount:Int, ..
		stringnumber:Int, stringptr:Byte Ptr)
	
	Function pcre_free_substring(strinptr:Byte Ptr)
	
	Function pcre_fullinfo:Int(pcre:Byte Ptr, extra:Byte Ptr, what:Int, where:Int Ptr)
End Extern

' Convert from Max to UTF8
Function convertISO8859toUTF8:String(text:String)
	If Not text Then
		Return ""
	End If
	
	Local l:Int = text.length
	If l = 0 Then
		Return ""
	End If
	
	Local count:Int = 0
	Local s:Byte[] = New Byte[l * 3]
	
	For Local i:Int = 0 Until l
		Local char:Int = text[i]

		If char < 128 Then
			s[count] = char
			count:+ 1
			Continue
		Else If char<2048
			s[count] = char/64 | 192
			count:+ 1
			s[count] = char Mod 64 | 128
			count:+ 1
			Continue
		Else
			s[count] =  char/4096 | 224
			count:+ 1
			s[count] = char/64 Mod 64 | 128
			count:+ 1
			s[count] = char Mod 64 | 128
			count:+ 1
			Continue
		EndIf
		
	Next

	Return String.fromBytes(s, count)
End Function

' Convert from UTF8 to Max
Function convertUTF8toISO8859:String(s:Byte Ptr)

	Local l:Int = _strlen(s)

	Local b:Short[] = New Short[l]
	Local bc:Int = -1
	Local c:Int
	Local d:Int
	Local e:Int
	For Local i:Int = 0 Until l

		bc:+1
		c = s[i]
		If c<128 
			b[bc] = c
			Continue
		End If
		i:+1
		d=s[i]
		If c<224 
			b[bc] = (c-192)*64+(d-128)
			Continue
		End If
		i:+1
		e = s[i]
		If c < 240 
			b[bc] = (c-224)*4096+(d-128)*64+(e-128)
			If b[bc] = 8233 Then
				b[bc] = 10
			End If
			Continue
		End If
	Next

	Return String.fromshorts(b, bc + 1)
End Function

Function sizedUTF8toISO8859:String(s:Byte Ptr, size:Int)

	Local l:Int = size
	Local b:Short[] = New Short[l]
	Local bc:Int = -1
	Local c:Int
	Local d:Int
	Local e:Int
	For Local i:Int = 0 Until l

		c = s[i]
		If c = 0 Continue

		bc:+1
		If c<128
			b[bc] = c
			Continue
		End If
		i:+1
		d=s[i]
		If c<224 
			b[bc] = (c-192)*64+(d-128)
			Continue
		End If
		i:+1
		e = s[i]
		If c < 240 
			b[bc] = (c-224)*4096+(d-128)*64+(e-128)
			If b[bc] = 8233 Then
				b[bc] = 10
			End If
			Continue
		End If
	Next

	Return String.fromshorts(b, bc + 1)
End Function

