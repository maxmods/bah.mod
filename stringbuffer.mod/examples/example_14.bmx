'
' Iterate a split string buffer
'
SuperStrict

Framework bah.stringbuffer
Import brl.standardio

Local sb:TStringBuffer = New TStringBuffer

sb.Append("One Two Three Four Five Six Seven Eight Nine Ten")

Local split:TSplitBuffer = sb.Split(" ")

For Local s:String = EachIn split
	Print s
Next




