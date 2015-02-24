'
' The start and end of a string buffer
'
SuperStrict

Framework bah.stringbuffer
Import brl.standardio

Local sb:TStringBuffer = New TStringBuffer

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



