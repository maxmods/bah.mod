'
' list the supported formats
'
SuperStrict

Framework BaH.Magick
Import BRL.StandardIO


Local formats:TList = coderInfoList()

For Local entry:TMCoderInfo = EachIn formats

	Local s:String = entry.name + ": (" + entry.description + ") : " 
	
	s:+ "Readable = "
	If entry.isReadable Then 
		s:+ "true"
	Else
		s:+ "false"
	End If
	
	s:+ ", "
	
    s:+ "Writable = "
	If entry.isWritable Then 
		s:+ "true"
	Else
		s:+ "false"
	End If

    s:+ ", "

    s:+ "Multiframe = "
	If entry.isMultiFrame Then 
		s:+ "true"
	Else
		s:+ "false"
	End If

	Print s

Next

