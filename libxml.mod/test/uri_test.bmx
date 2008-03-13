SuperStrict

Framework BaH.libxml
Import BaH.MaxUnit

Local test:MyTest = MyTest.create()
test.run()

Type MyTest Extends TTestSuite


	Function create:MyTest()
		Local this:MyTest = New MyTest

		Return this
	End Function

	Method setUp()
	End Method

	Function testParse1()
		Local uri:TxmlURI = TxmlURI.parseURI("blitzmax.com")
		
		assertEquals("blitzmax.com", uri.saveURI())
		
		assertEquals("", uri.getScheme(), "Scheme")
		assertEquals("", uri.getOpaque(), "Opaque")
		assertEquals("", uri.getAuthority(), "Authority")
		assertEquals("", uri.getServer(), "Server")
		assertEquals("", uri.getUser(), "User")
		assertEqualsI(0, uri.getPort(), "Port")
		assertEquals("blitzmax.com", uri.getPath(), "Path")
		assertEquals("", uri.getQuery(), "Query")
	End Function

	Function testParse2()
		Local uri:TxmlURI = TxmlURI.parseURI("http://blitzmax.com")
		
		assertEquals("http://blitzmax.com", uri.saveURI())
		
		assertEquals("http", uri.getScheme(), "Scheme")
		assertEquals("", uri.getOpaque(), "Opaque")
		assertEquals("", uri.getAuthority(), "Authority")
		assertEquals("blitzmax.com", uri.getServer(), "Server")
		assertEquals("", uri.getUser(), "User")
		assertEqualsI(0, uri.getPort(), "Port")
		assertEquals("", uri.getPath(), "Path")
		assertEquals("", uri.getQuery(), "Query")
	End Function
	
	Function testParse3()
		Local uri:TxmlURI = TxmlURI.parseURI("http://blitzmax.com:80/somepath?whatisit=10")
		
		assertEquals("http://blitzmax.com:80/somepath?whatisit=10", uri.saveURI())
		
		assertEquals("http", uri.getScheme(), "Scheme")
		assertEquals("", uri.getOpaque(), "Opaque")
		assertEquals("", uri.getAuthority(), "Authority")
		assertEquals("blitzmax.com", uri.getServer(), "Server")
		assertEquals("", uri.getUser(), "User")
		assertEqualsI(80, uri.getPort(), "Port")
		assertEquals("/somepath", uri.getPath(), "Path")
		assertEquals("whatisit=10", uri.getQuery(), "Query")
	End Function

	Method addTests()
		add("testParse1", testParse1)
		add("testParse2", testParse2)
		add("testParse3", testParse3)
	End Method

End Type
