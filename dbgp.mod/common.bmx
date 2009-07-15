SuperStrict


Type StringBuffer

	Field buf:String
	
	Method append:StringBuffer(value:String)
		buf:+ value
		Return Self
	End Method

	Method ToString:String()
		Return buf
	End Method
	
End Type

Function StringToByteArray:Byte[](text:String)
	Local b:Byte Ptr = text.ToUTF8String()
	Local s:String = String.FromCString(b)
	MemFree(b)

	Local arr:Byte[] = New Byte[s.length]
	For Local i:Int = 0 Until s.length
		arr[i] = s[i]
	Next
	Return arr
End Function
