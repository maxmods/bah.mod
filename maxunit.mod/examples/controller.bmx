SuperStrict

Type TMyController

	' add two numbers and multiply the result by 2
	' the observant amongst you will notice that the math is wrong!
	Method DoCalculation:Int(x:Int, y:Int)

		Return x + y * 2
	
	End Method

	' this is what the above method is *meant* to be doing.
	Method DoCalculationCorrect:Int(x:Int, y:Int)

		Return (x + y) * 2
	
	End Method

End Type

