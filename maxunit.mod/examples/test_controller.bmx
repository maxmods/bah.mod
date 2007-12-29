SuperStrict

Framework BaH.MaxUnit

Import "controller.bmx"


' run the tests!
New TTestSuite.run()


Rem
bbdoc: Tests for TMyController
End Rem
Type ControllerTest Extends TTest

	Field controller:TMyController

	Method setup() { before }
		' We create a new instance of TMyController for each test
		' You might also populate a type with starting data appropriate for the set of tests,
		' or reset counters etc.
		controller = New TMyController
	End Method


	Rem
	bbdoc: Test for TMyController.DoCalculation
	about: Tests that two numbers are added together and then multiplied by 2
	End Rem
	Method testDoCalculation() { test }
	
		Local result:Int = controller.DoCalculation( 1, 1 )
		Local expectedResult:Int = 4 '  1 + 1 = 2 * 2 = 4
		
		' This test will fail, because it turns out our calculation is wrong.
		' We missed brackets around the x+y part, so the answer is incorrect.
		'
		assertEqualsI(expectedResult, result, "This is actually meant to fail...")
	
	End Method

	Rem
	bbdoc: Test for TMyController.DoCalculationCorrect
	about: Tests that two numbers are added together and then multiplied by 2
	End Rem
	Method testDoCalculationCorrect() { test }
	
		Local result:Int = controller.DoCalculationCorrect( 1, 1 )
		Local expectedResult:Int = 4 '  1 + 1 = 2 * 2 = 4
		
		' This shows that were we to fix the error in the code, the test would
		' then pass :-)
		'
		assertEqualsI(expectedResult, result)
	
	End Method

End Type
