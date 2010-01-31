'
' Retrieves and displays the network interfaces information
'
SuperStrict

Framework BaH.Inet
Import BRL.StandardIO
Import BRL.retro ' for Hex()


Local interfaces:TInet[] = GetInetInterfaces()

For Local inet:TInet = EachIn interfaces
	Print ""
	Print "Interface   : " + inet.name
	Print "IP Address  : " + DottedIP(inet.address)
	Print "Netmask     : " + DottedIP(inet.netmask)
	Print "Broadcast   : " + DottedIP(inet.broadcast)
	Print "MAC Address : " + inet.MACAddressString
Next

Local inet:TInet = GetInetInterface("en0")

If inet Then
	Print inet.MACAddressString.ToUpper()
	
	' get MAC address as set of ints
	Local address:Int[] = inet.MACAddress
	
	' convert to a string representation
	Local a:String
	For Local i:Int = 0 Until address.length
	
		If i Then
			a :+ ":"
		End If
		
		a :+ Hex(address[i])[6..]
	
	Next
	
	Print a
End If




