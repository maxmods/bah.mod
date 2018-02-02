'
' Replace a substring in a string builder
'
SuperStrict

Framework bah.stringbuilder
Import brl.standardio

Local sb:TStringBuilder = New TStringBuilder

sb.Append("Strings are used to store sequences of characters.")

sb.Replace("to", "XX")

Print sb.ToString()




