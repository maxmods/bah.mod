SuperStrict

Framework BaH.Math
Import BRL.StandardIO

Print "Factorial"
Print "---------"
For Local i:Int = 1 To 10
	Print i + "! = " + Factorial(i)
Next

Print ""
Print "UncheckedFactorial"
Print "------------------"
For Local i:Int = 1 To 10
	Print i + "! = " + UncheckedFactorial(i)
Next

Print ""
Print "DoubleFactorial"
Print "---------------"
For Local i:Int = 1 To 10
	Print i + "!! = " + DoubleFactorial(i)
Next

