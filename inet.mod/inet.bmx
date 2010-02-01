' Copyright (c) 2010 Bruce A Henderson
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
bbdoc: Network Interfaces.
End Rem
Module BaH.Inet

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2010 Bruce A Henderson"

ModuleInfo "Version: 1.00"
ModuleInfo "History: Initial Release"



Import BRL.Blitz
Import "*.h"
Import "glue.cpp"

?win32
Import "-lws2_32"
Import "-liphlpapi"
?macos
'Import "-framework IOKit"
?

Extern
	Function bmx_inet_listinterfaces:TInet[]()
End Extern

Rem
bbdoc: A network interface
End Rem
Type TInet

	Rem
	bbdoc: The name of the interface.
	End Rem
	Field name:String
	Rem
	bbdoc: The interface IP Address.
	End Rem
	Field address:Int
	Rem
	bbdoc: The MAC address, as an array.
	End Rem
	Field MACAddress:Int[]
	Rem
	bbdoc: The MAC address as a String, in the form 00:00:00:00:00:00.
	End Rem
	Field MACAddressString:String
	Rem
	bbdoc: Netmask.
	End Rem
	Field netmask:Int
	Rem
	bbdoc: Broadcast address.
	End Rem
	Field broadcast:Int
	Rem
	bbdoc: Interface flags.
	End Rem
	Field flags:Int
	
	Function _create:TInet(name:String)
		Local this:TInet = New TInet
		this.name = name
		Return this
	End Function
	
	Function _setMACAddress(inet:TInet, address:Int[], addressString:String)
		inet.MACAddress = address
		inet.MACAddressString = addressString
	End Function
	
	Function _setAddress(inet:TInet, address:Int)
		inet.address = _swap(address)
	End Function

	Function _setNetmask(inet:TInet, netmask:Int)
		inet.netmask = _swap(netmask)
		inet.broadcast = inet.address | ~inet.netmask
	End Function
	
	Function _setBroadcast(inet:TInet, broadcast:Int)
		inet.broadcast = broadcast
	End Function

	Function _newArray:TInet[](size:Int)
		Return New TInet[size]
	End Function
	
	Function _newEntry(arr:TInet[], index:Int, entry:TInet)
		arr[index] = entry
	End Function
	
	Function _swap:Int(value:Int)
		Return value Shl 24 | ((value Shl 8) & $FF0000) | ((value Shr 8) & $FF00) | value Shr 24
	End Function
	
	Function _setFlags(inet:TInet, flags:Int)
		inet.flags = flags
	End Function

End Type

Rem
bbdoc: Retrieves the network interfaces for this computer.
End Rem
Function GetInetInterfaces:TInet[]()
	Return bmx_inet_listinterfaces()
End Function

Rem
bbdoc: Retrieves the network interface for the given @name.
End Rem
Function GetInetInterface:TInet(name:String)
	For Local inet:TInet = EachIn GetInetInterfaces()
		If inet.name.toupper() = name.toupper() Then
			Return inet
		End If
	Next
End Function

Rem
bbdoc: interface is up
end rem
Const IFF_UP:Int = $1
Rem
bbdoc: broadcast address valid
end rem
Const IFF_BROADCAST:Int = $2
Rem
bbdoc: turn on debugging
end rem
Const IFF_DEBUG:Int = $4
Rem
bbdoc: is a loopback net
end rem
Const IFF_LOOPBACK:Int = $8
Rem
bbdoc: interface is point-to-point link
end rem
Const IFF_POINTOPOINT:Int = $10
Rem
bbdoc: resources allocated
end rem
Const IFF_RUNNING:Int = $40
Rem
bbdoc: no address resolution protocol
end rem
Const IFF_NOARP:Int = $80
Rem
bbdoc: receive all packets
end rem
Const IFF_PROMISC:Int = $100
Rem
bbdoc: receive all multicast packets
end rem
Const IFF_ALLMULTI:Int = $200
Rem
bbdoc: transmission in progress
end rem
Const IFF_OACTIVE:Int = $400
Rem
bbdoc: can't hear own transmissions
end rem
Const IFF_SIMPLEX:Int = $800
Rem
bbdoc: per link layer defined bit
end rem
Const IFF_LINK0:Int = $1000
Rem
bbdoc: per link layer defined bit
end rem
Const IFF_LINK1:Int = $2000
Rem
bbdoc: per link layer defined bit
end rem
Const IFF_LINK2:Int = $4000
Rem
bbdoc: use alternate physical connection
end rem
Const IFF_ALTPHYS:Int = IFF_LINK2
Rem
bbdoc: supports multicast
end rem
Const IFF_MULTICAST:Int = $8000
