'
' Remove characters from a string builder
'
SuperStrict

Framework bah.stringbuilder
Import brl.standardio

Local sb:TStringBuilder = New TStringBuilder

'                    11
'          012345678901
sb.Append("Hello World!")

' remove characters 4, 5, and 6
sb.Remove(4, 7)

Print sb.ToString()


