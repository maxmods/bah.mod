'
' Lower and Upper a string buffer
'
SuperStrict

Framework bah.stringbuffer
Import brl.standardio

Local sb:TStringBuffer = New TStringBuffer

sb.Append("Hello World!")

Print sb.ToString()

Print sb.ToLower().ToString()

Print sb.ToUpper().ToString()



