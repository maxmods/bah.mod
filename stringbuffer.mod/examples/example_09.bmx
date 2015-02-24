'
' Trim a string buffer
'
SuperStrict

Framework bah.stringbuffer
Import brl.standardio

Local sb:TStringBuffer = New TStringBuffer

sb.Append("  ~t~nHello World!~t~t~r ")

Print "'" + sb.Trim().ToString() + "'"





