#include "RoomsPlugin.h"
#include "../raknet.mod/glue.h"
#include "RakNetTypes.h"
#include "ProfanityFilter.h"
#include "RoomsContainer.h"
#include "RoomsErrorCodes.h"

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
	void _bah_raknetrooms_TRKRoomsCallback__Chat_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::Chat_Func * callResult);
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
	void _bah_raknetrooms_TRKRoomsCallback__ChatNotification_Callback(BBObject * handle, MaxSystemAddress * address, RakNet::Chat_Notification * notification);


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
	RakNet::AllGamesRoomsContainer * bmx_raknetroomsplugin_RoomsContainer(RakNet::RoomsPlugin * rooms);

	MaxRoomsCallback * bmx_raknetroomscallback_new(BBObject * handle);

	ProfanityFilter * bmx_profanityfilter_new();
	int bmx_profanityfilter_HasProfanity(ProfanityFilter * filter, BBString * text);
	BBString * bmx_profanityfilter_FilterProfanity(ProfanityFilter * filter, BBString * text, int filter, int * ret);
	int bmx_profanityfilter_Count(ProfanityFilter * filter);
	void bmx_profanityfilter_AddWord(ProfanityFilter * filter, BBString * newWord);
	void bmx_profanityfilter_free(ProfanityFilter * filter);

	int bmx_allgamesroomcontainer_addtitle(RakNet::AllGamesRoomsContainer * rc, BBString * gameIdentifier);

	int bmx_roomspluginfunc_getresultcode(RakNet::RoomsPluginFunc * func);
	BBString * bmx_roomspluginfunc_getusername(RakNet::RoomsPluginFunc * func);
	void bmx_roomspluginfunc_setusername(RakNet::RoomsPluginFunc * func, BBString * name);

	RakNet::CreateRoom_Func * bmx_createroomfunc_create();
	void bmx_createroomfunc_free(RakNet::CreateRoom_Func * func);
	RakNet::NetworkedRoomCreationParameters * bmx_createroomfunc_GetNetworkedRoomCreationParameters(RakNet::CreateRoom_Func * func);
	BBString * bmx_createroomfunc_GetGameIdentifer(RakNet::CreateRoom_Func * func);
	void bmx_createroomfunc_SetGameIdentifer(RakNet::CreateRoom_Func * func, BBString * gameIdentifier);
	unsigned int bmx_createroomfunc_GetRoomId(RakNet::CreateRoom_Func * func);
	
	RakNet::Slots * bmx_networkedroomcreationparameters_GetSlots(RakNet::NetworkedRoomCreationParameters * param);
	BBString * bmx_networkedroomcreationparameters_GetRoomName(RakNet::NetworkedRoomCreationParameters * param);
	void bmx_networkedroomcreationparameters_SetRoomName(RakNet::NetworkedRoomCreationParameters * param, BBString * name);

	unsigned int bmx_slots_GetPublicSlots(RakNet::Slots * s);
	void bmx_slots_SetPublicSlots(RakNet::Slots * s, unsigned int slots);
	unsigned int bmx_slots_GetReservedSlots(RakNet::Slots * s);
	void bmx_slots_SetReservedSlots(RakNet::Slots * s, unsigned int slots);
	unsigned int bmx_slots_GetSpectatorSlots(RakNet::Slots * s);
	void bmx_slots_SetSpectatorSlots(RakNet::Slots * s, unsigned int slots);

	BBString * bmx_roomserrorcodedescription_ToEnglish(int result);

	RakNet::EnterRoom_Func * bmx_enterroomfunc_create();
	void bmx_enterroomfunc_free(RakNet::EnterRoom_Func * func);
	RakNet::NetworkedRoomCreationParameters * bmx_enterroomfunc_GetNetworkedRoomCreationParameters(RakNet::EnterRoom_Func * func);
	BBString * bmx_enterroomfunc_GetGameIdentifer(RakNet::EnterRoom_Func * func);
	void bmx_enterroomfunc_SetGameIdentifer(RakNet::EnterRoom_Func * func, BBString * gameIdentifier);
	RakNet::JoinedRoomResult * bmx_enterroomfunc_GetJoinedRoomResult(RakNet::EnterRoom_Func * func);
	int bmx_enterroomfunc_IsCreatedRoom(RakNet::EnterRoom_Func * func);
	unsigned int bmx_enterroomfunc_GetRoomId(RakNet::EnterRoom_Func * func);
	void bmx_enterroomfunc_SetRoomMemberMode(RakNet::EnterRoom_Func * func, int _mode);
	int bmx_enterroomfunc_GetRoomMemberMode(RakNet::EnterRoom_Func * func);
	RakNet::RoomQuery * bmx_enterroomfunc_GetQuery(RakNet::EnterRoom_Func * func);

	int bmx_roomdescriptor_getroommemberlistsize(RakNet::RoomDescriptor * desc);
	BBString * bmx_roomdescriptor_getpropertystring(RakNet::RoomDescriptor * desc, int index);
	double bmx_roomdescriptor_getpropertyvalue(RakNet::RoomDescriptor * desc, int index);
	unsigned int bmx_roomdescriptor_getlobbyroomid(RakNet::RoomDescriptor * desc);

	RakNet::RoomDescriptor * bmx_joinedroomresult_getroomdescriptor(RakNet::JoinedRoomResult * res);
	RakNet::RoomsParticipant * bmx_joinedroomresult_getacceptedinvitor(RakNet::JoinedRoomResult * res);
	BBString * bmx_joinedroomresult_getacceptedinvitorname(RakNet::JoinedRoomResult * res);
	MaxSystemAddress * bmx_joinedroomresult_getacceptedinvitoraddress(RakNet::JoinedRoomResult * res);
	RakNet::RoomsParticipant * bmx_joinedroomresult_getjoiningmember(RakNet::JoinedRoomResult * res);
	BBString * bmx_joinedroomresult_getjoiningmembername(RakNet::JoinedRoomResult * res);
	MaxSystemAddress * bmx_joinedroomresult_getjoiningmemberaddress(RakNet::JoinedRoomResult * res);
	RakNet::Room * bmx_joinedroomresult_getroomoutput(RakNet::JoinedRoomResult * res);

	RakNet::JoinByFilter_Func * bmx_joinbyfilterfunc_create();
	void bmx_joinbyfilterfunc_free(RakNet::JoinByFilter_Func * func);
	RakNet::JoinedRoomResult * bmx_joinbyfilterfunc_GetJoinedRoomResult(RakNet::JoinByFilter_Func * func);
	void bmx_joinbyfilterfunc_SetRoomMemberMode(RakNet::JoinByFilter_Func * func, int _mode);
	int bmx_joinbyfilterfunc_GetRoomMemberMode(RakNet::JoinByFilter_Func * func);
	RakNet::RoomQuery * bmx_joinbyfilterfunc_GetQuery(RakNet::JoinByFilter_Func * func);
	BBString * bmx_joinbyfilterfunc_GetGameIdentifer(RakNet::JoinByFilter_Func * func);
	void bmx_joinbyfilterfunc_SetGameIdentifer(RakNet::JoinByFilter_Func * func, BBString * gameIdentifier);

	void bmx_roomquery_AddQuery_NUMERIC(RakNet::RoomQuery * query, BBString * columnName, double numericValue, int op);
	void bmx_roomquery_AddQuery_STRING(RakNet::RoomQuery * query, BBString * columnName, BBString * charValue, int op);
	void bmx_roomquery_AddQuery_BINARY(RakNet::RoomQuery * query, BBString * columnName, const char * data, int inputLength, int op);

	BBString * bmx_defaultroomcolumns_GetColumnName(int columnId);

	RakNet::Chat_Func * bmx_chatfunc_create();
	void bmx_chatfunc_SetChatMessage(RakNet::Chat_Func * func, BBString * message);
	void bmx_chatfunc_SetPrivateMessageRecipient(RakNet::Chat_Func * func, BBString * name);
	void bmx_chatfunc_SetChatDirectedToRoom(RakNet::Chat_Func * func, int value);
	void bmx_chatfunc_free(RakNet::Chat_Func * func);

	RakNet::LeaveRoom_Func * bmx_leaveroomfunc_create();
	void bmx_leaveroomfunc_free(RakNet::LeaveRoom_Func * func);

	RakNet::GetInvitesToParticipant_Func * bmx_getinvitestoparticipantfunc_create();
	void bmx_getinvitestoparticipantfunc_free(RakNet::GetInvitesToParticipant_Func * func);

	RakNet::SendInvite_Func * bmx_sendinvitefunc_create();
	void bmx_sendinvitefunc_free(RakNet::SendInvite_Func * func);

	RakNet::AcceptInvite_Func * bmx_acceptinvitefunc_create();
	void bmx_acceptinvitefunc_free(RakNet::AcceptInvite_Func * func);

	RakNet::StartSpectating_Func * bmx_startspectatingfunc_create();
	void bmx_startspectatingfunc_free(RakNet::StartSpectating_Func * func);

	RakNet::StopSpectating_Func * bmx_stopspectatingfunc_create();
	void bmx_stopspectatingfunc_free(RakNet::StopSpectating_Func * func);

	RakNet::GrantModerator_Func * bmx_grantmoderatorfunc_create();
	void bmx_grantmoderatorfunc_free(RakNet::GrantModerator_Func * func);

	RakNet::ChangeSlotCounts_Func * bmx_changeslotcountsfunc_create();
	void bmx_changeslotcountsfunc_free(RakNet::ChangeSlotCounts_Func * func);

	BBString * bmx_roomspluginnotification_GetRecipient(RakNet::RoomsPluginNotification * note);

	BBString * bmx_chatnotification_GetSender(RakNet::Chat_Notification * note);
	BBString * bmx_chatnotification_GetPrivateMessageRecipient(RakNet::Chat_Notification * note);
	BBString * bmx_chatnotification_GetChatMessage(RakNet::Chat_Notification * note);
	BBString * bmx_chatnotification_GetFilteredChatMessage(RakNet::Chat_Notification * note);

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
	
	virtual void Chat_Callback( SystemAddress senderAddress, RakNet::Chat_Func *callResult) {
		_bah_raknetrooms_TRKRoomsCallback__Chat_Callback(maxHandle, new MaxSystemAddress(senderAddress), callResult);
	}
	
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
	
	virtual void Chat_Callback( SystemAddress senderAddress, RakNet::Chat_Notification *notification) {
		_bah_raknetrooms_TRKRoomsCallback__ChatNotification_Callback(maxHandle, new MaxSystemAddress(senderAddress), notification);
	}
	
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

RakNet::AllGamesRoomsContainer * bmx_raknetroomsplugin_RoomsContainer(RakNet::RoomsPlugin * rooms) {
	return &rooms->roomsContainer;
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


// ****************************

int bmx_allgamesroomcontainer_addtitle(RakNet::AllGamesRoomsContainer * rc, BBString * gameIdentifier) {
	char * g = bbStringToCString(gameIdentifier);
	int ret = rc->AddTitle(g);
	bbMemFree(g);
	return ret;
}

// ****************************

int bmx_roomspluginfunc_getresultcode(RakNet::RoomsPluginFunc * func) {
	return func->resultCode;
}

BBString * bmx_roomspluginfunc_getusername(RakNet::RoomsPluginFunc * func) {
	return bbStringFromCString(func->userName.C_String());
}

void bmx_roomspluginfunc_setusername(RakNet::RoomsPluginFunc * func, BBString * name) {
	char * n = bbStringToCString(name);
	func->userName = n;
	bbMemFree(n);
}

// ****************************

RakNet::CreateRoom_Func * bmx_createroomfunc_create() {
	return new RakNet::CreateRoom_Func;
}

void bmx_createroomfunc_free(RakNet::CreateRoom_Func * func) {
	delete func;
}

RakNet::NetworkedRoomCreationParameters * bmx_createroomfunc_GetNetworkedRoomCreationParameters(RakNet::CreateRoom_Func * func) {
	return &func->networkedRoomCreationParameters;
}

BBString * bmx_createroomfunc_GetGameIdentifer(RakNet::CreateRoom_Func * func) {
	return bbStringFromCString(func->gameIdentifier.C_String());
}

void bmx_createroomfunc_SetGameIdentifer(RakNet::CreateRoom_Func * func, BBString * gameIdentifier) {
	char * n = bbStringToCString(gameIdentifier);
	func->gameIdentifier = n;
	bbMemFree(n);
}

unsigned int bmx_createroomfunc_GetRoomId(RakNet::CreateRoom_Func * func) {
	return func->roomId;
}

// ****************************

RakNet::Slots * bmx_networkedroomcreationparameters_GetSlots(RakNet::NetworkedRoomCreationParameters * param) {
	return &param->slots;
}

BBString * bmx_networkedroomcreationparameters_GetRoomName(RakNet::NetworkedRoomCreationParameters * param) {
	return bbStringFromCString(param->roomName.C_String());
}

void bmx_networkedroomcreationparameters_SetRoomName(RakNet::NetworkedRoomCreationParameters * param, BBString * name) {
	char * n = bbStringToCString(name);
	param->roomName = n;
	bbMemFree(n);
}

// ****************************

unsigned int bmx_slots_GetPublicSlots(RakNet::Slots * s) {
	return s->publicSlots;
}

void bmx_slots_SetPublicSlots(RakNet::Slots * s, unsigned int slots) {
	s->publicSlots = slots;
}

unsigned int bmx_slots_GetReservedSlots(RakNet::Slots * s) {
	return s->reservedSlots;
}

void bmx_slots_SetReservedSlots(RakNet::Slots * s, unsigned int slots) {
	s->reservedSlots = slots;
}

unsigned int bmx_slots_GetSpectatorSlots(RakNet::Slots * s) {
	return s->spectatorSlots;
}

void bmx_slots_SetSpectatorSlots(RakNet::Slots * s, unsigned int slots) {
	s->spectatorSlots = slots;
}

// ****************************

BBString * bmx_roomserrorcodedescription_ToEnglish(int result) {
	return bbStringFromCString(RakNet::RoomsErrorCodeDescription::ToEnglish(static_cast<RakNet::RoomsErrorCode>(result)));
}

// ****************************

RakNet::EnterRoom_Func * bmx_enterroomfunc_create() {
	return new RakNet::EnterRoom_Func;
}

void bmx_enterroomfunc_free(RakNet::EnterRoom_Func * func) {
	delete func;
}

RakNet::NetworkedRoomCreationParameters * bmx_enterroomfunc_GetNetworkedRoomCreationParameters(RakNet::EnterRoom_Func * func) {
	return &func->networkedRoomCreationParameters;
}

BBString * bmx_enterroomfunc_GetGameIdentifer(RakNet::EnterRoom_Func * func) {
	return bbStringFromCString(func->gameIdentifier.C_String());
}

void bmx_enterroomfunc_SetGameIdentifer(RakNet::EnterRoom_Func * func, BBString * gameIdentifier) {
	char * n = bbStringToCString(gameIdentifier);
	func->gameIdentifier = n;
	bbMemFree(n);
}

RakNet::JoinedRoomResult * bmx_enterroomfunc_GetJoinedRoomResult(RakNet::EnterRoom_Func * func) {
	return &func->joinedRoomResult;
}

int bmx_enterroomfunc_IsCreatedRoom(RakNet::EnterRoom_Func * func) {
	return static_cast<int>(func->createdRoom);
}

unsigned int bmx_enterroomfunc_GetRoomId(RakNet::EnterRoom_Func * func) {
	return func->roomId;
}

void bmx_enterroomfunc_SetRoomMemberMode(RakNet::EnterRoom_Func * func, int _mode) {
	func->roomMemberMode = static_cast<RoomMemberMode>(_mode);
}

int bmx_enterroomfunc_GetRoomMemberMode(RakNet::EnterRoom_Func * func) {
	return func->roomMemberMode;
}

RakNet::RoomQuery * bmx_enterroomfunc_GetQuery(RakNet::EnterRoom_Func * func) {
	return &func->query;
}

// ****************************

int bmx_roomdescriptor_getroommemberlistsize(RakNet::RoomDescriptor * desc) {
	return desc->roomMemberList.Size();
}

BBString * bmx_roomdescriptor_getpropertystring(RakNet::RoomDescriptor * desc, int index) {
	return bbStringFromCString(desc->GetProperty(index)->c);
}

double bmx_roomdescriptor_getpropertyvalue(RakNet::RoomDescriptor * desc, int index) {
	return desc->GetProperty(index)->i;
}

unsigned int bmx_roomdescriptor_getlobbyroomid(RakNet::RoomDescriptor * desc) {
	return desc->lobbyRoomId;
}

// ****************************

RakNet::RoomDescriptor * bmx_joinedroomresult_getroomdescriptor(RakNet::JoinedRoomResult * res) {
	return &res->roomDescriptor;
}

RakNet::RoomsParticipant * bmx_joinedroomresult_getacceptedinvitor(RakNet::JoinedRoomResult * res) {
	return res->acceptedInvitor;
}

BBString * bmx_joinedroomresult_getacceptedinvitorname(RakNet::JoinedRoomResult * res) {
	return bbStringFromCString(res->acceptedInvitorName.C_String());
}

MaxSystemAddress * bmx_joinedroomresult_getacceptedinvitoraddress(RakNet::JoinedRoomResult * res) {
	return new MaxSystemAddress(res->acceptedInvitorAddress);
}

RakNet::RoomsParticipant * bmx_joinedroomresult_getjoiningmember(RakNet::JoinedRoomResult * res) {
	return res->joiningMember;
}

BBString * bmx_joinedroomresult_getjoiningmembername(RakNet::JoinedRoomResult * res) {
	return bbStringFromCString(res->joiningMemberName.C_String());
}

MaxSystemAddress * bmx_joinedroomresult_getjoiningmemberaddress(RakNet::JoinedRoomResult * res) {
	return new MaxSystemAddress(res->joiningMemberAddress);
}

RakNet::Room * bmx_joinedroomresult_getroomoutput(RakNet::JoinedRoomResult * res) {
	return res->roomOutput;
}

// ****************************

RakNet::JoinByFilter_Func * bmx_joinbyfilterfunc_create() {
	return new RakNet::JoinByFilter_Func;
}

void bmx_joinbyfilterfunc_free(RakNet::JoinByFilter_Func * func) {
	delete func;
}

RakNet::JoinedRoomResult * bmx_joinbyfilterfunc_GetJoinedRoomResult(RakNet::JoinByFilter_Func * func) {
	return &func->joinedRoomResult;
}

void bmx_joinbyfilterfunc_SetRoomMemberMode(RakNet::JoinByFilter_Func * func, int _mode) {
	func->roomMemberMode = static_cast<RoomMemberMode>(_mode);
}

int bmx_joinbyfilterfunc_GetRoomMemberMode(RakNet::JoinByFilter_Func * func) {
	return func->roomMemberMode;
}

RakNet::RoomQuery * bmx_joinbyfilterfunc_GetQuery(RakNet::JoinByFilter_Func * func) {
	return &func->query;
}

BBString * bmx_joinbyfilterfunc_GetGameIdentifer(RakNet::JoinByFilter_Func * func) {
	return bbStringFromCString(func->gameIdentifier.C_String());
}

void bmx_joinbyfilterfunc_SetGameIdentifer(RakNet::JoinByFilter_Func * func, BBString * gameIdentifier) {
	char * n = bbStringToCString(gameIdentifier);
	func->gameIdentifier = n;
	bbMemFree(n);
}

// ****************************

void bmx_roomquery_AddQuery_NUMERIC(RakNet::RoomQuery * query, BBString * columnName, double numericValue, int op) {
	char * c = bbStringToCString(columnName);
	query->AddQuery_NUMERIC(c, numericValue, static_cast<DataStructures::Table::FilterQueryType>(op));
	bbMemFree(c);
}

void bmx_roomquery_AddQuery_STRING(RakNet::RoomQuery * query, BBString * columnName, BBString * charValue, int op) {
	char * c = bbStringToCString(columnName);
	char * v = bbStringToCString(charValue);
	query->AddQuery_STRING(c, v, static_cast<DataStructures::Table::FilterQueryType>(op));
	bbMemFree(c);
	bbMemFree(v);
}

void bmx_roomquery_AddQuery_BINARY(RakNet::RoomQuery * query, BBString * columnName, const char * data, int inputLength, int op) {
	char * c = bbStringToCString(columnName);
	query->AddQuery_BINARY(c, data, inputLength, static_cast<DataStructures::Table::FilterQueryType>(op));
	bbMemFree(c);
}

// ****************************

BBString * bmx_defaultroomcolumns_GetColumnName(int columnId) {
	return bbStringFromCString(DefaultRoomColumns::GetColumnName(columnId));
}

// ****************************

RakNet::Chat_Func * bmx_chatfunc_create() {
	return new RakNet::Chat_Func;
}

void bmx_chatfunc_SetChatMessage(RakNet::Chat_Func * func, BBString * message) {
	char * m = bbStringToCString(message);
	func->chatMessage = m;
	bbMemFree(m);
}

void bmx_chatfunc_SetPrivateMessageRecipient(RakNet::Chat_Func * func, BBString * name) {
	char * n = bbStringToCString(name);
	func->privateMessageRecipient = n;
	bbMemFree(n);
}

void bmx_chatfunc_SetChatDirectedToRoom(RakNet::Chat_Func * func, int value) {
	func->chatDirectedToRoom = static_cast<bool>(value);
}

void bmx_chatfunc_free(RakNet::Chat_Func * func) {
	delete func;
}

// ****************************

RakNet::LeaveRoom_Func * bmx_leaveroomfunc_create() {
	return new RakNet::LeaveRoom_Func;
}

void bmx_leaveroomfunc_free(RakNet::LeaveRoom_Func * func) {
	delete func;
}

// ****************************

RakNet::GetInvitesToParticipant_Func * bmx_getinvitestoparticipantfunc_create() {
	return new RakNet::GetInvitesToParticipant_Func;
}

void bmx_getinvitestoparticipantfunc_free(RakNet::GetInvitesToParticipant_Func * func) {
	delete func;
}

// ****************************

RakNet::SendInvite_Func * bmx_sendinvitefunc_create() {
	return new RakNet::SendInvite_Func;
}

void bmx_sendinvitefunc_free(RakNet::SendInvite_Func * func) {
	delete func;
}

// ****************************

RakNet::AcceptInvite_Func * bmx_acceptinvitefunc_create() {
	return new RakNet::AcceptInvite_Func;
}

void bmx_acceptinvitefunc_free(RakNet::AcceptInvite_Func * func) {
	delete func;
}

// ****************************

RakNet::StartSpectating_Func * bmx_startspectatingfunc_create() {
	return new RakNet::StartSpectating_Func;
}

void bmx_startspectatingfunc_free(RakNet::StartSpectating_Func * func) {
	delete func;
}

// ****************************

RakNet::StopSpectating_Func * bmx_stopspectatingfunc_create() {
	return new RakNet::StopSpectating_Func;
}

void bmx_stopspectatingfunc_free(RakNet::StopSpectating_Func * func) {
	delete func;
}

// ****************************

RakNet::GrantModerator_Func * bmx_grantmoderatorfunc_create() {
	return new RakNet::GrantModerator_Func;
}

void bmx_grantmoderatorfunc_free(RakNet::GrantModerator_Func * func) {
	delete func;
}

// ****************************

RakNet::ChangeSlotCounts_Func * bmx_changeslotcountsfunc_create() {
	return new RakNet::ChangeSlotCounts_Func;
}

void bmx_changeslotcountsfunc_free(RakNet::ChangeSlotCounts_Func * func) {
	delete func;
}

// ****************************

BBString * bmx_roomspluginnotification_GetRecipient(RakNet::RoomsPluginNotification * note) {
	return bbStringFromCString(note->recipient.C_String());
}

// ****************************

BBString * bmx_chatnotification_GetSender(RakNet::Chat_Notification * note) {
	return bbStringFromCString(note->sender.C_String());
}

BBString * bmx_chatnotification_GetPrivateMessageRecipient(RakNet::Chat_Notification * note) {
	return bbStringFromCString(note->privateMessageRecipient.C_String());
}

BBString * bmx_chatnotification_GetChatMessage(RakNet::Chat_Notification * note) {
	return bbStringFromCString(note->chatMessage.C_String());
}

BBString * bmx_chatnotification_GetFilteredChatMessage(RakNet::Chat_Notification * note) {
	return bbStringFromCString(note->filteredChatMessage.C_String());
}

