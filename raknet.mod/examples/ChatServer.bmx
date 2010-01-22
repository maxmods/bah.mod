'
' Requires BaH.CEGUI
' Build as GUI.
'
'
SuperStrict

Framework BaH.Raknet
Import BRL.StandardIO
Import "example_helper.bmx"

AppTitle = "ChatServer"


New ChatServer.Run()



Type ChatServer Extends TExampleHelper

	Const STAGE_NONE:Int = 0
	Const STAGE_GET_SERVER_PORT:Int = 1
	Const STAGE_SET_BAN:Int = 2
	Const STAGE_GET_MESSAGE:Int = 3

	Field stage:Int = STAGE_NONE

	Field rss:TRKRakNetStatistics
	Field server:TRKRakPeerInterface = TRKRakNetworkFactory.GetRakPeerInterface()
	
	Field clientID:TRKSystemAddress = UNASSIGNED_SYSTEM_ADDRESS

	Field p:TRKPacket
	Field packetIdentifier:Int
	
	'Field clientPort:Int
	Field serverPort:Int
	Field ip:String

	Method Init()
		Super.Init()
		
		'Local i:Int = server.GetNumberOfAddresses()
		server.SetIncomingPassword("Rumpelstiltskin")


		AddMessage "This is a sample implementation of a text based chat server."
		AddMessage "Connect to the project 'Chat Example Client'."
		AddMessage "Difficulty: Beginner"


		stage = STAGE_GET_SERVER_PORT
		RequestValue("Enter server port number", "Enter the server port to listen on : (10000)")

	End Method
	
	Method RenderExtra()
	End Method

	Method Update()

		' Get a packet from either the server or the client
		
		p = server.Receive()
		
		
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

			Case ID_NEW_INCOMING_CONNECTION
				' Somebody connected.  We have their IP now
				AddMessage "ID_NEW_INCOMING_CONNECTION from " + p.GetSystemAddress().ToString() + " with GUID " +  p.GetGuid().ToString()
				clientID = p.GetSystemAddress()

			Case ID_MODIFIED_PACKET
				' Cheater!
				AddMessage "ID_MODIFIED_PACKET"
		
			Case ID_CONNECTION_LOST
				' Couldn't deliver a reliable packet - i.e. the other system was abnormally
				' terminated
				AddMessage "ID_CONNECTION_LOST"
		
			Default

				' The server knows the static data of all clients, so we can prefix the message
				' With the name data
				Local value:String = String.FromUTF8String(p.GetData())
				AddMessage value

				' Relay the message.  We prefix the name For other clients.  This demonstrates
				' That messages can be changed on the server before being broadcast
				' Sending is the same as before
				Local message:Byte Ptr = value.ToUTF8String()
				server.Send(message, String.FromCString(message).length + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.GetSystemAddress(), True)
				MemFree(message)


		End Select
		
		
		' We're done with the packet
		server.DeallocatePacket(p)
		
	End Method
	
	Method OnSubmit(value:String)
		Select stage
			Case STAGE_GET_SERVER_PORT
				
				serverPort = value.ToInt()
				
				If Not serverPort Then
					serverPort = 10000
				End If
				
				AddMessage "Starting server."

				' Starting the server is very simple.  2 players allowed.
				' 0 means we don't care about a connectionValidationInteger, and false
				' for low priority threads
				Local socketDescriptor:TRKSocketDescriptor = New TRKSocketDescriptor.Create(serverPort)
				Local b:Int = server.Startup(32, 30, socketDescriptor)
				server.SetMaximumIncomingConnections(2)
				
				If b Then
					AddMessage "Server started, waiting for connections."
				Else
					AddMessage "Server failed to start.  Terminating."
					quit = True
				End If
				
				server.SetOccasionalPing(True)
			
				AddMessage "My IP is " + server.GetLocalIP(0)
				AddMessage "My GUID is " + server.GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS).ToString()
				AddMessage "'quit' to quit. 'stat' to show stats. 'ping' to ping."
				AddMessage "'ban' to ban an IP from connecting. 'kick to kick the first connected player. Type to talk."
				
				ShowChatWindow()
			
			Case STAGE_SET_BAN
			
				If value Then
					server.AddToBanList(value)
					AddMessage "IP " + value + " added to ban list."
				End If

				ShowChatWindow()

			Case STAGE_GET_MESSAGE
			
				Select value
					Case "quit"
						AddMessage "Quitting."
						quit = True
					
						Return
					Case "stat"
					
						rss = server.GetStatistics(server.GetSystemAddressFromIndex(0))
						Local s:String = rss.ToVerboseString(2)
						s = s.Replace("~t", "    ")
						
						Local lines:String[] = s.Split("~n")
						For Local i:Int = 0 Until lines.length
							AddMessage lines[i]
						Next
						
						AddMessage "Ping " + server.GetAveragePing(server.GetSystemAddressFromIndex(0))
					
						ShowChatWindow()

						Return
					Case "ban"
					
						stage = STAGE_SET_BAN
						RequestValue("Ban IP", "Enter IP To ban.  You can use * as a wildcard")
						

						Return
					Case "kick"
					
						server.CloseConnection(clientID, True, 0)

						ShowChatWindow()

						Return
					Case "ping"
					
						server.Ping(clientID)

						ShowChatWindow()

						Return
				End Select
				
				
				value = "Server: " + value
				' message is the data To send
				' strlen(message)+1 is To send the Null terminator
				' HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
				' RELIABLE_ORDERED means make sure the message arrives in the right order
				Local message:Byte Ptr = value.ToCString()
				server.Send(message, value.length + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, True)
				MemFree(message)
		
				ShowChatWindow()

		End Select
		
	End Method
	
	Method ShowChatWindow()
		stage = STAGE_GET_MESSAGE
		RequestValue("Enter message", "Enter chat message, or 'quit' :")
	End Method
	
	Method Finalize()
	
		server.Shutdown(300)
		' We're done with the network
		TRKRakNetworkFactory.DestroyRakPeerInterface(server)
		
	End Method

End Type

