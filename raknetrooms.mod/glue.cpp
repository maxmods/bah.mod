#include "RoomsPlugin.h"
#include "../raknet.mod/glue.h"
#include "RakNetTypes.h"
#include "ProfanityFilter.h"

struct MaxRoomsCallback;

extern "C" {

#include "blitz.h"

	void _bah_raknetrooms_TRKRoomsCallback__CreateRoom_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::CreateRoom_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__EnterRoom_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::EnterRoom_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__JoinByFilter_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::JoinByFilter_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__LeaveRoom_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::LeaveRoom_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__GetInvitesToParticipant_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::GetInvitesToParticipant_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__SendInvite_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::SendInvite_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__AcceptInvite_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::AcceptInvite_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__StartSpectating_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::StartSpectating_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__StopSpectating_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::StopSpectating_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__GrantModerator_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::GrantModerator_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__ChangeSlotCounts_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::ChangeSlotCounts_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__SetCustomRoomProperties_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::SetCustomRoomProperties_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__ChangeRoomName_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::ChangeRoomName_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__SetHiddenFromSearches_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::SetHiddenFromSearches_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__SetDestroyOnModeratorLeave_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::SetDestroyOnModeratorLeave_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__SetReadyStatus_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::SetReadyStatus_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__GetReadyStatus_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::GetReadyStatus_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__SetRoomLockState_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::SetRoomLockState_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__GetRoomLockState_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::GetRoomLockState_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__AreAllMembersReady_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::AreAllMembersReady_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__KickMember_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::KickMember_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__UnbanMember_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::UnbanMember_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__GetBanReason_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::GetBanReason_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__AddUserToQuickJoin_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::AddUserToQuickJoin_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__RemoveUserFromQuickJoin_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::RemoveUserFromQuickJoin_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__IsInQuickJoin_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::IsInQuickJoin_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__SearchByFilter_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::SearchByFilter_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__ChangeHandle_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::ChangeHandle_Func * callResult);
//	void _bah_raknetrooms_TRKRoomsCallback__RoomChat_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::RoomChat_Func * callResult);
	void _bah_raknetrooms_TRKRoomsCallback__QuickJoinExpired_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::QuickJoinExpired_Notification * notification);
	void _bah_raknetrooms_TRKRoomsCallback__QuickJoinEnteredRoom_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::QuickJoinEnteredRoom_Notification * notification);
	void _bah_raknetrooms_TRKRoomsCallback__RoomMemberStartedSpectating_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::RoomMemberStartedSpectating_Notification * notification);
	void _bah_raknetrooms_TRKRoomsCallback__RoomMemberStoppedSpectating_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::RoomMemberStoppedSpectating_Notification * notification);
	void _bah_raknetrooms_TRKRoomsCallback__ModeratorChanged_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::ModeratorChanged_Notification * notification);
	void _bah_raknetrooms_TRKRoomsCallback__SlotCountsSet_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::SlotCountsSet_Notification * notification);
	void _bah_raknetrooms_TRKRoomsCallback__CustomRoomPropertiesSet_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::CustomRoomPropertiesSet_Notification * notification);
	void _bah_raknetrooms_TRKRoomsCallback__RoomNameSet_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::RoomNameSet_Notification * notification);
	void _bah_raknetrooms_TRKRoomsCallback__HiddenFromSearchesSet_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::HiddenFromSearchesSet_Notification * notification);
	void _bah_raknetrooms_TRKRoomsCallback__RoomMemberReadyStatusSet_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::RoomMemberReadyStatusSet_Notification * notification);
	void _bah_raknetrooms_TRKRoomsCallback__RoomLockStateSet_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::RoomLockStateSet_Notification * notification);
	void _bah_raknetrooms_TRKRoomsCallback__RoomMemberKicked_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::RoomMemberKicked_Notification * notification);
	void _bah_raknetrooms_TRKRoomsCallback__RoomMemberHandleSet_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::RoomMemberHandleSet_Notification * notification);
	void _bah_raknetrooms_TRKRoomsCallback__RoomMemberLeftRoom_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::RoomMemberLeftRoom_Notification * notification);
	void _bah_raknetrooms_TRKRoomsCallback__RoomMemberJoinedRoom_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::RoomMemberJoinedRoom_Notification * notification);
	void _bah_raknetrooms_TRKRoomsCallback__RoomInvitationSent_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::RoomInvitationSent_Notification * notification);
	void _bah_raknetrooms_TRKRoomsCallback__RoomInvitationWithdrawn_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::RoomInvitationWithdrawn_Notification * notification);
	void _bah_raknetrooms_TRKRoomsCallback__RoomDestroyedOnModeratorLeft_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::RoomDestroyedOnModeratorLeft_Notification * notification);
//	void _bah_raknetrooms_TRKRoomsCallback__RoomChatNotification_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::RoomChat_Notification * notification);


	RakNet::RoomsPlugin * bmx_raknetroomsplugin_new();
	void bmx_raknetroomsplugin_SetOrderingChannel(RakNet::RoomsPlugin * rooms, int oc);
	void bmx_raknetroomsplugin_SetSendPriority(RakNet::RoomsPlugin * rooms, PacketPriority pp);
	void bmx_raknetroomsplugin_ClearRoomMembers(RakNet::RoomsPlugin * rooms);
	void bmx_raknetroomsplugin_ClearLoginServerAdddresses(RakNet::RoomsPlugin * rooms);
	void bmx_raknetroomsplugin_SetRoomsCallback(RakNet::RoomsPlugin * rooms, RakNet::RoomsCallback * roomsCallback);
	void bmx_raknetroomsplugin_ExecuteFuncAddress(RakNet::RoomsPlugin * rooms, RakNet::RoomsPluginFunc * func, MaxSystemAddress * remoteAddress);
	void bmx_raknetroomsplugin_ExecuteFunc(RakNet::RoomsPlugin * rooms, RakNet::RoomsPluginFunc * func);
	void bmx_raknetroomsplugin_SetServerAddress(RakNet::RoomsPlugin * rooms, MaxSystemAddress * systemAddress);
	int bmx_raknetroomsplugin_LoginRoomsParticipant(RakNet::RoomsPlugin * rooms, BBString * userName, MaxSystemAddress * roomsParticipantAddress, RakNetGUID * guid, MaxSystemAddress * loginServerAddress);
	int bmx_raknetroomsplugin_LogoffRoomsParticipant(RakNet::RoomsPlugin * rooms, BBString * userName, MaxSystemAddress * loginServerAddress);
	void bmx_raknetroomsplugin_AddLoginServerAddress(RakNet::RoomsPlugin * rooms, MaxSystemAddress * systemAddress);
	void bmx_raknetroomsplugin_RemoveLoginServerAddress(RakNet::RoomsPlugin * rooms, MaxSystemAddress * systemAddress);
	void bmx_raknetroomsplugin_SetProfanityFilter(RakNet::RoomsPlugin * rooms, ProfanityFilter * pf);

	MaxRoomsCallback * bmx_raknetroomscallback_new(BBObject * handle);

	ProfanityFilter * bmx_profanityfilter_new();
	int bmx_profanityfilter_HasProfanity(ProfanityFilter * filter, BBString * text);
	BBString * bmx_profanityfilter_FilterProfanity(ProfanityFilter * filter, BBString * text, int filter, int * ret);
	int bmx_profanityfilter_Count(ProfanityFilter * filter);
	void bmx_profanityfilter_AddWord(ProfanityFilter * filter, BBString * newWord);
	void bmx_profanityfilter_free(ProfanityFilter * filter);

}

struct MaxRoomsCallback: public RakNet::RoomsCallback
{

	MaxRoomsCallback(BBObject * handle)
		: maxHandle(handle)
	{
	}


	BBObject * maxHandle;

	// Results of calls
	virtual void CreateRoom_Callback( SystemAddress senderAddress, RakNet::CreateRoom_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__CreateRoom_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void EnterRoom_Callback( SystemAddress senderAddress, RakNet::EnterRoom_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__EnterRoom_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void JoinByFilter_Callback( SystemAddress senderAddress, RakNet::JoinByFilter_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__JoinByFilter_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void LeaveRoom_Callback( SystemAddress senderAddress, RakNet::LeaveRoom_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__LeaveRoom_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void GetInvitesToParticipant_Callback( SystemAddress senderAddress, RakNet::GetInvitesToParticipant_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__GetInvitesToParticipant_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void SendInvite_Callback( SystemAddress senderAddress, RakNet::SendInvite_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__SendInvite_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void AcceptInvite_Callback( SystemAddress senderAddress, RakNet::AcceptInvite_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__AcceptInvite_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void StartSpectating_Callback( SystemAddress senderAddress, RakNet::StartSpectating_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__StartSpectating_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void StopSpectating_Callback( SystemAddress senderAddress, RakNet::StopSpectating_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__StopSpectating_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void GrantModerator_Callback( SystemAddress senderAddress, RakNet::GrantModerator_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__GrantModerator_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void ChangeSlotCounts_Callback( SystemAddress senderAddress, RakNet::ChangeSlotCounts_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__ChangeSlotCounts_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void SetCustomRoomProperties_Callback( SystemAddress senderAddress, RakNet::SetCustomRoomProperties_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__SetCustomRoomProperties_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void ChangeRoomName_Callback( SystemAddress senderAddress, RakNet::ChangeRoomName_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__ChangeRoomName_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void SetHiddenFromSearches_Callback( SystemAddress senderAddress, RakNet::SetHiddenFromSearches_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__SetHiddenFromSearches_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void SetDestroyOnModeratorLeave_Callback( SystemAddress senderAddress, RakNet::SetDestroyOnModeratorLeave_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__SetDestroyOnModeratorLeave_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void SetReadyStatus_Callback( SystemAddress senderAddress, RakNet::SetReadyStatus_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__SetReadyStatus_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void GetReadyStatus_Callback( SystemAddress senderAddress, RakNet::GetReadyStatus_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__GetReadyStatus_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void SetRoomLockState_Callback( SystemAddress senderAddress, RakNet::SetRoomLockState_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__SetRoomLockState_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void GetRoomLockState_Callback( SystemAddress senderAddress, RakNet::GetRoomLockState_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__GetRoomLockState_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void AreAllMembersReady_Callback( SystemAddress senderAddress, RakNet::AreAllMembersReady_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__AreAllMembersReady_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void KickMember_Callback( SystemAddress senderAddress, RakNet::KickMember_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__KickMember_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void UnbanMember_Callback( SystemAddress senderAddress, RakNet::UnbanMember_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__UnbanMember_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void GetBanReason_Callback( SystemAddress senderAddress, RakNet::GetBanReason_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__GetBanReason_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void AddUserToQuickJoin_Callback( SystemAddress senderAddress, RakNet::AddUserToQuickJoin_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__AddUserToQuickJoin_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void RemoveUserFromQuickJoin_Callback( SystemAddress senderAddress, RakNet::RemoveUserFromQuickJoin_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__RemoveUserFromQuickJoin_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void IsInQuickJoin_Callback( SystemAddress senderAddress, RakNet::IsInQuickJoin_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__IsInQuickJoin_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void SearchByFilter_Callback( SystemAddress senderAddress, RakNet::SearchByFilter_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__SearchByFilter_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
	virtual void ChangeHandle_Callback( SystemAddress senderAddress, RakNet::ChangeHandle_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__ChangeHandle_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
//	virtual void RoomChat_Callback( SystemAddress senderAddress, RakNet::RoomChat_Func *callResult) {
//		_bah_raknetrooms_TRKRoomsCallback__RoomChat_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
//	}
	
	// Notifications due to other room members
	virtual void QuickJoinExpired_Callback( SystemAddress senderAddress, RakNet::QuickJoinExpired_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__QuickJoinExpired_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
	virtual void QuickJoinEnteredRoom_Callback( SystemAddress senderAddress, RakNet::QuickJoinEnteredRoom_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__QuickJoinEnteredRoom_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
	virtual void RoomMemberStartedSpectating_Callback( SystemAddress senderAddress, RakNet::RoomMemberStartedSpectating_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__RoomMemberStartedSpectating_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
	virtual void RoomMemberStoppedSpectating_Callback( SystemAddress senderAddress, RakNet::RoomMemberStoppedSpectating_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__RoomMemberStoppedSpectating_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
	virtual void ModeratorChanged_Callback( SystemAddress senderAddress, RakNet::ModeratorChanged_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__ModeratorChanged_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
	virtual void SlotCountsSet_Callback( SystemAddress senderAddress, RakNet::SlotCountsSet_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__SlotCountsSet_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
	virtual void CustomRoomPropertiesSet_Callback( SystemAddress senderAddress, RakNet::CustomRoomPropertiesSet_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__CustomRoomPropertiesSet_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
	virtual void RoomNameSet_Callback( SystemAddress senderAddress, RakNet::RoomNameSet_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__RoomNameSet_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
	virtual void HiddenFromSearchesSet_Callback( SystemAddress senderAddress, RakNet::HiddenFromSearchesSet_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__HiddenFromSearchesSet_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
	virtual void RoomMemberReadyStatusSet_Callback( SystemAddress senderAddress, RakNet::RoomMemberReadyStatusSet_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__RoomMemberReadyStatusSet_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
	virtual void RoomLockStateSet_Callback( SystemAddress senderAddress, RakNet::RoomLockStateSet_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__RoomLockStateSet_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
	virtual void RoomMemberKicked_Callback( SystemAddress senderAddress, RakNet::RoomMemberKicked_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__RoomMemberKicked_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
	virtual void RoomMemberHandleSet_Callback( SystemAddress senderAddress, RakNet::RoomMemberHandleSet_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__RoomMemberHandleSet_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
	virtual void RoomMemberLeftRoom_Callback( SystemAddress senderAddress, RakNet::RoomMemberLeftRoom_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__RoomMemberLeftRoom_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
	virtual void RoomMemberJoinedRoom_Callback( SystemAddress senderAddress, RakNet::RoomMemberJoinedRoom_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__RoomMemberJoinedRoom_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
	virtual void RoomInvitationSent_Callback( SystemAddress senderAddress, RakNet::RoomInvitationSent_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__RoomInvitationSent_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
	virtual void RoomInvitationWithdrawn_Callback( SystemAddress senderAddress, RakNet::RoomInvitationWithdrawn_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__RoomInvitationWithdrawn_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
	virtual void RoomDestroyedOnModeratorLeft_Callback( SystemAddress senderAddress, RakNet::RoomDestroyedOnModeratorLeft_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__RoomDestroyedOnModeratorLeft_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
//	virtual void RoomChat_Callback( SystemAddress senderAddress, RakNet::RoomChat_Notification *notification) {
//		_bah_raknetrooms_TRKRoomsCallback__RoomChatNotification_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
//	}
	
};




RakNet::RoomsPlugin * bmx_raknetroomsplugin_new() {
	return new RakNet::RoomsPlugin();
}

void bmx_raknetroomsplugin_SetOrderingChannel(RakNet::RoomsPlugin * rooms, int oc) {
	rooms->SetOrderingChannel(static_cast<char>(oc));
}

void bmx_raknetroomsplugin_SetSendPriority(RakNet::RoomsPlugin * rooms, PacketPriority pp) {
	rooms->SetSendPriority(pp);
}

void bmx_raknetroomsplugin_ClearRoomMembers(RakNet::RoomsPlugin * rooms) {
	rooms->ClearRoomMembers();
}

void bmx_raknetroomsplugin_ClearLoginServerAdddresses(RakNet::RoomsPlugin * rooms) {
	rooms->ClearLoginServerAdddresses();
}

void bmx_raknetroomsplugin_SetRoomsCallback(RakNet::RoomsPlugin * rooms, RakNet::RoomsCallback * roomsCallback) {
	rooms->SetRoomsCallback(roomsCallback);
}

void bmx_raknetroomsplugin_ExecuteFuncAddress(RakNet::RoomsPlugin * rooms, RakNet::RoomsPluginFunc * func, MaxSystemAddress * remoteAddress) {
	rooms->ExecuteFunc(func, remoteAddress->Address());
}

void bmx_raknetroomsplugin_ExecuteFunc(RakNet::RoomsPlugin * rooms, RakNet::RoomsPluginFunc * func) {
	rooms->ExecuteFunc(func);
}

void bmx_raknetroomsplugin_SetServerAddress(RakNet::RoomsPlugin * rooms, MaxSystemAddress * systemAddress) {
	rooms->SetServerAddress(systemAddress->Address());
}

int bmx_raknetroomsplugin_LoginRoomsParticipant(RakNet::RoomsPlugin * rooms, BBString * userName, MaxSystemAddress * roomsParticipantAddress, RakNetGUID * guid, MaxSystemAddress * loginServerAddress) {
	char * u = bbStringToCString(userName);
	int ret = rooms->LoginRoomsParticipant(u, roomsParticipantAddress->Address(), *guid, loginServerAddress->Address());
	bbMemFree(u);
	return ret;
}

int bmx_raknetroomsplugin_LogoffRoomsParticipant(RakNet::RoomsPlugin * rooms, BBString * userName, MaxSystemAddress * loginServerAddress) {
	char * u = bbStringToCString(userName);
	int ret = rooms->LogoffRoomsParticipant(u, loginServerAddress->Address());
	bbMemFree(u);
	return ret;
}

void bmx_raknetroomsplugin_AddLoginServerAddress(RakNet::RoomsPlugin * rooms, MaxSystemAddress * systemAddress) {
	rooms->AddLoginServerAddress(systemAddress->Address());
}

void bmx_raknetroomsplugin_RemoveLoginServerAddress(RakNet::RoomsPlugin * rooms, MaxSystemAddress * systemAddress) {
	rooms->RemoveLoginServerAddress(systemAddress->Address());
}

void bmx_raknetroomsplugin_SetProfanityFilter(RakNet::RoomsPlugin * rooms, ProfanityFilter * pf) {
	rooms->SetProfanityFilter(pf);
}

// ****************************

MaxRoomsCallback * bmx_raknetroomscallback_new(BBObject * handle) {
	return new MaxRoomsCallback(handle);
}

// ****************************

ProfanityFilter * bmx_profanityfilter_new() {
	return new ProfanityFilter();
}

int bmx_profanityfilter_HasProfanity(ProfanityFilter * filter, BBString * text) {
	char * t = bbStringToCString(text);
	int ret = static_cast<int>(filter->HasProfanity(t));
	bbMemFree(t);
	return ret;
}

BBString * bmx_profanityfilter_FilterProfanity(ProfanityFilter * filter, BBString * text, int f, int * ret) {
	char * t = bbStringToCString(text);

	char * output = 0;
	if (filter) {
		output = (char *) malloc(strlen(t) + 1);
	}
	
	*ret = filter->FilterProfanity(t, output, static_cast<bool>(f));
	
	bbMemFree(t);
	
	BBString * o = 0;
	if (output) {
		o = bbStringFromCString(output);
		free(output);
	} else {
		o = text;
	}
	
	return o;
}

int bmx_profanityfilter_Count(ProfanityFilter * filter) {
	return filter->Count();
}

void bmx_profanityfilter_AddWord(ProfanityFilter * filter, BBString * newWord) {
	char * w = bbStringToCString(newWord);
	filter->AddWord(w);
	bbMemFree(w);
}

void bmx_profanityfilter_free(ProfanityFilter * filter) {
	delete filter;
}


