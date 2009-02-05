' Copyright (c) 2007-2009 Bruce A Henderson
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
bbdoc: Raknet Network Library
End Rem
Module BaH.Raknet

ModuleInfo "Version: 1.00"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: Raknet - The Creative Commons Attribution - NonCommercial 2.5, or see http://www.rakkarsoft.com/SingleApplicationLicense.html"
ModuleInfo "Copyright: 2007-2009 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Version(Raknet 3.3)"


Import "common.bmx"

' Notes :
'
' Win32 uses fake alloca.h which simply includes malloc.h
'
' Note to self :
' RakNetTime and __GET_TIME_64BIT definition.
'  * Remember code should check and cast where required !!!
'


' for documenting :
Rem
	<p>Parameters: 
	<ul>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	</ul>
	</p>

	<table width="100%">
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>XXXXXXXXXXXXX</td><td>xxxxxxxxxxxxxxxxxxx</td></tr>
	</table>

End Rem

Rem
bbdoc: Miscellaneous RakNet Functions.
End Rem
Type TRKRakNet

	Rem
	bbdoc: Initialise seed for Random Generator.
	about: Parameters: 
	<ul>
	<li><b>seed</b> : The seed value for the random number generator.</li>
	</ul>
	End Rem
	Function SeedMT(seed:Int)
		bmx_raknet_seedMT(seed)
	End Function
	
	Rem
	bbdoc: Gets a random int.
	End Rem
	Function RandomMT:Int()
		Return bmx_raknet_randomMT()
	End Function
	
	Rem
	bbdoc: Gets a random float.
	End Rem
	Function FRandomMT:Float()
		Return bmx_raknet_frandomMT()
	End Function
	
	Rem
	bbdoc: Randomizes a buffer.
	End Rem
	Function FillBufferMT(buffer:Byte Ptr, size:Int)
		bmx_raknet_fillBufferMT(buffer, size)
	End Function
	
	Rem
	bbdoc: Returns the time.
	End Rem
	Function GetTime:Long()
		Local v:Long
		bmx_raknet_gettime(Varptr v)
		Return v
	End Function
	
	Rem
	bbdoc: Returns the time, in nanoseconds.
	End Rem
	Function GetTimeNS:Long()
		Local v:Long
		bmx_raknet_gettimens(Varptr v)
		Return v
	End Function
	
End Type

Rem
bbdoc: 
End Rem
Type TRKNetworkIDManager

	Field networkIdManagerPtr:Byte Ptr

	Method SetIsNetworkIDAuthority(isAuthority:Int)
	' TODO
	End Method
	
	Method IsNetworkIDAuthority:Int()
	' TODO
	End Method

	Method SetExternalSystemAddress(address:TRKSystemAddress)
	' TODO
	End Method

	Method GetExternalSystemAddress:TRKSystemAddress()
	' TODO
	End Method

	Method GetSharedNetworkID:Int()
	' TODO
	End Method

	Method SetSharedNetworkID(id:Int)
	' TODO
	End Method

End Type


Rem
bbdoc: The main interface for network communications.
about: Abstract. See TRKRakPeer for implementation.
End Rem
Type TRKRakPeerInterface

	Field rakPeerPtr:Byte Ptr

	Method Startup:Int(maxConnections:Int, threadSleepTimer:Int, descriptor:TRKSocketDescriptor = Null) Abstract
	Method InitializeSecurity(pubKeyE:Int Ptr, pubKeyN:Byte Ptr, privKeyP:Byte Ptr, privKeyQ:Byte Ptr) Abstract
	Method DisableSecurity() Abstract
	Method AddToSecurityExceptionList(ip:String) Abstract
	Method RemoveFromSecurityExceptionList(ip:String) Abstract
	Method IsInSecurityExceptionList:Int(ip:String) Abstract
	Method SetMaximumIncomingConnections(numberAllowed:Int) Abstract
	Method GetMaximumIncomingConnections:Int() Abstract
	Method NumberOfConnections:Int() Abstract
	Method SetIncomingPassword(passwordData:String) Abstract
	Method GetIncomingPassword:String() Abstract
	Method Connect:Int(host:String, remotePort:Int, passwordData:String = Null, connectionSocketIndex:Int = 0) Abstract
	'Method Console2Connect:Int(networkServiceId:Byte Ptr, passwordData:String) Abstract
	Method Shutdown(blockDuration:Int, orderingChannel:Int = 0) Abstract
	Method IsActive:Int() Abstract
	Method Receive:TRKPacket() Abstract
	Method DeallocatePacket(p:TRKPacket) Abstract
	Method GetMaximumNumberOfPeers:Int() Abstract
	Method SetNetworkIDManager(manager:TRKNetworkIDManager) Abstract
	Method GetNetworkIDManager:TRKNetworkIDManager() Abstract

	Method RegisterAsRemoteProcedureCall(name:String, func(params:TRKRPCParameters)) Abstract
	Method RPC:Int(id:String, data:Byte Ptr, bitLength:Int, priority:Int, reliability:Int, orderingChannel:Int, addr:TRKSystemAddress, ..
			broadcast:Int, includedTimestamp:Long Var, replyFromTarget:TRKBitStream = Null) Abstract

	Method CloseConnection(target:TRKSystemAddress, sendDisconnectionNotification:Int, orderingChannel:Int = 0) Abstract
	Method IsConnected:Int(addr:TRKSystemAddress, includeInProgress:Int = False, includeDisconnecting:Int = False) Abstract
	Method GetIndexFromSystemAddress:Int(addr:TRKSystemAddress) Abstract
	Method GetSystemAddressFromIndex:TRKSystemAddress(index:Int) Abstract
	Method AddToBanList(ip:String, milliseconds:Long = 0) Abstract
	Method RemoveFromBanList(ip:String) Abstract
	Method ClearBanList() Abstract
	Method IsBanned:Int(ip:String) Abstract
	Method Ping(addr:TRKSystemAddress) Abstract
	Method GetAveragePing:Int(addr:TRKSystemAddress) Abstract
	Method GetLastPing:Int(addr:TRKSystemAddress) Abstract
	Method GetLowestPing:Int(addr:TRKSystemAddress) Abstract
	Method SetOccasionalPing(doPing:Int) Abstract
	Method SetOfflinePingResponse(data:String) Abstract
	Method GetOfflinePingResponse:String() Abstract
	
	Method SendBitStream:Int( bitStream:TRKBitStream, priority:Int, reliability:Int, orderingChannel:Int, systemAddress:TRKSystemAddress, broadcast:Int) Abstract
	Method Send:Int(data:Byte Ptr, length:Int, priority:Int, reliability:Int, orderingChannel:Int, systemAddress:TRKSystemAddress, broadcast:Int) Abstract

	
	' Compression methods
	Method SetCompileFrequencyTable(doCompile:Int) Abstract
	Method GetOutgoingFrequencyTable:Int(outputFrequencyTable:Int[]) Abstract
	Method GenerateCompressionLayer:Int(inputFrequencyTable:Int[], inputLayer:Int) Abstract
	Method DeleteCompressionLayer:Int(inputLayer:Int) Abstract
	Method GetCompressionRatio:Float() Abstract
	Method GetDecompressionRatio:Float() Abstract
	
	' Network Simulator
	Method ApplyNetworkSimulator(maxSendBPS:Double, minExtraPing:Int, extraPingVariance:Int) Abstract
	Method SetPerConnectionOutgoingBandwidthLimit(maxBitsPerSecond:Int) Abstract
	Method IsNetworkSimulatorActive:Int() Abstract
	
	' Statistics methods
	Method GetStatistics:TRKRakNetStatistics(addr:TRKSystemAddress) Abstract

	Method AttachPlugin(plugin:TRKPluginInterface) Abstract
	Method DetachPlugin(plugin:TRKPluginInterface) Abstract

	Method GetInternalID:TRKSystemAddress(systemAddress:TRKSystemAddress = Null) Abstract
	Method GetExternalID:TRKSystemAddress(target:TRKSystemAddress) Abstract
	Method SetTimeoutTime(timeMS:Long, target:TRKSystemAddress) Abstract
	Method SetMTUSize:Int(size:Int) Abstract
	Method GetMTUSize:Int(target:TRKSystemAddress) Abstract
	Method GetNumberOfAddresses:Int() Abstract
	Method GetLocalIP:String(index:Int) Abstract
	Method IsLocalIP:Int(ip:String) Abstract
	Method AllowConnectionResponseIPMigration(allow:Int) Abstract
	Method AdvertiseSystem:Int(host:String, remotePort:Int, data:Byte Ptr, dataLength:Int, connectionSocketIndex:Int = 0) Abstract
	Method SetSplitMessageProgressInterval(interval:Int) Abstract
	Method SetUnreliableTimeout(timeoutMS:Long) Abstract
	Method SendTTL(host:String, remotePort:Int, ttl:Int, connectionSocketIndex:Int = 0) Abstract
	Method GetGuidFromSystemAddress:TRKRakNetGUID(systemAddress:TRKSystemAddress) Abstract

	Method PushBackPacket(packet:TRKPacket, pushAtHead:Int) Abstract
	Method AllocatePacket:TRKPacket(dataSize:Int) Abstract
	Method PingHost(host:String, remotePort:Int, onlyReplyOnAcceptingConnections:Int, connectionSocketIndex:Int = 0) Abstract

End Type

' internal callback handler
Type TRKRCPCache

	Field RCPCallback(params:TRKRPCParameters)

End Type

Rem
bbdoc: The primary interface for RakNet, RakPeer contains all major methods for the library.
End Rem
Type TRKRakPeer Extends TRKRakPeerInterface

	Global rcpMap:TMap = New TMap

	Function _create:TRKRakPeer(rakPeerPtr:Byte Ptr)
		If rakPeerPtr Then
			Local this:TRKRakPeer = New TRKRakPeer
			this.rakPeerPtr = rakPeerPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Starts the network threads, opens the listen port.
	returns: False on failure (can't create socket or thread), true on success.
	about: You must call this before calling Connect(). Multiple calls while already active are ignored.  To call this method
	again with different settings, you must first call Shutdown().
	<p>
	Call SetMaximumIncomingConnections if you want to accept incoming connections.
	</p>
	<p>Parameters: 
	<ul>
	<li><b>maxConnections</b> : The maximum number of connections between this instance of RakPeer and another instance of RakPeer.
	Required so the network can preallocate and for thread safety. A pure client would set this to 1.  A pure server would set it to the
	number of allowed clients.- A hybrid would set it to the sum of both types of connections</li>
	<li><b>threadSleepTimer</b> : How many ms to Sleep each internal update cycle (30 to give the game priority, 0 for regular (recommended))</li>
	<li><b>descriptor</b> : A SocketDescriptor structures to force RakNet to listen on a particular IP address or port</li>
	</ul>
	</p>
	End Rem
	Method Startup:Int(maxConnections:Int, threadSleepTimer:Int, descriptor:TRKSocketDescriptor)
		If descriptor Then
			Return bmx_RakPeer_Startup(rakPeerPtr, maxConnections, threadSleepTimer, descriptor.socketDescriptorPtr)
		Else
			Return bmx_RakPeer_Startup(rakPeerPtr, maxConnections, threadSleepTimer, Null)
		End If
	End Method
	
	Rem
	bbdoc: Secures connections though a combination of SHA1, AES128, SYN Cookies, and RSA to prevent connection spoofing, replay attacks, data eavesdropping, packet tampering, and MitM attacks.
	about: There is a significant amount of processing and a slight amount of bandwidth overhead for this feature.
	<p>
	If you accept connections, you must call this or else secure connections will not be enabled for incoming connections.
	</p>
	<p>
	If you are connecting to another system, you can call this with values for the (e and p,q) public keys before connecting to prevent MitM
	</p>
	<p>Parameters: 
	<ul>
	<li><b>pubKeyE</b> : A pointer to the public keys from the RSACrypt class.</li>
	<li><b>pubKeyN</b> : A pointer to the public keys from the RSACrypt class. </li>
	<li><b>privKeyP</b> : Public key generated from the RSACrypt class. </li>
	<li><b>privKeyQ</b> : Public key generated from the RSACrypt class.</li>
	</ul>
	If the private keys are Null, then a new key will be generated when this method is called.
	</p>
	End Rem
	Method InitializeSecurity(pubKeyE:Int Ptr, pubKeyN:Byte Ptr, privKeyP:Byte Ptr, privKeyQ:Byte Ptr)
		If pubKeyE Then
			If pubKeyN Then
				If privKeyP Then
					If privKeyQ Then
						bmx_RakPeer_InitializeSecurity(rakPeerPtr, pubKeyE, pubKeyN, privKeyP, privKeyQ)
					Else
						bmx_RakPeer_InitializeSecurity(rakPeerPtr, pubKeyE, pubKeyN, privKeyP, Null)
					End If
				Else
					If privKeyQ Then
						bmx_RakPeer_InitializeSecurity(rakPeerPtr, pubKeyE, pubKeyN, Null, privKeyQ)
					Else
						bmx_RakPeer_InitializeSecurity(rakPeerPtr, pubKeyE, pubKeyN, Null, Null)
					End If
				End If
			Else
				If privKeyP Then
					If privKeyQ Then
						bmx_RakPeer_InitializeSecurity(rakPeerPtr, pubKeyE, Null, privKeyP, privKeyQ)
					Else
						bmx_RakPeer_InitializeSecurity(rakPeerPtr, pubKeyE, Null, privKeyP, Null)
					End If
				Else
					If privKeyQ Then
						bmx_RakPeer_InitializeSecurity(rakPeerPtr, pubKeyE, Null, Null, privKeyQ)
					Else
						bmx_RakPeer_InitializeSecurity(rakPeerPtr, pubKeyE, Null, Null, Null)
					End If
				End If
			End If
		Else
			If pubKeyN Then
				If privKeyP Then
					If privKeyQ Then
						bmx_RakPeer_InitializeSecurity(rakPeerPtr, Null, pubKeyN, privKeyP, privKeyQ)
					Else
						bmx_RakPeer_InitializeSecurity(rakPeerPtr, Null, pubKeyN, privKeyP, Null)
					End If
				Else
					If privKeyQ Then
						bmx_RakPeer_InitializeSecurity(rakPeerPtr, Null, pubKeyN, Null, privKeyQ)
					Else
						bmx_RakPeer_InitializeSecurity(rakPeerPtr, Null, pubKeyN, Null, Null)
					End If
				End If
			Else
				If privKeyP Then
					If privKeyQ Then
						bmx_RakPeer_InitializeSecurity(rakPeerPtr, Null, Null, privKeyP, privKeyQ)
					Else
						bmx_RakPeer_InitializeSecurity(rakPeerPtr, Null, Null, privKeyP, Null)
					End If
				Else
					If privKeyQ Then
						bmx_RakPeer_InitializeSecurity(rakPeerPtr, Null, Null, Null, privKeyQ)
					Else
						bmx_RakPeer_InitializeSecurity(rakPeerPtr, Null, Null, Null, Null)
					End If
				End If
			End If
		End If
	End Method
	
	Rem
	bbdoc: Disables all security.
	about: Must be called while offline.
	End Rem
	Method DisableSecurity()
		bmx_RakPeer_DisableSecurity(rakPeerPtr)
	End Method

	Rem
	bbdoc: If secure connections are on, do not use secure connections for a specific IP address.
	about: This is useful if you have a fixed-address internal server behind a LAN.
	<p>
	Secure connections are determined by the recipient of an incoming connection. This has no effect if called on the system attempting
	to connect.
	</p>
	<p>Parameters: 
	<ul>
	<li><b>ip</b> : IP address to add. * wildcards are supported.</li>
	</ul>
	</p>
	End Rem
	Method AddToSecurityExceptionList(ip:String)
		bmx_RakPeer_AddToSecurityExceptionList(rakPeerPtr, ip)
	End Method
	
	Rem
	bbdoc: Remove a specific connection previously added via AddToSecurityExceptionList.
	about: Parameters: 
	<ul>
	<li><b>ip</b> : IP address to remove. Pass Null to remove all IP addresses. * wildcards are supported.</li>
	</ul>
	End Rem
	Method RemoveFromSecurityExceptionList(ip:String)
		bmx_RakPeer_RemoveFromSecurityExceptionList(rakPeerPtr, ip)
	End Method
	
	Rem
	bbdoc: Checks to see if a given IP is in the security exception list.
	about: Parameters: 
	<ul>
	<li><b>ip</b> : IP address to check.</li>
	</ul>
	End Rem
	Method IsInSecurityExceptionList:Int(ip:String)
		Return bmx_RakPeer_IsInSecurityExceptionList(rakPeerPtr, ip)
	End Method
	
	Rem
	bbdoc: Sets how many incoming connections are allowed.
	about: If this is less than the number of players currently connected, no more players will be allowed to connect.
	If this is greater than the maximum number of peers allowed, 	it will be reduced to the maximum number of peers allowed.
	<p>
	Defaults to 0, meaning by default, nobody can connect to you.
	</p>
	<p>Parameters: 
	<ul>
	<li><b>numberAllowed</b> : Maximum number of incoming connections allowed.</li>
	</ul>
	</p>
	End Rem
	Method SetMaximumIncomingConnections(numberAllowed:Int)
		bmx_RakPeer_SetMaximumIncomingConnections(rakPeerPtr, numberAllowed)
	End Method
	
	Rem
	bbdoc: Returns the value passed to SetMaximumIncomingConnections().
	returns: The maximum number of incoming connections, which is always <= maxConnections.
	End Rem
	Method GetMaximumIncomingConnections:Int()
		Return bmx_RakPeer_GetMaximumIncomingConnections(rakPeerPtr)
	End Method
	
	Rem
	bbdoc: Returns how many open connections there are at this time.
	returns: The number of open connections.
	End Rem
	Method NumberOfConnections:Int()
		Return bmx_RakPeer_NumberOfConnections(rakPeerPtr)
	End Method
	
	Rem
	bbdoc: Sets the password incoming connections must match in the call to Connect (defaults to none).
	about: Pass Null to passwordData to specify no password.
	<p>
	This is a way to set a low level password for all incoming connections.  To selectively reject connections, implement your own
	scheme using CloseConnection() to remove unwanted connections
	</p>
	<p>Parameters: 
	<ul>
	<li><b>passwordData</b> : A data block that incoming connections must match. This can be just a password, or can be a stream of data.
	Specify Null for no password data</li>
	</ul>
	</p>
	End Rem
	Method SetIncomingPassword(passwordData:String)
		bmx_RakPeer_SetIncomingPassword(rakPeerPtr, passwordData)
	End Method
	
	Rem
	bbdoc: Gets the password passed to SetIncomingPassword.
	returns: The password data you passed to SetIncomingPassword().
	End Rem
	Method GetIncomingPassword:String()
		Return bmx_RakPeer_GetIncomingPassword(rakPeerPtr)
	End Method
	
	Rem
	bbdoc: Connects to the specified host (ip or domain name) and server port.
	about: Calling Connect and not calling SetMaximumIncomingConnections acts as a dedicated client.
	<p>
	Calling both acts as a true peer. This is a non-blocking connection.
	</p>
	<p>
	You know the connection is successful when IsConnected() returns true or Receive() gets a message with the type
	identifier ID_CONNECTION_ACCEPTED.
	</p>
	<p>
	If the connection is not successful, such as a rejected connection or no response then neither of these things will happen.
	</p>
	End Rem
	Method Connect:Int(host:String, remotePort:Int, passwordData:String = Null, connectionSocketIndex:Int = 0)
		Return bmx_RakPeer_Connect(rakPeerPtr, host, remotePort, passwordData)
	End Method
	
	Rem
	bbdoc: Stops the network threads and closes all connections.
	about: Parameters: 
	<ul>
	<li><b>blockDuration</b> : How long, in milliseconds, you should wait for all remaining messages to go out, including
	ID_DISCONNECTION_NOTIFICATION.  If 0, it doesn't wait at all.</li>
	<li><b>orderingChannel</b> : If blockDuration > 0, ID_DISCONNECTION_NOTIFICATION will be sent on this channel.</li>
	</ul>
	<p>
	If you set it to 0 then the disconnection notification won't be sent.
	</p>
	End Rem
	Method Shutdown(blockDuration:Int, orderingChannel:Int = 0)
		bmx_RakPeer_Shutdown(rakPeerPtr, blockDuration, orderingChannel)
	End Method
	
	Rem
	bbdoc: Returns if the network thread is running.
	End Rem
	Method IsActive:Int()
		Return bmx_RakPeer_IsActive(rakPeerPtr)
	End Method
	
	Rem
	bbdoc: Gets a message from the incoming message queue.
	returns: Null if no packets are waiting to be handled, otherwise a packet.
	about: Use DeallocatePacket() to deallocate the message after you are done with it.
	<p>
	COMMON MISTAKE: Be sure to call this in a loop, once per game tick, until it returns Null. If you only process one packet per
	game tick they will buffer up.
	</p>
	End Rem
	Method Receive:TRKPacket()
		Return TRKPacket._create(bmx_RakPeer_Receive(rakPeerPtr))
	End Method
	
	Rem
	bbdoc: Call this to deallocate a message returned by Receive() when you are done handling it.
	about: Parameters: 
	<ul>
	<li><b>packet</b> : The message to deallocate.</li>
	</ul>
	End Rem
	Method DeallocatePacket(packet:TRKPacket)
		If packet Then ' handle null - for the lazy folks :-p
			bmx_RakPeer_DeallocatePacket(rakPeerPtr, packet.packetPtr)
		End If
	End Method
	
	Rem
	bbdoc: Return the total number of connections we are allowed
	End Rem
	Method GetMaximumNumberOfPeers:Int()
		Return bmx_RakPeer_GetMaximumNumberOfPeers(rakPeerPtr)
	End Method
	
	Rem
	bbdoc: Unregisters a function as available for calling as a remote procedure call that was formerly registered with RegisterAsRemoteProcedureCall.
	about: Only call offline.
	<p>Parameters: 
	<ul>
	<li><b>name</b> : A string of only letters to identify this procedure.</li>
	</ul>
	</p>
	End Rem
	Method UnregisterAsRemoteProcedureCall(name:String)
		rcpMap.remove(name)
		bmx_RakPeer_UnregisterAsRemoteProcedureCall(rakPeerPtr, name)
	End Method
	
	Rem
	bbdoc: Used by Object member RPC to lookup objects given that object's ID.
	End Rem
	Method SetNetworkIDManager(manager:TRKNetworkIDManager)
	' TODO
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetNetworkIDManager:TRKNetworkIDManager()
	' TODO
	End Method
	
	Rem
	bbdoc: Register a function as available for calling as a remote procedure call
	about: Parameters: 
	<ul>
	<li><b>name</b> : A unique string to identify this procedure.</li>
	<li><b>func</b> : The name of the function to be used as a function pointer. This can be called whether active or not, and
	registered functions stay registered unless unregistered.</li>
	</ul>
	End Rem
	Method RegisterAsRemoteProcedureCall(name:String, func(params:TRKRPCParameters))
		Local this:TRKRCPCache = New TRKRCPCache
		this.RCPCallback = func
		rcpMap.Insert(name, this)
		
		bmx_RakPeer_RegisterAsRemoteProcedureCall(rakPeerPtr, name, _rpcCallback)
	End Method
	
	Function _rpcCallback(params:Byte Ptr)
		Local p:TRKRPCParameters = TRKRPCParameters._create(params)
		Local cache:TRKRCPCache = TRKRCPCache(rcpMap.ValueForKey(p.GetFunctionName()))
		If cache Then
			cache.RCPCallback(p)
		End If
	End Function

	Rem
	bbdoc: Calls a function on the remote system that was already registered using RegisterAsRemoteProcedureCall().
	about: Parameters: 
	<ul>
	<li><b>id</b> : A string identifying the function to call.</li>
	<li><b>data</b> : The data to send </li>
	<li><b>bitLength</b> : The number of bits of data </li>
	<li><b>priority</b> : What priority level to send on. One of HIGH_PRIORITY, MEDIUM_PRIORITY or LOW_PRIORITY.</li>
	<li><b>reliability</b> : How reliability to send this data. One of UNRELIABLE, UNRELIABLE_SEQUENCED, RELIABLE, RELIABLE_ORDERED or RELIABLE_SEQUENCED.</li>
	<li><b>orderingChannel</b> : When using ordered or sequenced message, what channel to order these on. </li>
	<li><b>addr</b> : Who to send this message to, or in the case of broadcasting who not to send it to. Use UNASSIGNED_SYSTEM_ADDRESS to specify none.</li>
	<li><b>broadcast</b> : True to send this packet to all connected systems. If true, then systemAddress specifies who not to send the packet to. </li>
	<li><b>includedTimestamp</b> : Pass a timestamp if you wish, to be adjusted in the usual fashion as per ID_TIMESTAMP. Pass 0 to not include a timestamp. </li>
	<li><b>replyFromTarget</b> : If Null, this method is non-blocking. Otherwise it will block while waiting for a reply from the target
	procedure, which should be remotely written to RPCParameters::replyToSender and copied to replyFromTarget. The block will return early
	on disconnect or if the sent packet is unreliable and more than 3X the ping has elapsed.</li>
	</ul>
	End Rem
	Method RPC:Int(id:String, data:Byte Ptr, bitLength:Int, priority:Int, reliability:Int, orderingChannel:Int, addr:TRKSystemAddress, ..
			broadcast:Int, includedTimestamp:Long Var, replyFromTarget:TRKBitStream = Null)
		
		If data Then
			If addr Then
				If replyFromTarget Then
					Return bmx_RakPeer_RPC(rakPeerPtr, id, data, bitLength, priority, reliability, orderingChannel, addr.systemAddressPtr, broadcast, Varptr includedTimestamp, replyFromTarget.bitStreamPtr)
				Else
					Return bmx_RakPeer_RPC(rakPeerPtr, id, data, bitLength, priority, reliability, orderingChannel, addr.systemAddressPtr, broadcast, Varptr includedTimestamp, Null)
				End If
			Else
				If replyFromTarget Then
					Return bmx_RakPeer_RPC(rakPeerPtr, id, data, bitLength, priority, reliability, orderingChannel, Null, broadcast, Varptr includedTimestamp, replyFromTarget.bitStreamPtr)
				Else
					Return bmx_RakPeer_RPC(rakPeerPtr, id, data, bitLength, priority, reliability, orderingChannel, Null, broadcast, Varptr includedTimestamp, Null)
				End If
			End If
		Else
			If addr Then
				If replyFromTarget Then
					Return bmx_RakPeer_RPC(rakPeerPtr, id, Null, bitLength, priority, reliability, orderingChannel, addr.systemAddressPtr, broadcast, Varptr includedTimestamp, replyFromTarget.bitStreamPtr)
				Else
					Return bmx_RakPeer_RPC(rakPeerPtr, id, Null, bitLength, priority, reliability, orderingChannel, addr.systemAddressPtr, broadcast, Varptr includedTimestamp, Null)
				End If
			Else
				If replyFromTarget Then
					Return bmx_RakPeer_RPC(rakPeerPtr, id, Null, bitLength, priority, reliability, orderingChannel, Null, broadcast, Varptr includedTimestamp, replyFromTarget.bitStreamPtr)
				Else
					Return bmx_RakPeer_RPC(rakPeerPtr, id, Null, bitLength, priority, reliability, orderingChannel, Null, broadcast, Varptr includedTimestamp, Null)
				End If
			End If
		End If
	End Method
	
	Rem
	bbdoc: Closes the connection to another host (if we initiated the connection it will disconnect, if they did it will kick them out).
	about: Parameters: 
	<ul>
	<li><b>target </b> : Which system to close the connection to. </li>
	<li><b>sendDisconnectionNotification </b> : True to send ID_DISCONNECTION_NOTIFICATION to the recipient. False to close it silently.</li>
	<li><b>channel </b> : Which ordering channel to send the disconnection notification on, if any</li>
	</ul>
	End Rem
	Method CloseConnection(target:TRKSystemAddress, sendDisconnectionNotification:Int, orderingChannel:Int = 0)
		bmx_RakPeer_CloseConnection(rakPeerPtr, target.systemAddressPtr, sendDisconnectionNotification, orderingChannel)
	End Method
	
	Rem
	bbdoc: Returns if a particular systemAddress is connected to us (this also returns true if we are in the process of connecting)
	about: Parameters: 
	<ul>
	<li><b>systemAddress</b> : The SystemAddress we are referring to</li>
	<li><b>includeInProgress</b> : If true, also return true for connections that are in progress but haven't completed</li>
	<li><b>includeDisconnecting</b> : If true, also return true for connections that are currently disconnecting</li>
	</ul>
	End Rem
	Method IsConnected:Int(systemAddress:TRKSystemAddress, includeInProgress:Int = False, includeDisconnecting:Int = False)
		Return bmx_RakPeer_IsConnected(rakPeerPtr, systemAddress.systemAddressPtr, includeInProgress, includeDisconnecting)
	End Method
	
	Rem
	bbdoc: Given a systemAddress, returns an index from 0 to the maximum number of players allowed - 1.
	returns: The index of this SystemAddress or -1 on system not found.
	about: This includes systems which were formerly connected, but are not now connected
	<p>Parameters: 
	<ul>
	<li><b>systemAddress</b> : The SystemAddress we are referring to</li>
	</ul>
	</p>
	End Rem
	Method GetIndexFromSystemAddress:Int(systemAddress:TRKSystemAddress)
		Return bmx_RakPeer_GetIndexFromSystemAddress(rakPeerPtr, systemAddress.systemAddressPtr)
	End Method
	
	Rem
	bbdoc: This method is only useful for looping through all systems.
	about: Given an index, will return a SystemAddress. 
	<p>Parameters: 
	<ul>
	<li><b>index</b> : Index should range between 0 and the maximum number of players allowed - 1.</li>
	</ul>
	</p>
	End Rem
	Method GetSystemAddressFromIndex:TRKSystemAddress(index:Int)
		Return TRKSystemAddress._create(bmx_RakPeer_GetSystemAddressFromIndex(rakPeerPtr, index))
	End Method
	
	Rem
	bbdoc: Bans an IP from connecting.
	about: Banned IPs persist between connections but are not saved on shutdown nor loaded on startup.
	<p>Parameters: 
	<ul>
	<li><b>ip</b> : Dotted IP address. Can use * as a wildcard, such as 128.0.0.* will ban all IP addresses starting with 128.0.0</li>
	<li><b>milliseconds</b> : how many ms for a temporary ban. Use 0 for a permanent ban</li>
	</ul>
	</p>
	End Rem
	Method AddToBanList(ip:String, milliseconds:Long = 0)
		bmx_RakPeer_AddToBanList(rakPeerPtr, ip, milliseconds)
	End Method
	
	Rem
	bbdoc: Allows a previously banned IP to connect.
	about: Parameters: 
	<ul>
	<li><b>ip</b> : Dotted IP address. Can use * as a wildcard, such as 128.0.0.* will banAll IP addresses starting with 128.0.0 </li>
	</ul>
	End Rem
	Method RemoveFromBanList(ip:String)
		bmx_RakPeer_RemoveFromBanList(rakPeerPtr, ip)
	End Method
	
	Rem
	bbdoc: Allows all previously banned IPs to connect. 
	End Rem
	Method ClearBanList()
		bmx_RakPeer_ClearBanList(rakPeerPtr)
	End Method
	
	Rem
	bbdoc: Returns true or false indicating if a particular IP is banned.
	returns: True if IP matches any IPs in the ban list, accounting for any wildcards. False otherwise. 
	about: Parameters: 
	<ul>
	<li><b>ip</b> : Dotted IP address.</li>
	</ul>
	End Rem
	Method IsBanned:Int(ip:String)
		Return bmx_RakPeer_IsBanned(rakPeerPtr, ip)
	End Method
	
	Rem
	bbdoc: Sends a ping to the specified connected system.
	about: The sender and recipient must already be started via a successful call to Startup()
	<p>Parameters: 
	<ul>
	<li><b>target</b> : Which system to ping</li>
	</ul>
	</p>
	End Rem
	Method Ping(target:TRKSystemAddress)
		bmx_RakPeer_Ping(rakPeerPtr, target.systemAddressPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method PingHost(host:String, remotePort:Int, onlyReplyOnAcceptingConnections:Int, connectionSocketIndex:Int = 0)
		bmx_RakPeer_PingHost(rakPeerPtr, host, remotePort, onlyReplyOnAcceptingConnections, connectionSocketIndex)
	End Method
	
	Rem
	bbdoc: Returns the average of all ping times read for the specific system or -1 if none read yet.
	about: Parameters: 
	<ul>
	<li><b>systemAddress</b> : Which system we are referring to </li>
	</ul>
	End Rem
	Method GetAveragePing:Int(systemAddress:TRKSystemAddress)
		Return bmx_RakPeer_GetAveragePing(rakPeerPtr, systemAddress.systemAddressPtr)
	End Method
	
	Rem
	bbdoc: Returns the last ping time read for the specific system or -1 if none read yet.
	about: Parameters: 
	<ul>
	<li><b>systemAddress</b> : Which system we are referring to</li>
	</ul>
	End Rem
	Method GetLastPing:Int(systemAddress:TRKSystemAddress)
		Return bmx_RakPeer_GetLastPing(rakPeerPtr, systemAddress.systemAddressPtr)
	End Method
	
	Rem
	bbdoc: Returns the lowest ping time read or -1 if none read yet.
	about: Parameters: 
	<ul>
	<li><b>systemAddress</b> : Which system we are referring to </li>
	</ul>
	End Rem
	Method GetLowestPing:Int(systemAddress:TRKSystemAddress)
		Return bmx_RakPeer_GetLowestPing(rakPeerPtr, systemAddress.systemAddressPtr)
	End Method
	
	Rem
	bbdoc: Ping the remote systems every so often, or not.
	about: This is off by default. Can be called anytime. 
	<p>Parameters: 
	<ul>
	<li><b>doPing</b> : True to start occasional pings. False to stop them.</li>
	</ul>
	</p>
	End Rem
	Method SetOccasionalPing(doPing:Int)
		bmx_RakPeer_SetOccasionalPing(rakPeerPtr, doPing)
	End Method
	
	Rem
	bbdoc: Sets the data to send along with a LAN server discovery or offline ping reply.
	about: Length should be under 400 bytes, as a security measure against flood attacks 
	<p>Parameters: 
	<ul>
	<li><b>data</b> : a block of data to store, or Null for none </li>
	</ul>
	</p>
	End Rem
	Method SetOfflinePingResponse(data:String)
		bmx_RakPeer_SetOfflinePingResponse(rakPeerPtr, data)
	End Method
	
	Rem
	bbdoc: Returns a copy of text passed to SetOfflinePingResponse.
	End Rem
	Method GetOfflinePingResponse:String()
		Return bmx_RakPeer_GetOfflinePingResponse(rakPeerPtr)
	End Method
	

	Rem
	bbdoc: Enables or disables frequency table tracking.
	about: This is required to get a frequency table, which is used in GenerateCompressionLayer(). This value
	persists between connect calls and defaults to false (no frequency tracking).
	<p>
	You can call this at any time - however you SHOULD only call it when disconnected. Otherwise you will only
	trackpart of the values sent over the network.
	</p>
	<p>Parameters: 
	<ul>
	<li><b>doCompile</b> : True to enable tracking</li>
	</ul>
	</p>
	End Rem
	Method SetCompileFrequencyTable(doCompile:Int)
		bmx_RakPeer_SetCompileFrequencyTable(rakPeerPtr, doCompile)
	End Method

	Method GetOutgoingFrequencyTable:Int(outputFrequencyTable:Int[])
	' TODO
	End Method

	Method GenerateCompressionLayer:Int(inputFrequencyTable:Int[], inputLayer:Int)
	' TODO
	End Method

	Rem
	bbdoc: Deletes the output or input layer as specified.
	returns: False (failure) if connected. Otherwise true (success)
	about: This is not necessary to call and is only valuable for freeing memory.
	<p>
	You should only call this when disconnected.
	</p>
	<p>Parameters: 
	<ul>
	<li><b>inputLayer</b> : True to mean the inputLayer, false to mean the output layer</li>
	</ul>
	</p>
	End Rem
	Method DeleteCompressionLayer:Int(inputLayer:Int)
		Return bmx_RakPeer_DeleteCompressionLayer(rakPeerPtr, inputLayer)
	End Method

	Rem
	bbdoc: Returns the compression ratio.
	about: A low compression ratio is good. Compression is for outgoing data.
	End Rem
	Method GetCompressionRatio:Float()
		Return bmx_RakPeer_GetCompressionRatio(rakPeerPtr)
	End Method

	Rem
	bbdoc: Returns the decompression ratio.
	about: A high decompression ratio is good. Decompression is for incoming data.
	End Rem
	Method GetDecompressionRatio:Float()
		Return bmx_RakPeer_GetDecompressionRatio(rakPeerPtr)
	End Method


	Rem
	bbdoc: Adds simulated ping and packet loss to the outgoing data flow.
	about: To simulate bi-directional ping and packet loss, you should call this on both the sender and the
	recipient, with half the total ping and maxSendBPS value on each. You can exclude network simulator code
	with the _RELEASE define to decrease code size.
	<p>Parameters: 
	<ul>
	<li><b>maxSendBPS</b> : Maximum bits per second to send. Packetloss grows linearly. 0 to disable. (CURRENTLY BROKEN - ALWAYS DISABLED) </li>
	<li><b>minExtraPing</b> : The minimum time to delay sends.</li>
	<li><b>extraPingVariance</b> : The additional random time to delay sends.</li>
	</ul>
	</p>
	End Rem
	Method ApplyNetworkSimulator(maxSendBPS:Double, minExtraPing:Int, extraPingVariance:Int)
		bmx_RakPeer_ApplyNetworkSimulator(rakPeerPtr, maxSendBPS, minExtraPing, extraPingVariance)
	End Method

	Rem
	bbdoc: Limits how much outgoing bandwidth can be sent per-connection.
	about: This limit does not apply to the sum of all connections! Exceeding the limit queues up outgoing traffic
	<p>Parameters: 
	<ul>
	<li><b>maxBitsPerSecond</b> : Maximum bits per second to send. Use 0 for unlimited (default). Once set, it takes effect immedately and persists until called again.</li>
	</ul>
	</p>
	End Rem
	Method SetPerConnectionOutgoingBandwidthLimit(maxBitsPerSecond:Int)
		bmx_RakPeer_SetPerConnectionOutgoingBandwidthLimit(rakPeerPtr, maxBitsPerSecond)
	End Method

	Rem
	bbdoc: Returns if you previously called ApplyNetworkSimulator.
	End Rem
	Method IsNetworkSimulatorActive:Int()
		Return bmx_RakPeer_IsNetworkSimulatorActive(rakPeerPtr)
	End Method

	
	' Statistics methods
	Rem
	bbdoc: Returns a Type containing a large set of network statistics for the specified system.
	returns: Null on can't find the specified system, or a TRKRakNetStatistics object otherwise.
	about: You can map this data to a string using the ToString() or ToVerboseString() methods.
	<p>Parameters: 
	<ul>
	<li><b>systemAddress</b> : Which connected system to get statistics for</li>
	</ul>
	</p>
	End Rem
	Method GetStatistics:TRKRakNetStatistics(systemAddress:TRKSystemAddress)
		Return TRKRakNetStatistics._create(bmx_RakPeer_GetStatistics(rakPeerPtr, systemAddress.systemAddressPtr))
	End Method

	Rem
	bbdoc: 
	about: Parameters: 
	<ul>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	</ul>
	End Rem
	Method Send:Int(data:Byte Ptr, length:Int, priority:Int, reliability:Int, orderingChannel:Int, systemAddress:TRKSystemAddress, broadcast:Int)
		If systemAddress Then
			Return bmx_RakPeer_Send(rakPeerPtr, data, length, priority, reliability, orderingChannel, systemAddress.systemAddressPtr, broadcast)
		Else
			Return bmx_RakPeer_Send(rakPeerPtr, data, length, priority, reliability, orderingChannel, Null, broadcast)
		End If
	End Method

	Rem
	bbdoc: 
	about: Parameters: 
	<ul>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	</ul>
	End Rem
	Method SendBitStream:Int( bitStream:TRKBitStream, priority:Int, reliability:Int, orderingChannel:Int, systemAddress:TRKSystemAddress, broadcast:Int)
		If systemAddress Then
			Return bmx_RakPeer_SendBitStream(rakPeerPtr, bitStream.bitStreamPtr, priority, reliability, orderingChannel, systemAddress.systemAddressPtr, broadcast)
		Else
			Return bmx_RakPeer_SendBitStream(rakPeerPtr, bitStream.bitStreamPtr, priority, reliability, orderingChannel, Null, broadcast)
		End If
	End Method

	Rem
	bbdoc: Attatches a Plugin interface to run code automatically on message receipt in the Receive call.
	about: If plugins have dependencies on each other then the order does matter - for example the router plugin should
	go first because it might route messages for other plugins.
	<p>Parameters: 
	<ul>
	<li><b>plugin</b> : the plugin to attach</li>
	</ul>
	</p>
	End Rem
	Method AttachPlugin(plugin:TRKPluginInterface)
		bmx_RakPeer_AttachPlugin(rakPeerPtr, plugin.pluginPtr)
	End Method
	
	Rem
	bbdoc: Detaches a Plugin interface to run code automatically on message receipt.
	about: Parameters: 
	<ul>
	<li><b>plugin</b> : the plugin to detach</li>
	</ul>
	End Rem
	Method DetachPlugin(plugin:TRKPluginInterface)
		bmx_RakPeer_DetachPlugin(rakPeerPtr, plugin.pluginPtr)
	End Method

	Rem
	bbdoc: Returns the unique address identifier that represents you on the the network and is based on your local IP / port.
	returns: The identifier of your system internally, which may not be how other systems see if you if you are behind a NAT or proxy.
	about: Parameters: 
	<ul>
	<li><b>systemAddress</b> : Use Null to get your behind-LAN address. Use a connected system to get their behind-LAN address</li>
	</ul>
	End Rem
	Method GetInternalID:TRKSystemAddress(systemAddress:TRKSystemAddress = Null)
		If systemAddress Then
			Return TRKSystemAddress._create(bmx_RakPeer_GetInternalID(rakPeerPtr, systemAddress.systemAddressPtr))
		Else
			Return TRKSystemAddress._create(bmx_RakPeer_GetInternalID(rakPeerPtr, Null))
		End If
	End Method
	
	Rem
	bbdoc: Returns the unique address identifier that represents you on the the network and is based on your externalIP / port (the IP / port the specified player uses to communicate with you).
	about: Parameters: 
	<ul>
	<li><b>target</b> : Which remote system you are referring to for your external ID. Usually the same for all systems, unless you have two or more network cards.</li>
	</ul>
	End Rem
	Method GetExternalID:TRKSystemAddress(target:TRKSystemAddress)
		Return TRKSystemAddress._create(bmx_RakPeer_GetExternalID(rakPeerPtr, target.systemAddressPtr))
	End Method
	
	Rem
	bbdoc: 
	about: Parameters: 
	<ul>
	<li><b>timeMS</b> : Time, in milliseconds</li>
	<li><b>target</b> : Which system to do this for. Pass UNASSIGNED_SYSTEM_ADDRESS for all systems.</li>
	</ul>
	End Rem
	Method SetTimeoutTime(timeMS:Long, target:TRKSystemAddress)
		bmx_RakPeer_SetTimeoutTime(rakPeerPtr, timeMS, target.systemAddressPtr)
	End Method
	
	Rem
	bbdoc: Set the MTU per datagram.
	returns: False on failure (we are connected), else True.
	about: It's important to set this correctly - otherwise packets will be needlessly split, decreasing performance
	and throughput. Maximum allowed size is MAXIMUM_MTU_SIZE. Too high of a value will cause packets not to arrive at
	worst and be fragmented at best. Too low of a value will split packets unnecessarily. Recommended size is 1500.
	<p>
	Can only be called when not connected.
	</p>
	<p>Parameters: 
	<ul>
	<li><b>size</b> : The MTU size</li>
	</ul>
	</p>
	End Rem
	Method SetMTUSize:Int(size:Int)
		Return bmx_RakPeer_SetMTUSize(rakPeerPtr, size)
	End Method
	
	Rem
	bbdoc: Returns the current MTU size.
	returns: The current MTU size.
	about: Parameters: 
	<ul>
	<li><b>target</b> : Which system to get this for. UNASSIGNED_SYSTEM_ADDRESS to get the default</li>
	</ul>
	End Rem
	Method GetMTUSize:Int(target:TRKSystemAddress)
		Return bmx_RakPeer_GetMTUSize(rakPeerPtr, target.systemAddressPtr)
	End Method
	
	Rem
	bbdoc: Returns the number of IP addresses this system has internally.
	about: Get the actual addresses from GetLocalIP(). 
	End Rem
	Method GetNumberOfAddresses:Int()
		Return bmx_RakPeer_GetNumberOfAddresses(rakPeerPtr)
	End Method
	
	Rem
	bbdoc: Returns an IP address at index 0 to GetNumberOfAddresses-1. 
	about: Parameters: 
	<ul>
	<li><b>index</b> : The index to get the IP address for.</li>
	</ul>
	End Rem
	Method GetLocalIP:String(index:Int)
		Return bmx_RakPeer_GetLocalIP(rakPeerPtr, index)
	End Method

	Rem
	bbdoc: Is this a local IP?
	returns: True if this is one of the IP addresses returned by GetLocalIP.
	about: Parameters: 
	<ul>
	<li><b>ip</b> : An IP address to check, excluding the port</li>
	</ul>
	End Rem
	Method IsLocalIP:Int(ip:String)
		Return bmx_RakPeer_IsLocalIP(rakPeerPtr, ip)
	End Method

	Rem
	bbdoc: Allow or disallow connection responses from any IP.
	about: Normally this should be false, but may be necessary when connecting to servers with multiple IP addresses.
	<p>Parameters: 
	<ul>
	<li><b>allow</b> : True to allow this behavior, false to not allow. Defaults to false. Value persists between connections</li>
	</ul>
	</p>
	End Rem
	Method AllowConnectionResponseIPMigration(allow:Int)
		bmx_RakPeer_AllowConnectionResponseIPMigration(rakPeerPtr, allow)
	End Method
	
	Rem
	bbdoc: Sends a one byte message ID_ADVERTISE_SYSTEM to the remote unconnected system.
	returns: False if IsActive()==false or the host is unresolvable. True otherwise
	about: This will tell the remote system our external IP outside the LAN along with some user data.
	<p>
	The sender and recipient must already be started via a successful call to Initialize.
	</p>
	<p>Parameters: 
	<ul>
	<li><b>host</b> : Either a dotted IP address or a domain name</li>
	<li><b>remotePort</b> : Which port to connect to on the remote machine</li>
	<li><b>data</b> : Optional data to append to the packet.</li>
	<li><b>dataLength</b> : length of data in bytes. Use 0 if no data. </li>
	<li><b>connectionSocketIndex</b> : Index into the array of socket descriptors passed to socketDescriptors in TRKRakPeer::Startup() to send on. </li>
	</ul>
	</p>
	End Rem
	Method AdvertiseSystem:Int(host:String, remotePort:Int, data:Byte Ptr, dataLength:Int, connectionSocketIndex:Int = 0)
		Return bmx_RakPeer_AdvertiseSystem(rakPeerPtr, host, remotePort, data, dataLength, connectionSocketIndex)
	End Method
	
	Rem
	bbdoc: Controls how often to return ID_DOWNLOAD_PROGRESS for large message downloads.
	about: ID_DOWNLOAD_PROGRESS is returned to indicate a new partial message chunk, roughly the MTU size,
	has arrived As it can be slow or cumbersome to get this notification for every chunk, you can set the
	interval at which it is returned. Defaults to 0 (never return this notification)
	<p>Parameters: 
	<ul>
	<li><b>interval</b> : How many messages to use as an interval</li>
	</ul>
	</p>
	End Rem
	Method SetSplitMessageProgressInterval(interval:Int)
		bmx_RakPeer_SetSplitMessageProgressInterval(rakPeerPtr, interval)
	End Method
	
	Rem
	bbdoc: Set how long to wait before giving up on sending an unreliable message.
	about: Useful if the network is clogged up. Set to 0 or less to never timeout. Defaults to 0.
	<p>Parameters: 
	<ul>
	<li><b>timeoutMS</b> : How many ms to wait before simply not sending an unreliable message.</li>
	</ul>
	</p>
	End Rem
	Method SetUnreliableTimeout(timeoutMS:Long)
		bmx_RakPeer_SetUnreliableTimeout(rakPeerPtr, timeoutMS)
	End Method
	
	Rem
	bbdoc: Send a message to host, with the IP socket option TTL set to 3.
	about: This message will not reach the host, but will open the router.
	<p>Parameters: 
	<ul>
	<li><b>host</b> : Either a dotted IP address or a domain name</li>
	<li><b>remotePort</b> : Which port to connect to on the remote machine</li>
	<li><b>ttl</b> : Max hops of datagram Used for NAT-Punchthrough</li>
	<li><b>connectionSocketIndex</b> : Index into the array of socket descriptors passed to socketDescriptors in TRKRakPeer::Startup() to send on. </li>
	</ul>
	End Rem
	Method SendTTL(host:String, remotePort:Int, ttl:Int, connectionSocketIndex:Int = 0)
		bmx_RakPeer_SendTTL(rakPeerPtr, host, remotePort, ttl, connectionSocketIndex)
	End Method
	
	Rem
	bbdoc: 
	about: Parameters: 
	<ul>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	</ul>
	End Rem
	Method GetGuidFromSystemAddress:TRKRakNetGUID(systemAddress:TRKSystemAddress)
		Return TRKRakNetGUID._create(bmx_RakPeer_GetGuidFromSystemAddress(rakPeerPtr, systemAddress.systemAddressPtr))
	End Method

	Rem
	bbdoc: 
	End Rem
	Method PushBackPacket(packet:TRKPacket, pushAtHead:Int)
		bmx_RakPeer_PushBackPacket(rakPeerPtr, packet.packetPtr, pushAtHead)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AllocatePacket:TRKPacket(dataSize:Int)
		Return TRKPacket._create(bmx_RakPeer_AllocatePacket(rakPeerPtr, dataSize))
	End Method

End Type


Rem
bbdoc: This Type allows you to write and read native types as a string of bits.
about: TRKBitStream is used extensively throughout RakNet and is designed to be used by users as well.
End Rem
Type TRKBitStream

	Field bitStreamPtr:Byte Ptr
	
	Field owner:Int

	Function _create:TRKBitStream(bitStreamPtr:Byte Ptr)
		If bitStreamPtr Then
			Local this:TRKBitStream = New TRKBitStream
			this.bitStreamPtr = bitStreamPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Function CreateBitStream:TRKBitStream()
		Return New TRKBitStream.Create()
	End Function

	Rem
	bbdoc: 
	End Rem
	Method Create:TRKBitStream()
		owner = True
		bitStreamPtr = bmx_BitStream_Create()
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function CreateFromData:TRKBitStream(data:Byte Ptr, size:Int, copy:Int)
		If data Then
			Local this:TRKBitStream = New TRKBitStream
			this.owner = True
			this.bitStreamPtr = bmx_BitStream_CreateFromData(data, size, copy)
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Resets the bitstream for reuse.
	End Rem
	Method Reset()
		bmx_BitStream_Reset(bitStreamPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeByte:Int(writeToBitstream:Int, value:Byte Var)
		Return bmx_BitStream_SerializeByte(bitStreamPtr, writeToBitstream, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeShort:Int(writeToBitstream:Int, value:Short Var)
		Return bmx_BitStream_SerializeShort(bitStreamPtr, writeToBitstream, Varptr value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SerializeInt:Int(writeToBitstream:Int, value:Int Var)
		Return bmx_BitStream_SerializeInt(bitStreamPtr, writeToBitstream, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeFloat:Int(writeToBitstream:Int, value:Float Var)
		Return bmx_BitStream_SerializeFloat(bitStreamPtr, writeToBitstream, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeDouble:Int(writeToBitstream:Int, value:Double Var)
		Return bmx_BitStream_SerializeDouble(bitStreamPtr, writeToBitstream, Varptr value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SerializeDeltaLastByte:Int(writeToBitstream:Int, currentValue:Byte Var, lastValue:Byte)
		Return bmx_BitStream_SerializeDeltaLastByte(bitStreamPtr, writeToBitstream, Varptr currentValue, lastValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeDeltaLastShort:Int(writeToBitstream:Int, currentValue:Short Var, lastValue:Short)
		Return bmx_BitStream_SerializeDeltaLastShort(bitStreamPtr, writeToBitstream, Varptr currentValue, lastValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeDeltaLastInt:Int(writeToBitstream:Int, currentValue:Int Var, lastValue:Int)
		Return bmx_BitStream_SerializeDeltaLastInt(bitStreamPtr, writeToBitstream, Varptr currentValue, lastValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeDeltaLastFloat:Int(writeToBitstream:Int, currentValue:Float Var, lastValue:Float)
		Return bmx_BitStream_SerializeDeltaLastFloat(bitStreamPtr, writeToBitstream, Varptr currentValue, lastValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeDeltaLastDouble:Int(writeToBitstream:Int, currentValue:Double Var, lastValue:Double)
		Return bmx_BitStream_SerializeDeltaLastDouble(bitStreamPtr, writeToBitstream, Varptr currentValue, lastValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeDeltaByte:Int(writeToBitstream:Int, currentValue:Byte Var)
		Return bmx_BitStream_SerializeDeltaByte(bitStreamPtr, writeToBitstream, Varptr currentValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeDeltaShort:Int(writeToBitstream:Int, currentValue:Short Var)
		Return bmx_BitStream_SerializeDeltaShort(bitStreamPtr, writeToBitstream, Varptr currentValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeDeltaInt:Int(writeToBitstream:Int, currentValue:Int Var)
		Return bmx_BitStream_SerializeDeltaInt(bitStreamPtr, writeToBitstream, Varptr currentValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeDeltaFloat:Int(writeToBitstream:Int, currentValue:Float Var)
		Return bmx_BitStream_SerializeDeltaFloat(bitStreamPtr, writeToBitstream, Varptr currentValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeDeltaDouble:Int(writeToBitstream:Int, currentValue:Double Var)
		Return bmx_BitStream_SerializeDeltaDouble(bitStreamPtr, writeToBitstream, Varptr currentValue)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SerializeCompressedByte:Int(writeToBitstream:Int, value:Byte Var)
		Return bmx_BitStream_SerializeCompressedByte(bitStreamPtr, writeToBitstream, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeCompressedShort:Int(writeToBitstream:Int, value:Short Var)
		Return bmx_BitStream_SerializeCompressedShort(bitStreamPtr, writeToBitstream, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeCompressedInt:Int(writeToBitstream:Int, value:Int Var)
		Return bmx_BitStream_SerializeCompressedInt(bitStreamPtr, writeToBitstream, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeCompressedFloat:Int(writeToBitstream:Int, value:Float Var)
		Return bmx_BitStream_SerializeCompressedFloat(bitStreamPtr, writeToBitstream, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeCompressedDouble:Int(writeToBitstream:Int, value:Double Var)
		Return bmx_BitStream_SerializeCompressedDouble(bitStreamPtr, writeToBitstream, Varptr value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SerializeCompressedDeltaLastByte:Int(writeToBitstream:Int, currentValue:Byte Var, lastValue:Byte)
		Return bmx_BitStream_SerializeCompressedDeltaLastByte(bitStreamPtr, writeToBitstream, Varptr currentValue, lastValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeCompressedDeltaLastShort:Int(writeToBitstream:Int, currentValue:Short Var, lastValue:Short)
		Return bmx_BitStream_SerializeCompressedDeltaLastShort(bitStreamPtr, writeToBitstream, Varptr currentValue, lastValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeCompressedDeltaLastInt:Int(writeToBitstream:Int, currentValue:Int Var, lastValue:Int)
		Return bmx_BitStream_SerializeCompressedDeltaLastInt(bitStreamPtr, writeToBitstream, Varptr currentValue, lastValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeCompressedDeltaLastFloat:Int(writeToBitstream:Int, currentValue:Float Var, lastValue:Float)
		Return bmx_BitStream_SerializeCompressedDeltaLastFloat(bitStreamPtr, writeToBitstream, Varptr currentValue, lastValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeCompressedDeltaLastDouble:Int(writeToBitstream:Int, currentValue:Double Var, lastValue:Double)
		Return bmx_BitStream_SerializeCompressedDeltaLastDouble(bitStreamPtr, writeToBitstream, Varptr currentValue, lastValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeCompressedDeltaByte:Int(writeToBitstream:Int, currentValue:Byte Var)
		Return bmx_BitStream_SerializeCompressedDeltaByte(bitStreamPtr, writeToBitstream, Varptr currentValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeCompressedDeltaShort:Int(writeToBitstream:Int, currentValue:Short Var)
		Return bmx_BitStream_SerializeCompressedDeltaShort(bitStreamPtr, writeToBitstream, Varptr currentValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeCompressedDeltaInt:Int(writeToBitstream:Int, currentValue:Int Var)
		Return bmx_BitStream_SerializeCompressedDeltaInt(bitStreamPtr, writeToBitstream, Varptr currentValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeCompressedDeltaFloat:Int(writeToBitstream:Int, currentValue:Float Var)
		Return bmx_BitStream_SerializeCompressedDeltaFloat(bitStreamPtr, writeToBitstream, Varptr currentValue)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SerializeCompressedDeltaDouble:Int(writeToBitstream:Int, currentValue:Double Var)
		Return bmx_BitStream_SerializeCompressedDeltaDouble(bitStreamPtr, writeToBitstream, Varptr currentValue)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method ReadBool:Int(value:Int Var)
		Return bmx_BitStream_ReadBool(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadByte:Int(value:Byte Var)
		Return bmx_BitStream_ReadByte(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadShort:Int(value:Short Var)
		Return bmx_BitStream_ReadShort(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadInt:Int(value:Int Var)
		Return bmx_BitStream_ReadInt(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadFloat:Int(value:Float Var)
		Return bmx_BitStream_ReadFloat(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadDouble:Int(value:Double Var)
		Return bmx_BitStream_ReadDouble(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadLong:Int(value:Long Var)
		Return bmx_BitStream_ReadLong(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadTime:Int(value:Long Var)
		Return bmx_BitStream_ReadTime(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadString:String(length:Int)
		Return bmx_BitStream_ReadString(bitStreamPtr, length)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadDeltaByte:Int(value:Byte Var)
		Return bmx_BitStream_ReadDeltaByte(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadDeltaBool:Int(value:Int Var)
		Return bmx_BitStream_ReadDeltaBool(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadDeltaShort:Int(value:Short Var)
		Return bmx_BitStream_ReadDeltaShort(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadDeltaInt:Int(value:Int Var)
		Return bmx_BitStream_ReadDeltaInt(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadDeltaFloat:Int(value:Float Var)
		Return bmx_BitStream_ReadDeltaFloat(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadDeltaDouble:Int(value:Double Var)
		Return bmx_BitStream_ReadDeltaDouble(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadCompressedByte:Int(value:Byte Var)
		Return bmx_BitStream_ReadCompressedByte(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadCompressedShort:Int(value:Short Var)
		Return bmx_BitStream_ReadCompressedShort(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadCompressedInt:Int(value:Int Var)
		Return bmx_BitStream_ReadCompressedInt(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadCompressedBool:Int(value:Int Var)
		Return bmx_BitStream_ReadCompressedBool(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadCompressedFloat:Int(value:Float Var)
		Return bmx_BitStream_ReadCompressedFloat(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadCompressedDouble:Int(value:Double Var)
		Return bmx_BitStream_ReadCompressedDouble(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadCompressedDeltaByte:Int(value:Byte Var)
		Return bmx_BitStream_ReadCompressedDeltaByte(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadCompressedDeltaShort:Int(value:Short Var)
		Return bmx_BitStream_ReadCompressedDeltaShort(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadCompressedDeltaInt:Int(value:Int Var)
		Return bmx_BitStream_ReadCompressedDeltaInt(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadCompressedDeltaBool:Int(value:Int Var)
		Return bmx_BitStream_ReadCompressedDeltaBool(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadCompressedDeltaFloat:Int(value:Float Var)
		Return bmx_BitStream_ReadCompressedDeltaFloat(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ReadCompressedDeltaDouble:Int(value:Double Var)
		Return bmx_BitStream_ReadCompressedDeltaDouble(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method WriteByte(value:Byte)
		Local b:Byte = value
		bmx_BitStream_WriteByte(bitStreamPtr, Varptr b)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method WriteShort(value:Short Var)
		bmx_BitStream_WriteShort(bitStreamPtr, Varptr value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method WriteInt(value:Int Var)
		bmx_BitStream_WriteInt(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method WriteFloat(value:Float Var)
		bmx_BitStream_WriteFloat(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method WriteDouble(value:Double Var)
		bmx_BitStream_WriteDouble(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method WriteLong(value:Long Var)
		bmx_BitStream_WriteLong(bitStreamPtr, Varptr value)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method WriteTime(value:Long Var)
		bmx_BitStream_WriteTime(bitStreamPtr, Varptr value)
	End Method
	
	Method AssertStreamEmpty()
		bmx_BitStream_AssertStreamEmpty(bitStreamPtr)
	End Method
	
	Method IgnoreBits(numberOfBits:Int)
		bmx_BitStream_IgnoreBits(bitStreamPtr, numberOfBits)
	End Method
	
	Method IgnoreBytes(numberOfBytes:Int)
		bmx_BitStream_IgnoreBytes(bitStreamPtr, numberOfBytes)
	End Method
	
	Method SetWriteOffset(offset:Int)
		bmx_BitStream_SetWriteOffset(bitStreamPtr, offset)
	End Method

	Method Delete()
		If bitStreamPtr And owner Then
			bmx_BitStream_Delete(bitStreamPtr)
			bitStreamPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Unique identifier for a system.
about: Corresponds to a network address.
End Rem
Type TRKSystemAddress

	Field systemAddressPtr:Byte Ptr

	Function _create:TRKSystemAddress(systemAddressPtr:Byte Ptr)
		If systemAddressPtr Then
			Local this:TRKSystemAddress = New TRKSystemAddress
			this.systemAddressPtr = systemAddressPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: The peer address from inet_addr.
	End Rem
	Method GetBinaryAddress:Int()
		Return bmx_SystemAddress_GetBinaryAddress(systemAddressPtr)
	End Method
	
	Rem
	bbdoc: The port number
	End Rem
	Method GetPort:Int()
		Return bmx_SystemAddress_GetPort(systemAddressPtr)
	End Method
	
	Rem
	bbdoc: Returns True if the two addresses are equal.
	End Rem
	Method Equals:Int(address:TRKSystemAddress)
		Return bmx_SystemAddress_Equals(systemAddressPtr, address.systemAddressPtr)
	End Method

	Rem
	bbdoc: Returns a String representation of the TRKSystemAddress.
	End Rem
	Method ToString:String()
		Return bmx_SystemAddress_ToString(systemAddressPtr)
	End Method
	
	Method Delete()
		If systemAddressPtr Then
			bmx_SystemAddress_delete(systemAddressPtr)
			systemAddressPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: An invalid/Null system address.
End Rem
Global UNASSIGNED_SYSTEM_ADDRESS:TRKSystemAddress = TRKSystemAddress._create(bmx_SystemAddress_unassigned())

Rem
bbdoc: 
End Rem
Type TRKNetworkID

	Field networkIDPtr:Byte Ptr

	Function _create:TRKNetworkID(networkIDPtr:Byte Ptr)
		If networkIDPtr Then
			Local this:TRKNetworkID = New TRKNetworkID
			this.networkIDPtr = networkIDPtr
			Return this
		End If
	End Function

	Method Delete()
		If networkIDPtr Then
			bmx_NetworkID_delete(networkIDPtr)
			networkIDPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Global UNASSIGNED_NETWORK_ID:TRKNetworkID = TRKNetworkID._create(bmx_NetworkID_unassigned())

Rem
bbdoc: Describes the local socket to use for TRKRakPeer::Startup.
End Rem
Type TRKSocketDescriptor
	
	Field socketDescriptorPtr:Byte Ptr

	Rem
	bbdoc: Creates a new SocketDescriptor.
	about: Parameters: 
	<ul>
	<li><b>port</b> : The local port to bind to. Pass 0 to have the OS autoassign a port.</li>
	<li><b>hostAddress</b> : The local network card address to bind to, such as "127.0.0.1". Pass an empty string to use INADDR_ANY.</li>
	</ul>
	End Rem
	Function CreateSocketDescriptor:TRKSocketDescriptor(port:Int = 0, hostAddress:String = "")
		Return New TRKSocketDescriptor.Create(port, hostAddress)
	End Function
	
	Rem
	bbdoc: Creates a new SocketDescriptor.
	about: Parameters: 
	<ul>
	<li><b>port</b> : The local port to bind to. Pass 0 to have the OS autoassign a port.</li>
	<li><b>hostAddress</b> : The local network card address to bind to, such as "127.0.0.1". Pass an empty string to use INADDR_ANY.</li>
	</ul>
	End Rem
	Method Create:TRKSocketDescriptor(port:Int = 0, hostAddress:String = Null)
		socketDescriptorPtr = bmx_SocketDescriptor_new(port, hostAddress)
		Return Self
	End Method
	
	Rem
	bbdoc: Sets the local port to bind to.
	about: Set to 0 to have the OS autoassign a port.
	End Rem
	Method SetPort(port:Int)
		bmx_SocketDescriptor_setport(socketDescriptorPtr, port)
	End Method
	
	Rem
	bbdoc: Returns the local bound port.
	End Rem
	Method GetPort:Int()
		Return bmx_SocketDescriptor_getport(socketDescriptorPtr)
	End Method
	
	Rem
	bbdoc: Sets the local network card address to bind to, such as "127.0.0.1".
	about: Set to an empty string to use INADDR_ANY
	End Rem
	Method SetHostAddress(hostAddress:String)
		If hostAddress.length > 32 Then
			hostAddress = hostAddress[..32]
		End If
		bmx_SocketDescriptor_sethostaddress(socketDescriptorPtr, hostAddress)
	End Method
	
	Rem
	bbdoc: Returns the local bound network address.
	End Rem
	Method GetHostAddress:String()
		Return bmx_SocketDescriptor_gethostaddress(socketDescriptorPtr)
	End Method
		
	Method Delete()
		If socketDescriptorPtr Then
			bmx_SocketDescriptor_delete(socketDescriptorPtr)
			socketDescriptorPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: This represents a user message from another system.
End Rem
Type TRKPacket

	Field packetPtr:Byte Ptr

	Function _create:TRKPacket(packetPtr:Byte Ptr)
		If packetPtr Then
			Local this:TRKPacket = New TRKPacket
			this.packetPtr = packetPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: The data from the sender.
	End Rem
	Method GetData:Byte Ptr()
		Return bmx_Packet_GetData(packetPtr)
	End Method
	
	Rem
	bbdoc: The length of the data in bits.
	End Rem
	Method GetBitSize:Int()
		Return bmx_Packet_GetBitSize(packetPtr)
	End Method
	
	Rem
	bbdoc: The length of the data, in bytes.
	about: GetBitSize() is more accurate.
	End Rem
	Method GetLength:Int()
		Return bmx_Packet_GetBitSize(packetPtr) / 8
	End Method
	
	Rem
	bbdoc: The system that send this packet.
	End Rem
	Method GetSystemAddress:TRKSystemAddress()
		Return TRKSystemAddress._create(bmx_Packet_GetSystemAddress(packetPtr))
	End Method
	
	Rem
	bbdoc: A unique identifier for the system that sent this packet, regardless of IP address (internal / external / remote system).
	about: Only valid once a connection has been established (ID_CONNECTION_REQUEST_ACCEPTED, or ID_NEW_INCOMING_CONNECTION).
	Until that time, will be UNASSIGNED_RAKNET_GUID
	End Rem
	Method GetGuid:TRKRakNetGUID()
		Return TRKRakNetGUID._create(bmx_Packet_GetGuid(packetPtr))
	End Method

	Rem
	bbdoc: The packet identifier. eg. ID_NEW_INCOMING_CONNECTION.
	End Rem
	Method GetPacketIdentifier:Int()
		Return bmx_Packet_GetPacketIdentifier(packetPtr)
	End Method
	
End Type

Rem
bbdoc: And RPC parameter list.
End Rem
Type TRKRPCParameters

	Field rpcParametersPtr:Byte Ptr

	Function _create:TRKRPCParameters(rpcParametersPtr:Byte Ptr)
		If rpcParametersPtr Then
			Local this:TRKRPCParameters = New TRKRPCParameters
			this.rpcParametersPtr = rpcParametersPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Returns the name of the function that was called. 
	End Rem
	Method GetFunctionName:String()
		Return bmx_RPCParameters_GetFunctionName(rpcParametersPtr)
	End Method
	
	Rem
	bbdoc: Returns the data from the remote system.
	End Rem
	Method GetInput:Byte Ptr()
		Return bmx_RPCParameters_GetInput(rpcParametersPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetInputAsBitStream:TRKBitStream()
		Return TRKBitStream.CreateFromData(GetInput(), (GetNumberOfBitsOfData()-1)/8+1, False)
	End Method
	
	Rem
	bbdoc: Returns the size of the data, as the number of bits.
	End Rem
	Method GetNumberOfBitsOfData:Int()
		Return bmx_RPCParameters_GetNumberOfBitsOfData(rpcParametersPtr)
	End Method
	
	Rem
	bbdoc: Returns the system which called the RPC.
	End Rem
	Method GetSender:TRKSystemAddress()
		Return TRKSystemAddress._create(bmx_RPCParameters_GetSender(rpcParametersPtr))
	End Method
	
	Rem
	bbdoc: Returns the RakPeer instance that got the call.
	End Rem
	Method GetRecipient:TRKRakPeerInterface()
		Return TRKRakPeer._create(bmx_RPCParameters_GetRecipient(rpcParametersPtr))
	End Method
	
	Rem
	bbdoc: You can return values from RPC calls by writing them to this BitStream.
	about: This is only sent back if the RPC call originally passed a BitStream to receive the reply.
	If you do so and your send is reliable, it will block until you get a reply or you get
	disconnected from the system you are sending to, whichever is first. If your send is not
	reliable, it will block for triple the ping time, or until you are disconnected, or you get a
	reply, whichever is first.
	End Rem
	Method GetReplyToSender:TRKBitStream()
		Return TRKBitStream._create(bmx_RPCParameters_GetReplyToSender(rpcParametersPtr))
	End Method
	
End Type


Rem
bbdoc: Factory for RakNet objects.
End Rem
Type TRKRakNetworkFactory

	Rem
	bbdoc: Creates an instance of TRKRakPeerInterface.
	End Rem
	Function GetRakPeerInterface:TRKRakPeerInterface()
		Return TRKRakPeer._create(bmx_RakNetworkFactory_GetRakPeerInterface())
	End Function
	
	Rem
	bbdoc: Creates an instance of TRKConsoleServer.
	End Rem
	Function GetConsoleServer:TRKConsoleServer()
		Return TRKConsoleServer._create(bmx_RakNetworkFactory_GetConsoleServer())
	End Function

	Rem
	bbdoc: Creates an instance of TRKReplicaManager.
	End Rem
	Function GetReplicaManager:TRKReplicaManager()
		Return TRKReplicaManager._create(bmx_RakNetworkFactory_GetReplicaManager())
	End Function

	Rem
	bbdoc: Creates an instance of TRKLogCommandParser.
	End Rem
	Function GetLogCommandParser:TRKLogCommandParser()
		Return TRKLogCommandParser._create(bmx_RakNetworkFactory_GetLogCommandParser())
	End Function

	Rem
	bbdoc: Creates an instance of TRKPacketLogger.
	End Rem
	Function GetPacketLogger:TRKPacketLogger()
		Return TRKPacketLogger._create(bmx_RakNetworkFactory_GetPacketLogger())
	End Function

	Rem
	bbdoc: Creates an instance of TRKRakNetCommandParser.
	End Rem
	Function GetRakNetCommandParser:TRKRakNetCommandParser()
		Return TRKRakNetCommandParser._create(bmx_RakNetworkFactory_GetRakNetCommandParser())
	End Function

	Rem
	bbdoc: Creates an instance of TRKRakNetTransport.
	End Rem
	Function GetRakNetTransport:TRKRakNetTransport()
		Return TRKRakNetTransport._create(bmx_RakNetworkFactory_GetRakNetTransport())
	End Function

	Rem
	bbdoc: Creates an instance of TRKTelnetTransport.
	End Rem
	Function GetTelnetTransport:TRKTelnetTransport()
		Return TRKTelnetTransport._create(bmx_RakNetworkFactory_GetTelnetTransport())
	End Function

	Rem
	bbdoc: Creates an instance of TRKPacketConsoleLogger.
	End Rem
	Function GetPacketConsoleLogger:TRKPacketConsoleLogger()
		Return TRKPacketConsoleLogger._create(bmx_RakNetworkFactory_GetPacketConsoleLogger())
	End Function

	Rem
	bbdoc: Creates an instance of TRKPacketFileLogger.
	End Rem
	Function GetPacketFileLogger:TRKPacketFileLogger()
		Return TRKPacketFileLogger._create(bmx_RakNetworkFactory_GetPacketFileLogger())
	End Function

	Rem
	bbdoc: Creates an instance of TRKRouter.
	End Rem
	Function GetRouter:TRKRouter()
		Return TRKRouter._create(bmx_RakNetworkFactory_GetRouter())
	End Function

	Rem
	bbdoc: Creates an instance of TRKConnectionGraph.
	End Rem
	Function GetConnectionGraph:TRKConnectionGraph()
		Return TRKConnectionGraph._create(bmx_RakNetworkFactory_GetConnectionGraph())
	End Function

	Rem
	bbdoc: Deletes the TRKRakPeerInterface instance.
	End Rem
	Function DestroyRakPeerInterface( p:TRKRakPeerInterface )
		If p Then
			bmx_RakNetworkFactory_DestroyRakPeerInterface(p.rakPeerPtr)
		End If
	End Function

	Function DestroyConsoleServer( s:TRKConsoleServer)
	' TODO
	End Function

	Function DestroyReplicaManager( m:TRKReplicaManager)
	' TODO
	End Function

	Function DestroyLogCommandParser( p:TRKLogCommandParser)
	' TODO
	End Function

	Function DestroyPacketLogger( l:TRKPacketLogger )
	' TODO
	End Function

	Function DestroyRakNetCommandParser( n:TRKRakNetCommandParser )
	' TODO
	End Function

	Function DestroyRakNetTransport(  n:TRKRakNetTransport )
	' TODO
	End Function

	Function DestroyTelnetTransport(  t:TRKTelnetTransport )
	' TODO
	End Function

	Function DestroyPacketConsoleLogger(  c:TRKPacketConsoleLogger )
	' TODO
	End Function

	Function DestroyPacketFileLogger(  f:TRKPacketFileLogger )
	' TODO
	End Function

	Function DestroyRouter(  r:TRKRouter )
	' TODO
	End Function

	Function DestroyConnectionGraph( g:TRKConnectionGraph )
	' TODO
	End Function
	
End Type


Rem
bbdoc: 
End Rem
Type TRKConsoleServer

	Field consoleServerPtr:Byte Ptr

	Function _create:TRKConsoleServer(consoleServerPtr:Byte Ptr)
		If consoleServerPtr Then
			Local this:TRKConsoleServer = New TRKConsoleServer
			this.consoleServerPtr = consoleServerPtr
			Return this
		End If
	End Function
	
	
End Type

Rem
bbdoc: 
End Rem
Type TRKReplicaManager

	Field replicaManagerPtr:Byte Ptr

	Function _create:TRKReplicaManager(replicaManagerPtr:Byte Ptr)
		If replicaManagerPtr Then
			Local this:TRKReplicaManager = New TRKReplicaManager
			this.replicaManagerPtr = replicaManagerPtr
			Return this
		End If
	End Function
	
	
End Type

Rem
bbdoc: 
End Rem
Type TRKLogCommandParser

	Field logCommandParserPtr:Byte Ptr

	Function _create:TRKLogCommandParser(logCommandParserPtr:Byte Ptr)
		If logCommandParserPtr Then
			Local this:TRKLogCommandParser = New TRKLogCommandParser
			this.logCommandParserPtr = logCommandParserPtr
			Return this
		End If
	End Function
	
	
End Type

Rem
bbdoc: 
End Rem
Type TRKPacketLogger

	Field packetLoggerPtr:Byte Ptr

	Function _create:TRKPacketLogger(packetLoggerPtr:Byte Ptr)
		If packetLoggerPtr Then
			Local this:TRKPacketLogger = New TRKPacketLogger
			this.packetLoggerPtr = packetLoggerPtr
			Return this
		End If
	End Function
	
	
End Type

Rem
bbdoc: 
End Rem
Type TRKRakNetCommandParser

	Field rakNetCommandParserPtr:Byte Ptr

	Function _create:TRKRakNetCommandParser(rakNetCommandParserPtr:Byte Ptr)
		If rakNetCommandParserPtr Then
			Local this:TRKRakNetCommandParser = New TRKRakNetCommandParser
			this.rakNetCommandParserPtr = rakNetCommandParserPtr
			Return this
		End If
	End Function
	
	
End Type

Rem
bbdoc: 
End Rem
Type TRKRakNetTransport

	Field rakNetTransportPtr:Byte Ptr

	Function _create:TRKRakNetTransport(rakNetTransportPtr:Byte Ptr)
		If rakNetTransportPtr Then
			Local this:TRKRakNetTransport = New TRKRakNetTransport
			this.rakNetTransportPtr = rakNetTransportPtr
			Return this
		End If
	End Function
	
	
End Type

Rem
bbdoc: 
End Rem
Type TRKTelnetTransport

	Field telnetTransportPtr:Byte Ptr

	Function _create:TRKTelnetTransport(telnetTransportPtr:Byte Ptr)
		If telnetTransportPtr Then
			Local this:TRKTelnetTransport = New TRKTelnetTransport
			this.telnetTransportPtr = telnetTransportPtr
			Return this
		End If
	End Function
	
	
End Type

Rem
bbdoc: 
End Rem
Type TRKPacketConsoleLogger

	Field packetConsoleLoggerPtr:Byte Ptr

	Function _create:TRKPacketConsoleLogger(packetConsoleLoggerPtr:Byte Ptr)
		If packetConsoleLoggerPtr Then
			Local this:TRKPacketConsoleLogger = New TRKPacketConsoleLogger
			this.packetConsoleLoggerPtr = packetConsoleLoggerPtr
			Return this
		End If
	End Function
	
	
End Type

Rem
bbdoc: 
End Rem
Type TRKPacketFileLogger

	Field packetFileLoggerPtr:Byte Ptr

	Function _create:TRKPacketFileLogger(packetFileLoggerPtr:Byte Ptr)
		If packetFileLoggerPtr Then
			Local this:TRKPacketFileLogger = New TRKPacketFileLogger
			this.packetFileLoggerPtr = packetFileLoggerPtr
			Return this
		End If
	End Function
	
	
End Type

Rem
bbdoc: 
End Rem
Type TRKRouter

	Field routerPtr:Byte Ptr

	Function _create:TRKRouter(routerPtr:Byte Ptr)
		If routerPtr Then
			Local this:TRKRouter = New TRKRouter
			this.routerPtr = routerPtr
			Return this
		End If
	End Function
	
	
End Type

Rem
bbdoc: 
End Rem
Type TRKConnectionGraph

	Field connectionGraphPtr:Byte Ptr

	Function _create:TRKConnectionGraph(connectionGraphPtr:Byte Ptr)
		If connectionGraphPtr Then
			Local this:TRKConnectionGraph = New TRKConnectionGraph
			this.connectionGraphPtr = connectionGraphPtr
			Return this
		End If
	End Function
	
	
End Type

Rem
bbdoc: Stores Statistics information related to network usage.
End Rem
Type TRKRakNetStatistics

	Field statsPtr:Byte Ptr

	Function _create:TRKRakNetStatistics(statsPtr:Byte Ptr)
		If statsPtr Then
			Local this:TRKRakNetStatistics = New TRKRakNetStatistics
			this.statsPtr = statsPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Returns a String representation of the TRKRakNetStatistics.
	about: Equivalent to ToVerboseString(0).
	End Rem
	Method ToString:String()
		Return ToVerboseString(0)
	End Method

	Rem
	bbdoc: Returns a String representation of the TRKRakNetStatistics, for a specified @verbosityLevel.
	End Rem
	Method ToVerboseString:String(verbosityLevel:Int)
		Return bmx_RakNetStatistics_ToStringLevel(statsPtr, verbosityLevel)
	End Method

	Rem
	bbdoc: Number of Messages in the send Buffer (high, medium, low priority)
	end rem
	Method messageSendBuffer:Int(priority:Int)
		Return bmx_RakNetStatistics_messageSendBuffer(statsPtr, priority)
	End Method
	Rem
	bbdoc: Number of messages sent (high, medium, low priority)
	end rem
	Method messagesSent:Int(priority:Int)
		Return bmx_RakNetStatistics_messagesSent(statsPtr, priority)
	End Method
	Rem
	bbdoc: Number of data bits used for user messages
	end rem
	Method messageDataBitsSent:Long(priority:Int)
		Local v:Long
		bmx_RakNetStatistics_messageDataBitsSent(statsPtr, priority, Varptr v)
		Return v
	End Method
	Rem
	bbdoc: Number of total bits used for user messages, including headers
	end rem
	Method messageTotalBitsSent:Long(priority:Int)
		Local v:Long
		bmx_RakNetStatistics_messageTotalBitsSent(statsPtr, priority, Varptr v)
		Return v
	End Method
	Rem
	bbdoc: Number of packets sent containing only acknowledgments
	end rem
	Method packetsContainingOnlyAcknowlegements:Int()
		Return bmx_RakNetStatistics_packetsContainingOnlyAcknowlegements(statsPtr)
	End Method
	Rem
	bbdoc: Number of acknowledgments sent
	end rem
	Method acknowlegementsSent:Int()
		Return bmx_RakNetStatistics_acknowlegementsSent(statsPtr)
	End Method
	Rem
	bbdoc: Number of acknowledgments waiting to be sent
	end rem
	Method acknowlegementsPending:Int()
		Return bmx_RakNetStatistics_acknowlegementsPending(statsPtr)
	End Method
	Rem
	bbdoc: Number of acknowledgments bits sent
	end rem
	Method acknowlegementBitsSent:Long()
		Local v:Long
		bmx_RakNetStatistics_acknowlegementBitsSent(statsPtr, Varptr v)
		Return v
	End Method
	Rem
	bbdoc: Number of packets containing only acknowledgments and resends
	end rem
	Method packetsContainingOnlyAcknowlegementsAndResends:Int()
		Return bmx_RakNetStatistics_packetsContainingOnlyAcknowlegementsAndResends(statsPtr)
	End Method
	Rem
	bbdoc: Number of messages resent
	end rem
	Method messageResends:Int()
		Return bmx_RakNetStatistics_messageResends(statsPtr)
	End Method
	Rem
	bbdoc: Number of bits resent of actual data
	end rem
	Method messageDataBitsResent:Long()
		Local v:Long
		bmx_RakNetStatistics_messageDataBitsResent(statsPtr, Varptr v)
		Return v
	End Method
	Rem
	bbdoc: Total number of bits resent, including headers
	end rem
	Method messagesTotalBitsResent:Long()
		Local v:Long
		bmx_RakNetStatistics_messagesTotalBitsResent(statsPtr, Varptr v)
		Return v
	End Method
	Rem
	bbdoc: Number of messages waiting for ack (// TODO - rename this)
	end rem
	Method messagesOnResendQueue:Int()
		Return bmx_RakNetStatistics_messagesOnResendQueue(statsPtr)
	End Method
	Rem
	bbdoc: Number of messages not split for sending
	end rem
	Method numberOfUnsplitMessages:Int()
		Return bmx_RakNetStatistics_numberOfUnsplitMessages(statsPtr)
	End Method
	Rem
	bbdoc: Number of messages split for sending
	end rem
	Method numberOfSplitMessages:Int()
		Return bmx_RakNetStatistics_numberOfSplitMessages(statsPtr)
	End Method
	Rem
	bbdoc: Total number of splits done for sending
	end rem
	Method totalSplits:Int()
		Return bmx_RakNetStatistics_totalSplits(statsPtr)
	End Method
	Rem
	bbdoc: Total packets sent
	end rem
	Method packetsSent:Int()
		Return bmx_RakNetStatistics_packetsSent(statsPtr)
	End Method
	Rem
	bbdoc: Number of bits added by encryption
	end rem
	Method encryptionBitsSent:Long()
		Local v:Long
		bmx_RakNetStatistics_encryptionBitsSent(statsPtr, Varptr v)
		Return v
	End Method
	Rem
	bbdoc: total bits sent
	end rem
	Method totalBitsSent:Long()
		Local v:Long
		bmx_RakNetStatistics_totalBitsSent(statsPtr, Varptr v)
		Return v
	End Method
	Rem
	bbdoc: Number of sequenced messages arrived out of order
	end rem
	Method sequencedMessagesOutOfOrder:Int()
		Return bmx_RakNetStatistics_sequencedMessagesOutOfOrder(statsPtr)
	End Method
	Rem
	bbdoc: Number of sequenced messages arrived in order
	end rem
	Method sequencedMessagesInOrder:Int()
		Return bmx_RakNetStatistics_sequencedMessagesInOrder(statsPtr)
	End Method
	Rem
	bbdoc: Number of ordered messages arrived out of order
	end rem
	Method orderedMessagesOutOfOrder:Int()
		Return bmx_RakNetStatistics_orderedMessagesOutOfOrder(statsPtr)
	End Method
	Rem
	bbdoc: Number of ordered messages arrived in order
	end rem
	Method orderedMessagesInOrder:Int()
		Return bmx_RakNetStatistics_orderedMessagesInOrder(statsPtr)
	End Method
	Rem
	bbdoc: Packets with a good CRC received
	end rem
	Method packetsReceived:Int()
		Return bmx_RakNetStatistics_packetsReceived(statsPtr)
	End Method
	Rem
	bbdoc: Packets with a bad CRC received
	end rem
	Method packetsWithBadCRCReceived:Int()
		Return bmx_RakNetStatistics_packetsWithBadCRCReceived(statsPtr)
	End Method
	Rem
	bbdoc: Bits with a good CRC received
	end rem
	Method bitsReceived:Long()
		Local v:Long
		bmx_RakNetStatistics_bitsReceived(statsPtr, Varptr v)
		Return v
	End Method
	Rem
	bbdoc: Bits with a bad CRC received
	end rem
	Method bitsWithBadCRCReceived:Long()
		Local v:Long
		bmx_RakNetStatistics_bitsWithBadCRCReceived(statsPtr, Varptr v)
		Return v
	End Method
	Rem
	bbdoc: Number of acknowledgement messages received for packets we are resending
	end rem
	Method acknowlegementsReceived:Int()
		Return bmx_RakNetStatistics_acknowlegementsReceived(statsPtr)
	End Method
	Rem
	bbdoc: Number of acknowledgement messages received for packets we are not resending
	end rem
	Method duplicateAcknowlegementsReceived:Int()
		Return bmx_RakNetStatistics_duplicateAcknowlegementsReceived(statsPtr)
	End Method
	Rem
	bbdoc: Number of data messages (anything other than an ack) received that are valid and not duplicate
	end rem
	Method messagesReceived:Int()
		Return bmx_RakNetStatistics_messagesReceived(statsPtr)
	End Method
	Rem
	bbdoc: Number of data messages (anything other than an ack) received that are invalid
	end rem
	Method invalidMessagesReceived:Int()
		Return bmx_RakNetStatistics_invalidMessagesReceived(statsPtr)
	End Method
	Rem
	bbdoc: Number of data messages (anything other than an ack) received that are duplicate
	end rem
	Method duplicateMessagesReceived:Int()
		Return bmx_RakNetStatistics_duplicateMessagesReceived(statsPtr)
	End Method
	Rem
	bbdoc: Number of messages waiting for reassembly
	end rem
	Method messagesWaitingForReassembly:Int()
		Return bmx_RakNetStatistics_messagesWaitingForReassembly(statsPtr)
	End Method
	Rem
	bbdoc: Number of messages in reliability output queue
	end rem
	Method internalOutputQueueSize:Int()
		Return bmx_RakNetStatistics_internalOutputQueueSize(statsPtr)
	End Method
	Rem
	bbdoc: Current bits per second
	end rem
	Method bitsPerSecond:Double()
		Return bmx_RakNetStatistics_bitsPerSecond(statsPtr)
	End Method
	Rem
	bbdoc: connection start time
	end rem
	Method connectionStartTime:Long()
		Local v:Long
		bmx_RakNetStatistics_connectionStartTime(statsPtr, Varptr v)
		Return v
	End Method
	Rem
	bbdoc: If true, not all the data can go out in one frame, and RakNet is sending continuously RakNet will try to increase the bandwidth, so this condition may be temporary and only last a second.
	about: However, it if stays on most of the time, you are at the maximum bandwidth and should slow down your sends, because other data is now waiting.
	end rem
	Method bandwidthExceeded:Int()
		Return bmx_RakNetStatistics_bandwidthExceeded(statsPtr)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TRKRSACrypt

	Field rsaCryptPtr:Byte Ptr

	Method New()
		rsaCryptPtr = bmx_RSACrypt_new()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetPrivateKey:Int(p:Byte Ptr, q:Byte Ptr, factorLimbs:Int)
	' TODO
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetPublicKey:Int(modulus:Byte Ptr, modLimbs:Int, e:Int)
	' TODO
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GeneratePrivateKey:Int(limbs:Int)
		Return bmx_RSACrypt_generatePrivateKey(rsaCryptPtr, limbs)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetFactorLimbs:Int()
		Return bmx_RSACrypt_getFactorLimbs(rsaCryptPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetPrivateP(p:Byte Ptr)
		bmx_RSACrypt_getPrivateP(rsaCryptPtr, p)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetPrivateQ(q:Byte Ptr)
		bmx_RSACrypt_getPrivateQ(rsaCryptPtr, q)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetModLimbs:Int()
		Return bmx_RSACrypt_getModLimbs(rsaCryptPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetPublicModulus(modulus:Byte Ptr)
		bmx_RSACrypt_getPublicModulus(rsaCryptPtr, modulus)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetPublicExponent:Int()
		Return bmx_RSACrypt_getPublicExponent(rsaCryptPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method encrypt:Int(ct:Byte[], pt:Byte[])
	' TODO
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method decrypt:Int(pt:Byte[], ct:Byte[])
	' TODO
	End Method
	
End Type

Rem
bbdoc: A plugin to provide a simple way to compress and incrementally send the files in the FileList structure.
about: Similar to the TRKDirectoryDeltaTransfer plugin, except that it doesn't send deltas based on pre-existing files or actually
write the files to disk.
<p>
Usage: Call SetupReceive to allow one file set to arrive. The value returned by TRKFileListTransfer::SetupReceive() is the setID that
is allowed. It's up to you to transmit this value to the other system, along with information indicating what kind of files you want
to get. The other system should then prepare a FileList and call TRKFileListTransfer::Send(), passing the return value of
TRKFileListTransfer::SetupReceive() as the setID parameter to TRKFileListTransfer::Send()
</p>
End Rem
Type TRKFileListTransfer

	Field fileListTransferPtr:Byte Ptr
	
	Method New()
		fileListTransferPtr = bmx_FileListTransfer_new() 
	End Method
	
	Rem
	bbdoc: Allows one corresponding Send() call from another system to arrive.
	about: Parameters: 
	<ul>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	</ul>
	End Rem
	Method SetupReceive:Int(handler:TRKFileListTransferCBInterface, deleteHandler:Int, allowedSender:TRKSystemAddress)
		Return bmx_FileListTransfer_SetupReceive(fileListTransferPtr, handler.cbInterfacePtr, deleteHandler, allowedSender.systemAddressPtr)
	End Method
	
	Rem
	bbdoc: 
	about: Parameters: 
	<ul>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	</ul>
	End Rem
	Method Send(fileList:TRKFileList, rakPeer:TRKRakPeerInterface, recipient:TRKSystemAddress, setID:Int, priority:Int, orderingChannel:Int, compressData:Int)
		bmx_FileListTransfer_Send(fileListTransferPtr, fileList.fileListPtr, rakPeer.rakPeerPtr, recipient.systemAddressPtr, setID, priority, orderingChannel, compressData)
	End Method
	
	Rem
	bbdoc: Stop a download. 
	about: Parameters: 
	<ul>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	</ul>
	End Rem
	Method CancelReceive(setId:Int)
		bmx_FileListTransfer_CancelReceive(fileListTransferPtr, setID)
	End Method
	
	Rem
	bbdoc: Remove all handlers associated with a particular system address.
	about: Parameters: 
	<ul>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	</ul>
	End Rem
	Method RemoveReceiver(systemAddress:TRKSystemAddress)
		bmx_FileListTransfer_RemoveReceiver(fileListTransferPtr, systemAddress.systemAddressPtr)
	End Method
	
	Rem
	bbdoc: Is a handler passed to SetupReceive still running?
	about: Parameters: 
	<ul>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	</ul>
	End Rem
	Method IsHandlerActive:Int(setId:Int)
		Return bmx_FileListTransfer_IsHandlerActive(fileListTransferPtr, setID)
	End Method
	
	Method Delete()
		If fileListTransferPtr Then
			bmx_FileListTransfer_delete(fileListTransferPtr)
			fileListTransferPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Used by TRKFileListTransfer plugin as a callback for when we get a file. You get the last file when fileIndex==setCount.
about: You should implement 
End Rem
Type TRKFileListTransferCBInterface

	Field cbInterfacePtr:Byte Ptr

	Method New()
' TODO
'		cbInterfacePtr = bmx_FileListTransferCBInterface_new(self)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method OnFile:Int(onFileStruct:TRKOnFileStruct)
	End Method
	
	Function _OnFile:Int(iface:TRKFileListTransferCBInterface, structPtr:Byte Ptr)
		Return iface.OnFile(TRKOnFileStruct._create(structPtr))
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method OnFileProgress(onFileStruct:TRKOnFileStruct, partCount:Int, partTotal:Int, partLength:Int)
	End Method
	
	Function _OnFileProgress(iface:TRKFileListTransferCBInterface, structPtr:Byte Ptr, partCount:Int, partTotal:Int, partLength:Int)
		iface.OnFileProgress(TRKOnFileStruct._create(structPtr), partCount, partTotal, partLength)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Update:Int()
	End Method
	
	Function _Update:Int(iface:TRKFileListTransferCBInterface)
		Return iface.Update()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method OnDownloadComplete:Int()
	End Method
	
	Function _OnDownloadComplete:Int(iface:TRKFileListTransferCBInterface)
		Return iface.OnDownloadComplete()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method OnDereference()
	End Method
	
	Function _OnDereference(iface:TRKFileListTransferCBInterface)
		iface.OnDereference()
	End Function

End Type

Rem
bbdoc: TODO
End Rem
Type TRKOnFileStruct

	Field structPtr:Byte Ptr

	Function _create:TRKOnFileStruct(structPtr:Byte Ptr)
		If structPtr Then
			Local this:TRKOnFileStruct = New TRKOnFileStruct
			this.structPtr = structPtr
			Return this
		End If
	End Function

	Method fileIndex:Int()
	' TODO
	End Method
	
	Method fileName:String()
	' TODO
	End Method
	
	Method fileData:Byte Ptr()
	' TODO
	End Method
	
	Method compressedTransmissionLength:Int()
	' TODO
	End Method
	
	Method finalDataLength:Int()
	' TODO
	End Method
	
	Method setID:Int()
	' TODO
	End Method
	
	Method setCount:Int()
	' TODO
	End Method
	
	Method setTotalCompressedTransmissionLength:Int()
	' TODO
	End Method
	
	Method setTotalFinalLength:Int()
	' TODO
	End Method
	
	Method context:TRKFileListNodeContext()
	' TODO
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TRKFileList

	Field fileListPtr:Byte Ptr
	
	Method New()
		fileListPtr = bmx_FileList_new()
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddFilesFromDirectory(applicationDirectory:String, subDirectory:String, writeHash:Int, writeData:Int, recursive:Int, ..
			context:TRKFileListNodeContext = Null)
		If context Then
			bmx_FileList_AddFilesFromDirectory(fileListPtr, applicationDirectory, subDirectory, writeHash, writeData, recursive, context.nodeContextPtr)
		Else
			bmx_FileList_AddFilesFromDirectory(fileListPtr, applicationDirectory, subDirectory, writeHash, writeData, recursive, Null)
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Clear()
		bmx_FileList_Clear(fileListPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Deserialize:Int(bitStream:TRKBitStream)
		Return bmx_FileList_Deserialize(fileListPtr, bitStream.bitStreamPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method ListMissingOrChangedFiles(applicationDirectory:String, missingOrChangedFiles:TRKFileList, alwaysWriteHash:Int, neverWriteHash:Int)
		bmx_FileList_ListMissingOrChangedFiles(fileListPtr, applicationDirectory, missingOrChangedFiles.fileListPtr, alwaysWriteHash, neverWriteHash)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetDeltaToCurrent(inputList:TRKFileList, outputList:TRKFileList, dirSubset:String, remoteSubdir:String)
		bmx_FileList_GetDeltaToCurrent(fileListPtr, inputList.fileListPtr, outputList.fileListPtr, dirSubset, remoteSubdir)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method PopulateDataFromDisk(applicationDirectory:String, writeFileData:Int, writeFileHash:Int, removeUnknownFiles:Int)
		bmx_FileList_PopulateDataFromDisk(fileListPtr, applicationDirectory, writeFileData, writeFileHash, removeUnknownFiles)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method WriteDataToDisk(applicationDirectory:String)
		bmx_FileList_WriteDataToDisk(fileListPtr, applicationDirectory)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method AddFileMemory(filename:String, data:Byte Ptr, dataLength:Int, fileLength:Int, context:TRKFileListNodeContext = Null, isAReference:Int = False)
		If context Then
			bmx_FileList_AddFileMemory(fileListPtr, filename, data, dataLength, fileLength, context.nodeContextPtr, isAReference)
		Else
			bmx_FileList_AddFileMemory(fileListPtr, filename, data, dataLength, fileLength, Null, isAReference)
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method AddFile(filepath:String, filename:String, context:TRKFileListNodeContext = Null)
		If context Then
			bmx_FileList_AddFile(fileListPtr, filepath, filename, context.nodeContextPtr)
		Else
			bmx_FileList_AddFile(fileListPtr, filepath, filename, Null)
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method DeleteFiles(applicationDirectory:String)
		bmx_FileList_DeleteFiles(fileListPtr, applicationDirectory)
	End Method

	Method Delete()
		If fileListPtr Then
			bmx_FileList_delete(fileListPtr)
			fileListPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TRKFileListNodeContext

	Field nodeContextPtr:Byte Ptr
	
	Method New()
		nodeContextPtr = bmx_FileListNodeContext_new()
	End Method
	
	
	Method Delete()
		If nodeContextPtr Then
			bmx_FileListNodeContext_delete(nodeContextPtr)
			nodeContextPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TRKPluginInterface

	Field pluginPtr:Byte Ptr

End Type

Rem
bbdoc: Assigns systems to FilterSets.
about: Each FilterSet limits what kinds of messages are allowed. The MessageFilter plugin is used for security where you limit
what systems can send what kind of messages. You implicitly define FilterSets, and add allowed message IDs and RPC calls to these
FilterSets. You then add systems to these filters, such that those systems are limited to sending what the filters allows. You can
automatically assign systems to a filter. You can automatically kick and possibly ban users that stay in a filter too long, or send
the wrong message. Each system is a member of either zero or one filters. Add this plugin before any plugin you wish to filter (most
likely just add this plugin before any other).
End Rem
Type TRKMessageFilter Extends TRKPluginInterface

	Method New()
		pluginPtr = bmx_MessageFilter_new()
	End Method
	
	Rem
	bbdoc: Automatically add all new systems to a particular filter
	about: Defaults to -1
	<p>Parameters: 
	<ul>
	<li><b>filterSetID</b> : Which filter to add new systems to. &lt; 0 for do not add. </li>
	</ul>
	</p>
	End Rem
	Method SetAutoAddNewConnectionsToFilter(filterSetID:Int)
		bmx_MessageFilter_SetAutoAddNewConnectionsToFilter(pluginPtr, filterSetID)
	End Method
	
	Rem
	bbdoc: Allow a range of message IDs.
	about: Always allowed by default: ID_CONNECTION_REQUEST_ACCEPTED through ID_DOWNLOAD_PROGRESS
	<p>
	Usually you specify a range to make it easier to add new enumerations without having to constantly refer back to this function.
	</p>
	<p>
	about: Parameters: 
	<ul>
	<li><b> allow </b> : True to allow this message ID, false to disallow. By default, all messageIDs except the noted types are disallowed. This includes messages from other plugins!</li>
	<li><b> messageIDStart </b> : The first ID_* message to allow in the range. Inclusive. </li>
	<li><b> messageIDEnd </b> : The last ID_* message to allow in the range. Inclusive. </li>
	<li><b> filterSetID </b> : A user defined ID to represent a filter set. If no filter with this ID exists, one will be created with default settings. </li>
	</ul>
	</p>
	End Rem
	Method SetAllowMessageID(allow:Int, messageIDStart:Int, messageIDEnd:Int, filterSetID:Int)
		bmx_MessageFilter_SetAllowMessageID(pluginPtr, allow, messageIDStart, messageIDEnd, filterSetID)
	End Method
	
	Rem
	bbdoc: Allow an RPC function, by name.
	about: Parameters: 
	<ul>
	<li><b> allow </b> : True to allow an RPC call with this function name, false to disallow. All RPCs are disabled by default. </li>
	<li><b> functionName </b> : The function name of the RPC call. Must match the function name exactly, including case. </li>
	<li><b> filterSetID </b> : A user defined ID to represent a filter set. If no filter with this ID exists, one will be created with default settings.</li>
	</ul>
	End Rem
	Method SetAllowRPC(allow:Int, functionName:String, filterSetID:Int)
		bmx_MessageFilter_SetAllowRPC(pluginPtr, allow, functionName, filterSetID)
	End Method
	
	Rem
	bbdoc: What action to take on a disallowed message.
	about: You can kick or not. You can add them to the ban list for some time By default no action is taken. The message is simply
	ignored. 0 for permanent ban, &gt; 0 for ban time in milliseconds.
	<p>Parameters: 
	<ul>
	<li><b> kickOnDisallowed </b> : kick the system that sent a disallowed message.</li>
	<li><b> banOnDisallowed </b> : ban the system that sent a disallowed message. See banTimeMS for the ban duration. </li>
	<li><b> banTimeMS </b> : Passed to the milliseconds parameter of TRKRakPeer::AddToBanList.</li>
	<li><b> filterSetID </b> : A user defined ID to represent a filter set. If no filter with this ID exists, one will be created with default settings.</li>
	</ul>
	</p>
	End Rem
	Method SetActionOnDisallowedMessage(kickOnDisallowed:Int, banOnDisallowed:Int, banTimeMS:Long, filterSetID:Int)
		bmx_MessageFilter_SetActionOnDisallowedMessage(pluginPtr, kickOnDisallowed, banOnDisallowed, banTimeMS, filterSetID)
	End Method
	
	Rem
	bbdoc: Limits how long a connection can stay in a particular filterSetID.
	about: After this time, the connection is kicked and possibly banned. By default there is no limit to how long a connection
	can stay in a particular filter set.
	<p>Parameters: 
	<ul>
	<li><b> allowedTimeMS </b> : How many milliseconds to allow a connection to stay in this filter set. </li>
	<li><b> banOnExceed </b> : True or false to ban the system, or not, when allowedTimeMS is exceeded </li>
	<li><b> banTimeMS </b> : Passed to the milliseconds parameter of TRKRakPeer::AddToBanList. </li>
	<li><b> filterSetID </b> : A user defined ID to represent a filter set. If no filter with this ID exists, one will be created with default settings. </li>
	</ul>
	</p>
	End Rem
	Method SetFilterMaxTime(allowedTimeMS:Int, banOnExceed:Int, banTimeMS:Long, filterSetID:Int)
		bmx_MessageFilter_SetFilterMaxTime(pluginPtr, allowedTimeMS, banOnExceed, banTimeMS, filterSetID)
	End Method
	
	Rem
	bbdoc: Gets the filterSetID a system is using. 
	returns: -1 for none.
	about: Parameters: 
	<ul>
	<li><b> systemAddress </b> : The system we are referring to </li>
	</ul>
	End Rem
	Method GetSystemFilterSet:Int(systemAddress:TRKSystemAddress)
		Return bmx_MessageFilter_GetSystemFilterSet(pluginPtr, systemAddress.systemAddressPtr)
	End Method
	
	Rem
	bbdoc: Assigns a system to a filter set.
	about: Systems are automatically added to filter sets (or not) based on SetAutoAddNewConnectionsToFilter(). This method is
	used to change the filter set a system is using, to add it to a new filter set, or to remove it from all existin filter sets.
	<p>Parameters: 
	<ul>
	<li><b> systemAddress </b> : The system we are referring to </li>
	<li><b> filterSetID </b> : A user defined ID to represent a filter set. If no filter with this ID exists, one will be created with default settings. If -1, the system will be removed from all filter sets. </li>
	</ul>
	</p>
	End Rem
	Method SetSystemFilterSet(systemAddress:TRKSystemAddress, filterSetID:Int)
		bmx_MessageFilter_SetSystemFilterSet(pluginPtr, systemAddress.systemAddressPtr, filterSetID)
	End Method
	
	Rem
	bbdoc: Returns the number of systems subscribed to a particular filter set.
	about: Using anything other than -1 for filterSetID is slow, so you should store the returned value.
	<p>Parameters: 
	<ul>
	<li><b> filterSetID </b> : The filter set to limit to. Use -1 for none (just returns the total number of filter systems in that case). </li>
	</ul>
	</p>
	End Rem
	Method GetSystemCount:Int(filterSetID:Int)
		Return bmx_MessageFilter_GetSystemCount(pluginPtr, filterSetID)
	End Method
	
	Rem
	bbdoc: Returns a system subscribed to a particular filter set,by index.
	about: @index should be between 0 and the GetSystemCount(filterSetID)-1
	<p>Parameters: 
	<ul>
	<li><b> filterSetID </b> : The filter set to limit to. Use -1 for none (just indexes all the filtered systems in that case). </li>
	<li><b> index </b> : A number between 0 and GetSystemCount(filterSetID)-1</li>
	</ul>
	</p>
	End Rem
	Method GetSystemByIndex:TRKSystemAddress(filterSetID:Int, index:Int)
		Return TRKSystemAddress._create(bmx_MessageFilter_GetSystemByIndex(pluginPtr, filterSetID, index))
	End Method
	
	Rem
	bbdoc: Returns the total number of filter sets.
	End Rem
	Method GetFilterSetCount:Int()
		Return bmx_MessageFilter_GetFilterSetCount(pluginPtr)
	End Method
	
	Rem
	bbdoc: Returns the ID of a filter set, by index.
	about: Parameters: 
	<ul>
	<li><b>index</b> : An 	index between 0 and GetFilterSetCount()-1 inclusive.</li>
	</ul>
	End Rem
	Method GetFilterSetIDByIndex:Int(index:Int)
		Return bmx_MessageFilter_GetFilterSetIDByIndex(pluginPtr, index)
	End Method
	
	Rem
	bbdoc: Deletes a FilterSet.
	about: All systems formerly subscribed to this filter are now unrestricted.
	<p>Parameters: 
	<ul>
	<li><b> filterSetID </b> : The ID of the filter set to delete.</li>
	</ul>
	</p>
	End Rem
	Method DeleteFilterSet(filterSetID:Int)
		bmx_MessageFilter_DeleteFilterSet(pluginPtr, filterSetID)
	End Method
	
	Method Delete()
		If pluginPtr Then
			bmx_MessageFilter_delete(pluginPtr)
			pluginPtr = Null
		End If
	End Method
End Type


Rem
bbdoc: 
End Rem
Global UNASSIGNED_RAKNET_GUID:TRKRakNetGUID = TRKRakNetGUID._create(bmx_RakNetGUID_unassigned())

Rem
bbdoc: Uniquely identifies an instance of RakPeer.
about: Use TRKRakPeer::GetGuidFromSystemAddress() and TRKRakPeer::GetSystemAddressFromGuid() to go between TRKSystemAddress and TRKRakNetGUID
<p>
Use TRKRakPeer::GetGuidFromSystemAddress(UNASSIGNED_SYSTSEM_ADDRESS) to get your own GUID
</p>
End Rem
Type TRKRakNetGUID

	Field guidPtr:Byte Ptr

	Function _create:TRKRakNetGUID(guidPtr:Byte Ptr)
		If guidPtr Then
			Local this:TRKRakNetGUID = New TRKRakNetGUID
			this.guidPtr = guidPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Returns the string representation of this TRKRakNetGUID.
	End Rem
	Method ToString:String()
		Return bmx_RakNetGUID_ToString(guidPtr)
	End Method

	Method Delete()
		If guidPtr Then
			bmx_RakNetGUID_delete(guidPtr)
			guidPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: The Ready event plugin is used to signal event completion when a set of systems all set a signal flag to true.
about: This is usually used for lobby systems, or peer to peer turn based environments. The user will get ID_READY_EVENT_SET
and ID_READY_EVENT_UNSET as the signal flag is set or unset The user will get ID_READY_EVENT_ALL_SET when all systems are done
waiting for all other systems, in which case the event is considered complete, and no longer tracked.
End Rem
Type TRKReadyEvent Extends TRKPluginInterface

	Method SetEvent:Int(_eventId:Int, isReady:Int)
	' TODO
	End Method
	
	Method DeleteEvent:Int(_eventId:Int)
	' TODO
	End Method
	
	Method IsEventSet:Int(_eventId:Int)
	' TODO
	End Method
	
	Method IsEventCompletionProcessing:Int(_eventId:Int)
	' TODO
	End Method
	
	Method IsEventCompleted:Int(_eventId:Int)
	' TODO
	End Method
	
	Method HasEvent:Int(_eventId:Int)
	' TODO
	End Method
	
	Method GetEventListSize:Int()
	' TODO
	End Method
	
	Method GetEventAtIndex:Int(index:Int)
	' TODO
	End Method
	
	Method AddToWaitList:Int(_eventId:Int, address:TRKSystemAddress)
	' TODO
	End Method
	
	Method RemoveFromWaitList:Int(_eventId:Int, address:TRKSystemAddress)
	' TODO
	End Method
	
	Method IsInWaitList:Int (_eventId:Int, address:TRKSystemAddress)
	' TODO
	End Method
	
	Method GetRemoteWaitListSize:Int(_eventId:Int)
	' TODO
	End Method
	
	Method GetFromWaitListAtIndex:TRKSystemAddress(_eventId:Int, index:Int)
	' TODO
	End Method
	
	Method GetReadyStatus:Int(_eventId:Int, address:TRKSystemAddress)
	' TODO
	End Method
	
	Method SetSendChannel(newChannel:Int)
	' TODO
	End Method
	
End Type
