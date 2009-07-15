SuperStrict

Import BRL.LinkedList

Type TDataBuffer

	Field data:TList = New TList

	Method addNullBasedData(newData:Byte[], length:Int)
		
		If length > 0 Then
			DebugLog "length = " + length
			
			Local b:Byte[] = New Byte[length]
			
			MemCopy(b, newData, length)
			
			data.addLast(b)
		End If
		
	End Method
	
	Method lineAvail:Int()
		Return Not data.IsEmpty()
	End Method
	
	Method readBytes:Byte[]()
		Return Byte[](data.RemoveFirst())
	End Method

End Type


