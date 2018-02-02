'
' The start and end of a string builder
'
SuperStrict

Framework bah.stringbuilder
Import brl.standardio

Local sb:TStringBuilder = New TStringBuilder

sb.Append("Hello World!")

Print ""

Print "Text = ~q" + sb.ToString() + "~q"

Print ""

Print "Starts with 'Hell' ? : " + sb.StartsWith("Hell")
Print "Starts with 'hell' ? : " + sb.StartsWith("hell")
Print "Starts with 'Bell' ? : " + sb.StartsWith("Bell")

Print "Ends with 'rld!' ? : " + sb.EndsWith("rld!")
Print "Ends with 'RLD!' ? : " + sb.EndsWith("RLD!")
Print "Ends with 'x' ? : " + sb.EndsWith("x")



