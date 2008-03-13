' textreader test

SuperStrict

Framework BaH.libxml
Import BRL.StandardIO

Local file:String = "test_small.xml"


Local reader:TxmlTextReader
Local ret:Int

reader = TxmlTextReader.fromFile(file)

If reader <> Null Then
	ret = reader.read()
	While ret = 1
		processNode(reader)
		ret = reader.read()
	Wend
	reader.free()
	If ret <> 0 Then
		Print file + " : failed to parse"
	End If
Else
	Print "Unable to open " + file
End If

	
Function processNode(reader:TxmlTextReader)

	Local indent:String = ""
	For Local i:Int = 0 Until reader.depth()
		indent:+ "  "
	Next
	
	Select reader.nodeType()
		Case 1
			Print indent + reader.name()
			Local count:Int = reader.attributeCount()
			For Local i:Int = 0 Until count
				Print indent + "  * att" + (i + 1) + " = " + reader.getAttributeByIndex(i)
			Next
	End Select

End Function
