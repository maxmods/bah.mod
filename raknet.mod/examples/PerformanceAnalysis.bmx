'
' Build as Console.
'
'
SuperStrict

Framework BaH.Raknet
Import BRL.StandardIO


Print "NOTE : Current implementations run out of memory before speed.~n"


Global peer1:TRKRakPeerInterface = TRKRakNetworkFactory.GetRakPeerInterface()
Global peer2:TRKRakPeerInterface = TRKRakNetworkFactory.GetRakPeerInterface()
peer1.SetMaximumIncomingConnections(1)
peer2.SetMaximumIncomingConnections(1)


Print "Testing UNRELIABLE with 100 byte messages."
DoSpeedTest(UNRELIABLE, 100)

Print "Testing UNRELIABLE with 400 byte messages."
DoSpeedTest(UNRELIABLE, 400)

Print "Testing RELIABLE with 100 byte messages."
DoSpeedTest(RELIABLE, 100)

Print "Testing RELIABLE with 400 byte messages."
DoSpeedTest(RELIABLE, 400)

Print "Testing RELIABLE_ORDERED with 100 byte messages."
DoSpeedTest(RELIABLE_ORDERED, 100)

Print "Testing RELIABLE_ORDERED with 400 byte messages."
DoSpeedTest(RELIABLE_ORDERED, 400)


TRKRakNetworkFactory.DestroyRakPeerInterface(peer1)

TRKRakNetworkFactory.DestroyRakPeerInterface(peer2)


End


Function DoSpeedTest(packetReliability:Int, sendSize:Int)

	Local rss1:TRKRakNetStatistics, rss2:TRKRakNetStatistics
	Local lastMessagesPerSecond1:Int, lastMessagesPerSecond2:Int
	Local riseCount1:Int, riseCount2:Int

	Local p:TRKPacket
	Local dummyData:Byte[400]
	
	Local socketDescriptor:TRKSocketDescriptor = New TRKSocketDescriptor.Create(1234)
	socketDescriptor.SetPort(1235)
	peer2.Startup(1, 0, socketDescriptor)
	peer1.Connect("127.0.0.1", 1235)
	peer2.Connect("127.0.0.1", 1234)
	Delay(500)
	
	Local messagesPerSecond:Int = 1000
	Local lastTime:Long = TRKRakNet.GetTime()
	Local currTime:Long = lastTime
	Local lastPrint:Int
	
	Print "Messages per second: "
	
	riseCount1 = 0
	riseCount2 = 0
	lastMessagesPerSecond1 = 0
	lastMessagesPerSecond2 = 0
	While riseCount1 < 100 And riseCount2 < 100
	
		rss1 = peer1.GetStatistics(peer1.GetSystemAddressFromIndex(0))
		rss2 = peer2.GetStatistics(peer2.GetSystemAddressFromIndex(0))
		If Not rss1 Then
			Print "Peer 1 can't get statistics from peer 2!"
			Exit
		End If
		
		If Not rss2 Then
			Print "Peer 2 can't get statistics from peer 1!"
			Exit
		End If
		
		If rss1.messageSendBuffer(HIGH_PRIORITY) > lastMessagesPerSecond1 Then
			riseCount1 :+ 1
			lastMessagesPerSecond1 = rss1.messageSendBuffer(HIGH_PRIORITY)
		Else
			riseCount1 = 0
		End If
		
		If rss2.messageSendBuffer(HIGH_PRIORITY) > lastMessagesPerSecond1 Then
			riseCount2 :+ 1
			lastMessagesPerSecond1 = rss1.messageSendBuffer(HIGH_PRIORITY)
		Else
			riseCount1 = 0
		End If

	
		currTime = TRKRakNet.GetTime()
		lastPrint = messagesPerSecond / 2000
		messagesPerSecond :+ 2 * (currTime - lastTime)
		If messagesPerSecond / 2000 <> lastPrint Then
			WriteStdout messagesPerSecond + " "
		End If
		lastTime = currTime
		For Local i:Int = 0 Until messagesPerSecond
			peer1.Send(dummyData, sendSize, HIGH_PRIORITY, packetReliability, 0, UNASSIGNED_SYSTEM_ADDRESS, True)
			peer2.Send(dummyData, sendSize, HIGH_PRIORITY, packetReliability, 0, UNASSIGNED_SYSTEM_ADDRESS, True)
		Next
	
		p = peer1.Receive()
		While p
			peer1.DeallocatePacket(p)
			p = peer1.Receive()
		Wend
		
		p = peer2.Receive()
		While p
			peer2.DeallocatePacket(p)
			p = peer2.Receive()
		Wend
		Delay(0)

	Wend


	' This means that messages were added faster than they were acknowledged
	Print "~nOutgoing message buffer not cleared at " + messagesPerSecond + " messages per second"
	peer1.Shutdown(500)
	peer2.Shutdown(500)
	

End Function

