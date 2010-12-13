' Simple program to test MIDI sysex sending and receiving.

SuperStrict

Framework BaH.RtMidi
Import BRL.StandardIO


Const NUM_BYTES:Int = 20


Local midiIn:TRtMidiIn = New TRtMidiIn.Create()
Local midiOut:TRtMidiOut = New TRtMidiOut.Create()


' Don't ignore sysex, timing, or active sensing messages.
midiIn.ignoreTypes(False, True, True)

' open ports...

If midiOut.getPortCount() = 0 Then
	Print "No Output Ports"
	End
End If

For Local i:Int = 0 Until midiOut.getPortCount()
	Print "  Output port #" + i + " : " + midiOut.getPortName(i)
Next

Print "Opening " + midiOut.getPortName()

midiOut.openPort(0)


If midiIn.getPortCount() = 0 Then
	Print "No Input Ports"
	End
End If

For Local i:Int = 0 Until midiIn.getPortCount()
	Print "  Input port #" + i + " : " + midiIn.getPortName(i)
Next

Print "Opening " + midiIn.getPortName()

midiIn.openPort(0)

' Create a long sysex messages of numbered bytes and send it out.

Local message:Byte[] = New Byte[NUM_BYTES]

message[0] = 240
For Local i:Int = 0 Until NUM_BYTES - 2
	message[i+1] = i Mod 128
Next
message[NUM_BYTES-1] = 247

midiOut.putMessage(message, message.length)

Delay(50) ' pause a little


' Look for one message (hopefully the previously sent sysex if the
' ports were connected together) and print out the values.
Local stamp:Double
message = midiIn.getMessage(stamp)
Local nBytes:Int = message.length

For Local i:Int = 0 Until nBytes
	Print "Byte " + i + " = " + message[i]
Next

midiOut.free()
midiIn.free()

