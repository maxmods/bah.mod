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
	Print "            : " + GetFlags(inet.flags)
	Print "IP Address  : " + DottedIP(inet.address)
	Print "Netmask     : " + DottedIP(inet.netmask)
	Print "Broadcast   : " + DottedIP(inet.broadcast)
	Print "MAC Address : " + inet.MACAddressString
Next

Local inet:TInet = GetInetInterface("en0")

If inet Then
	Print "~n"
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

Function GetFlags:String(flags:Int)
	Local f:String
	
	If flags & IFF_UP Then
		Add f, "UP"
	End If

	If flags & IFF_BROADCAST Then
		Add f, "BROADCAST"
	End If

	If flags & IFF_LOOPBACK Then
		Add f, "LOOPBACK"
	End If

	If flags & IFF_POINTOPOINT Then
		Add f, "POINTOPOINT"
	End If

	If flags & IFF_RUNNING Then
		Add f, "RUNNING"
	End If

	If flags & IFF_SIMPLEX Then
		Add f, "SIMPLEX"
	End If

	If flags & IFF_MULTICAST Then
		Add f, "MULTICAST"
	End If

	Return f
End Function

Function Add:String(f:String Var, s:String)
	If f Then
		f:+ ", "
	End If
	
	f:+ s
End Function


