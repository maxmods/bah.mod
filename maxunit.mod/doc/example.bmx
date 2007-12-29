Import BaH.MaxUnit

New TTestSuite.run()             ' <-- run all the tests!

Type MyTest Extends TTest        ' <-- extend from TTest

	Field value1:Int
	Field value2:Int

	Method setup() { before }    ' <-- {before} is run once per test method. Optional.
		value1 = 2
		value2 = 3
	End Method

	Method testAdd() { test }   ' <-- {test} defines a single test to run.
		Local result:Int = value1 + value2
		assertTrue(result = 5)
	End Method
	
	Method testMultiply() { test }
		Local result:Int = value1 * value2
		assertEqualsI(6, result)
	End Method

	Method testOops() { test }
		Local result:Int = value1 + value2
		assertEqualsI(2, result, "Just here to show a failed test...")
	End Method

End Type
