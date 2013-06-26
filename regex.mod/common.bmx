' Copyright (c) 2007-2013 Bruce A Henderson
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

Import "src/pcre16_byte_order.c"
Import "src/pcre16_chartables.c"
Import "src/pcre16_compile.c"
Import "src/pcre16_config.c"
Import "src/pcre16_dfa_exec.c"
Import "src/pcre16_exec.c"
Import "src/pcre16_fullinfo.c"
Import "src/pcre16_get.c"
Import "src/pcre16_globals.c"
Import "src/pcre16_jit_compile.c"
Import "src/pcre16_maketables.c"
Import "src/pcre16_newline.c"
Import "src/pcre16_ord2utf16.c"
Import "src/pcre16_printint.c"
Import "src/pcre16_refcount.c"
Import "src/pcre16_string_utils.c"
Import "src/pcre16_study.c"
Import "src/pcre16_tables.c"
'Import "src/pcre_try_flipped.c"
'Import "src/pcre_ucp_searchfuncs.c"
Import "src/pcre16_ucd.c"
Import "src/pcre16_utf16_utils.c"
Import "src/pcre16_valid_utf16.c"
Import "src/pcre16_version.c"
Import "src/pcre16_xclass.c"



' Request types for pcre_config().
Const PCRE_CONFIG_UTF8:Int =                    0
Const PCRE_CONFIG_NEWLINE:Int =                 1
Const PCRE_CONFIG_LINK_SIZE:Int =               2
Const PCRE_CONFIG_POSIX_MALLOC_THRESHOLD:Int =  3
Const PCRE_CONFIG_MATCH_LIMIT:Int =             4
Const PCRE_CONFIG_STACKRECURSE:Int =            5
Const PCRE_CONFIG_UNICODE_PROPERTIES:Int =      6
Const PCRE_CONFIG_MATCH_LIMIT_RECURSION:Int =   7
Const PCRE_CONFIG_BSR:Int =                     8
Const PCRE_CONFIG_JIT:Int =                     9
Const PCRE_CONFIG_UTF16:Int =                  10
Const PCRE_CONFIG_JITTARGET:Int =              11
Const PCRE_CONFIG_UTF32:Int =                  12

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
Const PCRE_ERROR_BADUTF8:Int =        -10  ' Same For 8/16/32
Const PCRE_ERROR_BADUTF16:Int =       -10  ' Same For 8/16/32
Const PCRE_ERROR_BADUTF32:Int =       -10  ' Same For 8/16/32
Const PCRE_ERROR_BADUTF8_OFFSET:Int = -11  ' Same For 8/16
Const PCRE_ERROR_BADUTF16_OFFSET:Int =-11  ' Same For 8/16
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
Const PCRE_ERROR_BADOFFSET:Int =      -24
Const PCRE_ERROR_SHORTUTF8:Int =      -25
Const PCRE_ERROR_SHORTUTF16:Int =     -25  ' Same For 8/16
Const PCRE_ERROR_RECURSELOOP:Int =    -26
Const PCRE_ERROR_JIT_STACKLIMIT:Int = -27
Const PCRE_ERROR_BADMODE:Int =        -28
Const PCRE_ERROR_BADENDIANNESS:Int =  -29
Const PCRE_ERROR_DFA_BADRESTART:Int = -30
Const PCRE_ERROR_JIT_BADOPTION:Int =  -31
Const PCRE_ERROR_BADLENGTH:Int =      -32
Const PCRE_ERROR_UNSET:Int =          -33

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
Const PCRE_NEWLINE_ANYCRLF:Int =    $00500000
Const PCRE_BSR_ANYCRLF:Int =        $00800000
Const PCRE_BSR_UNICODE:Int =        $01000000
Const PCRE_JAVASCRIPT_COMPAT:Int =  $02000000
Const PCRE_NO_START_OPTIMIZE:Int =  $04000000
Const PCRE_PARTIAL_HARD:Int =       $08000000
Const PCRE_NOTEMPTY_ATSTART:Int =   $10000000

' Request types for pcre_fullinfo()

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
Const PCRE_INFO_OKPARTIAL:Int =         12
Const PCRE_INFO_JCHANGED:Int =          13
Const PCRE_INFO_HASCRORLF:Int =         14
Const PCRE_INFO_MINLENGTH:Int =         15
Const PCRE_INFO_JIT:Int =               16
Const PCRE_INFO_JITSIZE:Int =           17
Const PCRE_INFO_MAXLOOKBEHIND:Int =     18
Const PCRE_INFO_FIRSTCHARACTER:Int =    19
Const PCRE_INFO_FIRSTCHARACTERFLAGS:Int=20
Const PCRE_INFO_REQUIREDCHAR:Int =      21
Const PCRE_INFO_REQUIREDCHARFLAGS:Int = 22
Const PCRE_INFO_MATCHLIMIT:Int =        23
Const PCRE_INFO_RECURSIONLIMIT:Int =    24

' Specific error codes for UTF-8 validity checks

Const PCRE_UTF8_ERR0:Int =               0
Const PCRE_UTF8_ERR1:Int =               1
Const PCRE_UTF8_ERR2:Int =               2
Const PCRE_UTF8_ERR3:Int =               3
Const PCRE_UTF8_ERR4:Int =               4
Const PCRE_UTF8_ERR5:Int =               5
Const PCRE_UTF8_ERR6:Int =               6
Const PCRE_UTF8_ERR7:Int =               7
Const PCRE_UTF8_ERR8:Int =               8
Const PCRE_UTF8_ERR9:Int =               9
Const PCRE_UTF8_ERR10:Int =             10
Const PCRE_UTF8_ERR11:Int =             11
Const PCRE_UTF8_ERR12:Int =             12
Const PCRE_UTF8_ERR13:Int =             13
Const PCRE_UTF8_ERR14:Int =             14
Const PCRE_UTF8_ERR15:Int =             15
Const PCRE_UTF8_ERR16:Int =             16
Const PCRE_UTF8_ERR17:Int =             17
Const PCRE_UTF8_ERR18:Int =             18
Const PCRE_UTF8_ERR19:Int =             19
Const PCRE_UTF8_ERR20:Int =             20
Const PCRE_UTF8_ERR21:Int =             21
Const PCRE_UTF8_ERR22:Int =             22  ' Unused (was non-character)

' Specific error codes for UTF-16 validity checks

Const PCRE_UTF16_ERR0:Int =              0
Const PCRE_UTF16_ERR1:Int =              1
Const PCRE_UTF16_ERR2:Int =              2
Const PCRE_UTF16_ERR3:Int =              3
Const PCRE_UTF16_ERR4:Int =              4  ' Unused (was non-character)

' Specific error codes for UTF-32 validity checks 

Const PCRE_UTF32_ERR0:Int =              0
Const PCRE_UTF32_ERR1:Int =              1
Const PCRE_UTF32_ERR2:Int =              2  ' Unused (was non-character)
Const PCRE_UTF32_ERR3:Int =              3

Extern
	Function _strlen:Int(s:Byte Ptr) = "strlen"

	Function pcre16_config:Int(what:Int, where:Int Ptr)
	
	Function pcre16_compile2:Byte Ptr(pattern:Byte Ptr, options:Int, errorcodeptr:Int Ptr, ..
		errptr:Byte Ptr, erroffset:Int Ptr, tableptr:Byte Ptr)
	
	Function pcre16_exec:Int(pcre:Byte Ptr, extra:Byte Ptr, subject:Byte Ptr, ..
		length:Int, startoffset:Int, options:Int, ovector:Byte Ptr, ovecsize:Int)
	
	Function pcre16_get_substring:Int(subject:Byte Ptr, ovector:Byte Ptr, stringcount:Int, ..
		stringnumber:Int, stringptr:Short Ptr Ptr)
	
	Function pcre16_free_substring(strinptr:Short Ptr)
	
	Function pcre16_fullinfo:Int(pcre:Byte Ptr, extra:Byte Ptr, what:Int, where:Int Ptr)
End Extern
