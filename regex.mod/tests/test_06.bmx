SuperStrict

Framework BaH.RegEx
Import BRL.StandardIO

Local change:String = "floats are 4.533, -10.232, 1446.2003 and even 100~n" + ..
	"If it wasn't enough 5 out of 10 applications said their users preferred them. That's 0.50 !~n"


Local regex:TRegEx = TRegEx.Create("[-+]?[0-9]*\.?[0-9]+")

Try

	Local s:String = regex.ReplaceAll(change, "!FLOAT!")
		
	Print "before = " + change
	Print "after  = " + s
	

Catch e:TRegExException

	Print "Error : " + e.toString()
	End
	
End Try

Print "Done."


