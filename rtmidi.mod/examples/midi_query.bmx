SuperStrict

Framework BaH.RtMidi
Import BRL.StandardIO

Local midiIn:TRtMidiIn = New TRtMidiIn.Create()
Local midiOut:TRtMidiOut = New TRtMidiOut.Create()

' Check inputs.
Local nPorts:Int = midiIn.getPortCount()

Print "~nThere are " + nPorts + " MIDI input sources available."

Local portName:String

For Local i:Int = 0 Until nPorts
	Try
		portName = midiIn.getPortName(i)
	Catch error:TRtError
		ShowError(error)
	End Try
	
	Print "  Input Port #" + (i+1) + ": " + portName
Next

' Check outputs.

nPorts = midiOut.getPortCount()

Print "~nThere are " + nPorts + " MIDI output sources available."

For Local i:Int = 0 Until nPorts
	Try
		portName = midiOut.getPortName(i)
	Catch error:TRtError
		ShowError(error)
	End Try
	
	Print "  Output Port #" + (i+1) + ": " + portName
Next


midiIn.Free()
midiOut.Free()


End

Function ShowError(error:TRtError)
	Print "Error : " + error.ToString()
	End
End Function
