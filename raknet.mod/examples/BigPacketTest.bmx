Framework BaH.Raknet
Import brl.StandardIO
Import brl.retro
Import "example_helper.bmx"

New BigPacketTest.Run()

Type BigPacketTest Extends TExampleHelper
	'Stages
	Const STAGE_NONE:Int = 0
	Const STAGE_GET_IP:Int = 1

	Const BIG_PACKET_SIZE:Int = 20739000

	Field sentPacket:Int = False
	Field text:Byte[BIG_PACKET_SIZE]
	Field nextStatTime:Int
	Field stage:Int
	Field peer1:TRKRakPeerInterface, peer2:TRKRakPeerInterface
	Field ip:String
	
	Field start:Int, stop:Int
	
	Method Init()
		Super.Init()
		
		nextStatTime = TRKRakNet.GetTime() + 1000
		AddMessage "This is a test I use to test the packet splitting capabilities of RakNet"
		AddMessage "All it does is send a large block of data to the feedback loop"
		AddMessage "Difficulty: Beginner"

		AddMessage "Server or Client? : Enter 's' to run as sender, 'r' to run as receiver, SPACE to run local."

		While True
			Cls
			DrawMessages()
			Flip
			
			Local s:Int = WaitKey()
		
			Select s
				Case KEY_R
					peer1 = TRKRakNetworkFactory.GetRakPeerInterface()
					AddMessage "Working as receiver"
					Exit
				Case KEY_S
					peer2 = TRKRakNetworkFactory.GetRakPeerInterface()
					AddMessage "Working as sender"
					stage = STAGE_GET_IP
					RequestValue("Enter remote IP", "Enter remote IP or leave blank for 127.0.0.1 : ")
					Exit
				Case KEY_SPACE
					peer1 = TRKRakNetworkFactory.GetRakPeerInterface()
					peer2 = TRKRakNetworkFactory.GetRakPeerInterface()
					ip = "127.0.0.1"
					Exit
			End Select
		
		Wend

		If peer1 Then
			peer1.SetMaximumIncomingConnections(1)
			Local socketDescriptor:TRKSocketDescriptor = New TRKSocketDescriptor.Create(10000)
			'peer1.SetMTUSize(1492)
			peer1.Startup(1, 0, socketDescriptor)
			peer1.SetSplitMessageProgressInterval(100) ' Get ID_DOWNLOAD_PROGRESS notifications
		End If
		
		If peer2 And ip Then
			StartPeer2()
		Else
			start = TRKRakNet.GetTime()
		End If
			

	End Method
	
	Method RenderExtra()
	End Method
	
	Method Finalize()
		stop = TRKRakNet.GetTime()
		Local seconds:Double = (stop - start) / 1000.0
		If peer2
			Local rssSender:TRKRakNetStatistics = peer2.GetStatistics(peer2.GetSystemAddressFromIndex(0))
			Print rssSender.ToVerboseString(2)
			Print Int(BIG_PACKET_SIZE / seconds) + " bytes per second transfered."
		End If

		TRKRakNetworkFactory.DestroyRakPeerInterface(peer1)
		TRKRakNetworkFactory.DestroyRakPeerInterface(peer2)
	End Method
	
	Method Update()
		
		If peer2 Then
			peer2.DeallocatePacket(peer2.Receive())
			If Not sentPacket And Not peer2.GetSystemAddressFromIndex(0).Equals(UNASSIGNED_SYSTEM_ADDRESS) Then
				sentPacket = True
				For Local i:Int = 0 Until BIG_PACKET_SIZE
					text[i] = i Mod 256
				Next
				text[0] = 255
				
				peer2.Send(text, BIG_PACKET_SIZE, HIGH_PRIORITY, RELIABLE_ORDERED, 0, peer2.GetSystemAddressFromIndex(0), False)
				'Keep the stat from updating until the messages move to the thread or it quits right away
				nextStatTime = TRKRakNet.GetTime() + 1000
			End If
		End If
		
		If peer1 Then
			Local packet:TRKPacket = peer1.Receive()
			While packet
				If packet.GetData()[0] = ID_DOWNLOAD_PROGRESS Then
					Local progressBS:TRKBitStream = TRKBitStream.CreateFromData(packet.GetData(), packet.GetLength(), False)
					progressBS.IgnoreBits(8) ' ID_DOWNLOAD_PROGRESS
					
					Local progress:Int
					Local total:Int
					Local partLength:Int
					Local rss:TRKRakNetStatistics = peer1.GetStatistics(peer1.GetSystemAddressFromIndex(0))
					Local data:String
					progressBS.ReadInt(progress)
					progressBS.ReadInt(total)
					'progressBS.ReadInt(partLength)
					data = ProgressBS.ReadString()
					AddMessage "Progress: msgID=" + Asc(data[0]) + " Progress " + progress + "/" + total + " Partsize=" + partLength + " Full=" + rss.bandwidthExceeded()
				ElseIf packet.GetData()[0] >= ID_USER_PACKET_ENUM
					If packet.GetData()[0] = 255 Then
						Local dataValid:Int = True
						Local pdata:Byte Ptr = packet.GetData()
						For i:Int = 1 Until BIG_PACKET_SIZE
							If pdata[i] <> i Mod 256 Then
								dataValid = False
								Exit
							End If
						Next
						
						If dataValid Then
							Local s:String = "Test succeeded. " + packet.GetLength() + " bytes."
							AddMessage s
							Print s
						Else
							AddMessage "Test failed. " + packet.GetLength() + " bytes."
						End If
					Else
						AddMessage "Unkown packet " + packet.GetData()[0] + ": Test failed. " + packet.GetLength() + " bytes."
					End If
					quit = True
				End If
				peer1.DeallocatePacket(packet)
				packet = peer1.Receive()
			Wend
		End If
		
		If TRKRakNet.GetTime() > nextStatTime Then
			nextStatTime = TRKRakNet.GetTime() + 1000
			Local rssSender:TRKRakNetStatistics
			Local rssReceiver:TRKRakNetStatistics
			If peer2 Then
				rssSender = peer2.GetStatistics(peer2.GetSystemAddressFromIndex(0))
				If rssSender Then
					'AddMessage "Snd: " + rssSender.messageSendBuffer(HIGH_PRIORITY) + " waiting. " + rssSender.messagesOnResendQueue() + " waiting on ack. Got " + rssSender.acknowlegementsReceived() + " acks. KBPS=" + (rssSender.bitsPerSecond() / 1000.0) + " Ploss=" + 100.0 * rssSender.messagesTotalBitsResent() / Float(Rsssender.totalBitsSent()) + " Full=" + rssSender.bandwidthExceeded()

					If sentPacket And rssSender.messageSendBuffer(HIGH_PRIORITY) = 0 And rsssender.messagesOnResendQueue() = 0 And peer1 = Null Then
						rss = peer2.GetStatistics(peer2.GetSystemAddressFromIndex(0))
						AddMessage rss.ToVerboseString(2)
						AddMessage "Sender quitting with no messages on resend queue."
						quit = True
					End If
				End If
			End If
			
			If peer1 Then
				rssReceiver = peer1.GetStatistics(peer1.GetSystemAddressFromIndex(0))
				If rssReceiver
					AddMessage "Receiver: " + rssReceiver.acknowlegementsPending() + " acks waiting."
				End If
			End If
			Delay(10)
		End If

	End Method
	
	Method OnSubmit(value:String)
		Select stage
			Case STAGE_GET_IP
				If Not value Then
					value = "127.0.0.1" 'Added for default value.
				End If
				ip = value

				StartPeer2()
				stage = STAGE_NONE

				Delay(500)

				start = TRKRakNet.GetTime()
		End Select

	End Method
	
	Method StartPeer2()
		socketDescriptor = New TRKSocketDescriptor.Create()
		'peer2.SetMTUSize(1492)
		peer2.Startup(1, 0, socketDescriptor)
		peer2.Connect(ip, 10000)
	End Method
	
End Type
