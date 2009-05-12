SuperStrict

Framework BaH.Expat
Import BRL.StandardIO

Local s:String = "<root><child>Hello World</child></root>"

Local parser:TXMLParser = New TXMLParser.Create()

parser.SetStartElementHandler(StartElementHandler)

If Not parser.Parse(s) Then

	Print XMLErrorString(parser.GetErrorCode()) + " at line " + ..
		parser.GetCurrentLineNumber()

End If


parser.Free()

End

Function StartElementHandler(userData:Object, name:String, attribs:String[])
	Print "name = " + name
End Function

