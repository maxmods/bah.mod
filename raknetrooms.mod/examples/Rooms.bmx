SuperStrict

Framework BaH.RaknetRooms
Import BRL.StandardIO
Import "example_helper.bmx"

AppTitle = "ReadyEvent"


Global roomsRef:Rooms = New Rooms
roomsRef.Run()



Type Rooms Extends TExampleHelper

	Field client:TRKRakPeerInterface
	Field server:TRKRakPeerInterface
	
	Field roomsPluginClient:TRKRoomsPlugin = New TRKRoomsPlugin
	Field roomsPluginServer:TRKRoomsPlugin = New TRKRoomsPlugin
	
	Field profanityFilter:TRKProfanityFilter = New TRKProfanityFilter
	
	Field sampleCallbacks:TSampleCallbacks = New TSampleCallbacks

	Field packetIdentifier:Int

	Const GAME_IDENTIFIER:String = "My Game"
	Const DEFAULT_ROOM_NAME:String = "My Room"


	Const STAGE_NONE:Int = 0
	
	Const STAGE_USERNAME:Int = 1
	Const STAGE_ROOMNAME:Int = 2
	Const STAGE_TARGETNAME:Int = 99
	
	Const STAGE_CREATEROOM:Int = 3
	Const STAGE_ENTERROOM:Int = 4
	Const STAGE_JOINBYFILTER:Int = 5
	Const STAGE_LEAVEROOM:Int = 6
	Const STAGE_GETINVITESTOPARTICIPANT:Int = 7
	Const STAGE_SENDINVITE:Int = 8
	Const STAGE_ACCEPTINVITE:Int = 9
	Const STAGE_STARTSPECTATING:Int = 10
	Const STAGE_STOPSPECTATING:Int = 11
	Const STAGE_GRANTMODERATOR:Int = 12
	Const STAGE_CHANGESLOTCOUNTS:Int = 13
	Const STAGE_SETCUSTOMROOMPROPERTIES:Int = 14
	Const STAGE_CHANGEROOMNAME:Int = 15
	Const STAGE_SETHIDDENFROMSEARCHES:Int = 16
	Const STAGE_SETDESTROYONMODERATORLEAVE:Int = 17
	Const STAGE_SETREADYSTATUS:Int = 18
	Const STAGE_GETREADYSTATUS:Int = 19
	Const STAGE_SETROOMLOCKSTATE:Int = 20
	Const STAGE_GETROOMLOCKSTATE:Int = 21
	Const STAGE_AREALLMEMBERSREADY:Int = 22
	Const STAGE_KICKMEMBER:Int = 23
	Const STAGE_UNBANMEMBER:Int = 24
	Const STAGE_GETBANREASON:Int = 25
	Const STAGE_ADDUSERTOQUICKJOIN:Int = 26
	Const STAGE_REMOVEUSERFROMQUICKJOIN:Int = 27
	Const STAGE_ISINQUICKJOIN:Int = 28
	Const STAGE_SEARCHBYFILTER:Int = 29
	Const STAGE_CHANGEHANDLE:Int = 30
	Const STAGE_ROOMCHAT:Int = 31
	Const STAGE_GETROOMPROPERTIES:Int = 32

	
	Field stage:Int = STAGE_NONE
	
	Field stageIdx:Int = 0
	Field stages:Int[]
	
	Field func:TRKRoomsPluginFunc
	Field user:Int = 0
	Field roomName:String = ""
	Field targetName:String = ""

	Method Init()
		Super.Init()

		client = TRKRakNetworkFactory.GetRakPeerInterface()
		server = TRKRakNetworkFactory.GetRakPeerInterface()

		client.Startup(1, 0, New TRKSocketDescriptor.Create(0))

		server.Startup(1, 0, New TRKSocketDescriptor.Create(1234))
		server.SetMaximumIncomingConnections(1)
		client.AttachPlugin(roomsPluginClient)
		server.AttachPlugin(roomsPluginServer)

		profanityFilter.AddWord("Crapola")
		
		roomsPluginServer.SetProfanityFilter(profanityFilter)
		roomsPluginServer.RoomsContainer().AddTitle(GAME_IDENTIFIER)
		
		Local localAddress:TRKSystemAddress = New TRKSystemAddress.Create()
		localAddress.SetBinaryAddress("127.0.0.1")
		localAddress.SetPort(1234)

		roomsPluginClient.SetServerAddress(localAddress)
		roomsPluginClient.SetRoomsCallback(sampleCallbacks)
		client.Connect("127.0.0.1", 1234)

	
		AddMessage "A system for creating rooms for players to meet in before starting games."
		AddMessage "Difficulty: Intermediate"


		AddMessage "A. CreateRoom"
		AddMessage "B. EnterRoom"
		AddMessage "C. JoinByFilter"
		AddMessage "D. LeaveRoom"
		AddMessage "E. GetInvitesToParticipant"
		AddMessage "F. SendInvite"
		AddMessage "G. AcceptInvite"
		AddMessage "H. StartSpectating"
		AddMessage "I. StopSpectating"
		AddMessage "J. GrantModerator"
		AddMessage "K. ChangeSlotCounts"
		AddMessage "L. SetCustomRoomProperties"
		AddMessage "M. ChangeRoomName"
		AddMessage "N. SetHiddenFromSearches"
		AddMessage "O. SetDestroyOnModeratorLeave"
		AddMessage "P. SetReadyStatus"
		AddMessage "Q. GetReadyStatus"
		AddMessage "R. SetRoomLockState"
		AddMessage "S. GetRoomLockState"
		AddMessage "T. AreAllMembersReady"
		AddMessage "U. KickMember"
		AddMessage "V. UnbanMember"
		AddMessage "W. GetBanReason"
		AddMessage "X. AddUserToQuickJoin"
		AddMessage "Y. RemoveUserFromQuickJoin"
		AddMessage "Z. IsInQuickJoin"
		AddMessage "1. SearchByFilter"
		AddMessage "2. ChangeHandle"
		AddMessage "3. RoomChat"
		AddMessage "4. GetRoomProperties"

		
	End Method
	
	Method RenderExtra()
	End Method

	Method Update()

		Local p:TRKPacket = client.Receive()
		
		If p Then
			packetIdentifier = p.GetPacketIdentifier()
			Select packetIdentifier
				Case ID_CONNECTION_REQUEST_ACCEPTED
					AddMessage "ID_CONNECTION_REQUEST_ACCEPTED"
				Case ID_CONNECTION_ATTEMPT_FAILED
					AddMessage "ID_CONNECTION_ATTEMPT_FAILED"
				Case ID_NO_FREE_INCOMING_CONNECTIONS
					AddMessage "ID_NO_FREE_INCOMING_CONNECTIONS"
			End Select
			
			client.DeallocatePacket(p)
		End If
		
		
		p = server.Receive()

		If p Then
			If p.GetPacketIdentifier() = ID_NEW_INCOMING_CONNECTION Then
				roomsPluginServer.LoginRoomsParticipant("User1", p.GetSystemAddress(), p.GetGuid(), UNASSIGNED_SYSTEM_ADDRESS)
				roomsPluginServer.LoginRoomsParticipant("User2", p.GetSystemAddress(), p.GetGuid(), UNASSIGNED_SYSTEM_ADDRESS)
				roomsPluginServer.LoginRoomsParticipant("User3", p.GetSystemAddress(), p.GetGuid(), UNASSIGNED_SYSTEM_ADDRESS)
			End If
			server.DeallocatePacket(p)
		End If

		If stage = STAGE_NONE Then
			If KeyHit(KEY_A) Then
				AddMessage "CreateRoom"
				
				func = New TRKCreateRoomFunc.Create()
				
				ResetStage([STAGE_USERNAME, STAGE_ROOMNAME, STAGE_CREATEROOM, STAGE_NONE])
				GetUserName()
			End If
			
			If KeyHit(KEY_B) Then
				AddMessage "EnterRoom"
				
				func = New TRKEnterRoomFunc.Create()

				ResetStage([STAGE_USERNAME, STAGE_ROOMNAME, STAGE_ENTERROOM, STAGE_NONE])
				GetUserName()
			End If

			If KeyHit(KEY_C) Then
				AddMessage "JoinByFilter"
				
				func = New TRKJoinByFilterFunc.Create()

				ResetStage([STAGE_USERNAME, STAGE_ROOMNAME, STAGE_JOINBYFILTER, STAGE_NONE])
				GetUserName()
			End If

			If KeyHit(KEY_D) Then
				AddMessage "LeaveRoom"
				
				func = New TRKLeaveRoomFunc.Create()

				ResetStage([STAGE_USERNAME, STAGE_LEAVEROOM, STAGE_NONE])
				GetUserName()
			End If

			If KeyHit(KEY_E) Then
				AddMessage "GetInvitesToParticipant"
				
				func = New TRKGetInvitesToParticipantFunc.Create()

				ResetStage([STAGE_USERNAME, STAGE_GETINVITESTOPARTICIPANT, STAGE_NONE])
				GetUserName()
			End If

			If KeyHit(KEY_F) Then
				AddMessage "SendInvite"
				
				func = New TRKSendInviteFunc.Create()

				ResetStage([STAGE_USERNAME, STAGE_TARGETNAME, STAGE_SENDINVITE, STAGE_NONE])
				GetUserName()
			End If






			If KeyHit(KEY_3) Then
				AddMessage "RoomChat"
				
				func = New TRKChatFunc.Create()

				ResetStage([STAGE_USERNAME, STAGE_ROOMCHAT, STAGE_NONE])
				GetUserName()
			End If
		End If
		
		Select stage
			Case STAGE_CREATEROOM
				SetUserName(func)
				TRKCreateRoomFunc(func).GetNetworkedRoomCreationParameters().SetRoomName(roomName)
				TRKCreateRoomFunc(func).GetNetworkedRoomCreationParameters().GetSlots().SetPublicSlots(1)
				TRKCreateRoomFunc(func).SetGameIdentifier(GAME_IDENTIFIER)
				roomsPluginClient.ExecuteFunc(func)
				UpdateState()

			Case STAGE_ENTERROOM
				SetUserName(func)
				' Create or join the specified room name
				TRKEnterRoomFunc(func).GetNetworkedRoomCreationParameters().SetRoomName(roomName)
				TRKEnterRoomFunc(func).GetQuery().AddQuery_STRING(TRKDefaultRoomColumns.GetColumnName(TRKDefaultRoomColumns.TC_ROOM_NAME), roomName)
				TRKEnterRoomFunc(func).GetNetworkedRoomCreationParameters().GetSlots().SetPublicSlots(2)
				TRKEnterRoomFunc(func).SetRoomMemberMode(RMM_PUBLIC)
				TRKEnterRoomFunc(func).SetGameIdentifier(GAME_IDENTIFIER)
				roomsPluginClient.ExecuteFunc(func)
				UpdateState()

			Case STAGE_JOINBYFILTER
				SetUserName(func)
				TRKJoinByFilterFunc(func).GetQuery().AddQuery_STRING(TRKDefaultRoomColumns.GetColumnName(TRKDefaultRoomColumns.TC_ROOM_NAME), roomName)
				TRKJoinByFilterFunc(func).SetGameIdentifier(GAME_IDENTIFIER)
				TRKJoinByFilterFunc(func).SetRoomMemberMode(RMM_PUBLIC)
				roomsPluginClient.ExecuteFunc(func)

				UpdateState()

			Case STAGE_LEAVEROOM
				SetUserName(func)
				roomsPluginClient.ExecuteFunc(func)

				UpdateState()

			Case STAGE_GETINVITESTOPARTICIPANT
				SetUserName(func)
				roomsPluginClient.ExecuteFunc(func)

				UpdateState()

			Case STAGE_SENDINVITE
				SetUserName(func)
				TRKSendInviteFunc(func).SetInviteeName(targetName)
				TRKSendInviteFunc(func).SetInviteToSpectatorSlot(False)
				TRKSendInviteFunc(func).SetSubject("SendInviteSubject")
				TRKSendInviteFunc(func).SetBody("SendInviteSubject")
				roomsPluginClient.ExecuteFunc(func)

				UpdateState()




			Case STAGE_ROOMCHAT
				SetUserName(func)
				TRKChatFunc(func).SetChatMessage("Hello world. This is Crapola")
				roomsPluginClient.ExecuteFunc(func)

				UpdateState()

		End Select

	End Method
	
	Method OnSubmit(value:String)
		Select stage
			Case STAGE_USERNAME
				user = value.ToInt()

				If Not user Then
					user = 3
				End If
				
				If user Then
					
					UpdateState()
					
					Select stage
						Case STAGE_ROOMNAME
							GetRoomName()
						Case STAGE_TARGETNAME
							GetTargetName()
					End Select
				End If
			
			Case STAGE_ROOMNAME
				If Not value Then
					roomName = DEFAULT_ROOM_NAME
				Else
					roomName = value
				End If

				UpdateState()
				
				Select stage
					Case STAGE_USERNAME
						GetUserName()
					Case STAGE_TARGETNAME
						GetTargetName()
				End Select

			Case STAGE_TARGETNAME
				If Not value Then
					targetName = "User3"
				Else
					targetName = value
				End If

				UpdateState()
				
				Select stage
					Case STAGE_USERNAME
						GetUserName()
					Case STAGE_ROOMNAME
						GetRoomName()
				End Select
		End Select
		
		FlushKeys()
	End Method
	
	Method Finalize()
	End Method
	
	Method ResetStage(s:Int[])
		stages = s
		stageIdx = 0
		UpdateState()
	End Method
	
	Method UpdateState()
		stage = stages[stageIdx]
		stageIdx :+ 1
		
		If stageIdx > stages.length Then
			stageIdx = 0
			stage = STAGE_NONE
		End If
	End Method
	
	Method GetUserName()
		user = 0
		RequestValue("User Name", "Which user? 1=User1, 2=User2, 3=User3 :")
	End Method
	
	Method SetUserName(func:TRKRoomsPluginFunc)
		If user = 1 Then
			func.SetUserName("User1")
		Else If user = 2 Then
			func.SetUserName("User2")
		Else
			func.SetUserName("User3")
		End If

	End Method
	
	Method GetRoomName()
		roomName = ""
		RequestValue("Room Name", "Enter room name, or enter for default :")
		'stage = stages[stageIdx]
	End Method

	Method GetTargetName()
		If user = 1 Then
			targetName = "User1"
		Else If user = 2 Then
			targetName = "User2"
		Else
			targetName = "User3"
		End If
	End Method

End Type

Type TSampleCallbacks Extends TRKRoomsCallback

	Method CreateRoom_Callback( senderAddress:TRKSystemAddress, callResult:TRKCreateRoomFunc)
		roomsRef.AddMessage callResult.PrintResult()
	End Method

	Method EnterRoom_Callback( senderAddress:TRKSystemAddress, callResult:TRKEnterRoomFunc)
		roomsRef.AddMessage callResult.PrintResult()
	End Method

	Method JoinByFilter_Callback( senderAddress:TRKSystemAddress, callResult:TRKJoinByFilterFunc)
		roomsRef.AddMessage callResult.PrintResult()
	End Method

	Method LeaveRoom_Callback( senderAddress:TRKSystemAddress, callResult:TRKLeaveRoomFunc)
		roomsRef.AddMessage callResult.PrintResult()
	End Method

	Method GetInvitesToParticipant_Callback( senderAddress:TRKSystemAddress, callResult:TRKGetInvitesToParticipantFunc)
		roomsRef.AddMessage callResult.PrintResult()
	End Method

	Method SendInvite_Callback( senderAddress:TRKSystemAddress, callResult:TRKSendInviteFunc)
		roomsRef.AddMessage callResult.PrintResult()
	End Method

	Method AcceptInvite_Callback( senderAddress:TRKSystemAddress, callResult:TRKAcceptInviteFunc)
		roomsRef.AddMessage callResult.PrintResult()
	End Method

	Method StartSpectating_Callback( senderAddress:TRKSystemAddress, callResult:TRKStartSpectatingFunc)
		roomsRef.AddMessage callResult.PrintResult()
	End Method

	Method StopSpectating_Callback( senderAddress:TRKSystemAddress, callResult:TRKStopSpectatingFunc)
		roomsRef.AddMessage callResult.PrintResult()
	End Method



	Method Chat_Callback( senderAddress:TRKSystemAddress, callResult:TRKChatFunc)
		roomsRef.AddMessage callResult.PrintResult()
	End Method

End Type


