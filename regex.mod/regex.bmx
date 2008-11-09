' Copyright (c) 2007,2008 Bruce A Henderson
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

ModuleInfo "Version: 1.04"
ModuleInfo "Author: PCRE - Philip Hazel"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: PCRE - 1997-2007 University of Cambridge"
ModuleInfo "Copyright: Wrapper - 2007, 2008 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.04"
ModuleInfo "History: Fixed offset problems when working with non-ascii text."
ModuleInfo "History: 1.03"
ModuleInfo "History: Updated to PCRE 7.4"
ModuleInfo "History: 1.02"
ModuleInfo "History: Added grable's subquery/replace tweak."
ModuleInfo "History: 1.01"
ModuleInfo "History: Options now global."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. (PCRE 7.0)"

ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H"

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
	
	' pointer to the target string (as a CString)
	Field targ:Byte Ptr
	' the length of the target string
	Field targLength:Int
	
	Field lastEndPos:Int
	
	' pointer to the compiled expression
	Field pcre:Byte Ptr
	
	' pointer to the offsets vector
	Field offsets:Byte Ptr
	' number of offsets
	Field sizeOffsets:Int
	
	Field compiled:Int = False
	
	Method Delete()
		If offsets Then
			MemFree(offsets)
		End If
		
		If targ Then
			MemFree(targ)
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
			
			If targ Then
				MemFree(targ)
			End If
		
			Local tg:String = convertISO8859toUTF8(target)
			targ = tg.toCString()
			targLength = tg.length
		End If

		' initial search...
		Local result:Int = pcre_exec(pcre, Null, targ, targLength, startPos, getExecOpt(), offsets, sizeOffsets)

		' process string as UTF-8. This should keep offsets correct when processing non-ascii text.
		Local lastTargetutf8:String = convertISO8859toUTF8(lastTarget)

		' if there wasn't an error... process the match (even for no-match)
		While result >= 0 Or result = REGEX_NOMATCH

			Local replaceStr:String = replaceWith
			Local ofs:Int Ptr = Int Ptr(offsets)
			For Local i:Int = 0 To result
				Local idx:Int = i * 2
				replaceStr = replaceStr.Replace( "\" + i, lastTargetutf8[ofs[idx]..ofs[idx+1]])
			Next

			If result > 0 Then
				' add text so far, and the replacement
				retString:+ lastTargetutf8[startPos..Int Ptr(offsets)[0]] + replaceStr
			
			Else
				' search finished. Fill to the end
				retString:+ lastTargetutf8[startPos..targLength]

				Exit
			End If
			
			' set start to the end of the last match position
			startPos = Int Ptr(offsets)[1]
			
			' only doing the first replace? Then we can exit now...
			If Not globalReplace Then
				
				' all done. Fill to the end
				retString:+ lastTargetutf8[startPos..targLength]

				Exit
			End If
			
			' find the next match
			result:Int = pcre_exec(pcre, Null, targ, targLength, startPos, getExecOpt(), offsets, sizeOffsets)

		Wend	
		
		Return convertUTF8toISO8859(retString) ' convert back to max string
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
				
				If targ Then
					MemFree(targ)
				End If
			
				Local tg:String = convertISO8859toUTF8(target)
				targ = tg.toCString()
				targLength = tg.length
			End If
		End If
		
		' set the startPos to 0 if not already set
		If startPos < 0 Then
			startPos = 0
		End If
		
		Local result:Int = pcre_exec(pcre, Null, targ, targLength, startPos, getExecOpt(), offsets, sizeOffsets)
		
		If result >= 0 Then

			Local match:TRegExMatch = New TRegExMatch
			match.count = result
			match._subExp = New String[result + 1]
			match._subStart = New Int[result + 1]
			match._subEnd = New Int[result + 1]

			For Local i:Int = 0 Until result

				Local sPtr:Byte Ptr
				Local size:Int = pcre_get_substring(targ, offsets, result, i, Varptr sPtr)

				match._subExp[i] = sizedUTF8toISO8859(sPtr, size)
				match._subStart[i] = Int Ptr(offsets)[i * 2]

				lastEndPos = Int Ptr(offsets)[i * 2 + 1]

				match._subEnd[i] = lastEndPos
				
				pcre_free_substring(sPtr)

			Next
			
			Return match
		Else

			' no point raising an exception when nothing found... we can just return a null object
			If result = REGEX_NOMATCH Then
				Return Null
			End If
			
			' there was an error of some kind... throw it!
			Throw TRegExException.Raise(result)
		End If
	End Method
	
	' resets and recompiles the regular expression
	Method init()
		lastPattern = searchPattern
		
		Local pat:String = convertISO8859toUTF8(lastPattern)
		Local errorcodeptr:Int
		Local buffer:Byte[256]
		Local erroffset:Int
		
		Local bptr:Byte Ptr = pcre_compile2(pat, getCompileOpt(), Varptr errorcodeptr, ..
 				Varptr buffer, Varptr erroffset, Null)
			
		If bptr Then
			pcre = bptr
		Else
			Throw TRegExException.Raise(-99, String.fromCString(buffer))
		End If

		If offsets Then
			MemFree(offsets)
		End If

		Local result:Int = pcre_fullinfo(pcre, Null, PCRE_INFO_CAPTURECOUNT, Varptr sizeOffsets)
		sizeOffsets:+ 1 ' make sure there's enough space...
		sizeOffsets:* 3 ' multiple of 3
		offsets = MemAlloc((sizeOffsets * SizeOf(sizeOffsets)))
		
		compiled = True
	End Method
	
	' possible options for compiling
	Method getCompileOpt:Int()
		Local opt:Int = PCRE_UTF8
		
		If Not options.caseSensitive Then
			opt:| PCRE_CASELESS
		End If
		
		If options.dotMatchAll Then
			opt:| PCRE_DOTALL
		End If
		
		If Not options.greedy Then
			opt:| PCRE_UNGREEDY
		End If
		
		Select options.lineEndType
			Case 1
				opt:| PCRE_NEWLINE_CR
			Case 2
				opt:| PCRE_NEWLINE_CRLF
			Case 3
				opt:| PCRE_NEWLINE_LF
			Default
				opt:| PCRE_NEWLINE_ANY
		End Select
		
		If Not options.matchEmpty Then
			opt:| PCRE_NOTEMPTY
		End If
		
		If options.targetIsMultiline Then
			opt:| PCRE_MULTILINE
		End If
		
		If options.dollarEndOnly Then
			opt:| PCRE_DOLLAR_ENDONLY
		End If
		
		If options.extended Then
			opt:| PCRE_EXTENDED
		End If
		
		Return opt
	End Method
	
	' possible options for execution
	Method getExecOpt:Int()
		Local opt:Int
		
		Select options.lineEndType
			Case 1
				opt:| PCRE_NEWLINE_CR
			Case 2
				opt:| PCRE_NEWLINE_CRLF
			Case 3
				opt:| PCRE_NEWLINE_LF
			Default
				opt:| PCRE_NEWLINE_ANY
		End Select

		If Not options.matchEmpty Then
			opt:| PCRE_NOTEMPTY
		End If
		
		If Not options.stringIsLineBeginning Then
			opt:| PCRE_NOTBOL
		End If
		
		If Not options.stringIsLineEnding Then
			opt:| PCRE_NOTEOL
		End If
		
		Return opt
	End Method
	
End Type

Rem
bbdoc: Used to extract the matched string when doing a search with regular expressions.
End Rem
Type TRegExMatch

	Field count:Int
	Field _subExp:String[]
	Field _subStart:Int[]
	Field _subEnd:Int[]

	Rem
	bbdoc: Returns the number of subexpressions as a result of the search.
	End Rem
	Method SubCount:Int()
		Return count
	End Method
	
	Rem
	bbdoc: Returns the subexpression for @matchNumber.
	returns: The matched string, the subexpression string, or "" if @matchNumber is out of range.
	about: For expressions with no subpattern groups, this method can be used without
	a parameter to return the matched string.
	End Rem
	Method SubExp:String(matchNumber:Int = 0)
		If matchNumber >= 0 And matchNumber < count Then
			Return _subExp[matchNumber]
		End If
		Return ""
	End Method
	
	Rem
	bbdoc: Returns the start position for subexpression @matchNumber.
	returns: The start position, or -1 if @matchNumber is out of range.
	about: For expressions with no subpattern groups, this method can be used without a parameter
	to return the start position of the matched string.
	End Rem
	Method SubStart:Int(matchNumber:Int = 0)
		If matchNumber >= 0 And matchNumber < count Then
			Return _subStart[matchNumber]
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
		If matchNumber >= 0 And matchNumber < count Then
			Return _subEnd[matchNumber]
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
	<li>2 - \r\n</li>
	<li>3 - \n</li>
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
				Return "PCRE_ERROR_NULL"
			Case -3
				Return "PCRE_ERROR_BADOPTION"
			Case -4
				Return "PCRE_ERROR_BADMAGIC"
			Case -5
				Return "PCRE_ERROR_UNKNOWN_OPCODE"
			Case -6
				Return "PCRE_ERROR_NOMEMORY"
			Case -7
				Return "PCRE_ERROR_NOSUBSTRING"
			Case -8
				Return "PCRE_ERROR_MATCHLIMIT"
			Case -9
				Return "PCRE_ERROR_CALLOUT"
			Case -10
				Return "PCRE_ERROR_BADUTF8"
			Case -11
				Return "PCRE_ERROR_BADUTF8_OFFSET"
			Case -12
				Return "PCRE_ERROR_PARTIAL"
			Case -13
				Return "PCRE_ERROR_BADPARTIAL"
			Case -14
				Return "PCRE_ERROR_INTERNAL"
			Case -15
				Return "PCRE_ERROR_BADCOUNT"
			Case -16
				Return "PCRE_ERROR_DFA_UITEM"
			Case -17
				Return "PCRE_ERROR_DFA_UCOND"
			Case -18
				Return "PCRE_ERROR_DFA_UMLIMIT"
			Case -19
				Return "PCRE_ERROR_DFA_WSSIZE"
			Case -20
				Return "PCRE_ERROR_DFA_RECURSE"
			Case -21
				Return "PCRE_ERROR_RECURSIONLIMIT"
			Case -22
				Return "PCRE_ERROR_NULLWSLIMIT"
			Case -23
				Return "PCRE_ERROR_BADNEWLINE"
		End Select
	End Method
	
End Type
