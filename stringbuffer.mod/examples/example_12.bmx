'
' Split a string buffer
'
SuperStrict

Framework bah.stringbuffer
Import brl.standardio

Local sb:TStringBuffer = New TStringBuffer

sb.Append("One Two Three Four Five Six Seven Eight Nine Ten")

Local split:TSplitBuffer = sb.Split(" ")

'Print split.Length()
For Local i:Int = 0 Until split.Length()
	Print split.Text(i)
Next


