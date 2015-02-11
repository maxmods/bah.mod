SuperStrict

Framework BaH.RegEx
Import BRL.StandardIO

Local regex:TRegEx = TRegEx.Create("^abc(\d+)")
Local match:TRegExMatch

Try

	match = regex.Find("xyz")
	
Catch e:TRegExException

	If e.num <> PCRE2_ERROR_NOMATCH Then
		Print "Error : " + e.toString()
		End
	Else
		Print "No Match"
	End If
	
End Try

If match Then

	For Local i:Int = 0 Until match.SubCount()
		Print i + ". " + match.SubExp(i)
	Next
	
End If

Print "Done."
