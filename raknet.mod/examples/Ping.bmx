'
' Requires BaH.CEGUI
' Build as GUI.
'
'
SuperStrict

Framework BaH.Raknet
Import BRL.StandardIO
Import "example_helper.bmx"

AppTitle = "Ping"


New Ping.Run()



Type Ping Extends TExampleHelper

	Const STAGE_NONE:Int = 0
	Const STAGE_GET_SERVER_PORT:Int = 1
	Const STAGE_GET_OFFLINE_DATA:Int = 2
	Const STAGE_GET_PING_IP:Int = 3
	Const STAGE_GET_PING_PORT:Int = 4

	Field stage:Int = STAGE_NONE

	' Pointers to the interfaces of our server and client.
	' Note we can easily have both in the same program
	Field client:TRKRakPeerInterface = TRKRakNetworkFactory.GetRakPeerInterface()
	Field server:TRKRakPeerInterface = TRKRakNetworkFactory.GetRakPeerInterface()

	' Holds packets
	Field p:TRKPacket

	' Record the first client that connects to us so we can pass it to the ping function
	Field clientID:TRKSystemAddress = UNASSIGNED_SYSTEM_ADDRESS
	Field packetFromServer:Int
	Field port:Int

	Field ip:String
	Field serverPort:Int

	Method Init()
		Super.Init()

		AddMessage "This is a sample of how to send offline pings and get offline ping"
		AddMessage "responses."
		AddMessage "Difficulty: Beginner"


		stage = STAGE_GET_SERVER_PORT
		RequestValue("Enter server port", "Enter the server port to listen on : (60000)")

	End Method
	
	Method RenderExtra()
	End Method

	Method Update()
	
		If stage = STAGE_NONE And KeyHit(KEY_SPACE) Then
			' Get our input
			stage = STAGE_GET_PING_IP
			RequestValue("Enter IP", "Enter IP to ping : (127.0.0.1)")

		End If
	
	
		' Get a packet from either the server or the client
		p = server.Receive()
	
		If Not p Then
			packetFromServer = False
			p = client.Receive()
		Else
			packetFromServer = True
		End If
	
		If Not p Then
			Return
		End If
	
	
		' Check if this is a network message packet
		Select p.GetData()[0]
	
			Case ID_PONG
				Local time:Long
				Local dataLength:Int
				Local pong:TRKBitStream = New TRKBitStream.CreateFromData( p.GetData()+1, SizeOfRakNetTime, False)
				pong.ReadTime(time)

				dataLength = p.GetLength() - 1 - SizeOfRakNetTime
				AddMessage "ID_PONG from SystemAddress:" + p.GetSystemAddress().GetBinaryAddress() + ":" + p.GetSystemAddress().GetPort() + "."
				AddMessage "Time is " + time
				AddMessage "Ping is " + (TRKRakNet.GetTime() - time)
				AddMessage "Data is " + dataLength + " bytes long."
				If dataLength Then
					AddMessage "Data is " + String.FromCString(p.GetData() + 1 + SizeOfRakNetTime)
				End If
				
			
			' In this sample since the client is not running a game we can save CPU cycles by
			' Stopping the network threads after receiving the pong.
			'client.Shutdown(100)
		End Select
	
		' We're done with the packet
		If packetFromServer Then
			server.DeallocatePacket(p)
		Else
			client.DeallocatePacket(p)
		End If
		
	End Method
	
	Method Finalize()
		' We're done with the network
		TRKRakNetworkFactory.DestroyRakPeerInterface(server)
		TRKRakNetworkFactory.DestroyRakPeerInterface(client)
	End Method
	
	Method OnSubmit(value:String)
		Select stage
			Case STAGE_GET_SERVER_PORT
				
				port = value.ToInt()
				If Not port Then
					port = 60000
				End If

				stage = STAGE_GET_OFFLINE_DATA
				RequestValue("Enter data", "Enter offline ping response data~n(for return by a LAN discovery for example)")
				
			Case STAGE_GET_OFFLINE_DATA
			
				If value Then
					server.SetOfflinePingResponse(value)
				End If
				
				AddMessage "Starting server."
				
	
				
				' The server has to be started to respond to pings.
				Local socketDescriptor:TRKSocketDescriptor = New TRKSocketDescriptor.Create(port)
				Local b:Int = server.Startup(2, 30, socketDescriptor)
				server.SetMaximumIncomingConnections(2)
				If b Then
					AddMessage "Server started, waiting for connections."
				Else
					AddMessage "Server failed to start.  Terminating."
					quit = True
				End If
	
				socketDescriptor.SetPort(0)
				client.Startup(1, 30, socketDescriptor)
				
				stage = STAGE_NONE
				AddMessage "Press SPACE to send a ping from the client."
			
			Case STAGE_GET_PING_IP

				ip = value
				If Not ip Then
					ip = "127.0.0.1"
				End If
			
				stage = STAGE_GET_PING_PORT
				RequestValue("Enter port", "Enter the port to ping : (60000)")
			
			Case STAGE_GET_PING_PORT

				stage = STAGE_NONE

				serverPort = value.ToInt()
				If Not serverPort Then
					serverPort = 60000
				End If
	
				client.PingHost(ip, serverPort, False)
	
				AddMessage "Pinging"

		End Select
		
		FlushKeys
	End Method
	
End Type


