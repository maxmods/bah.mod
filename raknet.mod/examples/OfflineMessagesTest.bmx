'
' Build as Console.
'
'
SuperStrict

Framework BaH.Raknet
Import BRL.StandardIO

Local peer1:TRKRakPeerInterface = TRKRakNetworkFactory.GetRakPeerInterface()
Local peer2:TRKRakPeerInterface = TRKRakNetworkFactory.GetRakPeerInterface()


Local sentPacket:Int
Local nextTest:Int

Print "This project tests the advertise system and offline ping messages."
Print "Difficulty: Beginner~n"

peer1.SetMaximumIncomingConnections(1)
Local socketDescriptor:TRKSocketDescriptor = New TRKSocketDescriptor.Create(60001)
peer1.Startup(1, 0, socketDescriptor)
socketDescriptor.SetPort(60002)
peer2.Startup(1, 0, socketDescriptor)
peer1.SetOfflinePingResponse("Offline Ping Data")

Print "Peer 1 guid = " + peer1.GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS).ToString()
Print "Peer 2 guid = " + peer2.GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS).ToString()
Print "Systems started.  Waiting for advertise system packet"

' Wait for connection to complete
Delay(300)

Print "Sending advertise system from " + peer1.GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS).ToString()
Local message:String = "hello world"
Local b:Byte Ptr = message.ToCString()
peer1.AdvertiseSystem("127.0.0.1", 60002, b, message.length)
MemFree(b)

While nextTest <> 2

	peer1.DeallocatePacket(peer1.Receive())
	Local packet:TRKPacket = peer2.Receive()
	If packet

		If packet.GetData()[0] = ID_ADVERTISE_SYSTEM Then

			If packet.GetLength() > 1 Then
				Print "Got Advertise system with data: " + String.FromCString(packet.GetData() + 1)
			Else
				Print "Got Advertise system with no data"
			End If
			Print "Was sent from GUID " + packet.GetGuid().ToString()

			Print "Sending ping from " + peer2.GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS).ToString()
			peer2.PingHost("127.0.0.1", 60001, False)
			nextTest:+ 1

		Else If packet.GetData()[0] = ID_PONG Then

			' Peer or client. Response from a ping for an unconnected system.
			Local packetTime:Long, dataLength:Int
			Local curTime:Long = TRKRakNet.GetTime()
			If SizeOfRakNetTime = 8 Then
				packetTime =  Long Ptr(packet.GetData() + 1)[0]
			Else
				packetTime =  Int Ptr(packet.GetData() + 1)[0]
			End If
			
			dataLength = packet.GetLength() - 1 - SizeOfRakNetTime
			If peer2.IsLocalIP(packet.GetSystemAddress().ToString()) Then
				WriteStdout "ID_PONG from our own"
			Else
				WriteStdout "ID_PONG from"
			End If
			Print " " + packet.GetSystemAddress().ToString() + " on 'peer2'.~nPing is " + (curTime-packetTime)+ "~nData is " + dataLength + " bytes Long."
			Print "Was sent from GUID " +  packet.GetGuid().ToString()

			If dataLength > 0 Then
				Print "Data is " + String.FromCString(packet.GetData() + 1 + SizeOfRakNetTime)
			End If

			nextTest :+ 1
			' ProcessUnhandledPacket(packet, ID_PONG,interfaceType)
		End If
		peer2.DeallocatePacket(packet)
	End If

	Delay(30)
Wend


