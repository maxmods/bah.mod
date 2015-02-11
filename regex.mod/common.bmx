' Copyright (c) 2007-2015 Bruce A Henderson
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

Import "pcre/src/pcre2_auto_possess.c"
Import "pcre/src/pcre2_chartables.c"
Import "pcre/src/pcre2_compile.c"
Import "pcre/src/pcre2_config.c"
Import "pcre/src/pcre2_context.c"
Import "pcre/src/pcre2_dfa_match.c"
Import "pcre/src/pcre2_error.c"
Import "pcre/src/pcre2_jit_compile.c"
Import "pcre/src/pcre2_maketables.c"
Import "pcre/src/pcre2_match.c"
Import "pcre/src/pcre2_match_data.c"
Import "pcre/src/pcre2_newline.c"
Import "pcre/src/pcre2_ord2utf.c"
Import "pcre/src/pcre2_pattern_info.c"
Import "pcre/src/pcre2_string_utils.c"
Import "pcre/src/pcre2_study.c"
Import "pcre/src/pcre2_substitute.c"
Import "pcre/src/pcre2_substring.c"
Import "pcre/src/pcre2_tables.c"
Import "pcre/src/pcre2_ucd.c"
Import "pcre/src/pcre2_valid_utf.c"
Import "pcre/src/pcre2_xclass.c"


' Request types for pcre2_config(). 
Const PCRE2_CONFIG_BSR:Int =                     0
Const PCRE2_CONFIG_JIT:Int =                     1
Const PCRE2_CONFIG_JITTARGET:Int =               2
Const PCRE2_CONFIG_LINKSIZE:Int =                3
Const PCRE2_CONFIG_MATCHLIMIT:Int =              4
Const PCRE2_CONFIG_NEWLINE:Int =                 5
Const PCRE2_CONFIG_PARENSLIMIT:Int =             6
Const PCRE2_CONFIG_RECURSIONLIMIT:Int =          7
Const PCRE2_CONFIG_STACKRECURSE:Int =            8
Const PCRE2_CONFIG_UNICODE:Int =                 9
Const PCRE2_CONFIG_UNICODE_VERSION:Int =        10
Const PCRE2_CONFIG_VERSION:Int =                11

' Exec-time and get/set-time error codes
' Error codes: no match and partial match are "expected" errors.
Const PCRE2_ERROR_NOMATCH:Int =         -1
Const PCRE2_ERROR_PARTIAL:Int =         -2

' Error codes for UTF-8 validity checks

Const PCRE2_ERROR_UTF8_ERR1:Int =        -3
Const PCRE2_ERROR_UTF8_ERR2:Int =        -4
Const PCRE2_ERROR_UTF8_ERR3:Int =        -5
Const PCRE2_ERROR_UTF8_ERR4:Int =        -6
Const PCRE2_ERROR_UTF8_ERR5:Int =        -7
Const PCRE2_ERROR_UTF8_ERR6:Int =        -8
Const PCRE2_ERROR_UTF8_ERR7:Int =        -9
Const PCRE2_ERROR_UTF8_ERR8:Int =       -10
Const PCRE2_ERROR_UTF8_ERR9:Int =       -11
Const PCRE2_ERROR_UTF8_ERR10:Int =      -12
Const PCRE2_ERROR_UTF8_ERR11:Int =      -13
Const PCRE2_ERROR_UTF8_ERR12:Int =      -14
Const PCRE2_ERROR_UTF8_ERR13:Int =      -15
Const PCRE2_ERROR_UTF8_ERR14:Int =      -16
Const PCRE2_ERROR_UTF8_ERR15:Int =      -17
Const PCRE2_ERROR_UTF8_ERR16:Int =      -18
Const PCRE2_ERROR_UTF8_ERR17:Int =      -19
Const PCRE2_ERROR_UTF8_ERR18:Int =      -20
Const PCRE2_ERROR_UTF8_ERR19:Int =      -21
Const PCRE2_ERROR_UTF8_ERR20:Int =      -22
Const PCRE2_ERROR_UTF8_ERR21:Int =      -23

' Error codes for UTF-16 validity checks

Const PCRE2_ERROR_UTF16_ERR1:Int =      -24
Const PCRE2_ERROR_UTF16_ERR2:Int =      -25
Const PCRE2_ERROR_UTF16_ERR3:Int =      -26

' Error codes for UTF-32 validity checks

Const PCRE2_ERROR_UTF32_ERR1:Int =      -27
Const PCRE2_ERROR_UTF32_ERR2:Int =      -28

' Error codes for pcre2[_dfa]_match(), substring extraction functions, and context functions.

Const PCRE2_ERROR_BADDATA:Int =           -29
Const PCRE2_ERROR_BADLENGTH:Int =         -30
Const PCRE2_ERROR_BADMAGIC:Int =          -31
Const PCRE2_ERROR_BADMODE:Int =           -32
Const PCRE2_ERROR_BADOFFSET:Int =         -33
Const PCRE2_ERROR_BADOPTION:Int =         -34
Const PCRE2_ERROR_BADREPLACEMENT:Int =    -35
Const PCRE2_ERROR_BADUTFOFFSET:Int =      -36
Const PCRE2_ERROR_CALLOUT:Int =           -37  ' Never used by PCRE2 itself
Const PCRE2_ERROR_DFA_BADRESTART:Int =    -38
Const PCRE2_ERROR_DFA_RECURSE:Int =       -39
Const PCRE2_ERROR_DFA_UCOND:Int =         -40
Const PCRE2_ERROR_DFA_UFUNC:Int =         -41
Const PCRE2_ERROR_DFA_UITEM:Int =         -42
Const PCRE2_ERROR_DFA_WSSIZE:Int =        -43
Const PCRE2_ERROR_INTERNAL:Int =          -44
Const PCRE2_ERROR_JIT_BADOPTION:Int =     -45
Const PCRE2_ERROR_JIT_STACKLIMIT:Int =    -46
Const PCRE2_ERROR_MATCHLIMIT:Int =        -47
Const PCRE2_ERROR_NOMEMORY:Int =          -48
Const PCRE2_ERROR_NOSUBSTRING:Int =       -49
Const PCRE2_ERROR_NOUNIQUESUBSTRING:Int = -50
Const PCRE2_ERROR_NULL:Int =              -51
Const PCRE2_ERROR_RECURSELOOP:Int =       -52
Const PCRE2_ERROR_RECURSIONLIMIT:Int =    -53
Const PCRE2_ERROR_UNAVAILABLE:Int =       -54
Const PCRE2_ERROR_UNSET:Int =             -55

' The following option bits can be passed to pcre2_compile(), pcre2_match(),
' or pcre2_dfa_match(). PCRE2_NO_UTF_CHECK affects only the function to which it
' is passed. Put these bits at the most significant end of the options word so
' others can be added next to them.

Const PCRE2_ANCHORED:Int =            $80000000
Const PCRE2_NO_UTF_CHECK:Int =        $40000000

' The following option bits can be passed only to pcre2_compile(). However,
' they may affect compilation, JIT compilation, and/or interpretive execution.
' The following tags indicate which:
'
' C   alters what is compiled by pcre2_compile()
' J   alters what is compiled by pcre2_jit_compile()
' M   is inspected during pcre2_match() execution
' D   is inspected during pcre2_dfa_match() execution

Const PCRE2_ALLOW_EMPTY_CLASS:Int =   $00000001  ' C      
Const PCRE2_ALT_BSUX:Int =            $00000002  ' C      
Const PCRE2_AUTO_CALLOUT:Int =        $00000004  ' C      
Const PCRE2_CASELESS:Int =            $00000008  ' C      
Const PCRE2_DOLLAR_ENDONLY:Int =      $00000010  '   J M D
Const PCRE2_DOTALL:Int =              $00000020  ' C      
Const PCRE2_DUPNAMES:Int =            $00000040  ' C      
Const PCRE2_EXTENDED:Int =            $00000080  ' C      
Const PCRE2_FIRSTLINE:Int =           $00000100  '   J M D
Const PCRE2_MATCH_UNSET_BACKREF:Int = $00000200  ' C J M  
Const PCRE2_MULTILINE:Int =           $00000400  ' C      
Const PCRE2_NEVER_UCP:Int =           $00000800  ' C      
Const PCRE2_NEVER_UTF:Int =           $00001000  ' C      
Const PCRE2_NO_AUTO_CAPTURE:Int =     $00002000  ' C      
Const PCRE2_NO_AUTO_POSSESS:Int =     $00004000  ' C      
Const PCRE2_NO_DOTSTAR_ANCHOR:Int =   $00008000  ' C      
Const PCRE2_NO_START_OPTIMIZE:Int =   $00010000  '   J M D
Const PCRE2_UCP:Int =                 $00020000  ' C J M D
Const PCRE2_UNGREEDY:Int =            $00040000  ' C      
Const PCRE2_UTF:Int =                 $00080000  ' C J M D

' These are for pcre2_jit_compile(). 

Const PCRE2_JIT_COMPLETE:Int =        $00000001  ' For full matching
Const PCRE2_JIT_PARTIAL_SOFT:Int =    $00000002
Const PCRE2_JIT_PARTIAL_HARD:Int =    $00000004

' These are for pcre2_match() and pcre2_dfa_match(). Note that PCRE2_ANCHORED,
' and PCRE2_NO_UTF_CHECK can also be passed to these functions, so take care not
' to define synonyms by mistake.

Const PCRE2_NOTBOL:Int =              $00000001
Const PCRE2_NOTEOL:Int =              $00000002
Const PCRE2_NOTEMPTY:Int =            $00000004  ' ) These two must be kept
Const PCRE2_NOTEMPTY_ATSTART:Int =    $00000008  ' ) adjacent to each other.
Const PCRE2_PARTIAL_SOFT:Int =        $00000010
Const PCRE2_PARTIAL_HARD:Int =        $00000020

' These are additional options for pcre2_dfa_match().

Const PCRE2_DFA_RESTART:Int =         $00000040
Const PCRE2_DFA_SHORTEST:Int =        $00000080

' This is an additional option for pcre2_substitute().

Const PCRE2_SUBSTITUTE_GLOBAL:Int =   $00000100


' Request types for Request types for pcre2_pattern_info()

Const PCRE2_INFO_ALLOPTIONS:Int =            0
Const PCRE2_INFO_ARGOPTIONS:Int =            1
Const PCRE2_INFO_BACKREFMAX:Int =            2
Const PCRE2_INFO_BSR:Int =                   3
Const PCRE2_INFO_CAPTURECOUNT:Int =          4
Const PCRE2_INFO_FIRSTCODEUNIT:Int =         5
Const PCRE2_INFO_FIRSTCODETYPE:Int =         6
Const PCRE2_INFO_FIRSTBITMAP:Int =           7
Const PCRE2_INFO_HASCRORLF:Int =             8
Const PCRE2_INFO_JCHANGED:Int =              9
Const PCRE2_INFO_JITSIZE:Int =              10
Const PCRE2_INFO_LASTCODEUNIT:Int =         11
Const PCRE2_INFO_LASTCODETYPE:Int =         12
Const PCRE2_INFO_MATCHEMPTY:Int =           13
Const PCRE2_INFO_MATCHLIMIT:Int =           14
Const PCRE2_INFO_MAXLOOKBEHIND:Int =        15
Const PCRE2_INFO_MINLENGTH:Int =            16
Const PCRE2_INFO_NAMECOUNT:Int =            17
Const PCRE2_INFO_NAMEENTRYSIZE:Int =        18
Const PCRE2_INFO_NAMETABLE:Int =            19
Const PCRE2_INFO_NEWLINE:Int =              20
Const PCRE2_INFO_RECURSIONLIMIT:Int =       21
Const PCRE2_INFO_SIZE:Int =                 22

' Newline and \R settings, for use in compile contexts. The newline values
' must be kept in step with values set in config.h and both sets must all be
' greater than zero.

Const PCRE2_NEWLINE_CR:Int =          1
Const PCRE2_NEWLINE_LF:Int =          2
Const PCRE2_NEWLINE_CRLF:Int =        3
Const PCRE2_NEWLINE_ANY:Int =         4
Const PCRE2_NEWLINE_ANYCRLF:Int =     5

Extern
	Function _strlen:Int(s:Byte Ptr) = "strlen"

	Function pcre2_config_16:Int(what:Int, where:Int Ptr)
	
	Function pcre2_compile_16:Byte Ptr(pattern:Short Ptr, patternLength:Int, options:Int, errorcodeptr:Int Ptr, ..
		erroffset:Int Ptr, contextptr:Byte Ptr)
	
	'Function pcre16_exec:Int(pcre:Byte Ptr, extra:Byte Ptr, subject:Byte Ptr, ..
	'	length:Int, startoffset:Int, options:Int, ovector:Byte Ptr, ovecsize:Int)
	Function pcre2_match_16:Int(pattern:Byte Ptr, subject:Byte Ptr, subjectLength:Int, startOffset:Int, ..
		options:Int, matchPtr:Byte Ptr, context:Byte Ptr)
	
	Function pcre2_substring_get_bynumber_16:Int(matchPtr:Byte Ptr, stringnumber:Int, ..
		stringptr:Short Ptr Ptr, stringlength:Int Ptr)
	Function pcre2_substring_length_bynumber_16:Int(matchPtr:Byte Ptr, stringnumber:Int, length:Int Ptr)
	
	Function pcre2_substring_free_16(strinptr:Short Ptr)
	
	Function pcre2_match_data_create_from_pattern_16:Byte Ptr(pcre:Byte Ptr, context:Byte Ptr)
	Function pcre2_match_data_free_16(matchPtr:Byte Ptr)
	
	Function pcre2_pattern_info_16:Int(pcre:Byte Ptr, what:Int, where:Int Ptr)
	
	Function pcre2_get_error_message_16:Int(errorcode:Int, buffer:Short Ptr, length:Int)
	
	Function pcre2_get_ovector_count_16:Int(matchPtr:Byte Ptr)
	Function pcre2_get_ovector_pointer_16:Int Ptr(matchPtr:Byte Ptr)
	
	Function pcre2_jit_compile_16:Int(pcre:Byte Ptr, options:Int)
	
	Function pcre2_substring_get_byname_16:Int(matchPtr:Byte Ptr, name:Short Ptr, stringptr:Short Ptr Ptr, stringlength:Int Ptr)
	Function pcre2_substring_number_from_name_16:Int(pcre:Byte Ptr, name:Short Ptr)
	
End Extern
