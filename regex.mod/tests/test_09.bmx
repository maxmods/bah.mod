SuperStrict

Framework brl.standardio

Import bah.regex

Local str$ = "yet another test"
Local match:TRegExMatch
Local regex:TRegEx = TRegEx.Create( "(?<word>\w*)" )
Local index:Int

Try
	match = regex.Find( str )
Catch e:TRegExException
	If e.num <> PCRE2_ERROR_NOMATCH Then
		Print "Error : " + e.toString()
		End
	Else
		Print "No Match"
	End If
End Try

If match Then
	'index = regex.getStringNumber("word")
	Print match.SubIndexByName("word") + ". " + match.SubExpByName("word")
End If
Print "Done."

