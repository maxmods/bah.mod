SuperStrict

Framework BaH.RegEx
Import BRL.StandardIO


Local regex:TRegEx = TRegEx.Create("(.*\/)(.*)$")

Try

	Local match:TRegExMatch = regex.Find("/home/mruser/my folder/coolfile.txt")
		
	
	While match
	
		Print "count = " + match.SubCount()
		
		For Local i:Int = 0 Until match.SubCount()
			Print "Found : " + match.SubExp(i)
		Next
		
		match = regex.Find()
	Wend

Catch e:TRegExException

	Print "Error : " + e.toString()
	End
	
End Try

Print "Done."
