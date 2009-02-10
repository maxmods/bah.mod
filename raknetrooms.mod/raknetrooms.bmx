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
	' TODO
	End Method
	
	Rem
	bbdoc: Execute a function, either using the system address passed to SetServerAddress(), or the one specified.
	end rem
	Method ExecuteFunc(func:TRKRoomsPluginFunc, remoteAddress:TRKSystemAddress = Null)
	' TODO
	End Method
	
	Rem
	bbdoc: Sets the remote server address that is running RoomsPlugin. Send calls will go to this function
	/// \param[in] systemAddress The remote system, which should be connected while calling client functions
	end rem
	Method SetServerAddress(systemAddress:TRKSystemAddress)
	' TODO
	End Method
	
	Rem
	bbdoc: Add a participant to the system
	about: Only participants can perform operations
	/// \param[in] userName A unique string identifying the user
	/// \param[in] roomsParticipantAddress The address of the user
	/// \param[in] loginServerAddress The server adding this user. Use UNASSIGNED_SYSTEM_ADDRESS for not applicable. Otherwise, the address must previously have been added using AddLoginServerAddress() or the function will fail.
	end rem
	Method LoginRoomsParticipant:Int(userName:String, roomsParticipantAddress:TRKSystemAddress, loginServerAddress:TRKSystemAddress)
	' TODO
	End Method
	
	Rem
	bbdoc: Removes a participant from the system
	/// \param[in] userName A unique string identifying the user
	/// \param[in] loginServerAddress The server removing. Use UNASSIGNED_SYSTEM_ADDRESS for not applicable. Otherwise, the address must previously have been added using AddLoginServerAddress() or the function will fail.
	end rem
	Method LogoffRoomsParticipant:Int(userName:String, loginServerAddress:TRKSystemAddress)
	' TODO
	End Method
	
	Rem
	bbdoc: Clear all users
	end rem
	Method ClearRoomMembers()
		bmx_raknetroomsplugin_ClearRoomMembers(pluginPtr)
	End Method
	
	Method ChangeHandle(oldHandle:String, newHandle:String)
	' TODO
	End Method
	
	Rem
	bbdoc: Add a SystemAddress to a list that will be checked when LoginRoomsParticipant() and LogoffRoomsParticipant() is called
	/// \param[in] systemAddress The address to add
	end rem
	Method AddLoginServerAddress(systemAddress:TRKSystemAddress)
	' TODO
	End Method
	
	Rem
	bbdoc: Remove a SystemAddress from a list that will be checked when LoginRoomsParticipant() and LogoffRoomsParticipant() is called
	/// \param[in] systemAddress The address to remove
	end rem
	Method RemoveLoginServerAddress(systemAddress:TRKSystemAddress)
	' TODO
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
	'Method SetProfanityFilter(pf:TRKProfanityFilter)
	' TODO
	'End Method

	
End Type

Rem
bbdoc: 
End Rem
Type TRKRoomsCallback

	Field roomsCallbackPtr:Byte Ptr
	
	Method New()
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
	
	Method RoomChat_Callback( senderAddress:TRKSystemAddress, callResult:TRKRoomChatFunc)
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
	
	Method RoomChatNotification_Callback( senderAddress:TRKSystemAddress, notification:TRKRoomChatNotification)
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
	
	Function _RoomChat_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, callResult:Byte Ptr)
		callback.RoomChat_Callback(TRKSystemAddress._create(senderAddress), TRKRoomChatFunc._create(callResult))
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
	
	Function _RoomChatNotification_Callback(callback:TRKRoomsCallback, senderAddress:Byte Ptr, notification:Byte Ptr)
		callback.RoomChatNotification_Callback(TRKSystemAddress._create(senderAddress), TRKRoomChatNotification._create(notification))
	End Function
	

End Type


Rem
bbdoc: Base type for rooms functionality
about: Operations performed on rooms are not in the RoomsPlugin - instead, each structure encapsulates one operation.
End Rem
Type TRKRoomsPluginFunc

	Field funcPtr:Byte Ptr
	
	Method GetResultCode:Int()
	End Method
	
	Method GetUserName:String()
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
Type TRKRoomChatFunc Extends TRKRoomsPluginFunc

	Function _create:TRKRoomChatFunc(funcPtr:Byte Ptr)
		If funcPtr Then
			Local this:TRKRoomChatFunc = New TRKRoomChatFunc
			this.funcPtr = funcPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: Base type for notification callbacks.
about: Recipient is always filled out so you know which user this callback applies to.
End Rem
Type TRKRoomsPluginNotification

	Field notificationPtr:Byte Ptr

	Method GetRecipient:String()
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
Type TRKRoomChatNotification Extends TRKRoomsPluginNotification

	Function _create:TRKRoomChatNotification(notificationPtr:Byte Ptr)
		If notificationPtr Then
			Local this:TRKRoomChatNotification = New TRKRoomChatNotification
			this.notificationPtr = notificationPtr
			Return this
		End If
	End Function

End Type


