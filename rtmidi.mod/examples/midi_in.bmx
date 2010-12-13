SuperStrict

Framework BaH.RtMidi
Import BRL.StandardIO

Local midiIn:TRtMidiIn = New TRtMidiIn.Create()

If midiIn.getPortCount() = 0 Then
	Print "No Input Ports"
	End
End If

For Local i:Int = 0 Until midiIn.getPortCount()
	Print "  Input port #" + i + " : " + midiIn.getPortName(i)
Next

Print "Opening " + midiIn.getPortName()

midiIn.openPort(0)





midiIn.Free()


