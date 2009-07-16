SuperStrict

Import BRL.LinkedList

Type TDataBuffer

	Field data:TList = New TList
	Field remaining:Byte[]

	Method addLineBasedData(newData:Byte[], length:Int)
		Local last:Int = -1
		
		For Local i:Int = 0 Until length
		
			' ASCII 10 is a newline
			If newData[i] = 10 Then
			
				' trim out ASCII 13
				Local t:Int = 0
				If i > 1 And newData[i - 1] = 13 Then
					t = 1
				End If
				
				Local size:Int = i - last - t
				If remaining Then
					size:+ remaining.length
				End If
				
				Local b:Byte[] = New Byte[size - 1]
				
				If remaining Then
					MemCopy(b, remaining, remaining.length)
					' FIXME System.arraycopy(newData, last + 1, b, remaining.length, i - last - 1 - t)
				Else

					b = newData[last + 1..last + 1 + (i - last - 1 - t)]
					'MemCopy(b, (Varptr newData)[last + 1], i - last - 1 - t)
				End If
				
				last = i
				
				data.AddLast(b)
			
			End If
		
		Next
		
		' if we have data left over, hold onto it for adding to the next chunk.
		If last + 1 < length Then
			remaining = New Byte[length - last]
			MemCopy(remaining, (Varptr newData)[last + 1], length - last) 
		End If
		
	End Method

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
	
	Method ReadLine:String()
		Local data:Byte[] = Byte[](data.RemoveFirst())
		Return String.FromBytes(data, data.length)
	End Method

End Type


