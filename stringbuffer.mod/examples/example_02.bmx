'
' Remove characters from a string buffer
'
SuperStrict

Framework bah.stringbuffer
Import brl.standardio

Local sb:TStringBuffer = New TStringBuffer

'                    11
'          012345678901
sb.Append("Hello World!")

' remove characters 4, 5, and 6
sb.Remove(4, 7)

Print sb.ToString()


