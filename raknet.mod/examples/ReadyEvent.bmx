'
' Requires BaH.CEGUI
' Build as GUI.
'
'
SuperStrict

Framework BaH.Raknet
Import BRL.StandardIO
Import "example_helper.bmx"

AppTitle = "ReadyEvent"


New ReadyEvent.Run()



Type ReadyEvent Extends TExampleHelper

	Const NUM_PEERS:Int = 3
	
	Field readyEventPlugin:TRKReadyEvent[] = New TRKReadyEvent[NUM_PEERS]
	Field rakPeer:TRKRakPeerInterface[] = New TRKRakPeerInterface[NUM_PEERS]
	
	Const STAGE_NONE:Int = 0
	Const STAGE_SIGNAL_SYSTEM:Int = 1
	Const STAGE_UNSIGNAL_SYSTEM:Int = 2
	Const STAGE_CONNECT_SYSTEM:Int = 3
	Const STAGE_DISCONNECT_SYSTEM:Int = 4
	Const STAGE_FORCE_COMPLETE:Int = 5
	
	Field stage:Int = STAGE_NONE
	
	Method Init()
		Super.Init()
		
		For Local i:Int = 0 Until NUM_PEERS
			rakPeer[i] = TRKRakNetworkFactory.GetRakPeerInterface()
			readyEventPlugin[i] = New TRKReadyEvent
		Next


		AddMessage "This project tests and demonstrates the ready event plugin."
		AddMessage "It is used in a peer to peer environment to have a group of systems signal an event."
		AddMessage "It is useful for changing turns in a turn based game,"
		AddMessage "or for lobby systems where everyone has to set ready before the game starts"
		AddMessage "Difficulty: Beginner"
		AddMessage ""


		' Initialize the message handlers
		For Local peerIndex:Int= 0 Until NUM_PEERS
			rakPeer[peerIndex].AttachPlugin(readyEventPlugin[peerIndex])
			rakPeer[peerIndex].SetMaximumIncomingConnections(NUM_PEERS)
		Next

	
		' Initialize the peers
		For Local peerIndex:Int = 0 Until NUM_PEERS
			Local socketDescriptor:TRKSocketDescriptor = New TRKSocketDescriptor.Create(60000 + peerIndex)
			rakPeer[peerIndex].Startup(NUM_PEERS, 0, socketDescriptor)
		Next

		' Give the threads time To properly start
		Delay 200

	
		AddMessage "Peers initialized."

		AddMessage "'C' to connect a system"
		AddMessage "'D' to disconnect a system"
		AddMessage "'S' to signal a system"
		AddMessage "'U' to unsignal a system"
		AddMessage "'F' to force all systems to be completed (cannot be unset)"
		AddMessage "'Q' to quit"
		AddMessage "' ' to print wait statuses"

	
	End Method
	
	Method RenderExtra()
	End Method

	Method Update()
	
		If stage = STAGE_NONE Then
		
			If KeyHit(KEY_S) Then

				stage = STAGE_SIGNAL_SYSTEM
				RequestValue("Which system?", "Signal which system? 0 to " + (NUM_PEERS - 1))
			
			Else If KeyHit(KEY_U) Then

				stage = STAGE_UNSIGNAL_SYSTEM
				RequestValue("Which system?", "Unsignal which system? 0 to " + (NUM_PEERS - 1))

			Else If KeyHit(KEY_C) Then

				stage = STAGE_CONNECT_SYSTEM
				RequestValue("Which system?", "Connect to which system? 0 to " + (NUM_PEERS - 1))

			Else If KeyHit(KEY_D) Then

				stage = STAGE_DISCONNECT_SYSTEM
				RequestValue("Which system?", "Disconnect from which system? 0 to " + (NUM_PEERS - 1))

			Else If KeyHit(KEY_F) Then

				stage = STAGE_FORCE_COMPLETE
				RequestValue("Which system?", "Force complete which system? 0 to " + (NUM_PEERS - 1))

			Else If KeyHit(KEY_SPACE) Then

				Local sysAddr:TRKSystemAddress = New TRKSystemAddress.Create()
				sysAddr.SetBinaryAddress("127.0.0.1")

				
				'PrintConnections()
				
				For Local i:Int = 0 Until NUM_PEERS
			
					Local s:String = "System " + i + ", "
					If readyEventPlugin[i].IsEventSet(0) Then
						s:+ "Set=True, "
					Else
						s:+ "Set=False, "
					End If
			
					If readyEventPlugin[i].IsEventCompleted(0) Then
						s:+ "Completed=True"
					Else If readyEventPlugin[i].IsEventCompletionProcessing(0) Then
						s:+ "Completed=InProgress"
					Else
						s:+ "Completed=False"
					End If
					AddMessage s
			
					For Local j:Int = 0 Until NUM_PEERS
						If i <> j Then
			
							Local ress:Int
							sysAddr.SetPort(60000 + j)
							ress = readyEventPlugin[i].GetReadyStatus(0, sysAddr)
							s = "  Remote system " + j +", status = "
							
							Select ress
			
								Case RES_NOT_WAITING
									s:+ "RES_NOT_WAITING"
			
								Case RES_WAITING
									s:+ "RES_WAITING"
			
								Case RES_READY
									s:+ "RES_READY"
			
								Case RES_ALL_READY
									s:+ "RES_ALL_READY"
			
								Case RES_UNKNOWN_EVENT
									s:+ "RES_UNKNOWN_EVENT"
			
							End Select
							AddMessage s
						End If
					Next
				Next


			Else If KeyHit(KEY_Q) Then
				quit = True
				AddMessage "Quitting."
			End If

		End If
	

		For Local i:Int = 0 Until NUM_PEERS

		
			Local p:TRKPacket = rakPeer[i].Receive()
			If p Then

				Select p.GetData()[0]

					Case ID_NEW_INCOMING_CONNECTION
						readyEventPlugin[i].AddToWaitList(0, p.GetSystemAddress())
				
					Case ID_CONNECTION_REQUEST_ACCEPTED
						readyEventPlugin[i].AddToWaitList(0, p.GetSystemAddress())

					Case ID_READY_EVENT_ALL_SET
						AddMessage "Got ID_READY_EVENT_ALL_SET from " + p.GetSystemAddress().ToString()

				End Select

				rakPeer[i].DeallocatePacket(p)
			End If
		Next

		' Keep raknet threads responsive
		Delay 30
	
	End Method

	Method OnSubmit(value:String)
	
		Local sysIndex:Int = value.ToInt()
		
		If sysIndex < 0 Or sysIndex > NUM_PEERS - 1 Then
		
			AddMessage "Invalid Range"
		
		Else
		
			Select stage
				Case STAGE_SIGNAL_SYSTEM
					If readyEventPlugin[sysIndex].SetEvent(0, True) Then
						AddMessage "Set system " + sysIndex + " to signaled"
					Else
						AddMessage "Set system " + sysIndex + " to signaled FAILED"
					End If
					
				Case STAGE_UNSIGNAL_SYSTEM
					If readyEventPlugin[sysIndex].SetEvent(0, False) Then
						AddMessage"Set index " + sysIndex + " to unsignaled"
					Else
						AddMessage"Set index " + sysIndex + " to unsignaled FAILED"
					End If
				
				Case STAGE_CONNECT_SYSTEM

					rakPeer[sysIndex].Connect("127.0.0.1", 60000)
					AddMessage "Connecting system " + sysIndex + "."
				
				Case STAGE_DISCONNECT_SYSTEM

					rakPeer[sysIndex].Shutdown(100, 0)
					Local socketDescriptor:TRKSocketDescriptor = New TRKSocketDescriptor.Create(60000 + sysIndex)
					rakPeer[sysIndex].Startup(NUM_PEERS, 0, socketDescriptor)
					AddMessage "Restarting system " + sysIndex + "."

				
				Case STAGE_FORCE_COMPLETE

				If readyEventPlugin[sysIndex].ForceCompletion(0) Then
					AddMessage "Set system " + sysIndex + " to force complete"
				Else
					AddMessage "Set system " + sysIndex + " to force complete FAILED"
				End If
				
			End Select
		
		End If
	
		FlushKeys
		stage = STAGE_NONE
		
	End Method
	
	Method Finalize()
		For Local i:Int = 0 Until NUM_PEERS
			TRKRakNetworkFactory.DestroyRakPeerInterface(rakPeer[i])
		Next

	End Method
	
End Type

