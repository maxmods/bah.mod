SuperStrict

Framework BaH.muParser
Import BRL.StandardIO


Global variables:Double[] = New Double[100]


Try

	Local parser:TmuParser = New TmuParser

	Local expr:String = "MyFunc(a)*pi+min(10,a)"

	parser.SetVarFactory(AddVariable, Null)

	parser.DefineFun1("MyFunc", MyFunction)
	parser.SetExpr(expr)

	variables[0] = 1

	Print parser.Eval()
	
	variables[0] = 15

	Print parser.Eval()

Catch e:TmuParserException

	Print e.message

End Try


Function MyFunction:Double(fVal:Double) 
	Return fVal * fVal
End Function


Function AddVariable:Double Ptr(name:String, userData:Object)

	Global iVal:Int = 0
	
	If iVal < 100
		Print "Generating new variable '" + name + "' (slots left: " + (99 - iVal) + ")"
	
		Return Double Ptr(variables) + iVal
		iVal:+ 1
	Else
		Throw "No more available variables."
	End If
	
End Function
