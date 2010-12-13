' Simple program to test MIDI output.

SuperStrict

Framework BaH.RtMidi
Import BRL.StandardIO

Local midiOut:TRtMidiOut = New TRtMidiOut.Create()

If midiOut.getPortCount() = 0 Then
	Print "No Output Ports"
	End
End If

For Local i:Int = 0 Until midiOut.getPortCount()
	Print "  Output port #" + i + " : " + midiOut.getPortName(i)
Next

Print "Opening " + midiOut.getPortName()

midiOut.openPort(0)


Local message:Byte[] = New Byte[2]

' Send out a series of MIDI messages.

' Program change: 192, 5
message[0] = 192
message[1] = 5
midiout.putMessage(message, 2)

message = New Byte[3]
' Control Change: 176, 7, 100 (volume)
message[0] = 176
message[1] = 7
message[2] = 100
midiout.putMessage(message, 3)

' Note On: 144, 64, 90
message[0] = 144
message[1] = 64
message[2] = 90
midiout.putMessage(message, 3)

Delay(500)

' Note Off: 128, 64, 40
message[0] = 128
message[1] = 64
message[2] = 40
midiout.putMessage(message, 3)

message = New Byte[6]
' Sysex: 240, 67, 4, 3, 2, 247
message[0] = 240
message[1] = 67
message[2] = 4
message[3] = 3
message[4] = 2
message[5] = 247
midiout.putMessage(message, 6)

midiout.free()
