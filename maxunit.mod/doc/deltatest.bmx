Import BaH.MaxUnit

New TTestSuite.run()

Type MyTest Extends TTest

	Method testFloat() { test }
		assertEqualsF(11.99,11.991, 0.01, "Float")
	End Method 

	Method testDouble() { test }
		assertEqualsD(11.99,11.991, 0.01, "Double")
	End Method 

	Method testFloatFail() { test }
		assertEqualsF(11.99,11.991, 0.001, "Meant to fail - Float")
	End Method 

	Method testDoubleFail() { test }
		assertEqualsD(11.99,11.991, 0.001, "Meant to fail - Double")
	End Method 
	
End Type

