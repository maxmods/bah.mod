SuperStrict

Framework BaH.Expat
Import BRL.StandardIO
Import BRL.RamStream

Incbin "plant_catalog.xml"

Local stream:TStream = ReadStream("incbin::plant_catalog.xml")

Local parser:TXMLParser = New TXMLParser.Create()

parser.SetStartElementHandler(StartElementHandler)

If Not parser.Parse(stream) Then

	Print XMLErrorString(parser.GetErrorCode()) + " at line " + ..
		parser.GetCurrentLineNumber()

End If

stream.Close()
parser.Free()

End

Function StartElementHandler(userData:Object, name:String, attribs:String[])
	Print "name = " + name
End Function


