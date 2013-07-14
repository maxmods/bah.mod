' Copyright (c) 2013 Bruce A Henderson
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
bbdoc: Serial port interface.
End Rem
Module BaH.Serial

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: Serial Library - 2012 William Woodall, John Harrison"
ModuleInfo "Copyright: BlitzMax wrapper - 2013 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release"

ModuleInfo "CC_OPTS: -fexceptions"


Import "common.bmx"

' Changes
'   win.h
'  Added malloc.h include for alloca
'


Rem
bbdoc: A serial port interface.
End Rem
Type TSerial

	Field serialPtr:Byte Ptr

	Rem
	bbdoc: 
	End Rem
	Method Create:TSerial(port:String = "", baudrate:Int = 9600, timeout:TTimeout = Null, bytesize:Int = EIGHTBITS, ..
			parity:Int = PARITY_NONE, stopbits:Int = STOPBITS_ONE, flowcontrol:Int = FLOWCONTROL_NONE)
		If timeout Then
			serialPtr = bmx_serial_create(port, baudrate, timeout.timeoutPtr, bytesize, parity, stopbits, flowcontrol)
		Else
			serialPtr = bmx_serial_create(port, baudrate, Null, bytesize, parity, stopbits, flowcontrol)
		End If
		Return Self
	End Method

	Rem
	bbdoc: Opens the serial port as long as the port is set and the port isn't already open.
	about: If the port is provided to the constructor then an explicit call to open is not needed.
	End Rem
	Method open()
		bmx_serial_open(serialPtr)
	End Method
	
	Rem
	bbdoc: Closes the serial port.
	End Rem
	Method close()
		bmx_serial_close(serialPtr)
	End Method
	
	Rem
	bbdoc: Gets the open status of the serial port.
	returns: True if the port is open, False otherwise.
	End Rem
	Method isOpen:Int()
		Return bmx_serial_isopen(serialPtr)
	End Method
	
	Rem
	bbdoc: Returns the number of characters in the buffer.
	End Rem
	Method available:Int()
		Return bmx_serial_available(serialPtr)
	End Method
	
	Rem
	bbdoc: Read a given amount of bytes from the serial port into a given buffer.
	returns: A value representing the number of bytes read as a result of the call to read.
	about: The read method will return in one of three cases:
	<ul>
	<li>The number of requested bytes was read.
		<ul>
			<li>In this case the number of bytes requested will match the value returned by read.</li>
		</ul>
	</li>
	<li>A timeout occurred, in this case the number of bytes read will not match the amount requested, but no exception
	will be thrown. One of two possible timeouts occurred:
		<ul>
			<li>The inter byte timeout expired, this means that number of milliseconds elapsed between receiving bytes
			from the serial port exceeded the inter byte timeout.</li>
			<li>The total timeout expired, which is calculated by multiplying the read timeout multiplier by the number
			of requested bytes and then added to the read timeout constant. If that total number of milliseconds elapses
			after the initial call to read a timeout will occur.</li>
		</ul>
	</li>
	<li>An exception occurred, in this case an actual exception will be thrown
	</li>
	</ul>
	End Rem
	Method read:Int(buffer:Byte Ptr, size:Int)
		Return bmx_serial_read(serialPtr, buffer, size)
	End Method
	
	Rem
	bbdoc: Reads in a line or until a given delimiter has been processed.
	returns: A string containing the line.
	about: Reads from the serial port until a single line has been read.
	End Rem
	Method ReadLine:String(size:Int = 65536, eol:String = "~n")
		Return bmx_serial_readline(serialPtr, size, eol)
	End Method
	
	Rem
	bbdoc: Write a given amount of bytes to the serial port.
	returns: A value representing the number of bytes actually written to the serial port.
	End Rem
	Method write:Int(data:Byte Ptr, size:Int)
		Return bmx_serial_write(serialPtr, data, size)
	End Method
	
	Rem
	bbdoc: Write a string to the serial port.
	returns: A value representing the number of bytes actually written to the serial port.
	End Rem
	Method WriteString:Int(data:String)
		Return bmx_serial_writestring(serialPtr, data)
	End Method
	
	Rem
	bbdoc: Sets the serial port identifier.
	about: A value containing the address of the serial port, which would be something like 'COM1' on 	Windows and '/dev/ttyS0' on Linux.
	End Rem
	Method setPort(port:String)
		bmx_serial_setport(serialPtr, port)
	End Method
	
	Rem
	bbdoc: Gets the serial port identifier.
	End Rem
	Method getPort:String()
		Return bmx_serial_getport(serialPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setTimeout(timeout:TTimeout)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setTimeoutValues(interByteTimeout:Int, readTimeoutConstant:Int, readTimeoutMultiplier:Int, writeTimeoutConstant:Int, writeTimeoutMultiplier:Int)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getTimeout:TTimeout()
	End Method
	
	Rem
	bbdoc: Sets the baudrate for the serial port.
	about: Possible baudrates depends on the system but some safe baudrates include: 110, 300, 600, 1200, 2400, 4800, 9600,
	14400, 19200, 28800, 38400, 56000, 57600, 115200.
	Some other baudrates that are supported by some comports: 128000, 153600, 230400, 256000, 460800, 921600
	End Rem
	Method setBaudrate(baudrate:Int)
		bmx_serial_setbaudrate(serialPtr, baudrate)
	End Method
	
	Rem
	bbdoc: Gets the baudrate for the serial port.
	returns: An integer that sets the baud rate for the serial port.
	End Rem
	Method getBaudrate:Int()
		Return bmx_serial_getbaudrate(serialPtr)
	End Method
	
	Rem
	bbdoc: Sets the bytesize for the serial port.
	about: A value for the size of each byte in the serial transmission of data, default is EIGHTBITS, possible values are FIVEBITS, SIXBITS, SEVENBITS or EIGHTBITS.
	End Rem
	Method setBytesize(bytesize:Int)
		bmx_serial_setbytesize(serialPtr, bytesize)
	End Method
	
	Rem
	bbdoc: Gets the bytesize for the serial port.
	returns: One of FIVEBITS, SIXBITS, SEVENBITS or EIGHTBITS.
	End Rem
	Method getBytesize:Int()
		Return bmx_serial_getbytesize(serialPtr)
	End Method
	
	Rem
	bbdoc: Sets the parity for the serial port.
	about: A value for the method of parity, default is PARITY_NONE, possible values are PARITY_NONE, PARITY_ODD or PARITY_EVEN.
	End Rem
	Method setParity(parity:Int)
		bmx_serial_setparity(serialPtr, parity)
	End Method
	
	Rem
	bbdoc: Gets the parity for the serial port.
	returns: One of PARITY_NONE, PARITY_ODD or PARITY_EVEN.
	End Rem
	Method getParity:Int()
		Return bmx_serial_getparity(serialPtr)
	End Method
	
	Rem
	bbdoc: Sets the stop bits for the serial port.
	about: A value for the number of stop bits used, default is STOPBITS_ONE, possible values are STOPBITS_ONE, STOPBITS_ONE_POINT_FIVE or STOPBITS_TWO.
	End Rem
	Method setStopbits(stopbits:Int)
		bmx_serial_setstopbits(serialPtr, stopbits)
	End Method
	
	Rem
	bbdoc: Gets the stopbits for the serial port.
	returns: One of STOPBITS_ONE, STOPBITS_ONE_POINT_FIVE or STOPBITS_TWO.
	End Rem
	Method getStopbits:Int()
		Return bmx_serial_getstopbits(serialPtr)
	End Method
	
	Rem
	bbdoc: Sets the flow control for the serial port.
	about: A value for the type of flow control used, default is FLOWCONTROL_NONE, possible values are FLOWCONTROL_NONE, FLOWCONTROL_SOFTWARE or FLOWCONTROL_HARDWARE.
	End Rem
	Method setFlowcontrol(flowcontrol:Int)
		bmx_serial_setflowcontrol(serialPtr, flowcontrol)
	End Method
	
	Rem
	bbdoc: Gets the flow control for the serial port.
	returns: One of FLOWCONTROL_NONE, FLOWCONTROL_SOFTWARE or FLOWCONTROL_HARDWARE.
	End Rem
	Method getFlowcontrol:Int()
		Return bmx_serial_getflowcontrol(serialPtr)
	End Method
	
	Rem
	bbdoc: Flush the input and output buffers.
	End Rem
	Method flush()
		bmx_serial_flush(serialPtr)
	End Method
	
	Rem
	bbdoc: Flush only the input buffer.
	End Rem
	Method flushInput()
		bmx_serial_flushinput(serialPtr)
	End Method
	
	Rem
	bbdoc: Flush only the output buffer.
	End Rem
	Method flushOutput()
		bmx_serial_flushoutput(serialPtr)
	End Method
	
	Rem
	bbdoc: Sends the RS-232 break signal.
	about: See tcsendbreak(3).
	End Rem
	Method sendBreak(duration:Int)
		bmx_serial_sendbreak(serialPtr, duration)
	End Method
	
	Rem
	bbdoc: Sets the break condition to a given level.
	about: Defaults to true.
	End Rem
	Method setBreak(level:Int = True)
		bmx_serial_setbreak(serialPtr, level)
	End Method

	Rem
	bbdoc: Sets the RTS handshaking line to the given level.
	about: Defaults to true.
	End Rem
	Method setRTS(level:Int = True)
		bmx_serial_setrts(serialPtr, level)
	End Method

	Rem
	bbdoc: Sets the DTR handshaking line to the given level.
	about: Defaults to true.
	End Rem
	Method setDTR(level:Int = True)
		bmx_serial_setdtr(serialPtr, level)
	End Method
	
	Rem
	bbdoc: Blocks until CTS, DSR, RI, CD changes or something interrupts it.
	returns: True if one of the lines changed, False if something else occurred.
	about: Can throw an exception if an error occurs while waiting.
	You can check the status of CTS, DSR, RI, and CD once this returns.
	Uses TIOCMIWAIT via ioctl if available (mostly only on Linux) with a resolution of less than +-1ms and as good as +-0.2ms.
	Otherwise a polling method is used which can give +-2ms.
	End Rem
	Method waitForChange()
		bmx_serial_waitforchange(serialPtr)
	End Method
	
	Rem
	bbdoc: Returns the current status of the CTS line.
	End Rem
	Method getCTS:Int()
		Return bmx_serial_getcts(serialPtr)
	End Method
	
	Rem
	bbdoc: Returns the current status of the DSR line.
	End Rem
	Method getDSR:Int()
		Return bmx_serial_getdsr(serialPtr)
	End Method
	
	Rem
	bbdoc: Returns the current status of the RI line.
	End Rem
	Method getRI:Int()
		Return bmx_serial_getri(serialPtr)
	End Method
	
	Rem
	bbdoc: Returns the current status of the CD line.
	End Rem
	Method getCD:Int()
		Return bmx_serial_getcd(serialPtr)
	End Method

End Type

Rem
bbdoc: Type for setting the timeout of the serial port, times are in milliseconds.
abotu: In order to disable the interbyte timeout, set it to TTimeout::maxTime().
End Rem
Type TTimeout

	Field timeoutPtr:Byte Ptr
	
	Rem
	bbdoc: Returns the maximum timeout value;
	End Rem
	Function maxTime:Int()
		Return bmx_serial_timeout_max()
	End Function
	
	Function _create:TTimeout(timeoutPtr:Byte Ptr)
		If timeoutPtr Then
			Local this:TTimeout = New TTimeout
			this.timeoutPtr = timeoutPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TTimeout(interByteTimeout:Int = 0, readTimeoutConstant:Int = 0, readTimeoutMultiplier:Int = 0, writeTimeoutConstant:Int = 0, ..
			writeTimeoutMultiplier:Int = 0)
		timeoutPtr = bmx_serial_timeout_create(interByteTimeout, readTimeoutConstant, readTimeoutMultiplier, writeTimeoutConstant, writeTimeoutMultiplier)
		Return Self
	End Method
	
	
	Method Delete()
		If timeoutPtr Then
			bmx_serial_timeout_free(timeoutPtr)
			timeoutPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TSerialException Extends TRuntimeException
	Field what:String
	
	Function _create:TSerialException(what:String)
		Return New TSerialException.CreateException(what)
	End Function
	
	Method CreateException:TSerialException(what:String)
		Self.what = what
		Return Self
	End Method
	
	Method ToString:String()
		Return what
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TIOException Extends TSerialException

	Function _create:TSerialException(what:String)
		Return New TIOException.CreateException(what)
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TPortNotOpenedException Extends TSerialException

	Function _create:TSerialException(what:String)
		Return New TPortNotOpenedException.CreateException(what)
	End Function

End Type
