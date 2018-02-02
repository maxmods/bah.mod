'
' Lower and Upper a string builder
'
SuperStrict

Framework bah.stringbuilder
Import brl.standardio

Local sb:TStringBuilder = New TStringBuilder

sb.Append("Hello World!")

Print sb.ToString()

Print sb.ToLower().ToString()

Print sb.ToUpper().ToString()



