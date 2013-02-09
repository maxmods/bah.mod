SuperStrict

Framework BaH.muParser
Import BRL.StandardIO

Try

	Local parser:TmuParser = New TmuParser

	Local fVal:Double = 1
	Local expr:String = "MyFunc(a)*pi+min(10,a)"

	parser.DefineVar("a", fVal)
	parser.DefineFun1("MyFunc", MyFunction)
	parser.SetExpr(expr)

	Print parser.Eval()
	
	fVal = 15

	Print parser.Eval()

Catch e:TmuParserException

	Print e.message

End Try


Function MyFunction:Double(fVal:Double) 
	Return fVal * fVal
End Function

