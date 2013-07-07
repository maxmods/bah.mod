SuperStrict

Framework BaH.Serial
Import BRL.StandardIO

Local serial:TSerial

Try
	serial = New TSerial.Create("/dev/cu.usbserial-AH01NCPN")

Catch obj: TSerialException
	Print obj.ToString()
	End
End Try

Local data:Byte[1024]

While True

	Local count:Int = serial.available()
	
	
	If count Then
		
		Print "read = " + serial.read(data, count)
		
	Else
		Delay 100
	End If

Wend

