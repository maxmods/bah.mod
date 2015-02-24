'
' Replace a substring in a string buffer
'
SuperStrict

Framework bah.stringbuffer
Import brl.standardio

Local sb:TStringBuffer = New TStringBuffer

sb.Append("Strings are used to store sequences of characters.")

sb.Replace("to", "XX")

Print sb.ToString()




