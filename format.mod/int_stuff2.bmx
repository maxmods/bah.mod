
num:Double = 1234.68832343:Double

Local negative:Int = Long Ptr(Varptr num)[0] Shr 63
Local exponent:Int = (Long Ptr(Varptr num)[0] Shr 52) & $7ff
Local mantissa:Long
If Not exponent
	mantissa = (Long Ptr(Varptr num)[0] & 4503599627370495:Long) Shl 1:Long
Else
	mantissa = (Long Ptr(Varptr num)[0] & 4503599627370495:Long) | 4503599627370496:Long
End If
exponent:- 1075

' Normalize
While mantissa & 1:Long = 0
    mantissa = mantissa Shr 1:Long
    exponent:+1
Wend

Local intVal:Long = mantissa
If exponent < 0 Then
	Print (5 ^ -exponent)
End If

Print negative
Print exponent
Print mantissa

Local l:Long = 59994555565:Long
Print (l Shr 32)
l = $de0b6b3a7640000:Long
Print l

'1234.688323430000082225888036191463470458984375