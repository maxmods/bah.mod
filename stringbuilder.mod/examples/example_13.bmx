'
' Get an array from a split string builder
'
SuperStrict

Framework bah.stringbuilder
Import brl.standardio

Local sb:TStringBuilder = New TStringBuilder

sb.Append("One Two Three Four Five Six Seven Eight Nine Ten")

Local split:TSplitBuffer = sb.Split(" ")

Local strings:String[] = split.ToArray()

For Local i:Int = 0 Until strings.length
	Print strings[i]
Next



