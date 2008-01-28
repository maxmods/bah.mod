SuperStrict

Type TBigInt
	Const mask:Long = $ffffffff:Long

	Field sign:Int = 0
	Field magnitude:Int[]
	
	Function Create:TBigInt(value:Long)
		Local this:TBigInt = New TBigInt
		
		If value < 0 Then
			this.sign = -1
			value = -value
		ElseIf value > 0 Then
			this.sign = 1
		Else
			this.magnitude = New Int[0]
			Return this
		End If
		
		Local high:Int = value Shr 32
		If Not high Then
			this.magnitude = New Int[1]
			this.magnitude[0] = Int(value)
		Else
			this.magnitude = New Int[2]
			this.magnitude[0] = high
			this.magnitude[1] = Int(value)
		End If
		
		Return this
	End Function
	
	Function CreateA:TBigInt(magnitude:Int[], sign:Int)
		Local this:TBigInt = New TBigInt
		
		this.magnitude = magnitude
		this.sign = sign
		
		Return this
	End Function
	
	Method power:TBigInt(exponent:Int)
		If exponent < 0 Then
			Return Null
		End If
		
		If sign = 0 Then
			If exponent = 0 Then
				Return TBigInt.Create(1)
			Else
				Return Self
			End If
		End If
		
		Local newSign:Int
		If sign < 0 And exponent & 1 = 1 Then
			newSign = -1
		Else
			newSign = 1
		End If
		
		Local baseSqr:Int[] = magnitude
		Local res:Int[] = [1]

		While exponent <> 0
			If exponent & 1 Then
				res = multiplyArrays(res, baseSqr)
				res = stripLeadingZeros(res)
			End If
			exponent = exponent Shr 1
			If exponent <> 0 Then
				baseSqr = squareToLength(baseSqr)
				baseSqr = stripLeadingZeros(baseSqr)
			End If
		Wend

		Return TBigInt.CreateA(res, newSign)
	End Method
	
	Method multiplyArrays:Int[](x:Int[], y:Int[])		
		Local xLength:Int = x.length
		Local yLength:Int = y.length
		
		Local xStart:Int = xLength - 1
		Local yStart:Int = yLength - 1
		
		Local result:Int[] = New Int[xLength + yLength]

		Local carry:Long
		Local yPos:Int = ystart
		Local rPos:Int = yStart + 1 + xStart
		
		While yPos >= 0
			Local product:Long = (y[yPos] & mask) * (x[xStart] & mask) + carry
			result[rPos] = Int(product)
			carry = product Shr 32
			
			rPos:- 1
			yPos:- 1
		Wend
		
		result[xStart] = Int(carry)
		
		Local i:Int = xStart - 1
		While i >= 0
		
			carry = 0

			yPos = ystart
			rPos = yStart + 1 + i
			
			While yPos >= 0
			
				Local product:Long = (y[yPos] & mask) * (x[i] & mask) + (result[rPos] & mask) + carry
				result[rPos] = Int(product)
				carry = product Shr 32
			
				yPos:- 1
				rPos:- 1
			Wend

			result[i] = Int(carry)
			i:- 1
		Wend

		Return result
	End Method
	
	Method stripLeadingZeros:Int[](value:Int[])
	
		Local offset:Int
		
		While offset < value.length
			If value[offset] <> 0 Then
				Exit
			End If
			offset:+ 1
		Wend
		
		If offset > 0 Then
			Return value[offset..]
		End If
		
		Return value
	End Method
	
	Method squareToLength:Int[](value:Int[])
		
		Local length:Int = value.length
		Local newLength:Int = length Shl 1
		Local result:Int[] = New Int[newLength]
		
		Local count:Int
		Local i:Int
		Local lastProductLowWord:Int = 0
		
		While count < length
			Local piece:Long = value[count] & mask
			Local product:Long = piece * piece
			
			result[i] = (lastProductLowWord Shl 31) | (product Shr 33)
			i:+ 1
			result[i] = product Shr 1
			i:+ 1
			
			lastProductLowWord = product
			count:+ 1
		Wend

		Local offset:Int = 1
		i = length
		
		While i > 0
			Local t:Int = value[i - 1]
			
			
			' muladd
			Local kl:Long = t & mask
			Local carry:Long
			Local ln:Int = i -1
			Local off:Int = result.length - offset - 1
			
			Local j:Int = ln - 1
			
			While j >= 0
				Local product:Long = Long((value[j] & mask)) * kl + Long(result[off] & mask) + carry
				result[off] = Int(product)
				off:- 1
				carry = product Shr 32
			
				j:- 1
			Wend
			
			t = carry
			
			' addone
			ln = i
			off = result.length - 1 - ln - (offset - 1)
			carry = Long(result[off] & mask) + Long(t & mask)
			
			result[off] = Int(carry)
			
			If carry Shr 32 <> 0 Then
				ln:-1
				
				While ln >= 0
					off:- 1
					
					If off < 0 Then
						Exit
					Else
						result[off]:+ 1
						If result[off] <> 0 Then
							Exit
						End If
					End If
				
					ln:- 1
				Wend
				
			End If
			
			i:- 1
			offset:+ 2
		Wend
		

        ' Shift up and set low bit
		Local n2:Int = 31
		i = 0
		Local c:Int = result[i]
		Local m:Int = result.length - 1
		
		While i < m
			Local b:Int = c
			c = result[i + 1]
			result[i] = (b Shl 1) | (c Shr 31)
			i:+1
		Wend
		result[result.length -1] :Shl 1
		
		result[newLength - 1]:| value[length - 1] & 1

		Return result
		
	End Method
	
	Method absInt:TBigInt()
		If sign >= 0 Then
			Return Self
		Else
			Return TBigInt.CreateA(magnitude, -sign)
		End If
	End Method
	
	Method ToString:String()
		If sign = 0 Then
			Return "0"
		End If
		
		Local maxGroups:Int = (4 * magnitude.length + 6) / 7
		Local digitGroup:String[] = New String[maxGroups]

		Local temp:TBigInt = absInt()
		
		While temp.sign <> 0
			Local d:TBigInt = TBigInt.Create($de0b6b3a7640000:Long)
		
			
		
'			temp = q2
		Wend
		
	End Method
End Type
