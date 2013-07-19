'
' List the available serial ports
'
'
SuperStrict

Framework BaH.Serial
Import BRL.StandardIO

Local ports:TList = TSerial.listPorts()

For Local port:TSerialPortInfo = EachIn ports

	Print "~nSerial Device"
	Print "~tPort            : " + port.portName
	Print "~tPhysical Name   : " + port.physicalName
	Print "~tProduct Name    : " + port.productName
	Print "~tEnumerator Name : " + port.enumeratorName
	Print "~tVendor Id       : " + port.vendorId
	Print "~tProduct Id      : " + port.productId
Next
