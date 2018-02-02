'
' Return a substring from a string builder
'
SuperStrict

Framework bah.stringbuilder
Import brl.standardio

Local sb:TStringBuilder = New TStringBuilder

sb.Append("Hello World!")

Print sb.Substring(0, 5)
Print sb.Substring(6)
