SuperStrict

Framework BaH.libiconv
Import BRL.StandardIO

DebugStop
Local iconv:TIConv = TIConv.Create("CP1256", "ISO-8859-6")

If iconv Then
	'Local s:String = 
	'Local b:Byte Ptr = s.ToCString()
	Local b:Byte[] = [$df:Byte]
	Local bsize:Int = 1
	
	Local out:Byte[1024]
	Local outLen:Int = 1024
	
	If iconv.Convert(b, bsize, out, outLen) < 0 Then
		Print iconv.LastError()
	End If
	
	For Local i:Int = 0 Until 1024-outLen
'		Print b[i] + " : " + s[i] + " : " + out[i]
		Print b[i] + " : " + out[i]
	Next
	
	Local s2:String = String.FromBytes(out, 1024 - outLen)
	Print s2
	
Else
	If TIconv.LastError() = 22 Then
		Print "conversion from " + "CP1256" + " To " + "ISO-8859-6" + " not available"
	End If
End If