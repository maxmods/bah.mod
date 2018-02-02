'
' Remove a character from a string builder
'
SuperStrict

Framework bah.stringbuilder
Import brl.standardio

Local sb:TStringBuilder = New TStringBuilder

'          01234567
sb.Append("Hello funny old World!")

Print sb.Length()
Print sb.RemoveCharAt(7).ToString()
Print sb.Length()

' remove last char
Print sb.RemoveCharAt(sb.Length() - 1).ToString()
Print sb.Length()


