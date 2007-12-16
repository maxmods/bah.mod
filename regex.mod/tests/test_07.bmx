' replace full dates with years

SuperStrict

Framework BaH.RegEx
Import BRL.StandardIO

Local date:String = "The dates are: 12/30/1969, 06/04/1974 and 15/08/1980"
Print "Original : " + date + "~n"

Local regex:TRegEx = TRegEx.Create("(\d\d)[-/](\d\d)[-/](\d\d(?:\d\d)?)")

Try

	Print regex.replaceAll(date, "~q\3~q")

Catch e:TRegExException

	Print "Error : " + e.toString()
	End
	
End Try

Print "Done."

