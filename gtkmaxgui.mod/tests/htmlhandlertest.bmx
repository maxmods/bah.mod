
Import BaH.gtkmaxgui
Import BaH.MaxUnit
'Import "../htmlhandler.bmx"

Local test:MyTest = MyTest.create()
test.run()

Type MyTest Extends TTestSuite

	Global html:THtmlHandler

	Function Create:MyTest()
		Local this:MyTest = New MyTest

		Return this
	End Function

	Method setUp()
		html = Null
	End Method

	Function testNoProtocol()
		html = THtmlHandler.html_url_new("brucey.net")

		assertEquals("", html._protocol)
	End Function

	Function testNoProtocolHostname()
		html = THtmlHandler.html_url_new("brucey.net")

		assertEquals("", html._hostname)
	End Function

	Function testNoProtocolPath()
		html = THtmlHandler.html_url_new("brucey.net")

		assertEquals("brucey.net", html._path)
	End Function

	Function testProtocol()
		html = THtmlHandler.html_url_new("http://brucey.net/dir/fred")

		assertEquals("http", html._protocol)
	End Function

	Function testHostname()
		html = THtmlHandler.html_url_new("http://brucey.net/dir/fred")

		assertEquals("brucey.net", html._hostname)
	End Function

	Function testPath()
		html = THtmlHandler.html_url_new("http://brucey.net/dir/fred")

		assertEquals("/dir/fred", html._path)
	End Function

	Function testPort()
		html = THtmlHandler.html_url_new("http://brucey.net:15000/dir/fred")

		assertEqualsI(15000, html._port)
	End Function
	
	Function testHostWithPort()
		html = THtmlHandler.html_url_new("http://brucey.net:15000/dir/fred")

		assertEquals("brucey.net", html._hostname)
	End Function

	Method addTests()
		add("testNoProtocol", testNoProtocol)
		add("testNoProtocolHostname", testNoProtocolHostname)
		add("testNoProtocolPath", testNoProtocolPath)
		add("testProtocol", testProtocol)
		add("testHostname", testHostname)
		add("testPath", testPath)
		add("testPort", testPort)
		add("testHostWithPort", testHostWithPort)
	End Method

End Type

