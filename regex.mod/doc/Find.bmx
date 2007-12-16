' Search for a floating point number in a string

SuperStrict

Framework BaH.RegEx
Import BRL.StandardIO

Local floats:String = "floats are 4.533, -10.232, 1446.2003 and even 100"
Print "Original : " + floats + "~n"

Local regex:TRegEx = TRegEx.Create("[-+]?[0-9]*\.?[0-9]+")

Try

	Local match:TRegExMatch = regex.Find(floats)
		
	' get each match, and print it out.
	While match
	
		Print "Found : " + match.SubExp()
		
		match = regex.Find()
	Wend

Catch e:TRegExException

	Print "Error : " + e.toString()
	End
	
End Try

Print "Done."
