'
' Build as Console.
'
'
SuperStrict

Framework BaH.Raknet
Import BRL.StandardIO

' The message filter parses all incoming messages and only allows messages of a certain type
Local messageFilter:TRKMessageFilter = New TRKMessageFilter
Local peer1:TRKRakPeerInterface, peer2:TRKRakPeerInterface

Local packet:TRKPacket
peer1 = TRKRakNetworkFactory.GetRakPeerInterface()
peer2 = TRKRakNetworkFactory.GetRakPeerInterface()

' Set up the filter rules.
' All new connections go to filter 0
messageFilter.SetAutoAddNewConnectionsToFilter(0)
' Filter 0 only allows ID_USER_PACKET_ENUM
messageFilter.SetAllowMessageID(True, ID_USER_PACKET_ENUM, ID_USER_PACKET_ENUM, 0)
' Filter 1 only allows ID_USER_PACKET_ENUM
messageFilter.SetAllowMessageID(True, ID_USER_PACKET_ENUM + 1, ID_USER_PACKET_ENUM + 1, 1)
' Use the filter on peer 1.
peer1.AttachPlugin(messageFilter)

' Connect the systems to each other
Local socketDescriptor:TRKSocketDescriptor = New TRKSocketDescriptor.Create(60000)
peer1.Startup(1, 0, socketDescriptor)
peer1.SetMaximumIncomingConnections(1)

socketDescriptor.SetPort(60001)
peer2.Startup(1, 0, socketDescriptor)
peer2.Connect("127.0.0.1", 60000)

' Wait for the connection to complete
While True
	packet = peer1.Receive()
	If packet Then
		If packet.GetData()[0] = ID_NEW_INCOMING_CONNECTION Then
			Print "Connected."
			peer1.DeallocatePacket(packet)
			Exit
		Else
			Print "Problem. Unexpected ID : " + packet.GetData()[0]
			TRKRakNetworkFactory.DestroyRakPeerInterface(peer1)
			TRKRakNetworkFactory.DestroyRakPeerInterface(peer2)
			End
		End If
		peer1.DeallocatePacket(packet)
	End If
	Delay 100
Wend

Print "Peer 2 sending ID_USER_PACKET_ENUM+1 and ID_USER_PACKET_ENUM"

' Have peer 2 send a disallowed message, then the allowed message.
Local message:Byte = ID_USER_PACKET_ENUM+1
peer2.Send(Varptr message, 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, True)

' Allowed message
message = ID_USER_PACKET_ENUM
peer2.Send(Varptr message, 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, True)

Delay(1000)

' Print out the messages that peer 1 got
Print "We should get ID_USER_PACKET_ENUM and not ID_USER_PACKET_ENUM+1."
packet = peer1.Receive()
While packet

	Select packet.GetData()[0]
		Case ID_USER_PACKET_ENUM
			Print "ID_USER_PACKET_ENUM"
			Print "User switched to group 1"
			' Switch the sender to group 1 now so that ID_USER_PACKET_ENUM+1 is allowed.
			messageFilter.SetSystemFilterSet(packet.GetSystemAddress(), 1)
		Case ID_USER_PACKET_ENUM+1
			Print "ID_USER_PACKET_ENUM+1"
	End Select
	peer1.DeallocatePacket(packet)
	Delay(10)
	packet = peer1.Receive()
Wend

' Have peer 2 send the messages again.
message=ID_USER_PACKET_ENUM+1
peer2.Send(Varptr message, 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, True)
message=ID_USER_PACKET_ENUM
peer2.Send(Varptr message, 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, True)

Delay(1000)

Print "We should now NOT get ID_USER_PACKET_ENUM and should get ID_USER_PACKET_ENUM+1"
packet = peer1.Receive()
While packet

	Select packet.GetData()[0]
		Case ID_USER_PACKET_ENUM
			Print "ID_USER_PACKET_ENUM"
		Case ID_USER_PACKET_ENUM+1
			Print "ID_USER_PACKET_ENUM+1"
	End Select
	peer1.DeallocatePacket(packet)
	packet = peer1.Receive()
	Delay(10)
Wend

Print "Done."
TRKRakNetworkFactory.DestroyRakPeerInterface(peer1)
TRKRakNetworkFactory.DestroyRakPeerInterface(peer2)
