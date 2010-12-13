SuperStrict

Framework BaH.RtMidi
Import BRL.StandardIO
Import BRL.GLMax2d
Import BRL.LinkedList

Const MAX_MESSAGES:Int = 30
Global messages:TList = New TList

Local midiIn:TRtMidiIn = New TRtMidiIn.Create()

' Check available ports.

If midiIn.getPortCount() = 0 Then
	Print "No Input Ports"
	End
End If

For Local i:Int = 0 Until midiIn.getPortCount()
	Print "  Input port #" + i + " : " + midiIn.getPortName(i)
Next

Print "Opening " + midiIn.getPortName()

midiIn.openPort(0)

' Don't ignore sysex, timing, or active sensing messages.
midiIn.ignoreTypes(False, False, False)


' Periodically check input queue.
Print "Reading MIDI from port ... quit with ESCAPE."

Graphics 800, 600, 0

Local stamp:Double

While Not KeyDown(KEY_ESCAPE)

	Cls
	
	DrawText "Reading MIDI from port ... quit with ESCAPE.", 60, 10


	Local message:Byte[] = midiIn.getMessage(stamp)
	Local nBytes:Int = message.length
	
	Local s:String
	For Local i:Int = 0 Until nBytes
		s :+ "Byte " + i + " = " + message[i] + ", "
	Next
	
	If nBytes > 0 Then
		s :+ "stamp = " + stamp
	End If
	
	If s Then
		Print s
		AddMessage(s)
	End If
	
	ShowMessages(10, 30)
	
	Delay (10)

	Flip
	
Wend

midiIn.Free()

End

Function ShowMessages(x:Int, y:Int)
	For Local message:String = EachIn messages
		DrawText message, x, y
		y:+ 15
	Next
End Function

Function AddMessage(message:String)
	Local count:Int = messages.Count()
	If count >= MAX_MESSAGES Then
		messages.RemoveFirst()
	End If
	
	messages.AddLast(message)
End Function


