'
' Insert text into a string buffer
'
SuperStrict

Framework bah.stringbuffer
Import brl.standardio

Local sb:TStringBuffer = New TStringBuffer

sb.Append("Hello World!")

sb.Insert(6, "New ")

Print sb.ToString()

