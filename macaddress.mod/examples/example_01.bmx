'
' Retrieves and displays the MAC address
'
SuperStrict

Framework BaH.MACAddress
Import BRL.StandardIO
Import BRL.retro ' for Hex()

' get MAC address as set of ints
Local address:Int[] = GetMACAddress()

' convert to a string representation
Local a:String
For Local i:Int = 0 Until address.length

	If i Then
		a :+ ":"
	End If
	
	a :+ Hex(address[i])[6..]

Next

Print a

' get MAC address as string
Print GetMACAddressString()




