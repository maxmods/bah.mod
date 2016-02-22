'
' Remove a character from a string buffer
'
SuperStrict

Framework bah.stringbuffer
Import brl.standardio

Local sb:TStringBuffer = New TStringBuffer

'          01234567
sb.Append("Hello funny old World!")

Print sb.Length()
Print sb.RemoveCharAt(7).ToString()
Print sb.Length()

' remove last char
Print sb.RemoveCharAt(sb.Length() - 1).ToString()
Print sb.Length()


