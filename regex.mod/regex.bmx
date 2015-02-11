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

Rem
bbdoc: Regular Expressions
End Rem
Module BaH.RegEx

ModuleInfo "Version: 1.06"
ModuleInfo "Author: PCRE - Philip Hazel"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: PCRE - 1997-2015 University of Cambridge"
ModuleInfo "Copyright: Wrapper - 2007-2015 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.06"
ModuleInfo "History: Updated to PCRE 10.00"
ModuleInfo "History: Changed TRegExMatch to get data as required, rather than cache it."
ModuleInfo "History: Added support for JIT compilation via new options."
ModuleInfo "History: Added ByName methods for sub expression retrieval."
ModuleInfo "History: 1.05"
ModuleInfo "History: Updated to PCRE 8.34"
ModuleInfo "History: Changed to use pcre16 functions, which is BlitzMax's native character size. (no more utf8 conversions)"
ModuleInfo "History: Don't include the zero-termination character in sub expressions."
ModuleInfo "History: 1.04"
ModuleInfo "History: Updated to PCRE 8.0"
ModuleInfo "History: Fixed offset problems when working with non-ascii text."
ModuleInfo "History: Fixed Replace() where loop was overflowing."
ModuleInfo "History: Added test_08 for utf-8."
ModuleInfo "History: 1.03"
ModuleInfo "History: Updated to PCRE 7.4"
ModuleInfo "History: 1.02"
ModuleInfo "History: Added grable's subquery/replace tweak."
ModuleInfo "History: 1.01"
ModuleInfo "History: Options now global."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. (PCRE 7.0)"

ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H -DPCRE2_CODE_UNIT_WIDTH=16"

Import "common.bmx"

Rem
bbdoc: Performs #Find and #Replace / #ReplaceAll on strings using Perl Compatible Regular Expressions.
End Rem
Type TRegEx
	' Configures the way in which Regular Expressions are handled.
	Global options:TRegExOptions

	' The pattern to search for.
	Field searchPattern:String


	' The replacement pattern string.
	'Field replacePattern:String
	
	Field lastPattern:String
	Field lastTarget:String
	
	' pointer to the target string (as a WString)
	Field TArg:Byte Ptr
	' the length of the target string
	Field targLength:Int
	
	Field lastEndPos:Int
	
	' pointer to the compiled expression
	Field pcre:Byte Ptr
	
	' pointer to the offsets vector
	Field offsets:Int Ptr
	' number of offsets
	Field sizeOffsets:Int
	
	Field matchPtr:Byte Ptr
	
	Field compiled:Int = False
	
	Method Delete()
		If offsets Then
			MemFree(offsets)
		End If
		
		If TArg Then
			MemFree(TArg)
		End If
		
		If matchPtr Then
			pcre2_match_data_free_16(matchPtr)
		End If
		
	End Method
	
	Rem
	bbdoc: Creates a new #TRegEx object.
	about: @searchPattern is the regular expression with which to perform the search.<br>
	@options sets the global #TRegExOptions. Overrides all previous options.
	End Rem
	Function Create:TRegEx(searchPattern:String, options:TRegExOptions = Null)
		Local this:TRegEx = New TRegEx
		
		this.searchPattern = searchPattern
		'this.replacePattern = replacePattern
		
		If options Then
			TRegEx.options = options
		End If
		
		Return this
	End Function
	
	Rem
	bbdoc: Replaces all occurances of the search Pattern with @replaceWith on @target, from @startPos.
	returns: The newly replaced string.
	about: Doesn't affect the original @target contents.
	End Rem
	Method ReplaceAll:String(target:String, replaceWith:String, startPos:Int = 0)

		If Not options Then
			options = New TRegExOptions
		End If

		' remember the current setting
		Local oldOpt:Int = options.replaceAllMatches
		
		' enable global replace
		options.replaceAllMatches = True
		
		' let Replace do all the work
		Local s:String = Replace(target, replaceWith, startPos)
		
		' put back the "current" setting
		options.replaceAllMatches = oldOpt
		
		Return s
	End Method
	
	Rem
	bbdoc: Replaces the first occurance of the search Pattern with @replaceWith on @target, from @startPos.
	returns: The newly replaced string.
	about: To access a specific subquery during the replace, you can use the \n syntax in @replaceWith, where \0
	refers to the whole match, and \1 refers to the first subquery/group, and so on. <i>(see
	<a href="../tests/test_07.bmx">test_07</a> for an example)</i>.
	<p>Doesn't affect the original @target contents.</p>
	End Rem
	Method Replace:String(target:String, replaceWith:String, startPos:Int = 0)
		If Not options Then
			options = New TRegExOptions
		End If

		Local globalReplace:Int = options.replaceAllMatches

		' the search pattern has changed !
		' recompile
		' this only happens very occasionally... (probably ;-)
		'If lastPattern <> searchPattern Or Not pcre Then
		If Not compiled Then
			init()
		End If

		Local retString:String
		
		' this is a new target... 
		If target <> lastTarget Then
			lastTarget = target
			
			If TArg Then
				MemFree(TArg)
			End If
		
			TArg = target.toWString()
			targLength = target.length
		End If

		' initial search...
		Local result:Int = pcre2_match_16(pcre, TArg, targLength, startPos, getExecOpt(), matchPtr, Null)

		' if there wasn't an error... process the match (even for no-match)
		While result >= 0 Or result = PCRE2_ERROR_NOMATCH

			sizeOffsets = pcre2_get_ovector_count_16(matchPtr)
			offsets = pcre2_get_ovector_pointer_16(matchPtr)

			Local replaceStr:String = replaceWith 
			Local ofs:Int Ptr = offsets
			For Local i:Int = 0 Until result
				Local idx:Int = i * 2
				replaceStr = replaceStr.Replace( "\" + i, lastTarget[ofs[idx]..ofs[idx+1]])
			Next

			If result > 0 Then
				' add text so far, and the replacement
				retString:+ lastTarget[startPos..offsets[0]] + replaceStr
			
			Else
				' search finished. Fill to the end
				retString:+ lastTarget[startPos..targLength]

				Exit
			End If
			
			' set start to the end of the last match position
			startPos = offsets[1]
			
			' only doing the first replace? Then we can exit now...
			If Not globalReplace Then
				
				' all done. Fill to the end
				retString:+ lastTarget[startPos..targLength]

				Exit
			End If
			
			' find the next match
			result = pcre2_match_16(pcre, TArg, targLength, startPos, getExecOpt(), matchPtr, Null)

		Wend	
		
		Return retString ' convert back to max string
	End Method
	
	Rem
	bbdoc: Performs a search on the given @target from @startPos, using the search Pattern.
	returns: A #TRegExMatch object or Null if no matches found.
	about: Both parameters are optional.<br>
	If @target is <b>not</b> set, the search will use the <b>previous</b> @target.
	You will want to set @target the first time this method is called.<br>
	If you call this method with no parameters it will start the search from the end of the last search, effectively
	iterating through the target string.
	End Rem
	Method Find:TRegExMatch(target:String = Null, startPos:Int = -1)
	
		If Not options Then
			options = New TRegExOptions
		End If

		' the search pattern has changed !
		' recompile
		' this only happens very occasionally... (probably ;-)
		'If lastPattern <> searchPattern Or Not pcre Then
		If Not compiled Then
			init()
		End If


		' no target specified, we are probably performing another search on the original target
		If Not target Then
		
			' no lastTarget? Not allowed.
			If Not lastTarget Then
				Return Null
			End If
			
			' no startPos? then we'll start from the end of the last search point
			If startPos < 0 Then
				startPos = lastEndPos
			End If
		Else
			' this is a new target... 
			If target <> lastTarget Then
				lastTarget = target
				
				If TArg Then
					MemFree(TArg)
				End If
			
				'Local tg:String = convertISO8859toUTF8(target)
				TArg = target.toWString()
				targLength = target.length
			End If
		End If
		
		' set the startPos to 0 if not already set
		If startPos < 0 Then
			startPos = 0
		End If
		
		Local result:Int = pcre2_match_16(pcre, TArg, targLength, startPos, getExecOpt(), matchPtr, Null)
		
		If result >= 0 Then

			sizeOffsets = pcre2_get_ovector_count_16(matchPtr)
			offsets = pcre2_get_ovector_pointer_16(matchPtr)

			Local match:TRegExMatch = New TRegExMatch
			match.pcre = pcre
			match.matchPtr = matchPtr

			lastEndPos = offsets[sizeOffsets]
			
			Return match
		Else

			' no point raising an exception when nothing found... we can just return a null object
			If result = PCRE2_ERROR_NOMATCH Then
				Return Null
			End If
			
			' there was an error of some kind... throw it!
			Throw TRegExException.Raise(result)
		End If
	End Method
	
	' resets and recompiles the regular expression
	Method init()
		lastPattern = searchPattern
		
		Local pat:Short Ptr = lastPattern.ToWString()
		Local errorcode:Int
		Local erroffset:Int

		Local bptr:Byte Ptr = pcre2_compile_16(pat, lastPattern.length, getCompileOpt(), Varptr errorcode, ..
 				Varptr erroffset, Null)

		MemFree(pat)
		
		If bptr Then
			pcre = bptr
		Else
			Local buffer:Short[256]
			pcre2_get_error_message_16(errorcode, buffer, 256)
			Throw TRegExException.Raise(-99, String.fromWString(buffer))
		End If

		Local jitOptions:Int = getJITOpt()
		If jitOptions Then
			Local result:Int = pcre2_jit_compile_16(pcre, jitOptions)
		End If
		
		If matchPtr Then
			pcre2_match_data_free_16(matchPtr)
		End If

		matchPtr = pcre2_match_data_create_from_pattern_16(pcre, Null)
	
		compiled = True
	End Method
	
	' possible options for compiling
	Method getCompileOpt:Int()
		Local opt:Int = PCRE2_UTF
		
		If Not options.caseSensitive Then
			opt:| PCRE2_CASELESS
		End If
		
		If options.dotMatchAll Then
			opt:| PCRE2_DOTALL
		End If
		
		If Not options.greedy Then
			opt:| PCRE2_UNGREEDY
		End If
		
		Select options.lineEndType
			Case 1
				opt:| PCRE2_NEWLINE_CR
			Case 2
				opt:| PCRE2_NEWLINE_LF
			Case 3
				opt:| PCRE2_NEWLINE_CRLF
			Default
				opt:| PCRE2_NEWLINE_ANY
		End Select
		
		If Not options.matchEmpty Then
			opt:| PCRE2_NOTEMPTY
		End If
		
		If options.targetIsMultiline Then
			opt:| PCRE2_MULTILINE
		End If
		
		If options.dollarEndOnly Then
			opt:| PCRE2_DOLLAR_ENDONLY
		End If
		
		If options.extended Then
			opt:| PCRE2_EXTENDED
		End If
		
		Return opt
	End Method
	
	' possible options for execution
	Method getExecOpt:Int()
		Local opt:Int
		
		Select options.lineEndType
			Case 1
				opt:| PCRE2_NEWLINE_CR
			Case 2
				opt:| PCRE2_NEWLINE_LF
			Case 3
				opt:| PCRE2_NEWLINE_CRLF
			Default
				opt:| PCRE2_NEWLINE_ANY
		End Select

		If Not options.matchEmpty Then
			opt:| PCRE2_NOTEMPTY
		End If
		
		If Not options.stringIsLineBeginning Then
			opt:| PCRE2_NOTBOL
		End If
		
		If Not options.stringIsLineEnding Then
			opt:| PCRE2_NOTEOL
		End If
		
		Return opt
	End Method
	
	' possible options for jit
	Method getJITOpt:Int()
		Local opt:Int
		
		If options.jitComplete Then
			opt :| PCRE2_JIT_COMPLETE
		End If

		If options.jitPartialSoft Then
			opt :| PCRE2_JIT_PARTIAL_SOFT
		End If

		If options.jitPartialHard Then
			opt :| PCRE2_JIT_PARTIAL_HARD
		End If
		
	End Method
	
End Type

Rem
bbdoc: Used to extract the matched string when doing a search with regular expressions.
End Rem
Type TRegExMatch

	Field pcre:Byte Ptr
	Field matchPtr:Byte Ptr

	Rem
	bbdoc: Returns the number of subexpressions as a result of the search.
	End Rem
	Method SubCount:Int()
		Return pcre2_get_ovector_count_16(matchPtr)
	End Method
	
	Rem
	bbdoc: Returns the subexpression for @matchNumber.
	returns: The matched string, the subexpression string, or "" if @matchNumber is out of range.
	about: For expressions with no subpattern groups, this method can be used without
	a parameter to return the matched string.
	End Rem
	Method SubExp:String(matchNumber:Int = 0)
		Local _subExpr:String
		
		If matchNumber >= 0 And matchNumber < pcre2_get_ovector_count_16(matchPtr) Then
			Local sPtr:Short Ptr
			Local sLen:Int
			Local result:Int = pcre2_substring_get_bynumber_16(matchPtr, matchNumber, Varptr sPtr, Varptr sLen)

			If Not result Then

				_subExpr = String.FromShorts(sPtr, sLen)
				
				pcre2_substring_free_16(sPtr)
			End If
		End If	
	
	
		Return _subExpr
	End Method
	
	Rem
	bbdoc: Returns the start position for subexpression @matchNumber.
	returns: The start position, or -1 if @matchNumber is out of range.
	about: For expressions with no subpattern groups, this method can be used without a parameter
	to return the start position of the matched string.
	End Rem
	Method SubStart:Int(matchNumber:Int = 0)
		If matchNumber >= 0 And matchNumber <  pcre2_get_ovector_count_16(matchPtr) Then
			Local offsets:Int Ptr = pcre2_get_ovector_pointer_16(matchPtr)
			Return offsets[matchNumber]
		End If
		Return -1
	End Method

	Rem
	bbdoc: Returns the end position for subexpression @matchNumber.
	returns: The end position, or -1 if @matchNumber is out of range.
	about: For expressions with no subpattern groups, this method can be used without a parameter
	to return the end position of the matched string.
	End Rem
	Method SubEnd:Int(matchNumber:Int = 0)
		If matchNumber >= 0 And matchNumber <  pcre2_get_ovector_count_16(matchPtr) Then
			Local offsets:Int Ptr = pcre2_get_ovector_pointer_16(matchPtr)
			Return offsets[matchNumber + 1] - 1
		End If
		Return -1
	End Method
	
	Rem
	bbdoc: Returns the subexpression for the given @name.
	End Rem
	Method SubExpByName:String(name:String)
		Local _subExpr:String

		If name Then
			Local sPtr:Short Ptr
			Local sLen:Int
			Local n:Short Ptr = name.ToWString()
			Local result:Int = pcre2_substring_get_byname_16(matchPtr, n, Varptr sPtr, Varptr sLen)
			MemFree(n)

			If Not result Then

				_subExpr = String.FromShorts(sPtr, sLen)
				
				pcre2_substring_free_16(sPtr)
			End If
		End If
		
		Return _subExpr
	End Method

	Rem
	bbdoc: Returns the index of the subexpression for the given @name.
	End Rem
	Method SubIndexByName:Int(name:String)
		If name Then
			Local n:Short Ptr = name.ToWString()
			Local index:Int = pcre2_substring_number_from_name_16(pcre, n)
			MemFree(n)
			
			If index >= 0 Then
				Return index
			End If
		End If
		Return -1
	End Method

	Rem
	bbdoc: Returns the start position of the subexpression for the given @name.
	End Rem
	Method SubStartByName:Int(name:String)
		If name Then
			Local n:Short Ptr = name.ToWString()
			Local index:Int = pcre2_substring_number_from_name_16(pcre, n)
			MemFree(n)
			
			If index >= 0 Then
				Local offsets:Int Ptr = pcre2_get_ovector_pointer_16(matchPtr)
				Return offsets[index]
			End If
		End If
		Return -1
	End Method

	Rem
	bbdoc: Returns the end position of the subexpression for the given @name.
	End Rem
	Method SubEndByName:Int(name:String)
		If name Then
			Local n:Short Ptr = name.ToWString()
			Local index:Int = pcre2_substring_number_from_name_16(pcre, n)
			MemFree(n)
			
			If index >= 0 Then
				Local offsets:Int Ptr = pcre2_get_ovector_pointer_16(matchPtr)
				Return offsets[index + 1] - 1
			End If
		End If
		Return -1
	End Method

End Type

Rem
bbdoc: Specifies options used when performing searches.
End Rem
Type TRegExOptions

	Rem
	bbdoc: Whether matches are case sensitive.
	End Rem
	Field caseSensitive:Int = False
	Rem
	bbdoc: Allow dot (period) to match new lines as well as everything else.
	about: False indicates dot doesn't match new lines.
	End Rem
	Field dotMatchAll:Int = False
	Rem
	bbdoc: Greedy matches everything from the beginning of the first delimeter to the end of the last delimiter, and everything in between.
	about: 
	End Rem
	Field greedy:Int = True
	Rem
	bbdoc: Determines how new lines are interpreted.
	about:
	<ul>
	<li>0 - any line ending</li>
	<li>1 - \r</li>
	<li>2 - \n</li>
	<li>3 - \r\n</li>
	</ul>
	End Rem
	Field lineEndType:Int = 0
	Rem
	bbdoc: Allow patterns to match empty strings.
	End Rem
	Field matchEmpty:Int = True
	'Rem
	'bbdoc: Indicates whether all occurances of matches will be replaced.
	'about: Only applicable during a Replace.
	'End Rem
	Field replaceAllMatches:Int = False
	Rem
	bbdoc: Count the beginning of a string as the beginning of a line.
	End Rem
	Field stringIsLineBeginning:Int = True
	Rem
	bbdoc: Count the end of a string as the end of a line.
	End Rem
	Field stringIsLineEnding:Int = True
	Rem
	bbdoc: Matches internal new lines against ^ and $.
	about: Set to false to ignore internal new lines.
	End Rem
	Field targetIsMultiline:Int = True
	Rem
	bbdoc: Dollar ($) matches newline at end.
	about: Set to True for dollar to only match the end of the string, otherwise
	matches a newline before the end of the string.
	End Rem
	Field dollarEndOnly:Int = False
	Rem
	bbdoc: Ignore whitespace and # comments.
	about: When set to True, whitespace in the pattern (other than in a character class) and
	characters between a # outside a character class and the next newline are ignored.<br>
	An escaping backslash can be used to include a whitespace or # character as part of the pattern.
	End Rem
	Field extended:Int = False

	Rem
	bbdoc: Compile code for full matching.
	about: When set to True, the JIT compiler is enabled.
	End Rem
	Field jitComplete:Int = False
	Rem
	bbdoc: Compile code For soft partial matching.
	about: When set to True, the JIT compiler is enabled.
	End Rem
	Field jitPartialSoft:Int = False
	Rem
	bbdoc: Compile code for hard partial matching.
	about: When set to True, the JIT compiler is enabled.
	End Rem
	Field jitPartialHard:Int = False

End Type

Rem
bbdoc: A Regular Expression exception.
about: This can be thrown either during regular expression compilation (-99) or during a search (-1 to -23).
End Rem
Type TRegExException
	Rem
	bbdoc: The type of error thrown.
	about: -99 is a regular expression compile error. Read #message for details.<br>
	-1 to -23 is thrown during a search. Read #message for details.
	End Rem
	Field num:Int
	Rem
	bbdoc: The error text.
	End Rem
	Field message:String
	
	Function Raise:TRegExException(num:Int, message:String = Null)
		Local this:TRegExException = New TRegExException
		
		If message Then
			this.message = message
		Else
			this.message = this.getFromNum(num)
		End If
		
		this.num = num
		Return this
	End Function
		
	Rem
	bbdoc: Returns the exception as a String.
	End Rem
	Method toString:String()
		Return "( " + num + " ) " + message
	End Method
	
	Method getFromNum:String(err:Int)
		Select err
			Case -1
				Return "No Match"
			Case -2
				Return "PCRE2_ERROR_PARTIAL"
			Case -3
				Return "PCRE2_ERROR_UTF8_ERR1"
			Case -4
				Return "PCRE2_ERROR_UTF8_ERR2"
			Case -5
				Return "PCRE2_ERROR_UTF8_ERR3"
			Case -6
				Return "PCRE2_ERROR_UTF8_ERR4"
			Case -7
				Return "PCRE2_ERROR_UTF8_ERR5"
			Case -8
				Return "PCRE2_ERROR_UTF8_ERR6"
			Case -9
				Return "PCRE2_ERROR_UTF8_ERR7"
			Case -10
				Return "PCRE2_ERROR_UTF8_ERR8"
			Case -11
				Return "PCRE2_ERROR_UTF8_ERR9"
			Case -12
				Return "PCRE2_ERROR_UTF8_ERR10"
			Case -13
				Return "PCRE2_ERROR_UTF8_ERR11"
			Case -14
				Return "PCRE2_ERROR_UTF8_ERR12"
			Case -15
				Return "PCRE2_ERROR_UTF8_ERR13"
			Case -16
				Return "PCRE2_ERROR_UTF8_ERR14"
			Case -17
				Return "PCRE2_ERROR_UTF8_ERR15"
			Case -18
				Return "PCRE2_ERROR_UTF8_ERR16"
			Case -19
				Return "PCRE2_ERROR_UTF8_ERR17"
			Case -20
				Return "PCRE2_ERROR_UTF8_ERR18"
			Case -21
				Return "PCRE2_ERROR_UTF8_ERR19"
			Case -22
				Return "PCRE2_ERROR_UTF8_ERR20"
			Case -23
				Return "PCRE2_ERROR_UTF8_ERR21"
			Case -24
				Return "PCRE2_ERROR_UTF16_ERR1"
			Case -25
				Return "PCRE2_ERROR_UTF16_ERR2"
			Case -26
				Return "PCRE2_ERROR_UTF16_ERR3"
			Case -27
				Return "PCRE2_ERROR_UTF32_ERR1"
			Case -28
				Return "PCRE2_ERROR_UTF32_ERR2"
			Case -29
				Return "PCRE2_ERROR_BADDATA"
			Case -30
				Return "PCRE2_ERROR_BADLENGTH"
			Case -31
				Return "PCRE2_ERROR_BADMAGIC"
			Case -32
				Return "PCRE2_ERROR_BADMODE"
			Case -33
				Return "PCRE2_ERROR_BADOFFSET"
			Case -34
				Return "PCRE2_ERROR_BADOPTION"
			Case -35
				Return "PCRE2_ERROR_BADREPLACEMENT"
			Case -36
				Return "PCRE2_ERROR_BADUTFOFFSET"
			Case -37
				Return "PCRE2_ERROR_CALLOUT"
			Case -38
				Return "PCRE2_ERROR_DFA_BADRESTART"
			Case -39
				Return "PCRE2_ERROR_DFA_RECURSE"
			Case -40
				Return "PCRE2_ERROR_DFA_UCOND"
			Case -41
				Return "PCRE2_ERROR_DFA_UFUNC"
			Case -42
				Return "PCRE2_ERROR_DFA_UITEM"
			Case -43
				Return "PCRE2_ERROR_DFA_WSSIZE"
			Case -44
				Return "PCRE2_ERROR_INTERNAL"
			Case -45
				Return "PCRE2_ERROR_JIT_BADOPTION"
			Case -46
				Return "PCRE2_ERROR_JIT_STACKLIMIT"
			Case -47
				Return "PCRE2_ERROR_MATCHLIMIT"
			Case -48
				Return "PCRE2_ERROR_NOMEMORY"
			Case -49
				Return "PCRE2_ERROR_NOSUBSTRING"
			Case -50
				Return "PCRE2_ERROR_NOUNIQUESUBSTRING"
			Case -51
				Return "PCRE2_ERROR_NULL"
			Case -52
				Return "PCRE2_ERROR_RECURSELOOP"
			Case -53
				Return "PCRE2_ERROR_RECURSIONLIMIT"
			Case -54
				Return "PCRE2_ERROR_UNAVAILABLE"
			Case -55
				Return "PCRE2_ERROR_UNSET"
		End Select
	End Method
	
End Type
