'
' Join a string array to a string buffer
'
SuperStrict

Framework bah.stringbuffer
Import brl.standardio

Local sb:TStringBuffer = New TStringBuffer


Local strings:String[] = ["one", "two", "three", "four", "five"]

sb.Append(" ")

Local sb2:TStringBuffer = sb.Join(strings)


Print sb2.ToString()


