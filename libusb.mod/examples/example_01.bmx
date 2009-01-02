SuperStrict

Framework BaH.libusb
Import BRL.StandardIO

Global verbose:Int

If AppArgs.length > 1 And AppArgs[1] = "-v" Then
	verbose = 1
End If

?win32
' Needs to be run once... at some point, to install the service.
USBInstallService()
?

USBInit()

USBRescan()

Local bus:USBBus = USBGetBusses()

While bus

	If bus.rootDevice() And Not verbose Then
		printDevice(bus.rootDevice(), 0)
	Else
		Local device:USBDevice = bus.devices()
		
		While device
			printDevice(device, 0)
			
			device = device.nextDevice()
		Wend
		
	End If

	' get the next bus
	bus = bus.nextBus()
	
Wend

End

Function printDevice(device:USBDevice, level:Int)

	' open the device
	Local handle:USBDeviceHandle = USBOpen(device)
	
	Local description:String

	' did we get a handle ?  (ie. was it opened?)
	If handle Then

		If device.descriptor().manufacturer() Then
			description = device.descriptor().manufacturer() + " - "
		Else
			description = "0x"+ ToHex(device.descriptor().vendorId(), 4) + " - "
		End If

		If device.descriptor().product() Then
			description:+ device.descriptor().product()
		Else
			description = "0x"+ ToHex(device.descriptor().productId(), 4)
		End If
		
	Else
		description = "0x"+ ToHex(device.descriptor().vendorId(), 4) + " - " + "0x"+ ToHex(device.descriptor().productId(), 4)
	End If

	Print indent(level) + " " + device.deviceNumber() + ": " + description
	
	If (handle And verbose) Then
		If device.descriptor().serialNumber() Then
			Print indent(level) + " - Serial Number: " + device.descriptor().serialNumber()
		End If
	End If
	
	' remember to close the device if we opened it !!
	If handle Then
		USBClose(handle)
	End If

	If verbose Then
		If Not device.hasConfig() Then
			Print "  Couldn't retrieve descriptors"
			Return
		End If
	
		For Local i:Int = 0 Until device.descriptor().numConfigurations()
			printConfiguration(device.config(i))
		Next
	Else
	For Local i:Int = 0 Until device.numChildren()
			printDevice(device.children(i), level + 1)
		Next
	End If

End Function

Function indent:String(level:Int)
	Local txt:String
	For Local i:Int = 0 Until level * 2
		txt:+ "      "
	Next
	Return txt
End Function

Function printConfiguration(config:USBConfigDescriptor)
	Print "  totalLength:         " + config.totalLength()
	Print "  numInterfaces:       " + config.numInterfaces()
	Print "  configurationValue:  " + config.configurationValue()
	Print "  configuration:       " + config.configuration()
	Print "  attributes:          " + "0x"+ ToHex(config.attributes(), 2)
	Print "  maxPower:            " + config.maxPower()

	For Local i:Int = 0 Until config.numInterfaces()
		printInterface(config.interface(i))
	Next

End Function

Function printInterface(interface:USBInterface)
	For Local i:Int = 0 Until interface.numAltsetting()
		printAltsetting(interface.altsetting(i))
	Next
End Function

Function printAltsetting(interface:USBInterfaceDescriptor)
	Print "    interfaceNumber:   " + interface.interfaceNumber()
	Print "    alternateSetting:  " + interface.alternateSetting()
	Print "    numEndpoints:      " + interface.numEndpoints()
	Print "    interfaceClass:    " + interface.interfaceClass()
	Print "    interfaceSubClass: " + interface.interfaceSubClass()
	Print "    interfaceProtocol: " + interface.interfaceProtocol()
	Print "    interface:         " + interface.interface()
	
	For Local i:Int = 0 Until interface.numEndpoints()
		printEndpoint(interface.endpoint(i))
	Next
End Function

Function printEndpoint(endpoint:USBEndpointDescriptor)
	Print "      endpointAddress: " + "0x"+ ToHex(endpoint.endpointAddress(), 2)
	Print "      attributes:      " + "0x"+ ToHex(endpoint.attributes(), 2)
	Print "      maxPacketSize:   " + endpoint.maxPacketSize()
	Print "      interval:        " + endpoint.interval()
	Print "      refresh:         " + endpoint.refresh()
	Print "      synchAddress:    " + endpoint.synchAddress()
End Function

