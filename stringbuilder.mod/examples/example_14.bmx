'
' Iterate a split string builder
'
SuperStrict

Framework bah.stringbuilder
Import brl.standardio

Local sb:TStringBuilder = New TStringBuilder

sb.Append("One Two Three Four Five Six Seven Eight Nine Ten")

Local split:TSplitBuffer = sb.Split(" ")

For Local s:String = EachIn split
	Print s
Next




