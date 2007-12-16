' Match and capture group
' Extract fields from date string.

SuperStrict

Framework BaH.RegEx
Import BRL.StandardIO

Local date:String = "The dates are: 12/30/1969, 06/04/1974 and 15/08/1980"
Print "Original : " + date + "~n"

Local regex:TRegEx = TRegEx.Create("(\d\d)[-/](\d\d)[-/](\d\d(?:\d\d)?)")

Try

	Local match:TRegExMatch = regex.Find(date)
	
	While match

		Print "~nDate -"
		For Local i:Int = 0 Until match.SubCount()
			Print i + ": " + match.SubExp(i)
		Next

		match = regex.Find()
	Wend

Catch e:TRegExException

	Print "Error : " + e.toString()
	End
	
End Try

Print "Done."

