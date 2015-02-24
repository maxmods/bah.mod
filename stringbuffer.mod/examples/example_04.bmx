'
' Reverse a string buffer
'
SuperStrict

Framework bah.stringbuffer
Import brl.standardio

Local sb:TStringBuffer = New TStringBuffer

sb.Append("Hello World!")

sb.Reverse()

Print sb.ToString()

