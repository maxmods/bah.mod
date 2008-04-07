'
' Enumerate the available devices.
'
SuperStrict

Framework BaH.Bass
Import BRL.StandardIO

Local i:Int = 0
Local info:TBassDeviceInfo = TBass.GetDeviceInfo(i)
While info
	Print i + " : " + info.GetName()

	i:+ 1
	info = TBass.GetDeviceInfo(i)
Wend



