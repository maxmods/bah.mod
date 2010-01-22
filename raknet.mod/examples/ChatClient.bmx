'
' Requires BaH.CEGUI
' Build as GUI.
'
'
SuperStrict

Framework BaH.Raknet
Import BRL.StandardIO
Import "example_helper.bmx"

AppTitle = "ChatClient"


New ChatClient.Run()

Type ChatClient Extends TExampleHelper

	Const STAGE_NONE:Int = 0
	Const STAGE_GET_CLIENT_PORT:Int = 1
	Const STAGE_GET_IP:Int = 2
	Const STAGE_GET_CONNECT_PORT:Int = 3
	Const STAGE_GET_MESSAGE:Int = 4

	Field stage:Int = STAGE_NONE

	Field rss:TRKRakNetStatistics
	Field client:TRKRakPeerInterface = TRKRakNetworkFactory.GetRakPeerInterface()

	Field p:TRKPacket
	Field packetIdentifier:Int
	
	Field clientPort:Int
	Field serverPort:Int
	Field ip:String

	Method Init()
		Super.Init()

		AddMessage "This is a sample implementation of a text based chat client."
		AddMessage "Connect to the project 'Chat Example Server'."
		AddMessage "Difficulty: Beginner"


		stage = STAGE_GET_CLIENT_PORT
		RequestValue("Enter client port number", "Enter the client port to listen on : (0)")

	End Method
	
	Method RenderExtra()
	End Method

	Method Update()

		' Get a packet from either the server or the client
		
		p = client.Receive()
		
		
		If Not p Then
			Return ' Didn't get any packets
		End If
		
		' We got a packet, get the identifier with our handy function
		packetIdentifier = p.GetPacketIdentifier()
		
		' Check if this is a network message packet
		Select packetIdentifier
		
			Case ID_DISCONNECTION_NOTIFICATION
				  ' Connection lost normally
				AddMessage "ID_DISCONNECTION_NOTIFICATION"

			Case ID_ALREADY_CONNECTED
				' Connection lost normally
				AddMessage "ID_ALREADY_CONNECTED"

			Case ID_REMOTE_DISCONNECTION_NOTIFICATION ' Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				AddMessage "ID_REMOTE_DISCONNECTION_NOTIFICATION"

			Case ID_REMOTE_CONNECTION_LOST ' Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				AddMessage "ID_REMOTE_CONNECTION_LOST"

			Case ID_REMOTE_NEW_INCOMING_CONNECTION ' Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
				AddMessage "ID_REMOTE_NEW_INCOMING_CONNECTION"

			Case ID_CONNECTION_BANNED ' Banned from this server
				AddMessage "We are banned from this server."

			Case ID_CONNECTION_ATTEMPT_FAILED
				AddMessage "Connection attempt failed"

			Case ID_NO_FREE_INCOMING_CONNECTIONS
				 ' Sorry, the server is full.  I don't do anything here but
				' A real app should tell the user
				AddMessage "ID_NO_FREE_INCOMING_CONNECTIONS"

			Case ID_MODIFIED_PACKET
				' Cheater!
				AddMessage "ID_MODIFIED_PACKET"
		
			Case ID_INVALID_PASSWORD
				AddMessage "ID_INVALID_PASSWORD"
		
			Case ID_CONNECTION_LOST
				' Couldn't deliver a reliable packet - i.e. the other system was abnormally
				' terminated
				AddMessage "ID_CONNECTION_LOST"
		
			Case ID_CONNECTION_REQUEST_ACCEPTED
				' This tells the client they have connected
				AddMessage "ID_CONNECTION_REQUEST_ACCEPTED to " + p.GetSystemAddress().ToString() + " with GUID " +  p.GetGuid().ToString()

			Default
				' It's a client, so just show the message
				AddMessage String.FromUTF8String(p.GetData())
		End Select
		
		
		' We're done with the packet
		client.DeallocatePacket(p)

		
	End Method
	
	Method OnSubmit(value:String)
		Select stage
			Case STAGE_GET_CLIENT_PORT
				
				clientPort = value.ToInt()

				stage = STAGE_GET_IP
				RequestValue("Enter IP Address", "Enter IP to connect to : (127.0.0.1)")
				
			Case STAGE_GET_IP
				If Not value Then
					value = "127.0.0.1"
				End If
				
				ip = value

				stage = STAGE_GET_CONNECT_PORT
				RequestValue("Enter connect port", "Enter the port to connect to : (10000)")
				
			Case STAGE_GET_CONNECT_PORT

				serverPort = value.ToInt()
				
				If Not serverPort Then
					serverPort = 10000
				End If

				' Connecting the client is very simple.  0 means we don't care about
				' a connectionValidationInteger, and false for low priority threads
				Local socketDescriptor:TRKSocketDescriptor = New TRKSocketDescriptor.Create(clientPort)
				client.Startup(1, 30, socketDescriptor)
				client.SetOccasionalPing(True)
				Local b:Int = client.Connect(ip, serverPort, "Rumpelstiltskin")	
			
				If b Then
					AddMessage "Attempting connection"
				Else
					AddMessage "Bad connection attempt.  Terminating."
					quit = True
				End If
			
				AddMessage "My IP is " + client.GetLocalIP(0)
				AddMessage "My GUID is " + client.GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS).ToString()
				AddMessage "'quit' to quit. 'stat' to show stats. 'ping' to ping. 'disconnect' to disconnect. Type to talk."
				
				ShowChatWindow()
			
			Case STAGE_GET_MESSAGE
			
				Select value
					Case "quit"
						AddMessage "Quitting."
						quit = True
					
						Return
					Case "stat"

						rss = client.GetStatistics(client.GetSystemAddressFromIndex(0))
						Local s:String = rss.ToVerboseString(2)
						s = s.Replace("~t", "    ")
						
						Local lines:String[] = s.Split("~n")
						For Local i:Int = 0 Until lines.length
							AddMessage lines[i]
						Next
						
						AddMessage "Ping " + client.GetAveragePing(client.GetSystemAddressFromIndex(0))
					
						ShowChatWindow()

						Return
					Case "disconnect"
					
						client.CloseConnection(client.GetSystemAddressFromIndex(0), True, 0)
						AddMessage "Disconnecting."

						ShowChatWindow()

						Return
					Case "ping"
					
						If Not client.GetSystemAddressFromIndex(0).Equals(UNASSIGNED_SYSTEM_ADDRESS) Then
							client.Ping(client.GetSystemAddressFromIndex(0))
						End If


						ShowChatWindow()

						Return
				End Select

				' message is the data To send
				' strlen(message)+1 is To send the Null terminator
				' HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
				' RELIABLE_ORDERED means make sure the message arrives in the right order
				Local message:Byte Ptr = value.ToUTF8String()
				client.Send(message, String.FromCString(message).length + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, True)
				MemFree(message)
		
				ShowChatWindow()

		End Select
		
	End Method
	
	Method ShowChatWindow()
		stage = STAGE_GET_MESSAGE
		RequestValue("Enter message", "Enter chat message, or 'quit' :")
	End Method
	
	Method Finalize()
	
		client.Shutdown(300)
		' We're done with the network
		TRKRakNetworkFactory.DestroyRakPeerInterface(client)
		
	End Method

End Type

