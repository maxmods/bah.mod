SuperStrict

Framework brl.standardio
Import bah.libembroidery


Local format:TEmbFormatList = New TEmbFormatList.Create()

While format

	Print format.Extension() + " : " + format.Description()

	format = format.NextFormat()

Wend

