' Copyright (c) 2010-2011 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
'
SuperStrict

Rem
bbdoc: RTMidi
End Rem
Module BaH.RTMidi

ModuleInfo "Version: 1.01"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: RtMidi - 2003-2011 Gary P. Scavone"
ModuleInfo "Copyright: Wrapper - 2010-2011 Bruce A Henderson"

ModuleInfo "History: 1.01"
ModuleInfo "History: Update to RTMidi 1.0.14"
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. (RTMidi 1.0.11)"


ModuleInfo "CC_OPTS: -fexceptions"
?macos
ModuleInfo "CC_OPTS: -D__MACOSX_CORE__"
?linux
ModuleInfo "CC_OPTS: -D__LINUX_ALSASEQ__"
?win32
ModuleInfo "CC_OPTS: -D__WINDOWS_MM__"
?

Import "common.bmx"


Type TRtMidi

	Field midiPtr:Byte Ptr

	Method openPort(portNumber:Int = 0, portName:String = "RtMidi")
	End Method
	
	Method openVirtualPort(portName:String = "RtMidi")
	End Method
	
	Method getPortCount:Int()
	End Method
	
	Method getPortName:String(portNumber:Int = 0)
	End Method
	
	Method closePort:Int()
	End Method

End Type

Rem
bbdoc: A realtime MIDI input type.
about: This type provides a common, platform-independent API for
    realtime MIDI input.  It allows access to a single MIDI input
    port.  Incoming MIDI messages are saved to a queue for
    retrieval using the getMessage() method.
	Create multiple instances of
    this class to connect to more than one MIDI device at the same
    time.  With the OS-X and Linux ALSA MIDI APIs, it is also possible
    to open a virtual input port to which other MIDI software clients
    can connect.
End Rem
Type TRtMidiIn Extends TRtMidi

	Method Create:TRtMidiIn(clientName:String = "RtMidi Input Client")
		midiPtr = bmx_rtmidiin_create(clientName)
		Return Self
	End Method

	Rem
	bbdoc: Open a MIDI input connection.
	about: An optional port number greater than 0 can be specified.
      Otherwise, the default or first port found is opened.
	End Rem
	Method openPort(portNumber:Int = 0, portName:String = "RtMidi Input")
		bmx_rtmidiin_openPort(midiPtr, portNumber, portName)
	End Method
	
	Rem
	bbdoc: Close an open MIDI connection (if one exists).
	End Rem
	Method closePort()
		bmx_rtmidiin_closePort(midiPtr)
	End Method
	
	Rem
	bbdoc: Create a virtual input port, with optional name, to allow software connections (OS X and ALSA only).
	about: This method creates a virtual MIDI input port to which other
      software applications can connect.  This type of functionality
      is currently only supported by the Macintosh OS-X and Linux ALSA
      APIs (the method does nothing for the other APIs).
	End Rem
	Method openVirtualPort(portName:String = "RtMidi Input")
		bmx_rtmidiin_openVirtualPort(midiPtr, portName)
	End Method
	
	Rem
	bbdoc: Return the number of available MIDI input ports.
	End Rem
	Method getPortCount:Int()
		Return bmx_rtmidiin_getPortCount(midiPtr)
	End Method
	
	Rem
	bbdoc: Return a string identifier for the specified MIDI input port number.
	about: An exception is thrown if an invalid port specifier is provided.
	End Rem
	Method getPortName:String(portNumber:Int = 0)
		Return bmx_rtmidiin_getPortName(midiPtr, portNumber)
	End Method
	
	Rem
	bbdoc: Set the maximum number of MIDI messages to be saved in the queue.
	about: If the queue size limit is reached, incoming messages will be ignored.  The default limit is 1024.
	End Rem
	Method setQueueSizeLimit(queueSize:Int)
		bmx_rtmidiin_setQueueSizeLimit(midiPtr, queueSize)
	End Method
	
	Rem
	bbdoc: Specify whether certain MIDI message types should be queued or ignored during input.
	about: By default, MIDI timing and active sensing messages are ignored
      during message input because of their relative high data rates.
      MIDI sysex messages are ignored by default as well.  Variable
      values of "true" imply that the respective message type will be
      ignored.
	End Rem
	Method ignoreTypes(midiSysex:Int = True, midiTime:Int = True, midiSense:Int = True)
		bmx_rtmidiin_ignoreTypes(midiPtr, midiSysex, midiTime, midiSense)
	End Method
	
	Function _newByteArray:Byte[](size:Int)
		Return New Byte[size]
	End Function
	
	Rem
	bbdoc: Returns a Byte array of the data bytes for the next available MIDI message in the input queue and populates the event delta-timestamp in seconds.
	about: This method returns immediately whether a new message is
      available or not.  A valid message is indicated by a non-zero
      array size.  An exception is thrown if an error occurs during
      message retrieval or an input connection was not previously
      established.
	End Rem
	Method getMessage:Byte[](timestamp:Double Var)
		Return bmx_rtmidiin_getMessage(midiPtr, Varptr timestamp)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Free()
		If midiPtr Then
			bmx_rtmidiin_free(midiPtr)
			midiPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type

Rem
bbdoc: A realtime MIDI output type.
about: This type provides a common, platform-independent API for MIDI
    output.  It allows one to probe available MIDI output ports, to
    connect to one such port, and to send MIDI bytes immediately over
    the connection.  Create multiple instances of this type to
    connect to more than one MIDI device at the same time.
End Rem
Type TRtMidiOut Extends TRtMidi

	Method Create:TRtMidiOut(clientName:String = "RtMidi Output Client")
		midiPtr = bmx_rtmidiout_create(clientName)
		Return Self
	End Method
	
	Rem
	bbdoc: Open a MIDI output connection.
	about: An optional port number greater than 0 can be specified.
      Otherwise, the default or first port found is opened.  An
      exception is thrown if an error occurs while attempting to make
      the port connection.
	End Rem
	Method openPort(portNumber:Int = 0, portName:String = "RtMidi Output")
		bmx_rtmidiout_openPort(midiPtr, portNumber, portName)
	End Method
	
	Rem
	bbdoc: Close an open MIDI connection (if one exists).
	End Rem
	Method closePort()
		bmx_rtmidiout_closePort(midiPtr)
	End Method
	
	Rem
	bbdoc: Create a virtual output port, with optional name, to allow software connections (OS X and ALSA only).
	about: This method creates a virtual MIDI output port to which other
      software applications can connect.  This type of functionality
      is currently only supported by the Macintosh OS-X and Linux ALSA
      APIs (the function does nothing with the other APIs).  An
      exception is thrown if an error occurs while attempting to create
      the virtual port.

	End Rem
	Method openVirtualPort(portName:String = "RtMidi Output")
		bmx_rtmidiout_openVirtualPort(midiPtr, portName)
	End Method
	
	Rem
	bbdoc: Return the number of available MIDI output ports.
	End Rem
	Method getPortCount:Int()
		Return bmx_rtmidiout_getPortCount(midiPtr)
	End Method
	
	Rem
	bbdoc: Return a string identifier for the specified MIDI port type and number.
	about: An exception is thrown if an invalid port specifier is provided.
	End Rem
	Method getPortName:String(portNumber:Int = 0)
		Return bmx_rtmidiout_getPortName(midiPtr, portNumber)
	End Method
	
	Rem
	bbdoc: Immediately send a single message out an open MIDI output port.
	about: An exception is thrown if an error occurs during output or an
      output connection was not previously established.
	End Rem
	Method putMessage(message:Byte Ptr, length:Int)
		bmx_rtmidiout_sendMessage(midiPtr, message, length)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Free()
		If midiPtr Then
			bmx_rtmidiout_free(midiPtr)
			midiPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TRtError Extends TRuntimeException
	Field message:String
	Field kind:Int

	Rem
	bbdoc:  A non-critical error.
	End Rem
	Const WARNING:Int = 0
	
	Rem
	bbdoc: A non-critical error which might be useful for debugging.
	End Rem
	Const DEBUG_WARNING:Int = 1
	
	Rem
	bbdoc: The default, unspecified error type.
	End Rem
	Const UNSPECIFIED:Int = 2
	
	Rem
	bbdoc: No devices found on system.
	End Rem
	Const NO_DEVICES_FOUND:Int = 3
	
	Rem
	bbdoc: An invalid device ID was specified.
	End Rem
	Const INVALID_DEVICE:Int = 4
	
	Rem
	bbdoc: An invalid stream ID was specified.
	End Rem
	Const INVALID_STREAM:Int = 5
	
	Rem
	bbdoc: An error occured during memory allocation.
	End Rem
	Const MEMORY_ERROR:Int = 6
	
	Rem
	bbdoc: An invalid parameter was specified to a function.
	End Rem
	Const INVALID_PARAMETER:Int = 7
	
	Rem
	bbdoc: A system driver error occured.
	End Rem
	Const DRIVER_ERROR:Int = 8
	
	Rem
	bbdoc: A system error occured.
	End Rem
	Const SYSTEM_ERROR:Int = 9
	
	Rem
	bbdoc:  A thread error occured.
	End Rem
	Const THREAD_ERROR:Int = 10

	
	Function _create:TRtError(message:String, kind:Int)
		Local e:TRtError = New TRtError
		e.message = message
		e.kind = kind
		Return e
	End Function
	
	Method ToString:String()
		Return message
	End Method

End Type

