'
' Just some helper functions for the examples
'
SuperStrict

Global default_precision:Int = 6

Function SetPrecision(precision:Int)
	default_precision = precision
End Function

' prettify Double output
Function nice:String(value:Double, prec:Int = 0)

	If prec < 1 Then
		prec = default_precision
	End If
	
	Local v:String = String.FromDouble(value)
	
	' handle exponential
	If v.find("e") > 0 Then
		Return v
	End If
	
	Local i:Int = v.Find(".")
	Local s:String = v[..i]
	Local p:String = ".", count:Int

	i:+ 1
	While i < v.length
		Local c:Int = v[i]
		If c > 48 And c < 58 Or count > 0 Then
			count:+ 1
		End If

		p:+ Chr(c)
		
		If count = prec Then
			Exit
		End If
	
		i:+ 1
	Wend
	
	If count = 0 Then
		p = ""
	End If

	Return s + p
End Function


