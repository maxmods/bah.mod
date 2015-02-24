'
' Append some strings, with basic timing comparison.
'
SuperStrict

Framework bah.stringbuffer
Import brl.standardio

Const iterations:Int = 20000
Local ms:Int, nms:Int

Local sb:TStringBuffer = New TStringBuffer

Print ""

Local s:String
ms = MilliSecs()
For Local i:Int = 0 Until iterations
	s :+ i + " "
Next
nms = MilliSecs()
Print "String concat : " + (nms - ms) + " ms"
Print "String length = " + s.Length

Print ""

ms = MilliSecs()
For Local i:Int = 0 Until iterations
	sb.Append(i).Append(" ")
Next
nms = MilliSecs()

Print "StringBuffer concat : " + (nms - ms) + " ms"

Print "StringBuffer String length = " + sb.Length()
Print "StringBuffer capacity = " + sb.Capacity()

