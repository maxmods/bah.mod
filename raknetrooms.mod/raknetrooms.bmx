' Copyright (c) 2009 Bruce A Henderson
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
bbdoc: Raknet Rooms
End Rem
Module BaH.RaknetRooms

ModuleInfo "Version: 1.00"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: Raknet - The Creative Commons Attribution - NonCommercial 2.5, or see http://www.rakkarsoft.com/SingleApplicationLicense.html"
ModuleInfo "Copyright: 2009 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Version"

Import "common.bmx"


' Build Notes
'
' ProfanityFilter.cpp - Modified to build on Mac.
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
bbdoc: Used to create rooms for players where they can matchmake.
about: A room is similar to the rooms you see in other lobby systems, where groups of players can join together in order
to start a game match.
<ul>
<li>Each player can be in at most one room</li>
<li>Each player name must be unique</li>
<li>Each room has one moderator, which can perform operations on the room such as kicking out members</li>
</ul>
To use as a client:
<ol>
<li>Connect to the server and attach the plugin as normal.</li>
<li>Call SetServerAddress to tell the system where the server is.</li>
<li>Call RoomsPlugin::SetRoomsCallback() with a pointer to a callback structure</li>
<li>Fill in the input parameters of the desired structure(s)</li>
<li>Call RoomsPlugin::ExecuteFunc with a pointer to the structure.</li>
<li>Process the callback, which will contain the original input parameters, plus the new output parameters. All structures contain resultCode, which indicates if the operation was successful (REC_SUCCESS) or not (Anything else)</li>
</ol>
To use as a server:
<ol>
<li>Start RakNet as usual, accepting connections and attaching the plugin</li>
<li>Call RoomsPlugin::SetProfanityFilter() with the ProfanityFilter class, if desired</li>
<li>Call RoomsPlugin::AddTitle() for each title (game) you want to support</li>
<li>If you want other systems to be able to call RoomsPlugin::LoginRoomsParticipant(), call RoomsPlugin::AddLoginServerAddress() with the addresses of those systems</li>
<li>As users go online, call RoomsPlugin::LoginRoomsParticipant(). Login and Logoff is up to you to implement (or rely on other systems, such as Lobby2)</li>
<li>As users go offline, call RoomsPlugin::LogoffRoomsParticipant();</li>
</ol>
End Rem
Type TRKRoomsPlugin Extends TRKPluginInterface

	Field _roomsCallback:TRKRoomsCallback

	Method New()
		pluginPtr = bmx_raknetroomsplugin_new()
	End Method

	Rem
	bbdoc: Ordering channel to send messages on
	end rem
	Method SetOrderingChannel(oc:Int)
		bmx_raknetroomsplugin_SetOrderingChannel(pluginPtr, oc)
	End Method
	
	Rem
	bbdoc: Send priority to send messages on
	end rem
	Method SetSendPriority(pp:Int)
		bmx_raknetroomsplugin_SetSendPriority(pluginPtr, pp)
	End Method
	
	Rem
	bbdoc: Set the callback to get notification and ExecuteFunc() results
	end rem
	Method SetRoomsCallback(roomsCallback:TRKRoomsCallback)
		_roomsCallback = roomsCallback
		bmx_raknetroomsplugin_SetRoomsCallback(pluginPtr, roomsCallback.roomsCallbackPtr)
	End Method
	
	Rem
	bbdoc: Execute a function, either using the system address passed to SetServerAddress(), or the one specified.
	end rem
	Method ExecuteFunc(func:TRKRoomsPluginFunc, remoteAddress:TRKSystemAddress = Null)
		If remoteAddress Then
			bmx_raknetroomsplugin_ExecuteFuncAddress(pluginPtr, func.funcPtr, remoteAddress.systemAddressPtr)
		Else
			bmx_raknetroomsplugin_ExecuteFunc(pluginPtr, func.funcPtr)
		End If
	End Method
	
	Rem
	bbdoc: Sets the remote server address that is running RoomsPlugin. Send calls will go to this function
	/// \param[in] systemAddress The remote system, which should be connected while calling client functions
	end rem
	Method SetServerAddress(systemAddress:TRKSystemAddress)
		bmx_raknetroomsplugin_SetServerAddress(pluginPtr, systemAddress.systemAddressPtr)
	End Method
	
	Rem
	bbdoc: Add a participant to the system
	about: Only participants can perform operations
	/// \param[in] userName A unique string identifying the user
	/// \param[in] roomsParticipantAddress The address of the user
	/// \param[in] loginServerAddress The server adding this user. Use UNASSIGNED_SYSTEM_ADDRESS for not applicable. Otherwise, the address must previously have been added using AddLoginServerAddress() or the function will fail.
	end rem
	Method LoginRoomsParticipant:Int(userName:String, roomsParticipantAddress:TRKSystemAddress, guid:TRKRakNetGUID, loginServerAddress:TRKSystemAddress)
		Return bmx_raknetroomsplugin_LoginRoomsParticipant(pluginPtr, userName, roomsParticipantAddress.systemAddressPtr, guid.guidPtr, loginServerAddress.systemAddressPtr)
	End Method
	
	Rem
	bbdoc: Removes a participant from the system
	/// \param[in] userName A unique string identifying the user
	/// \param[in] loginServerAddress The server removing. Use UNASSIGNED_SYSTEM_ADDRESS for not applicable. Otherwise, the address must previously have been added using AddLoginServerAddress() or the function will fail.
	end rem
	Method LogoffRoomsParticipant:Int(userName:String, loginServerAddress:TRKSystemAddress)
		Return bmx_raknetroomsplugin_LogoffRoomsParticipant(pluginPtr, userName, loginServerAddress.systemAddressPtr)
	End Method
	
	Rem
	bbdoc: Clear all users
	end rem
	Method ClearRoomMembers()
		bmx_raknetroomsplugin_ClearRoomMembers(pluginPtr)
	End Method
	
	Rem
	bbdoc: Add a SystemAddress to a list that will be checked when LoginRoomsParticipant() and LogoffRoomsParticipant() is called
	/// \param[in] systemAddress The address to add
	end rem
	Method AddLoginServerAddress(systemAddress:TRKSystemAddress)
		bmx_raknetroomsplugin_AddLoginServerAddress(pluginPtr, systemAddress.systemAddressPtr)
	End Method
	
	Rem
	bbdoc: Remove a SystemAddress from a list that will be checked when LoginRoomsParticipant() and LogoffRoomsParticipant() is called
	/// \param[in] systemAddress The address to remove
	end rem
	Method RemoveLoginServerAddress(systemAddress:TRKSystemAddress)
		bmx_raknetroomsplugin_RemoveLoginServerAddress(pluginPtr, systemAddress.systemAddressPtr)
	End Method
	
	Rem
	bbdoc: Remove all addresses added with AddLoginServerAddress()
	end rem
	Method ClearLoginServerAdddresses()
		bmx_raknetroomsplugin_ClearLoginServerAdddresses(pluginPtr)
	End Method
	
	Rem
	bbdoc: Sets the profanity filter for the system to use (optional)
	/// If set, room names and player handles will be checked for profanity.
	/// Room invitations and other messages are not checked.
	/// \param[in] pf An instance of a profanity filter
	end rem
	Method SetProfanityFilter(pf:TRKProfanityFilter)
		bmx_raknetroomsplugin_SetProfanityFilter(pluginPtr, pf.filterPtr)
	End Method

	Method RoomsContainer:TRKAllGamesRoomsContainer()
		Return TRKAllGamesRoomsContainer._create(bmx_raknetroomsplugin_RoomsContainer(pluginPtr))
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TRKRoomsCallback

	Field roomsCallbackPtr:Byte Ptr
	
	Method New()
		roomsCallbackPtr = bmx_raknetroomscallback_new(Self)
	End Method
	
	Method CreateRoom_Callback( senderAddress:TRKSystemAddress, callResult:TRKCreateRoomFunc)
	End Method
	
	Method EnterRoom_Callback( senderAddress:TRKSystemAddress, callResult:TRKEnterRoomFunc)
	End Method
	
	Method JoinByFilter_Callback( senderAddress:TRKSystemAddress, callResult:TRKJoinByFilterFunc)
	End Method
	
	Method LeaveRoom_Callback( senderAddress:TRKSystemAddress, callResult:TRKLeaveRoomFunc)
	End Method
	
	Method GetInvitesToParticipant_Callback( senderAddress:TRKSystemAddress, callResult:TRKGetInvitesToParticipantFunc)
	End Method
	
	Method SendInvite_Callback( senderAddress:TRKSystemAddress, callResult:TRKSendInviteFunc)
	End Method
	
	Method AcceptInvite_Callback( senderAddress:TRKSystemAddress, callResult:TRKAcceptInviteFunc)
	End Method
	
	Method StartSpectating_Callback( senderAddress:TRKSystemAddress, callResult:TRKStartSpectatingFunc)
	End Method
	
	Method StopSpectating_Callback( senderAddress:TRKSystemAddress, callResult:TRKStopSpectatingFunc)
	End Method
	
	Method GrantModerator_Callback( senderAddress:TRKSystemAddress, callResult:TRKGrantModeratorFunc)
	End Method
	
	Method ChangeSlotCounts_Callback( senderAddress:TRKSystemAddress, callResult:TRKChangeSlotCountsFunc)
	End Method
	
	Method SetCustomRoomProperties_Callback( senderAddress:TRKSystemAddress, callResult:TRKSetCustomRoomPropertiesFunc)
	End Method
	
	Method ChangeRoomName_Callback( senderAddress:TRKSystemAddress, callResult:TRKChangeRoomNameFunc)
	End Method
	
	Method SetHiddenFromSearches_Callback( senderAddress:TRKSystemAddress, callResult:TRKSetHiddenFromSearchesFunc)
	End Method
	
	Method SetDestroyOnModeratorLeave_Callback( senderAddress:TRKSystemAddress, callResult:TRKSetDestroyOnModeratorLeaveFunc)
	End Method
	
	Method SetReadyStatus_Callback( senderAddress:TRKSystemAddress, callResult:TRKSetReadyStatusFunc)
	End Method
	
	Method GetReadyStatus_Callback( senderAddress:TRKSystemAddress, callResult:TRKGetReadyStatusFunc)
	End Method
	
	Method SetRoomLockState_Callback( senderAddress:TRKSystemAddress, callResult:TRKSetRoomLockStateFunc)
	End Method
	
	Method GetRoomLockState_Callback( senderAddress:TRKSystemAddress, callResult:TRKGetRoomLockStateFunc)
	End Method
	
	Method AreAllMembersReady_Callback( senderAddress:TRKSystemAddress, callResult:TRKAreAllMembersReadyFunc)
	End Method
	
	Method KickMember_Callback( senderAddress:TRKSystemAddress, callResult:TRKKickMemberFunc)
	End Method
	
	Method UnbanMember_Callback( senderAddress:TRKSystemAddress, callResult:TRKUnbanMemberFunc)
	End Method
	
	Method GetBanReason_Callback( senderAddress:TRKSystemAddress, callResult:TRKGetBanReasonFunc)
	End Method
	
	Method AddUserToQuickJoin_Callback( senderAddress:TRKSystemAddress, callResult:TRKAddUserToQuickJoinFunc)
	End Method
	
	Method RemoveUserFromQuickJoin_Callback( senderAddress:TRKSystemAddress, callResult:TRKRemoveUserFromQuickJoinFunc)
	End Method
	
	Method IsInQuickJoin_Callback( senderAddress:TRKSystemAddress, callResult:TRKIsInQuickJoinFunc)
	End Method
	
	Method SearchByFilter_Callback( senderAddress:TRKSystemAddress, callResult:TRKSearchByFilterFunc)
	End Method
	
	Method ChangeHandle_Callback( senderAddress:TRKSystemAddress, callResult:TRKChangeHandleFunc)
	End Method
	
	Method Chat_Callback( senderAddress:TRKSystemAddress, callResult:TRKChatFunc)
	End Method
	
	' Notifications due to other room members
	Method QuickJoinExpired_Callback( senderAddress:TRKSystemAddress, notification:TRKQuickJoinExpiredNotification)
	End Method
	
	Method QuickJoinEnteredRoom_Callback( senderAddress:TRKSystemAddress, notification:TRKQuickJoinEnteredRoomNotification)
	End Method
	
	Method RoomMemberStartedSpectating_Callback( senderAddress:TRKSystemAddress, notification:TRKRoomMemberStartedSpectatingNotification)
	End Method
	
	Method RoomMemberStoppedSpectating_Callback( senderAddress:TRKSystemAddress, notification:TRKRoomMemberStoppedSpectatingNotification)
	End Method
	
	Method ModeratorChanged_Callback( senderAddress:TRKSystemAddress, notification:TRKModeratorChangedNotification)
	End Method
	
	Method SlotCountsSet_Callback( senderAddress:TRKSystemAddress, notification:TRKSlotCountsSetNotification)
	End Method
	
	Method CustomRoomPropertiesSet_Callback( senderAddress:TRKSystemAddress, notification:TRKCustomRoomPropertiesSetNotification)
	End Method
	
	Method RoomNameSet_Callback( senderAddress:TRKSystemAddress, notification:TRKRoomNameSetNotification)
	End Method
	
	Method HiddenFromSearchesSet_Callback( senderAddress:TRKSystemAddress, notification:TRKHiddenFromSearchesSetNotification)
	End Method
	
	Method RoomMemberReadyStatusSet_Callback( senderAddress:TRKSystemAddress, notification:TRKRoomMemberReadyStatusSetNotification)
	End Method
	
	Method RoomLockStateSet_Callback( senderAddress:TRKSystemAddress, notification:TRKRoomLockStateSetNotification)
	End Method
	
	Method RoomMemberKicked_Callback( senderAddress:TRKSystemAddress, notification:TRKRoomMemberKickedNotification)
	End Method
	
	Method RoomMemberHandleSet_Callback( senderAddress:TRKSystemAddress, notification:TRKRoomMemberHandleSetNotification)
	End Method
	
	Method RoomMemberLeftRoom_Callback( senderAddress:TRKSystemAddress, notification:TRKRoomMemberLeftRoomNotification)
	End Method
	
	Method RoomMemberJoinedRoom_Callback( senderAddress:TRKSystemAddress, notification:TRKRoomMemberJoinedRoomNotification)
	End Method
	
	Method RoomInvitationSent_Callback( senderAddress:TRKSystemAddress, notification:TRKRoomInvitationSentNotification)
	End Method
	
	Method RoomInvitationWithdrawn_Callback( senderAddress:TRKSystemAddress, notification:TRKRoomInvitationWithdrawnNotification)
	End Method
	
	Method RoomDestroyedOnModeratorLeft_Callback( senderAddress:TRKSystemAddress, notification:TRKRoomDestroyedOnModeratorLeftNotification)
	End Method
	
	Method ChatNotification_Callback( senderAddress:TRKSystemAddress, notification:TRKChatNotification)
	End Method

	Function _CreateRoom_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.CreateRoom_Callback(TRKSystemAddress._create(senderAddress), TRKCreateRoomFunc._create(callResult))
	End Function
	
	Function _EnterRoom_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.EnterRoom_Callback(TRKSystemAddress._create(senderAddress), TRKEnterRoomFunc._create(callResult))
	End Function
	
	Function _JoinByFilter_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.JoinByFilter_Callback(TRKSystemAddress._create(senderAddress), TRKJoinByFilterFunc._create(callResult))
	End Function
	
	Function _LeaveRoom_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.LeaveRoom_Callback(TRKSystemAddress._create(senderAddress), TRKLeaveRoomFunc._create(callResult))
	End Function
	
	Function _GetInvitesToParticipant_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.GetInvitesToParticipant_Callback(TRKSystemAddress._create(senderAddress), TRKGetInvitesToParticipantFunc._create(callResult))
	End Function
	
	Function _SendInvite_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.SendInvite_Callback(TRKSystemAddress._create(senderAddress), TRKSendInviteFunc._create(callResult))
	End Function
	
	Function _AcceptInvite_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.AcceptInvite_Callback(TRKSystemAddress._create(senderAddress), TRKAcceptInviteFunc._create(callResult))
	End Function
	
	Function _StartSpectating_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.StartSpectating_Callback(TRKSystemAddress._create(senderAddress), TRKStartSpectatingFunc._create(callResult))
	End Function
	
	Function _StopSpectating_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.StopSpectating_Callback(TRKSystemAddress._create(senderAddress), TRKStopSpectatingFunc._create(callResult))
	End Function
	
	Function _GrantModerator_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.GrantModerator_Callback(TRKSystemAddress._create(senderAddress), TRKGrantModeratorFunc._create(callResult))
	End Function
	
	Function _ChangeSlotCounts_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.ChangeSlotCounts_Callback(TRKSystemAddress._create(senderAddress), TRKChangeSlotCountsFunc._create(callResult))
	End Function
	
	Function _SetCustomRoomProperties_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.SetCustomRoomProperties_Callback(TRKSystemAddress._create(senderAddress), TRKSetCustomRoomPropertiesFunc._create(callResult))
	End Function
	
	Function _ChangeRoomName_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.ChangeRoomName_Callback(TRKSystemAddress._create(senderAddress), TRKChangeRoomNameFunc._create(callResult))
	End Function
	
	Function _SetHiddenFromSearches_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.SetHiddenFromSearches_Callback(TRKSystemAddress._create(senderAddress), TRKSetHiddenFromSearchesFunc._create(callResult))
	End Function
	
	Function _SetDestroyOnModeratorLeave_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.SetDestroyOnModeratorLeave_Callback(TRKSystemAddress._create(senderAddress), TRKSetDestroyOnModeratorLeaveFunc._create(callResult))
	End Function
	
	Function _SetReadyStatus_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.SetReadyStatus_Callback(TRKSystemAddress._create(senderAddress), TRKSetReadyStatusFunc._create(callResult))
	End Function
	
	Function _GetReadyStatus_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.GetReadyStatus_Callback(TRKSystemAddress._create(senderAddress), TRKGetReadyStatusFunc._create(callResult))
	End Function
	
	Function _SetRoomLockState_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.SetRoomLockState_Callback(TRKSystemAddress._create(senderAddress), TRKSetRoomLockStateFunc._create(callResult))
	End Function
	
	Function _GetRoomLockState_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.GetRoomLockState_Callback(TRKSystemAddress._create(senderAddress), TRKGetRoomLockStateFunc._create(callResult))
	End Function
	
	Function _AreAllMembersReady_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.AreAllMembersReady_Callback(TRKSystemAddress._create(senderAddress), TRKAreAllMembersReadyFunc._create(callResult))
	End Function
	
	Function _KickMember_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.KickMember_Callback(TRKSystemAddress._create(senderAddress), TRKKickMemberFunc._create(callResult))
	End Function
	
	Function _UnbanMember_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.UnbanMember_Callback(TRKSystemAddress._create(senderAddress), TRKUnbanMemberFunc._create(callResult))
	End Function
	
	Function _GetBanReason_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.GetBanReason_Callback(TRKSystemAddress._create(senderAddress), TRKGetBanReasonFunc._create(callResult))
	End Function
	
	Function _AddUserToQuickJoin_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.AddUserToQuickJoin_Callback(TRKSystemAddress._create(senderAddress), TRKAddUserToQuickJoinFunc._create(callResult))
	End Function
	
	Function _RemoveUserFromQuickJoin_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.RemoveUserFromQuickJoin_Callback(TRKSystemAddress._create(senderAddress), TRKRemoveUserFromQuickJoinFunc._create(callResult))
	End Function
	
	Function _IsInQuickJoin_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.IsInQuickJoin_Callback(TRKSystemAddress._create(senderAddress), TRKIsInQuickJoinFunc._create(callResult))
	End Function
	
	Function _SearchByFilter_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.SearchByFilter_Callback(TRKSystemAddress._create(senderAddress), TRKSearchByFilterFunc._create(callResult))
	End Function
	
	Function _ChangeHandle_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.ChangeHandle_Callback(TRKSystemAddress._create(senderAddress), TRKChangeHandleFunc._create(callResult))
	End Function
	
	Function _Chat_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.Chat_Callback(TRKSystemAddress._create(senderAddress), TRKChatFunc._create(callResult))
	End Function
	
	' Notifications due to other room members
	Function _QuickJoinExpired_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.QuickJoinExpired_Callback(TRKSystemAddress._create(senderAddress), TRKQuickJoinExpiredNotification._create(notification))
	End Function
	
	Function _QuickJoinEnteredRoom_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.QuickJoinEnteredRoom_Callback(TRKSystemAddress._create(senderAddress), TRKQuickJoinEnteredRoomNotification._create(notification))
	End Function
	
	Function _RoomMemberStartedSpectating_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.RoomMemberStartedSpectating_Callback(TRKSystemAddress._create(senderAddress), TRKRoomMemberStartedSpectatingNotification._create(notification))
	End Function
	
	Function _RoomMemberStoppedSpectating_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.RoomMemberStoppedSpectating_Callback(TRKSystemAddress._create(senderAddress), TRKRoomMemberStoppedSpectatingNotification._create(notification))
	End Function
	
	Function _ModeratorChanged_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.ModeratorChanged_Callback(TRKSystemAddress._create(senderAddress), TRKModeratorChangedNotification._create(notification))
	End Function
	
	Function _SlotCountsSet_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.SlotCountsSet_Callback(TRKSystemAddress._create(senderAddress), TRKSlotCountsSetNotification._create(notification))
	End Function
	
	Function _CustomRoomPropertiesSet_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.CustomRoomPropertiesSet_Callback(TRKSystemAddress._create(senderAddress), TRKCustomRoomPropertiesSetNotification._create(notification))
	End Function
	
	Function _RoomNameSet_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.RoomNameSet_Callback(TRKSystemAddress._create(senderAddress), TRKRoomNameSetNotification._create(notification))
	End Function
	
	Function _HiddenFromSearchesSet_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.HiddenFromSearchesSet_Callback(TRKSystemAddress._create(senderAddress), TRKHiddenFromSearchesSetNotification._create(notification))
	End Function
	
	Function _RoomMemberReadyStatusSet_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.RoomMemberReadyStatusSet_Callback(TRKSystemAddress._create(senderAddress), TRKRoomMemberReadyStatusSetNotification._create(notification))
	End Function
	
	Function _RoomLockStateSet_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.RoomLockStateSet_Callback(TRKSystemAddress._create(senderAddress), TRKRoomLockStateSetNotification._create(notification))
	End Function
	
	Function _RoomMemberKicked_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.RoomMemberKicked_Callback(TRKSystemAddress._create(senderAddress), TRKRoomMemberKickedNotification._create(notification))
	End Function
	
	Function _RoomMemberHandleSet_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.RoomMemberHandleSet_Callback(TRKSystemAddress._create(senderAddress), TRKRoomMemberHandleSetNotification._create(notification))
	End Function
	
	Function _RoomMemberLeftRoom_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.RoomMemberLeftRoom_Callback(TRKSystemAddress._create(senderAddress), TRKRoomMemberLeftRoomNotification._create(notification))
	End Function
	
	Function _RoomMemberJoinedRoom_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.RoomMemberJoinedRoom_Callback(TRKSystemAddress._create(senderAddress), TRKRoomMemberJoinedRoomNotification._create(notification))
	End Function
	
	Function _RoomInvitationSent_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.RoomInvitationSent_Callback(TRKSystemAddress._create(senderAddress), TRKRoomInvitationSentNotification._create(notification))
	End Function
	
	Function _RoomInvitationWithdrawn_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.RoomInvitationWithdrawn_Callback(TRKSystemAddress._create(senderAddress), TRKRoomInvitationWithdrawnNotification._create(notification))
	End Function
	
	Function _RoomDestroyedOnModeratorLeft_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.RoomDestroyedOnModeratorLeft_Callback(TRKSystemAddress._create(senderAddress), TRKRoomDestroyedOnModeratorLeftNotification._create(notification))
	End Function
	
	Function _ChatNotification_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.ChatNotification_Callback(TRKSystemAddress._create(senderAddress), TRKChatNotification._create(notification))
	End Function
	

End Type


Rem
bbdoc: Base type for rooms functionality
about: Operations performed on rooms are not in the RoomsPlugin - instead, each structure encapsulates one operation.
End Rem
Type TRKRoomsPluginFunc

	Field funcPtr:Byte Ptr
	Field owner:Int = False
	
	Method GetResultCode:Int()
		Return bmx_roomspluginfunc_getresultcode(funcPtr)
	End Method

	Method GetUserName:String()
		Return bmx_roomspluginfunc_getusername(funcPtr)
	End Method

	Method SetUserName(name:String)
		bmx_roomspluginfunc_setusername(funcPtr, name)
	End Method

	Method PrintResult:String()
		Return "Result for user " + GetUserName() + ": " + TRKRoomsErrorCodeDescription.ToEnglish(GetResultCode())
	End Method


End Type

Rem
bbdoc: 
End Rem
Type TRKCreateRoomFunc Extends TRKRoomsPluginFunc

	Function _create:TRKCreateRoomFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKCreateRoomFunc = New TRKCreateRoomFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

	Method Create:TRKCreateRoomFunc()
		funcPtr = bmx_createroomfunc_create()
		owner = True
		Return Self
	End Method
	
	Method GetNetworkedRoomCreationParameters:TRKNetworkedRoomCreationParameters()
		Return TRKNetworkedRoomCreationParameters._create(bmx_createroomfunc_GetNetworkedRoomCreationParameters(funcPtr))
	End Method
	
	Method GetGameIdentifier:String()
		Return bmx_createroomfunc_GetGameIdentifer(funcPtr)
	End Method
	
	Method SetGameIdentifier(gameIdentifier:String)
		bmx_createroomfunc_SetGameIdentifer(funcPtr, gameIdentifier)
	End Method

	Method GetRoomId:Int()
		Return bmx_createroomfunc_GetRoomId(funcPtr)
	End Method
	
	Method Delete()
		If funcPtr And owner Then
			bmx_createroomfunc_free(funcPtr)
			funcPtr = Null
		End If
	End Method
End Type

Rem
bbdoc: 
End Rem
Type TRKEnterRoomFunc Extends TRKRoomsPluginFunc

	Function _create:TRKEnterRoomFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKEnterRoomFunc = New TRKEnterRoomFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

	Method Create:TRKEnterRoomFunc()
		funcPtr = bmx_enterroomfunc_create()
		owner = True
		Return Self
	End Method
	
	Method GetNetworkedRoomCreationParameters:TRKNetworkedRoomCreationParameters()
		Return TRKNetworkedRoomCreationParameters._create(bmx_enterroomfunc_GetNetworkedRoomCreationParameters(funcPtr))
	End Method
	
	Method GetGameIdentifier:String()
		Return bmx_enterroomfunc_GetGameIdentifer(funcPtr)
	End Method
	
	Method SetGameIdentifier(gameIdentifier:String)
		bmx_enterroomfunc_SetGameIdentifer(funcPtr, gameIdentifier)
	End Method

	Method GetJoinedRoomResult:TRKJoinedRoomResult()
		Return TRKJoinedRoomResult._create(bmx_enterroomfunc_GetJoinedRoomResult(funcPtr))
	End Method
	
	Method IsCreatedRoom:Int()
		Return bmx_enterroomfunc_IsCreatedRoom(funcPtr)
	End Method
	
	Method GetRoomId:Int()
		Return bmx_enterroomfunc_GetRoomId(funcPtr)
	End Method
	
	Method SetRoomMemberMode(_mode:Int)
		bmx_enterroomfunc_SetRoomMemberMode(funcPtr, _mode)
	End Method
	
	Method GetRoomMemberMode:Int()
		Return bmx_enterroomfunc_GetRoomMemberMode(funcPtr)
	End Method
	
	Method GetQuery:TRKRoomQuery()
		Return TRKRoomQuery._create(bmx_enterroomfunc_GetQuery(funcPtr))
	End Method

	Method PrintResult:String()
		If GetResultCode() <> REC_SUCCESS Then
			Return Super.PrintResult()
		Else If IsCreatedRoom() Then
			Return GetUserName() + " created a new room"
		Else
			Return GetUserName() + " entered room with " + GetJoinedRoomResult().GetRoomDescriptor().GetRoomMemberListSize() + " members"
		End If
	End Method


	Method Delete()
		If funcPtr And owner Then
			bmx_enterroomfunc_free(funcPtr)
			funcPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TRKJoinByFilterFunc Extends TRKRoomsPluginFunc

	Function _create:TRKJoinByFilterFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKJoinByFilterFunc = New TRKJoinByFilterFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

	Method Create:TRKJoinByFilterFunc()
		funcPtr = bmx_joinbyfilterfunc_create()
		owner = True
		Return Self
	End Method
	
	Method GetJoinedRoomResult:TRKJoinedRoomResult()
		Return TRKJoinedRoomResult._create(bmx_joinbyfilterfunc_GetJoinedRoomResult(funcPtr))
	End Method

	Method GetGameIdentifier:String()
		Return bmx_joinbyfilterfunc_GetGameIdentifer(funcPtr)
	End Method
	
	Method SetGameIdentifier(gameIdentifier:String)
		bmx_joinbyfilterfunc_SetGameIdentifer(funcPtr, gameIdentifier)
	End Method

	Method SetRoomMemberMode(_mode:Int)
		bmx_joinbyfilterfunc_SetRoomMemberMode(funcPtr, _mode)
	End Method
	
	Method GetRoomMemberMode:Int()
		Return bmx_joinbyfilterfunc_GetRoomMemberMode(funcPtr)
	End Method

	Method GetQuery:TRKRoomQuery()
		Return TRKRoomQuery._create(bmx_joinbyfilterfunc_GetQuery(funcPtr))
	End Method

	Method PrintResult:String()
		If GetResultCode() <> REC_SUCCESS Then
			Return Super.PrintResult()
		Else
			Local s:String = "Joined room " + GetJoinedRoomResult().GetRoomDescriptor().GetPropertyString(TRKDefaultRoomColumns.TC_ROOM_NAME) + ..
				" with id " + GetJoinedRoomResult().GetRoomDescriptor().GetLobbyRoomId() + " and " + ..
				GetJoinedRoomResult().GetRoomDescriptor().GetPropertyValue(TRKDefaultRoomColumns.TC_ROOM_NAME) + " used slots"

			Local list:TRKRoomMemberDescriptor[] =  GetJoinedRoomResult().GetRoomDescriptor().GetRoomMemberList()
			For Local i:Int = 0 Until list.length
				s:+ "~n"
				
			Next
			Return s
		End If
	End Method


	Method Delete()
		If funcPtr And owner Then
			bmx_joinbyfilterfunc_free(funcPtr)
			funcPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TRKLeaveRoomFunc Extends TRKRoomsPluginFunc

	Function _create:TRKLeaveRoomFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKLeaveRoomFunc = New TRKLeaveRoomFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

	Method Create:TRKLeaveRoomFunc()
		funcPtr = bmx_leaveroomfunc_create()
		owner = True
		Return Self
	End Method
	
	Method GetRemoveUserResult:TRKRemoveUserResult()
	' TODO
	End Method
	
	Method Delete()
		If funcPtr And owner Then
			bmx_leaveroomfunc_free(funcPtr)
			funcPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TRKGetInvitesToParticipantFunc Extends TRKRoomsPluginFunc

	Function _create:TRKGetInvitesToParticipantFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKGetInvitesToParticipantFunc = New TRKGetInvitesToParticipantFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

	Method Create:TRKGetInvitesToParticipantFunc()
		funcPtr = bmx_getinvitestoparticipantfunc_create()
		owner = True
		Return Self
	End Method
	
	Method GetInvitedUsers:TRKInvitedUser[]()
	End Method
	
	Method Delete()
		If funcPtr And owner Then
			bmx_getinvitestoparticipantfunc_free(funcPtr)
			funcPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TRKSendInviteFunc Extends TRKRoomsPluginFunc

	Function _create:TRKSendInviteFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKSendInviteFunc = New TRKSendInviteFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

	Method Create:TRKSendInviteFunc()
		funcPtr = bmx_sendinvitefunc_create()
		owner = True
		Return Self
	End Method

	Method SetInviteeName(name:String)
	' TODO
	End Method
	
	Method SetInviteToSpectatorSlot(value:Int)
	' TODO
	End Method
	
	Method SetSubject(subject:String)
	' TODO
	End Method
	
	Method SetBody(body:String)
	' TODO
	End Method
	
	Method Delete()
		If funcPtr And owner Then
			bmx_sendinvitefunc_free(funcPtr)
			funcPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TRKAcceptInviteFunc Extends TRKRoomsPluginFunc

	Function _create:TRKAcceptInviteFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKAcceptInviteFunc = New TRKAcceptInviteFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

	Method Create:TRKAcceptInviteFunc()
		funcPtr = bmx_acceptinvitefunc_create()
		owner = True
		Return Self
	End Method
	
	Method SetInviteSender(sender:String)
	End Method
	
	Method SetRoomId(id:Int)
	End Method
	
	Method Delete()
		If funcPtr And owner Then
			bmx_acceptinvitefunc_free(funcPtr)
			funcPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TRKStartSpectatingFunc Extends TRKRoomsPluginFunc

	Function _create:TRKStartSpectatingFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKStartSpectatingFunc = New TRKStartSpectatingFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

	Method Create:TRKStartSpectatingFunc()
		funcPtr = bmx_startspectatingfunc_create()
		owner = True
		Return Self
	End Method
	
	Method Delete()
		If funcPtr And owner Then
			bmx_startspectatingfunc_free(funcPtr)
			funcPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TRKStopSpectatingFunc Extends TRKRoomsPluginFunc

	Function _create:TRKStopSpectatingFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKStopSpectatingFunc = New TRKStopSpectatingFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

	Method Create:TRKStopSpectatingFunc()
		funcPtr = bmx_stopspectatingfunc_create()
		owner = True
		Return Self
	End Method
	
	Method Delete()
		If funcPtr And owner Then
			bmx_stopspectatingfunc_free(funcPtr)
			funcPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TRKGrantModeratorFunc Extends TRKRoomsPluginFunc

	Function _create:TRKGrantModeratorFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKGrantModeratorFunc = New TRKGrantModeratorFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

	Method Create:TRKGrantModeratorFunc()
		funcPtr = bmx_grantmoderatorfunc_create()
		owner = True
		Return Self
	End Method
	
	Method SetNewModerator(name:String)
	' TODO
	End Method
	
	Method Delete()
		If funcPtr And owner Then
			bmx_grantmoderatorfunc_free(funcPtr)
			funcPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TRKChangeSlotCountsFunc Extends TRKRoomsPluginFunc

	Function _create:TRKChangeSlotCountsFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKChangeSlotCountsFunc = New TRKChangeSlotCountsFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

	Method Create:TRKChangeSlotCountsFunc()
		funcPtr = bmx_changeslotcountsfunc_create()
		owner = True
		Return Self
	End Method
	
	Method GetSlots:TRKSlots()
	' TODO
	End Method
	
	Method Delete()
		If funcPtr And owner Then
			bmx_changeslotcountsfunc_free(funcPtr)
			funcPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TRKSetCustomRoomPropertiesFunc Extends TRKRoomsPluginFunc

	Function _create:TRKSetCustomRoomPropertiesFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKSetCustomRoomPropertiesFunc = New TRKSetCustomRoomPropertiesFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKChangeRoomNameFunc Extends TRKRoomsPluginFunc

	Function _create:TRKChangeRoomNameFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKChangeRoomNameFunc = New TRKChangeRoomNameFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKSetHiddenFromSearchesFunc Extends TRKRoomsPluginFunc

	Function _create:TRKSetHiddenFromSearchesFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKSetHiddenFromSearchesFunc = New TRKSetHiddenFromSearchesFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKSetDestroyOnModeratorLeaveFunc Extends TRKRoomsPluginFunc

	Function _create:TRKSetDestroyOnModeratorLeaveFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKSetDestroyOnModeratorLeaveFunc = New TRKSetDestroyOnModeratorLeaveFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKSetReadyStatusFunc Extends TRKRoomsPluginFunc

	Function _create:TRKSetReadyStatusFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKSetReadyStatusFunc = New TRKSetReadyStatusFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKGetReadyStatusFunc Extends TRKRoomsPluginFunc

	Function _create:TRKGetReadyStatusFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKGetReadyStatusFunc = New TRKGetReadyStatusFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKSetRoomLockStateFunc Extends TRKRoomsPluginFunc

	Function _create:TRKSetRoomLockStateFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKSetRoomLockStateFunc = New TRKSetRoomLockStateFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKGetRoomLockStateFunc Extends TRKRoomsPluginFunc

	Function _create:TRKGetRoomLockStateFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKGetRoomLockStateFunc = New TRKGetRoomLockStateFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKAreAllMembersReadyFunc Extends TRKRoomsPluginFunc

	Function _create:TRKAreAllMembersReadyFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKAreAllMembersReadyFunc = New TRKAreAllMembersReadyFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKKickMemberFunc Extends TRKRoomsPluginFunc

	Function _create:TRKKickMemberFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKKickMemberFunc = New TRKKickMemberFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKUnbanMemberFunc Extends TRKRoomsPluginFunc

	Function _create:TRKUnbanMemberFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKUnbanMemberFunc = New TRKUnbanMemberFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKGetBanReasonFunc Extends TRKRoomsPluginFunc

	Function _create:TRKGetBanReasonFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKGetBanReasonFunc = New TRKGetBanReasonFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKAddUserToQuickJoinFunc Extends TRKRoomsPluginFunc

	Function _create:TRKAddUserToQuickJoinFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKAddUserToQuickJoinFunc = New TRKAddUserToQuickJoinFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKRemoveUserFromQuickJoinFunc Extends TRKRoomsPluginFunc

	Function _create:TRKRemoveUserFromQuickJoinFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKRemoveUserFromQuickJoinFunc = New TRKRemoveUserFromQuickJoinFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKIsInQuickJoinFunc Extends TRKRoomsPluginFunc

	Function _create:TRKIsInQuickJoinFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKIsInQuickJoinFunc = New TRKIsInQuickJoinFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKSearchByFilterFunc Extends TRKRoomsPluginFunc

	Function _create:TRKSearchByFilterFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKSearchByFilterFunc = New TRKSearchByFilterFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKChangeHandleFunc Extends TRKRoomsPluginFunc

	Function _create:TRKChangeHandleFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKChangeHandleFunc = New TRKChangeHandleFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKChatFunc Extends TRKRoomsPluginFunc

	Function _create:TRKChatFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKChatFunc = New TRKChatFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function
	
	Method Create:TRKChatFunc()
		funcPtr = bmx_chatfunc_create()
		owner = True
		Return Self
	End Method
	
	Method SetChatMessage(message:String)
		bmx_chatfunc_SetChatMessage(funcPtr, message)
	End Method
	
	Method SetPrivateMessageRecipient(name:String)
		bmx_chatfunc_SetPrivateMessageRecipient(funcPtr, name)
	End Method
	
	Method SetChatDirectedToRoom(value:Int)
		bmx_chatfunc_SetChatDirectedToRoom(funcPtr, value)
	End Method

	Method Delete()
		If funcPtr And owner Then
			bmx_chatfunc_free(funcPtr)
			funcPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: Base type for notification callbacks.
about: Recipient is always filled out so you know which user this callback applies to.
End Rem
Type TRKRoomsPluginNotification

	Field notificationPtr:Byte Ptr

	Method GetRecipient:String()
		Return bmx_roomspluginnotification_GetRecipient(notificationPtr)
	End Method

	Method PrintResult:String()
	End Method

End Type

Rem
bbdoc: The quick join duration has expired without joining or creating any rooms.
End Rem
Type TRKQuickJoinExpiredNotification Extends TRKRoomsPluginNotification

	Function _create:TRKQuickJoinExpiredNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKQuickJoinExpiredNotification = New TRKQuickJoinExpiredNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function
	
End Type

Rem
bbdoc: 
End Rem
Type TRKQuickJoinEnteredRoomNotification Extends TRKRoomsPluginNotification

	Function _create:TRKQuickJoinEnteredRoomNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKQuickJoinEnteredRoomNotification = New TRKQuickJoinEnteredRoomNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKRoomMemberStartedSpectatingNotification Extends TRKRoomsPluginNotification

	Function _create:TRKRoomMemberStartedSpectatingNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKRoomMemberStartedSpectatingNotification = New TRKRoomMemberStartedSpectatingNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKRoomMemberStoppedSpectatingNotification Extends TRKRoomsPluginNotification

	Function _create:TRKRoomMemberStoppedSpectatingNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKRoomMemberStoppedSpectatingNotification = New TRKRoomMemberStoppedSpectatingNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKModeratorChangedNotification Extends TRKRoomsPluginNotification

	Function _create:TRKModeratorChangedNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKModeratorChangedNotification = New TRKModeratorChangedNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKSlotCountsSetNotification Extends TRKRoomsPluginNotification

	Function _create:TRKSlotCountsSetNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKSlotCountsSetNotification = New TRKSlotCountsSetNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKCustomRoomPropertiesSetNotification Extends TRKRoomsPluginNotification

	Function _create:TRKCustomRoomPropertiesSetNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKCustomRoomPropertiesSetNotification = New TRKCustomRoomPropertiesSetNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKRoomNameSetNotification Extends TRKRoomsPluginNotification

	Function _create:TRKRoomNameSetNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKRoomNameSetNotification = New TRKRoomNameSetNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKHiddenFromSearchesSetNotification Extends TRKRoomsPluginNotification

	Function _create:TRKHiddenFromSearchesSetNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKHiddenFromSearchesSetNotification = New TRKHiddenFromSearchesSetNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKRoomMemberReadyStatusSetNotification Extends TRKRoomsPluginNotification

	Function _create:TRKRoomMemberReadyStatusSetNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKRoomMemberReadyStatusSetNotification = New TRKRoomMemberReadyStatusSetNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKRoomLockStateSetNotification Extends TRKRoomsPluginNotification

	Function _create:TRKRoomLockStateSetNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKRoomLockStateSetNotification = New TRKRoomLockStateSetNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKRoomMemberKickedNotification Extends TRKRoomsPluginNotification

	Function _create:TRKRoomMemberKickedNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKRoomMemberKickedNotification = New TRKRoomMemberKickedNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKRoomMemberHandleSetNotification Extends TRKRoomsPluginNotification

	Function _create:TRKRoomMemberHandleSetNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKRoomMemberHandleSetNotification = New TRKRoomMemberHandleSetNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKRoomMemberLeftRoomNotification Extends TRKRoomsPluginNotification

	Function _create:TRKRoomMemberLeftRoomNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKRoomMemberLeftRoomNotification = New TRKRoomMemberLeftRoomNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKRoomMemberJoinedRoomNotification Extends TRKRoomsPluginNotification

	Function _create:TRKRoomMemberJoinedRoomNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKRoomMemberJoinedRoomNotification = New TRKRoomMemberJoinedRoomNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKRoomInvitationSentNotification Extends TRKRoomsPluginNotification

	Function _create:TRKRoomInvitationSentNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKRoomInvitationSentNotification = New TRKRoomInvitationSentNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKRoomInvitationWithdrawnNotification Extends TRKRoomsPluginNotification

	Function _create:TRKRoomInvitationWithdrawnNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKRoomInvitationWithdrawnNotification = New TRKRoomInvitationWithdrawnNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKRoomDestroyedOnModeratorLeftNotification Extends TRKRoomsPluginNotification

	Function _create:TRKRoomDestroyedOnModeratorLeftNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKRoomDestroyedOnModeratorLeftNotification = New TRKRoomDestroyedOnModeratorLeftNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKChatNotification Extends TRKRoomsPluginNotification

	Function _create:TRKChatNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKChatNotification = New TRKChatNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

	Method GetSender:String()
		Return bmx_chatnotification_GetSender(notificationPtr)
	End Method
	
	Method GetPrivateMessageRecipient:String()
		Return bmx_chatnotification_GetPrivateMessageRecipient(notificationPtr)
	End Method
	
	Method GetChatMessage:String()
		Return bmx_chatnotification_GetChatMessage(notificationPtr)
	End Method

	Method GetFilteredChatMessage:String()
		Return bmx_chatnotification_GetFilteredChatMessage(notificationPtr)
	End Method
	
	Method PrintResult:String()
		Return "Chat_Notification to " + GetRecipient()
	End Method


End Type

Rem
bbdoc: 
End Rem
Type TRKProfanityFilter

	Field filterPtr:Byte Ptr

	Method New()
		filterPtr = bmx_profanityfilter_new()
	End Method
	
	Rem
	bbdoc: Returns true if the string has profanity, false if not.
	End Rem
	Method HasProfanity:Int(text:String)
		Return bmx_profanityfilter_HasProfanity(filterPtr, text)
	End Method

	Rem
	bbdoc: Removes profanity.
	about: Returns number of occurrences of profanity matches (including 0)
	End Rem
	Method FilterProfanity:Int(text:String, output:String Var, filter:Int = True)
		Local ret:Int
		output = bmx_profanityfilter_FilterProfanity(filterPtr, text, filter, Varptr ret)
		Return ret
	End Method
	
	Rem
	bbdoc: Number of profanity words loaded.
	End Rem
	Method Count:Int()
		Return bmx_profanityfilter_Count(filterPtr)
	End Method

	Rem
	bbdoc: Adds a word to the profanity words list.
	End Rem
	Method AddWord(newWord:String)
		bmx_profanityfilter_AddWord(filterPtr, newWord)
	End Method
	
	Method Delete()
		If filterPtr Then
			bmx_profanityfilter_free(filterPtr)
			filterPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TRKAllGamesRoomsContainer

	Field containerPtr:Byte Ptr
	
	Function _create:TRKAllGamesRoomsContainer(containerPtr:Byte Ptr)
		If containerPtr Then
			Local this:TRKAllGamesRoomsContainer = New TRKAllGamesRoomsContainer
			this.containerPtr = containerPtr
			Return this
		End If
	End Function

	Method AddTitle:Int(gameIdentifier:String)
		Return bmx_allgamesroomcontainer_addtitle(containerPtr, gameIdentifier)
	End Method
	
End Type

Type TRKNetworkedRoomCreationParameters

	Field paramPtr:Byte Ptr
	
	Function _create:TRKNetworkedRoomCreationParameters(paramPtr:Byte Ptr)
		If paramPtr Then
			Local this:TRKNetworkedRoomCreationParameters = New TRKNetworkedRoomCreationParameters
			this.paramPtr = paramPtr
			Return this
		End If
	End Function
	
	Method GetSlots:TRKSlots()
		Return TRKSlots._create(bmx_networkedroomcreationparameters_GetSlots(paramPtr))
	End Method
	
	Method GetRoomName:String()
		Return bmx_networkedroomcreationparameters_GetRoomName(paramPtr)
	End Method
	
	Method SetRoomName(name:String)
		bmx_networkedroomcreationparameters_SetRoomName(paramPtr, name)
	End Method

End Type

Type TRKSlots

	Field slotsPtr:Byte Ptr
	
	Function _create:TRKSlots(slotsPtr:Byte Ptr)
		If slotsPtr Then
			Local this:TRKSlots = New TRKSlots
			this.slotsPtr = slotsPtr
			Return this
		End If
	End Function
	
	Method GetPublicSlots:Int()
		Return bmx_slots_GetPublicSlots(slotsPtr)
	End Method
	
	Method SetPublicSlots(slots:Int)
		bmx_slots_SetPublicSlots(slotsPtr, slots)
	End Method
	
	Method GetReservedSlots:Int()
		Return bmx_slots_GetReservedSlots(slotsPtr)
	End Method
	
	Method SetReservedSlots(slots:Int)
		bmx_slots_SetReservedSlots(slotsPtr, slots)
	End Method
	
	Method GetSpectatorSlots:Int()
		Return bmx_slots_GetSpectatorSlots(slotsPtr)
	End Method
	
	Method SetSpectatorSlots(slots:Int)
		bmx_slots_SetSpectatorSlots(slotsPtr, slots)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TRKRoomsErrorCodeDescription

	Rem
	bbdoc: 
	End Rem
	Function ToEnglish:String(result:Int)
		Return bmx_roomserrorcodedescription_ToEnglish(result)
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TRKJoinedRoomResult

	Field resultPtr:Byte Ptr

	Function _create:TRKJoinedRoomResult(resultPtr:Byte Ptr)
		If resultPtr Then
			Local this:TRKJoinedRoomResult = New TRKJoinedRoomResult
			this.resultPtr = resultPtr
			Return this
		End If
	End Function
	
	Method GetRoomDescriptor:TRKRoomDescriptor()
		Return TRKRoomDescriptor._create(bmx_joinedroomresult_getroomdescriptor(resultPtr))
	End Method

	Method GetAcceptedInvitor:TRKRoomsParticipant()
		Return TRKRoomsParticipant._create(bmx_joinedroomresult_getacceptedinvitor(resultPtr))
	End Method
	
	Method GetAcceptedInvitorName:String()
		Return bmx_joinedroomresult_getacceptedinvitorname(resultPtr)
	End Method
	
	Method GetAcceptedInvitorAddress:TRKSystemAddress()
		Return TRKSystemAddress._create(bmx_joinedroomresult_getacceptedinvitoraddress(resultPtr))
	End Method
	
	Method GetJoiningMember:TRKRoomsParticipant()
		Return TRKRoomsParticipant._create(bmx_joinedroomresult_getjoiningmember(resultPtr))
	End Method
	
	Method GetJoiningMemberName:String()
		Return bmx_joinedroomresult_getjoiningmembername(resultPtr)
	End Method
	
	Method GetJoiningMemberAddress:TRKSystemAddress()
		Return TRKSystemAddress._create(bmx_joinedroomresult_getjoiningmemberaddress(resultPtr))
	End Method
	
	Method GetRoomOutput:TRKRoom()
		Return TRKRoom._create(bmx_joinedroomresult_getroomoutput(resultPtr))
	End Method
	
End Type

Type TRKRoomDescriptor

	Field descPtr:Byte Ptr

	Function _create:TRKRoomDescriptor(descPtr:Byte Ptr)
		If descPtr Then
			Local this:TRKRoomDescriptor = New TRKRoomDescriptor
			this.descPtr = descPtr
			Return this
		End If
	End Function
	
	Method GetRoomMemberListSize:Int()
		Return bmx_roomdescriptor_getroommemberlistsize(descPtr)
	End Method
	
	Method GetRoomMemberList:TRKRoomMemberDescriptor[]()
	' TODO
	End Method

	Method GetPropertyString:String(index:Int)
		Return bmx_roomdescriptor_getpropertystring(descPtr, index)
	End Method
	
	Method GetPropertyValue:Double(index:Int)
		Return bmx_roomdescriptor_getpropertyvalue(descPtr, index)
	End Method
	
	Method GetLobbyRoomId:Int()
		Return bmx_roomdescriptor_getlobbyroomid(descPtr)
	End Method
	
End Type


Type TRKRoomsParticipant

	Field partPtr:Byte Ptr

	Function _create:TRKRoomsParticipant(partPtr:Byte Ptr)
		If partPtr Then
			Local this:TRKRoomsParticipant = New TRKRoomsParticipant
			this.partPtr = partPtr
			Return this
		End If
	End Function

End Type

Type TRKRoom

	Field roomPtr:Byte Ptr

	Function _create:TRKRoom(roomPtr:Byte Ptr)
		If roomPtr Then
			Local this:TRKRoom = New TRKRoom
			this.roomPtr = roomPtr
			Return this
		End If
	End Function

End Type

Type TRKRoomMemberDescriptor

	Field descPtr:Byte Ptr

	Function _create:TRKRoomMemberDescriptor(descPtr:Byte Ptr)
		If descPtr Then
			Local this:TRKRoomMemberDescriptor = New TRKRoomMemberDescriptor
			this.descPtr = descPtr
			Return this
		End If
	End Function

End Type

Type TRKDefaultRoomColumns

	Const TC_TITLE_NAME:Int = 0
	Const TC_TITLE_ID:Int = 1
	Const TC_ROOM_NAME:Int = 2
	Const TC_ROOM_ID:Int = 3
	Const TC_TOTAL_SLOTS:Int = 4
	Const TC_TOTAL_PUBLIC_PLUS_RESERVED_SLOTS:Int = 5
	Const TC_USED_SLOTS:Int = 6
	Const TC_USED_PUBLIC_PLUS_RESERVED_SLOTS:Int = 7
	Const TC_REMAINING_SLOTS:Int = 8
	Const TC_REMAINING_PUBLIC_PLUS_RESERVED_SLOTS:Int = 9
	Const TC_TOTAL_PUBLIC_SLOTS:Int = 10
	Const TC_TOTAL_RESERVED_SLOTS:Int = 11
	Const TC_TOTAL_SPECTATOR_SLOTS:Int = 12
	Const TC_USED_PUBLIC_SLOTS:Int = 13
	Const TC_USED_RESERVED_SLOTS:Int = 14
	Const TC_USED_SPECTATOR_SLOTS:Int = 15
	Const TC_REMAINING_PUBLIC_SLOTS:Int = 16
	Const TC_REMAINING_RESERVED_SLOTS:Int = 17
	Const TC_REMAINING_SPECTATOR_SLOTS:Int = 18
	Const TC_CREATION_TIME:Int = 19
	Const TC_DESTROY_ON_MODERATOR_LEAVE:Int = 20
	Const TC_LOBBY_ROOM_PTR:Int = 21
	Const TC_TABLE_COLUMNS_COUNT:Int = 22


	Function GetColumnName:String(columnId:Int)
		Return bmx_defaultroomcolumns_GetColumnName(columnId)
	End Function
	
End Type

Type TRKRoomQuery

	Field queryPtr:Byte Ptr

	Function _create:TRKRoomQuery(queryPtr:Byte Ptr)
		If queryPtr Then
			Local this:TRKRoomQuery = New TRKRoomQuery
			this.queryPtr = queryPtr
			Return this
		End If
	End Function

	Method AddQuery_NUMERIC(columnName:String, numericValue:Double, op:Int = QF_EQUAL)
		bmx_roomquery_AddQuery_NUMERIC(queryPtr, columnName, numericValue, op)
	End Method
	
	Method AddQuery_STRING(columnName:String, charValue:String, op:Int = QF_EQUAL)
		bmx_roomquery_AddQuery_STRING(queryPtr, columnName, charValue, op)
	End Method
	
	Method AddQuery_BINARY(columnName:String, data:Byte Ptr, inputLength:Int, op:Int = QF_EQUAL)
		bmx_roomquery_AddQuery_BINARY(queryPtr, columnName, data, inputLength, op)
	End Method
	
End Type

Type TRKRemoveUserResult
	
	Field resultPtr:Byte Ptr
	
	Function _create:TRKRemoveUserResult(resultPtr:Byte Ptr)
		If resultPtr Then
			Local this:TRKRemoveUserResult = New TRKRemoveUserResult
			this.resultPtr = resultPtr
			Return this
		End If
	End Function

	Method IsRemovedFromQuickJoin:Int()
	' TODO
	End Method
	
	Method IsRemovedFromRoom:Int()
	' TODO
	End Method
	
	Method GetRemovedUserAddress:TRKSystemAddress()
	' TODO
	End Method
	
	Method GetRemovedUserName:String()
	' TODO
	End Method
	
End Type

Type TRKInvitedUser

	Field userPtr:Byte Ptr
	
	Method GetRoom:TRKRoom()
	' TODO
	End Method
	
	Method GetRoomId:Int()
	' TODO
	End Method
	
	Method GetInvitorName:String()
	' TODO
	End Method
	
	Method GetInvitorSystemAddress:TRKSystemAddress()
	' TODO
	End Method
	
	Method GetTarget:String()
	' TODO
	End Method
	
	Method GetSubject:String()
	' TODO
	End Method
	
	Method GetBody:String()
	' TODO
	End Method
	
	Method IsInvitedAsSpectator:Int()
	' TODO
	End Method

End Type


