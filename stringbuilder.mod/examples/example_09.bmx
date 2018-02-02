'
' Trim a string builder
'
SuperStrict

Framework bah.stringbuilder
Import brl.standardio

Local sb:TStringBuilder = New TStringBuilder

sb.Append("  ~t~nHello World!~t~t~r ")

Print "'" + sb.Trim().ToString() + "'"





