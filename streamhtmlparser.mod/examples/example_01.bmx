SuperStrict

Framework BaH.StreamHtmlParser
Import BRl.StandardIO

Local parser:THtmlParser = New THtmlParser
Local stream:TStream = OpenStream("example_01.html", True, False)
Local out:TStream = StandardIOStream

Local c:Byte

While Not Eof(stream)
	stream.Read(Varptr c, 1)
	
	If c = 36 Then ' "$"
		out.WriteString("[[ ")
		
		If parser.tag() Then
			out.WriteString("tag=" + parser.tag() + " ")
		End If
		
		If parser.attr() Then
			out.WriteString("attr=" + parser.attr() + " ")
		End If
		
		out.WriteString("]]")
	Else
		parser.parseChr(c)
		out.WriteString(Chr(c))
	End If

Wend

End


