'
' Return a substring from a string buffer
'
SuperStrict

Framework bah.stringbuffer
Import brl.standardio

Local sb:TStringBuffer = New TStringBuffer

sb.Append("Hello World!")

Print sb.Substring(0, 5)
Print sb.Substring(6)
